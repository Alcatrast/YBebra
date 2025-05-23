#pragma once

#include"geopr.h"

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