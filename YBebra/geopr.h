#pragma once

//#include <SFML/Graphics.hpp>

#include"scenesys.h"

class Entity {
public:
    sf::Color color;
    sf::PrimitiveType type;
    Entity(sf::Color color, sf::PrimitiveType type) : color(color), type(type) {}
    virtual void Render(MetaScene* meta) {}
};

class CheckredPlane : public Entity {
    int gridSize;
    int planeSize;
    Point* center;
    std::vector<sf::Vertex> gridVertices;
public:
    CheckredPlane(Point* center, int planeSize, int gridSize, sf::Color color) : Entity(color, sf::Lines), planeSize(planeSize), gridSize(gridSize) { this->center = center; }
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
        axesVertices.push_back(sf::Vertex(meta->RenderPoint(Point(0, 0, center->Z() + 50)), sf::Color::Blue));
        meta->window->draw(axesVertices.data(), axesVertices.size(), type);
    }
};

class BrokenLine : public Entity {
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
