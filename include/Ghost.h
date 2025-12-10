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

    // Asegúrate de que esta definición coincida con la de tu ghost.cpp
    void move(int dx, int dy, int mapa[15][20], int tileSize);

    void draw(sf::RenderWindow& window);
    sf::FloatRect getBounds() const;
    sf::Vector2f getPosition() const;

    // --- CORRECCIÓN IMPORTANTE ---
    // Mantenemos la definición aquí (inline) para evitar el error de "undefined reference"
    sf::Sprite& getSprite() { return sprite; } 

    void sumarPunto();
    void agregarVida();
    void activarVelocidad();
    void actualizarVelocidad();
    
    int getPuntos() const;
    int getVidas() const;
    float getVelocidad() const;
};