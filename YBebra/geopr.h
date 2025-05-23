#pragma once

#include"scenesys.h"

namespace View {
    namespace Geometry {
        class Entity {
        protected:
            sf::Color color;
            sf::PrimitiveType type;
            std::vector<sf::Vertex> vertices;
        public:
            Entity(sf::Color color, sf::PrimitiveType type) : color(color), type(type) {}
            virtual void Render(MetaScene* meta) {}
            friend class Scene;
            virtual ~Entity() { }
        };

        class CheckredPlane : public Entity {
            int gridSize;
            int planeSize;
            Point* center;
        public:
            CheckredPlane(Point* center, int planeSize, int gridSize, sf::Color color) : Entity(color, sf::Lines), planeSize(planeSize), gridSize(gridSize) { this->center = center; }
            void Render(MetaScene* meta) override {
                for (int x = -planeSize; x <= planeSize; x += gridSize) {
                    vertices.push_back(sf::Vertex(meta->RenderPoint(Point(-planeSize, static_cast<float>(x), center->Z())), color));
                    vertices.push_back(sf::Vertex(meta->RenderPoint(Point(planeSize, static_cast<float>(x), center->Z())), color));
                    vertices.push_back(sf::Vertex(meta->RenderPoint(Point(static_cast<float>(x), -planeSize, center->Z())), color));
                    vertices.push_back(sf::Vertex(meta->RenderPoint(Point(static_cast<float>(x), planeSize, center->Z())), color));
                }
            }
        };
        class Origin : public Entity {
            Point* center;
        public:
            Origin(Point* center) : Entity(color, sf::Lines) { this->center = center; }
            void Render(MetaScene* meta) override {
                vertices.push_back(sf::Vertex(meta->RenderPoint(Point(0, 0, center->Z())), sf::Color::Red));
                vertices.push_back(sf::Vertex(meta->RenderPoint(Point(50, 0, center->Z())), sf::Color::Red));
                vertices.push_back(sf::Vertex(meta->RenderPoint(Point(0, 0, center->Z())), sf::Color::Green));
                vertices.push_back(sf::Vertex(meta->RenderPoint(Point(0, 50, center->Z())), sf::Color::Green));
                vertices.push_back(sf::Vertex(meta->RenderPoint(Point(0, 0, center->Z())), sf::Color::Blue));
                vertices.push_back(sf::Vertex(meta->RenderPoint(Point(0, 0, center->Z() + 50)), sf::Color::Blue));
            }
        };

        class BrokenLine : public Entity {
        public:
            std::vector<Point> points;
            BrokenLine(const std::vector<Point>& points, sf::Color color) : Entity(color, sf::LineStrip), points(points) {}
            ~BrokenLine() {
                points.clear();
            }
            void Render(MetaScene* meta) override {
                for (const auto& point : points) {
                    vertices.push_back(sf::Vertex(meta->RenderPoint(point), color));
                }
            }
        };
    }
}