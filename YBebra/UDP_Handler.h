#include <iostream>
#include <string>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <chrono>

#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")
#else
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#endif

class UDPObserver {
private:
    int socket_desc;
#ifdef _WIN32
    WSADATA wsaData;
#endif
    sockaddr_in server_addr;

    std::queue<std::string> message_queue;
    std::mutex queue_mutex;
    std::condition_variable queue_cv;
    std::atomic<bool> running;
    std::thread receiver_thread;

    void ReceiverLoop() {
        char buffer[1024];
        sockaddr_in client_addr;
        socklen_t client_size = sizeof(client_addr);

        while (running) {
            memset(buffer, 0, sizeof(buffer));

            // Используем select для таймаута, чтобы периодически проверять running
            fd_set readfds;
            FD_ZERO(&readfds);
            FD_SET(socket_desc, &readfds);

            struct timeval tv;
            tv.tv_sec = 1;
            tv.tv_usec = 0;

            int ready = select(socket_desc + 1, &readfds, NULL, NULL, &tv);

            if (ready > 0 && FD_ISSET(socket_desc, &readfds)) {
                int recv_len = recvfrom(socket_desc, buffer, sizeof(buffer) - 1, 0,
                    (sockaddr*)&client_addr, &client_size);

                if (recv_len > 0) {
                    buffer[recv_len] = '\0';
                    std::string message(buffer);

                    std::lock_guard<std::mutex> lock(queue_mutex);
                    message_queue.push(message);
                    queue_cv.notify_one();
                }
            }
        }
    }

public:
    UDPObserver(unsigned short port) : running(true) {
#ifdef _WIN32
        if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
            throw std::runtime_error("WSAStartup failed");
        }
#endif

        socket_desc = socket(AF_INET, SOCK_DGRAM, 0);
        if (socket_desc == -1) {
            throw std::runtime_error("Could not create socket");
        }

        server_addr.sin_family = AF_INET;
        server_addr.sin_addr.s_addr = INADDR_ANY;
        server_addr.sin_port = htons(port);

        if (bind(socket_desc, (sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
#ifdef _WIN32
            closesocket(socket_desc);
            WSACleanup();
#else
            close(socket_desc);
#endif
            throw std::runtime_error("Bind failed");
        }

        receiver_thread = std::thread(&UDPObserver::ReceiverLoop, this);
        std::cout << "Listening on port " << port << "..." << std::endl;
    }

    ~UDPObserver() {
        running = false;
        if (receiver_thread.joinable()) {
            receiver_thread.join();
        }

#ifdef _WIN32
        closesocket(socket_desc);
        WSACleanup();
#else
        close(socket_desc);
#endif
    }

    std::string Read() {
        std::unique_lock<std::mutex> lock(queue_mutex);
        if (queue_cv.wait_for(lock, std::chrono::milliseconds(100),
            [this] { return !message_queue.empty(); })) {
            std::string message = message_queue.front();
            message_queue.pop();
            return message;
        }
        return "";
    }
};

