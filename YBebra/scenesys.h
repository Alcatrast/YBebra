#pragma once

#include <SFML/Graphics.hpp>

#define _USE_MATH_DEFINES
#include <math.h>

class Point {
private:
    float _x, _y, _z;
public:
    float X() const { return _x; }
    float Y() const { return _y; }
    float Z() const { return _z; }
    Point(float x, float y, float z) : _x(x), _y(y), _z(z) {}
    Point() : _x(0.0f), _y(0.0f), _z(0.0f) {}

    friend Point operator-(const Point& a, const Point& b);
};

inline Point operator-(const Point& a, const Point& b) {
    return Point(a._x - b._x, a._y - b._y, a._z - b._z);
}

class Camera {
public:
    Point* center;
    float focalLength = 500.0f;
    float cameraDistance = 500.0f;
    float rotationX = 135.0f;
    float rotationZ = 0.0f;

    Camera(Point* center) { this->center = center; }

    void CamRotate(sf::Vector2i mouseDelta) {
        float rotationSpeedZ = mouseDelta.x * 0.1f;
        this->rotationZ += rotationSpeedZ;

        float rotationSpeedX = mouseDelta.y * 0.1f;
        this->rotationX += rotationSpeedX;
    }

    void SetZoom(float zoomDelta) {
        cameraDistance -= zoomDelta * 50.0f;
        cameraDistance = std::max(cameraDistance, 10.0f);
        cameraDistance = std::min(cameraDistance, 10000.0f);
    }

    Point rotatePoint(const Point& p) {
        float px = p.X() - center->X();
        float py = p.Y() - center->Y();
        float pz = p.Z() - center->Z();

        float radZ = rotationZ * M_PI / 180.0f;
        float x1 = px * cos(radZ) - py * sin(radZ);
        float y1 = px * sin(radZ) + py * cos(radZ);
        float z1 = pz;

        float radX = rotationX * M_PI / 180.0f;
        float y2 = y1 * cos(radX) - z1 * sin(radX);
        float z2 = y1 * sin(radX) + z1 * cos(radX);
        float x2 = x1;

        return Point(
            x2 + center->X(),
            y2 + center->Y(),
            z2 + center->Z() + cameraDistance
        );
    }
};

class MetaScene {
    sf::Vector2f projectTo2D(const Point& point) {
        float x = point.X() * camera->focalLength / (point.Z() + camera->focalLength);
        float y = point.Y() * camera->focalLength / (point.Z() + camera->focalLength);
        return sf::Vector2f(x + screenWidth / 2.0f, y + screenHeight / 2.0f);
    }
public:
    sf::RenderWindow* window;
    Camera* camera;
    const int screenHeight;
    const int screenWidth;
    sf::Vector2f RenderPoint(const Point& point) { return projectTo2D(camera->rotatePoint(point)); }
    MetaScene(sf::RenderWindow* window, Camera* camera, int screenHeight, int screenWidth) : screenHeight(screenHeight), screenWidth(screenWidth) {
        this->camera = camera;
        this->window = window;
    }
};