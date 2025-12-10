#pragma once
#include <SFML/Graphics.hpp>

class Pacman {
private:
    sf::Sprite sprite;
    sf::Texture texture;        // Boca abierta
    sf::Texture textureClosed;  // Boca cerrada
    bool isMouthOpen;           
    sf::Clock animClock;        

    int puntos = 0;
    int vidas = 3;
    float speed;
    
    bool velocidadExtra = false;
    sf::Clock velocidadTimer;

public:
    Pacman();
    sf::Vector2f getPosition() const;
    void draw(sf::RenderWindow& window);

    // La firma coincide exactamente con el .cpp (importante el array mapa[15][20])
    void move(int dx, int dy, int mapa[15][20], int tileSize);

    // Función para la animación
    void updateAnimation(); 

    // Definición "inline" para evitar errores del Linker
    sf::Sprite& getSprite() { return sprite; } 
    
    sf::FloatRect getBounds() const;
    
    void sumarPunto();
    void agregarVida();
    void activarVelocidad();
    void actualizarVelocidad();
    
    int getPuntos() const;
    int getVidas() const;
    float getVelocidad() const;
};