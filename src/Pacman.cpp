#include "../include/pacman.h"  // Asegúrate de incluir el archivo de cabecera correcto

// Constructor de Pacman
Pacman::Pacman() {
    texture.loadFromFile("assets/images/pacman.png");
    sprite.setTexture(texture);
    float escala = (32.0f * 0.8f) / texture.getSize().x;
    sprite.setScale(escala, escala);
    sprite.setOrigin(0, 0);
    sprite.setPosition(1 * 32, 1 * 32);
    speed = 32.0f / 8.0f;
}

// Función getPosition (solo una vez definida)
sf::Vector2f Pacman::getPosition() const {
    return sprite.getPosition();
}

// Función draw (solo una vez definida)
void Pacman::draw(sf::RenderWindow& window) {
    window.draw(sprite);
}

// Función move para Pacman
void Pacman::move(int dx, int dy, int mapa[15][20], int tileSize) {
    float nextX = sprite.getPosition().x + dx * getVelocidad();
    float nextY = sprite.getPosition().y + dy * getVelocidad();
    int fila = nextY / tileSize;
    int col = nextX / tileSize;

    if (fila >= 0 && fila < 15 && col >= 0 && col < 20 && mapa[fila][col] == 0)
        sprite.move(dx * getVelocidad(), dy * getVelocidad());

    if (animClock.getElapsedTime().asMilliseconds() > 150) {
        frame = (frame + 1) % 2;
        animClock.restart();
    }

    // Rotar Pacman según la dirección
    if (dx == -1) sprite.setRotation(180);
    else if (dx == 1) sprite.setRotation(0);
    else if (dy == -1) sprite.setRotation(270);
    else if (dy == 1) sprite.setRotation(90);
}

// Función para obtener los límites del sprite de Pacman (usado para colisiones)
sf::FloatRect Pacman::getBounds() const {
    return sprite.getGlobalBounds();
}

// Incrementar puntos de Pacman
void Pacman::sumarPunto() {
    puntos++;
}

// Agregar vida a Pacman
void Pacman::agregarVida() {
    vidas++;
}


void Pacman::activarVelocidad() {
    velocidadExtra = true;
    velocidadTimer.restart();
}

void Pacman::actualizarVelocidad() {
    if (velocidadExtra && velocidadTimer.getElapsedTime().asSeconds() > 5)
        velocidadExtra = false;
}

int Pacman::getPuntos() const {
    return puntos;
}

int Pacman::getVidas() const {
    return vidas;
}

float Pacman::getVelocidad() const {
    return velocidadExtra ? speed * 1.0f : speed;
}