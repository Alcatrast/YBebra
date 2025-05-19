#include <SFML/Graphics.hpp>
#include <vector>
#include <cmath>
#include <string>
#include <sstream>

#define _USE_MATH_DEFINES // For M_PI in Windows
#include <math.h> //For M_PI in Windows, and for older compilers

class Point {
private:
    float _x, _y, _z;
public:
    float X() const { return _x; }
    float Y() const { return _y; }
    float Z() const { return _z; }
    Point(float x, float y, float z) : _x(x), _y(y), _z(z) {}
};

class Camera {
public:
    Point* center;
    float focalLength = 500.0f;
    float cameraDistance = 500.0f;
    float rotationX = 135.0f;
    float rotationZ = 0.0f;

    Camera(Point* center) { this->center = center; }

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
    MetaScene(sf::RenderWindow* window,  Camera* camera, int screenHeight, int screenWidth) : screenHeight(screenHeight), screenWidth(screenWidth) {
        this->camera = camera;
        this->window = window;
    }
};

class Entity {
public:
    sf::Color color;
    sf::PrimitiveType type;
    Entity(sf::Color color, sf::PrimitiveType type) : color(color), type(type)  {}
    virtual void Render(MetaScene* meta) {}
};

class CheckredPlane : public Entity {
    int gridSize;
    int planeSize;
    Point* center;
    std::vector<sf::Vertex> gridVertices;
public:
    CheckredPlane(Point* center, int planeSize, int gridSize, sf::Color color) : Entity(color, sf::Lines), planeSize(planeSize) , gridSize(gridSize) { this->center = center; }
    void Render(MetaScene* meta) override {
        gridVertices.clear();
        for (int x = -planeSize; x <= planeSize; x += gridSize) {
            gridVertices.push_back(sf::Vertex(meta->RenderPoint(Point(-planeSize, static_cast<float>(x), center->Z())), color));
            gridVertices.push_back(sf::Vertex(meta->RenderPoint(Point(planeSize, static_cast<float>(x), center->Z())), color));
            gridVertices.push_back(sf::Vertex(meta->RenderPoint(Point(static_cast<float>(x), -planeSize, center->Z())), color));
            gridVertices.push_back(sf::Vertex(meta->RenderPoint(Point(static_cast<float>(x), planeSize, center->Z())), color));
        }
        meta->window->draw(gridVertices.data(), gridVertices.size(), type);
    }
};
class Origin : public Entity {
    Point* center;
    std::vector<sf::Vertex> axesVertices;
public:
    Origin(Point* center) : Entity(color, sf::Lines) { this->center = center; }
    void Render(MetaScene* meta) override {
        axesVertices.clear();
        axesVertices.push_back(sf::Vertex(meta->RenderPoint(Point(0, 0, center->Z())), sf::Color::Red));
        axesVertices.push_back(sf::Vertex(meta->RenderPoint(Point(50, 0, center->Z())), sf::Color::Red));
        axesVertices.push_back(sf::Vertex(meta->RenderPoint(Point(0, 0, center->Z())), sf::Color::Green));
        axesVertices.push_back(sf::Vertex(meta->RenderPoint(Point(0, 50, center->Z())), sf::Color::Green));
        axesVertices.push_back(sf::Vertex(meta->RenderPoint(Point(0, 0, center->Z())), sf::Color::Blue));
        axesVertices.push_back(sf::Vertex(meta->RenderPoint(Point(0, 0, center->Z()+50)), sf::Color::Blue));
        meta->window->draw(axesVertices.data(), axesVertices.size(), type);
    }
};

class BrokenLine : public Entity{
public:
    std::vector<Point> points;
    BrokenLine(const std::vector<Point>& points, sf::Color color) : Entity(color, sf::LineStrip), points(points) {}
    
    void Render(MetaScene* meta) override {
        std::vector<sf::Vertex> vertices;
        for (const auto& point : points) {
            vertices.push_back(sf::Vertex(meta->RenderPoint(point), color));
        }
        meta->window->draw(vertices.data(), vertices.size(), type);
    }
};

class Scene {
    MetaScene* meta;
public:
    std::vector<Entity*> entities;
    void Render() {
        for (const auto& entity : entities) {
            entity->Render(meta);
        }
    }
    Scene(sf::RenderWindow* window, Camera* camera, int screenWidth, int screenHeight) { meta = new MetaScene(window, camera, screenWidth, screenHeight); }
};

int main() {
    std::vector<Point> points = {
        Point(0.0f, 0.0f, 100.0f),
        Point(50.0f, 30.0f, 150.0f),
        Point(100.0f, -20.0f, 200.0f),
        Point(150.0f, 40.0f, 250.0f),
        Point(200.0f, 0.0f, 300.0f)
    };

    Camera camera(&points[0]);
    CheckredPlane* plane = new CheckredPlane(&points[0], 200, 20, sf::Color(100, 100, 100));
    Origin* origin = new Origin(&points[0]);
    BrokenLine* rocketTrace = new BrokenLine(points, sf::Color::White);

    int screenWidth = 1200, screenHeight = 800;
    sf::RenderWindow window(sf::VideoMode(screenWidth, screenHeight), "SFML 3D Line");
    window.setFramerateLimit(60);
    Scene scene( &window, &camera, screenWidth, screenHeight);

    scene.entities.push_back(plane);
    scene.entities.push_back(origin);
    scene.entities.push_back(rocketTrace);


    camera.focalLength = 500.0f;
    camera.cameraDistance = 500.0f;
    camera.rotationX = 135.0f;
    camera.rotationZ = 0.0f;

    bool isRotating = false;
    sf::Vector2i lastMousePos;

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();

            if (event.type == sf::Event::MouseWheelScrolled) {
                float zoomDelta = 0.0f;
#if SFML_VERSION_MAJOR < 2 || (SFML_VERSION_MAJOR == 2 && SFML_VERSION_MINOR < 5)
                zoomDelta = event.mouseWheelScroll.wheel;
#else
                zoomDelta = event.mouseWheelScroll.delta;
#endif
                camera.cameraDistance -= zoomDelta * 50.0f;
                camera.cameraDistance = std::max(camera.cameraDistance, 10.0f);
                camera.cameraDistance = std::min(camera.cameraDistance, 10000.0f);
            }

            if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
                isRotating = true;
                lastMousePos = sf::Mouse::getPosition(window);
            }

            if (event.type == sf::Event::MouseButtonReleased && event.mouseButton.button == sf::Mouse::Left) {
                isRotating = false;
            }
        }

        if (isRotating) {
            sf::Vector2i currentMousePos = sf::Mouse::getPosition(window);
            sf::Vector2i mouseDelta = currentMousePos - lastMousePos;

            float rotationSpeedZ = mouseDelta.x * 0.1f;
            camera.rotationZ += rotationSpeedZ;

            float rotationSpeedX = mouseDelta.y * 0.1f;
            camera.rotationX += rotationSpeedX;

            lastMousePos = currentMousePos;
        }

        window.clear();

        if (points.size() > 1) {
            scene.Render();
        }

        window.display();
    }

    return 0;
}