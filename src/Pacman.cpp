#include "../include/pacman.h" 

Pacman::Pacman() {
    texture.loadFromFile("assets/images/pacman.png");
    textureClosed.loadFromFile("assets/images/pacman2.png"); // Cargar la segunda imagen

    sprite.setTexture(texture);
    
    // --- CORRECCIÓN CRÍTICA: Quitamos setOrigin y el desfase de posición ---
    float escala = (32.0f * 0.8f) / texture.getSize().x;
    sprite.setScale(escala, escala);
    
    // Posición exacta en la rejilla (sin sumar 16)
    sprite.setPosition(1 * 32, 1 * 32); 

    speed = 32.0f / 8.0f; // Ajusta esto si sientes que va muy lento o rápido
    isMouthOpen = true;
}

// Esta es la nueva función que llamaremos desde el juego
void Pacman::updateAnimation() {
    if (animClock.getElapsedTime().asMilliseconds() > 100) { 
        if (isMouthOpen) {
            sprite.setTexture(textureClosed);
            isMouthOpen = false;
        } else {
            sprite.setTexture(texture);
            isMouthOpen = true;
        }
        animClock.restart();
    }
}

sf::Vector2f Pacman::getPosition() const { return sprite.getPosition(); }

void Pacman::draw(sf::RenderWindow& window) { window.draw(sprite); }

sf::FloatRect Pacman::getBounds() const { return sprite.getGlobalBounds(); }

void Pacman::sumarPunto() { puntos++; }
void Pacman::agregarVida() { vidas++; }
int Pacman::getPuntos() const { return puntos; }
int Pacman::getVidas() const { return vidas; }

void Pacman::activarVelocidad() {
    velocidadExtra = true;
    velocidadTimer.restart();
}

void Pacman::actualizarVelocidad() {
    if (velocidadExtra && velocidadTimer.getElapsedTime().asSeconds() > 5)
        velocidadExtra = false;
}

float Pacman::getVelocidad() const {
    return velocidadExtra ? speed * 1.5f : speed;
}