#pragma once

#include"geopr.h"


class Scene {
    MetaScene* meta;
public:
    std::vector<View::Geometry::Entity*> entities;
    void Render() {
        for (const auto& entity : entities) {
            entity->vertices.clear();
            entity->Render(meta);
            meta->window->draw(entity->vertices.data(), entity->vertices.size(), entity->type);

        }
    }
    Scene(sf::RenderWindow* window, Camera* camera, int screenWidth, int screenHeight) { meta = new MetaScene(window, camera, screenWidth, screenHeight); }
    ~Scene() {
        for (auto& el : entities) delete el;
        entities.clear();
        delete meta;
    }
};

