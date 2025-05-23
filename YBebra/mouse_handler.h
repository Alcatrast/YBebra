#pragma once

#include <SFML/Graphics.hpp>
#include <functional>

class MouseObserver {
    std::atomic<bool> isRotating{ false };
    sf::Vector2i lastMousePos;
    sf::RenderWindow* window;
public:
    std::vector<std::function<void(sf::Vector2i)>> OnPressedMouseRotate;
    std::vector<std::function<void(float)>> OnM3Rotate;

    MouseObserver(sf::RenderWindow* window) : window(window) {}
    void UpdateMouserotation() {
        if (isRotating) {
            sf::Vector2i currentMousePos = sf::Mouse::getPosition(*window);
            sf::Vector2i mouseDelta = currentMousePos - lastMousePos;
            lastMousePos = currentMousePos;

            for (auto& func : OnPressedMouseRotate) func(mouseDelta);
        }
    }
    void HandleEvent(const sf::Event& event) {
        if (event.type == sf::Event::MouseWheelScrolled) {
            float zoomDelta = 0.0f;
#if SFML_VERSION_MAJOR < 2 || (SFML_VERSION_MAJOR == 2 && SFML_VERSION_MINOR < 5)
            zoomDelta = event.mouseWheelScroll.wheel;
#else
            zoomDelta = event.mouseWheelScroll.delta;
#endif              
            for (auto& func : OnM3Rotate) func(zoomDelta);
        }
        else if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
            isRotating = true;
            lastMousePos = sf::Mouse::getPosition(*window);
        }
        else if (event.type == sf::Event::MouseButtonReleased && event.mouseButton.button == sf::Mouse::Left) {
            isRotating = false;
        }
        if (isRotating) UpdateMouserotation();
    }
    void RunEvents() {
        sf::Event event;
        while (window->pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window->close();
            }
            HandleEvent(event); 
        }
    }
};