#include <SFML/Graphics.hpp>
#include <vector>
#include <cmath>
#include <string>
#include <sstream>
#include <functional>
#include<iostream>


#include"scene.h"
#include"mouse_handler.h"
#include"UDP_Handler.h"

bool TryParse(const std::string& inp, float* result) {
    std::istringstream iss(inp);
    std::vector<float> values;
    float value;
    while (iss >> value) values.push_back(value);
    if (values.size() == 6 && iss.eof()) {
        for (int i = 0; i < 6; ++i) {
            result[i] = values[i];
        }
        return true;
    }
    return false;
}

Point firstpoint;
bool FPGetted = false;

int main() {

     std::vector<Point> points = {
        Point(0.0f, 0.0f, 0.0f),
       // Point(50.0f, 30.0f, 150.0f),
        
    };

    Camera camera(&points[0]);
    CheckredPlane* plane = new CheckredPlane(&points[0], 200, 20, sf::Color(100, 100, 100));
    Origin* origin = new Origin(&points[0]);
    BrokenLine* rocketTrace = new BrokenLine(points, sf::Color::White);

    int screenWidth = 1200, screenHeight = 800;
    sf::RenderWindow window(sf::VideoMode(screenWidth, screenHeight), "SFML 3D Line");
    window.setFramerateLimit(60);
    Scene scene(&window, &camera, screenWidth, screenHeight);

    scene.entities.push_back(plane);
    scene.entities.push_back(origin);
    scene.entities.push_back(rocketTrace);

    MouseObserver observer(&window);
    observer.OnPressedMouseRotate.push_back([&camera](sf::Vector2i v) { camera.CamRotate(v); });
    observer.OnM3Rotate.push_back([&camera](float x) { camera.SetZoom(x); });

    UDPObserver udp(8151);

    while (window.isOpen()) {
        observer.RunEvents();

        std::string UDPinput=udp.Read();
        float* res= new float[6];
        if (TryParse(UDPinput, res)) {
            if(FPGetted) rocketTrace->points.push_back(*(new Point(res[2], res[1], res[0]))-firstpoint);
            else { firstpoint = *new Point(res[0], res[1], res[0]); FPGetted = true; }
        }

        window.clear();
       /* if (points.size() > 1) {*/
            scene.Render();
        //}
        window.display();
    }

    return 0;
}