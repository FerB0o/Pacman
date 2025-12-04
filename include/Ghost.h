#pragma once
#include <SFML/Graphics.hpp>

class Ghost {
private:
    sf::Sprite sprite;
    sf::Texture texture;
    float speed;

    int puntos = 0;
    int vidas = 1;
    bool velocidadExtra = false;
    sf::Clock velocidadTimer;

public:
    Ghost();
    void move(int dx, int dy, int mapa[15][20], int tileSize);
    void draw(sf::RenderWindow& window);
    sf::FloatRect getBounds() const;
    sf::Vector2f getPosition() const;
    sf::Sprite& getSprite(); 

    void sumarPunto();
    void agregarVida();
    void activarVelocidad();
    void actualizarVelocidad();
    int getPuntos() const;
    int getVidas() const;
    float getVelocidad() const;
};