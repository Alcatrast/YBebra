#pragma once

#include <SFML/Graphics.hpp>
#include <sstream>
#include <iostream>
#include <iomanip>
#include <vector>

class UIElement {
protected:
    sf::Vector2f position;
    sf::Vector2f size;
public:
    UIElement(const sf::Vector2f& pos, const sf::Vector2f& sz)
        : position(pos), size(sz) {
    }
    virtual ~UIElement() = default;

    virtual void draw(sf::RenderWindow& window) = 0;
    virtual void update() {}
};

class UINumberDisplay : public UIElement {
private:
    sf::Font& font;
    sf::Text text;
    float* valuePtr;
    std::string label;

public:
    UINumberDisplay(sf::Font& fontRef, const sf::Vector2f& pos,
        const sf::Vector2f& sz, const std::string& lbl,
        float* valPtr)
        : UIElement(pos, sz), font(fontRef), label(lbl), valuePtr(valPtr)
    {
        text.setFont(font);
        text.setCharacterSize(20);
        text.setFillColor(sf::Color::White);
        text.setPosition(pos.x + 5, pos.y + (sz.y - 20) / 2);
    }

    void draw(sf::RenderWindow& window) override {
        if (!text.getFont() || !valuePtr) return;

        std::ostringstream oss;
        oss << label << ": " << std::fixed << std::setprecision(2) << *valuePtr;
        text.setString(oss.str());

        if (text.getLocalBounds().width > size.x - 10) {
            text.setScale((size.x - 10) / text.getLocalBounds().width, 1.f);
        }

        sf::RectangleShape background(size);
        background.setPosition(position);
        background.setFillColor(sf::Color(50, 50, 50, 200));
        background.setOutlineColor(sf::Color::White);
        background.setOutlineThickness(1);

        window.draw(background);
        window.draw(text);
    }
};

class UIPanel {
private:
    sf::Vector2f position;
    sf::Vector2f size;
    std::vector<UIElement*> elements;
    sf::Font font;

public:
    UIPanel(const sf::Vector2f& pos, const sf::Vector2f& sz)
        : position(pos), size(sz)
    {
        if (!font.loadFromFile("arial.ttf")) {
            std::cerr << "Failed to load font!" << std::endl;
        }
    }

    ~UIPanel() {
        clearElements();
    }

    void addElement(UIElement* element) {
        elements.push_back(element);
    }

    void clearElements() {
        for (auto* elem : elements) {
            delete elem;
        }
        elements.clear();
    }

    UIPanel(const UIPanel&) = delete;
    UIPanel& operator=(const UIPanel&) = delete;

    void draw(sf::RenderWindow& window) {
        sf::RectangleShape background(size);
        background.setPosition(position);
        background.setFillColor(sf::Color(30, 30, 30, 220));
        background.setOutlineColor(sf::Color::White);
        background.setOutlineThickness(2);
        window.draw(background);

        for (auto* elem : elements) {
            if (elem) elem->draw(window);
        }
    }

    void update() {
        for (auto* elem : elements) {
            if (elem) elem->update();
        }
    }

    sf::Font& getFont() { return font; }
};