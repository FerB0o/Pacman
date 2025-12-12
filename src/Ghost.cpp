#include "../include/ghost.h"  // Asegúrate de incluir el archivo de cabecera correcto

// Constructor de Ghost
Ghost::Ghost() {
    texture.loadFromFile("assets/images/ghost.png");
    sprite.setTexture(texture);
    float escala = (32.0f * 0.8f) / texture.getSize().x;
    sprite.setScale(escala, escala);
    sprite.setOrigin(0, 0);
    sprite.setPosition(13 * 32, 9 * 32);
    speed = 32.0f / 8.0f;
}

// Obtener la posición del fantasma
sf::Vector2f Ghost::getPosition() const {
    return sprite.getPosition();
}

// Dibujar el fantasma en la ventana
void Ghost::draw(sf::RenderWindow& window) {
    window.draw(sprite);
}

// Mover el fantasma según los parámetros
void Ghost::move(int dx, int dy, int mapa[15][20], int tileSize) {
    float nextX = sprite.getPosition().x + dx * getVelocidad();
    float nextY = sprite.getPosition().y + dy * getVelocidad();
    int fila = nextY / tileSize;
    int col = nextX / tileSize;

    // Comprobamos si el movimiento es válido (no colisiona con las paredes)
    if (fila >= 0 && fila < 15 && col >= 0 && col < 20 && mapa[fila][col] == 0)
        sprite.move(dx * getVelocidad(), dy * getVelocidad());
}

// Obtener los límites del fantasma (para colisiones)
sf::FloatRect Ghost::getBounds() const {
    return sprite.getGlobalBounds();
}

// Incrementar puntos del fantasma
void Ghost::sumarPunto() {
    puntos++;
}

void Ghost::agregarVida() {
    vidas++;
}

void Ghost::activarVelocidad() {
    velocidadExtra = true;
    velocidadTimer.restart();
}

void Ghost::actualizarVelocidad() {
    if (velocidadExtra && velocidadTimer.getElapsedTime().asSeconds() > 5)
        velocidadExtra = false;
}

int Ghost::getPuntos() const {
    return puntos;
}

int Ghost::getVidas() const {
    return vidas;
}

float Ghost::getVelocidad() const {
    return velocidadExtra ? speed * 2.0f : speed;
}

void Ghost::resetPosition() {
    // Coordenadas iniciales del Fantasma.
    // Ajusta estos números a donde quieras que reaparezca.
    // Ejemplo: Centro del mapa (aprox)
    sprite.setPosition(320, 320);
}