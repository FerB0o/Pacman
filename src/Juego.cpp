// === src/Juego.cpp ===
#include "../include/ghost.h" 
#include "../include/pacman.h"
#include "../include/Juego.h"
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <iostream>

// --- CORRECCIÓN 1: Actualizar dimensión a [21][21] ---
bool puedeMover(float x, float y, int dx, int dy, int tileSize, const int mapa[21][21]) {
    int fila = (y + dy * tileSize) / tileSize;
    int col = (x + dx * tileSize) / tileSize;
    // --- CORRECCIÓN 2: Actualizar límites a 21 ---
    return fila >= 0 && fila < 21 && col >= 0 && col < 21 && mapa[fila][col] == 0;
}

bool estaCentradoEnTile(sf::Vector2f pos, int tileSize) {
    return static_cast<int>(pos.x) % tileSize == 0 && static_cast<int>(pos.y) % tileSize == 0;
}

// --- CORRECCIÓN 3: Actualizar dimensión a [21][21] ---
void moverPorBuffer(sf::Sprite& sprite, Direccion& actual, Direccion& buffer, int tileSize, const int mapa[21][21], float velocidad) {
    sf::Vector2f pos = sprite.getPosition();

    if (estaCentradoEnTile(pos, tileSize)) {
        if (puedeMover(pos.x, pos.y, buffer.dx, buffer.dy, tileSize, mapa)) {
            actual = buffer;
        }
        if (!puedeMover(pos.x, pos.y, actual.dx, actual.dy, tileSize, mapa)) {
            actual = {0, 0};
        }
    }
    sprite.move(actual.dx * velocidad, actual.dy * velocidad);
}

void GameManager::run() {
    // Definimos constantes antes de usar la ventana
    const int filas = 21, columnas = 21, tileSize = 32;

    sf::RenderWindow window(sf::VideoMode(columnas * tileSize, filas * tileSize), "PACMAN");
    window.setFramerateLimit(60);

    // Pantalla de inicio
    sf::Texture inicioTexture;
    if (!inicioTexture.loadFromFile("assets/images/inicio.png")) {
        std::cerr << "No se pudo cargar la imagen de inicio." << std::endl;
    }
    sf::Sprite inicioSprite(inicioTexture);
    // Ajustar escala de inicio al nuevo tamaño de ventana (672x672 aprox)
    inicioSprite.setScale(
        (float)(columnas * tileSize) / inicioTexture.getSize().x, 
        (float)(filas * tileSize) / inicioTexture.getSize().y
    );

    // Sonido de inicio
    sf::SoundBuffer inicioBuffer;
    if (!inicioBuffer.loadFromFile("assets/music/inicio.ogg")) {
        std::cerr << "No se pudo cargar el sonido de inicio." << std::endl;
    }
    sf::Sound inicioSound(inicioBuffer);
    inicioSound.setLoop(true);
    inicioSound.play();

    bool juegoIniciado = false;
    while (window.isOpen() && !juegoIniciado) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Space)
                juegoIniciado = true;
        }
        window.clear();
        window.draw(inicioSprite);
        window.display();
    }
    inicioSound.stop();

    sf::Texture fondoTexture;
    fondoTexture.loadFromFile("assets/images/background.png");
    sf::Sprite fondo(fondoTexture);
    // Ajustar fondo al nuevo tamaño
    fondo.setScale(
        (float)(columnas * tileSize) / fondoTexture.getSize().x, 
        (float)(filas * tileSize) / fondoTexture.getSize().y
    );

    // MAPA NUEVO 21x21
    int mapa[filas][columnas] = {
        {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
        {1,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,1},
        {1,0,1,1,1,0,1,1,1,0,1,0,1,1,1,0,1,1,1,0,1},
        {1,0,1,1,1,0,1,1,1,0,1,0,1,1,1,0,1,1,1,0,1},
        {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
        {1,0,1,1,1,0,1,0,1,1,1,1,1,0,1,0,1,1,1,0,1},
        {1,0,0,0,0,0,1,0,0,0,1,0,0,0,1,0,0,0,0,0,1},
        {1,1,1,1,1,0,1,1,1,0,1,0,1,1,1,0,1,1,1,1,1},
        {0,0,0,0,0,0,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0}, 
        {1,1,1,1,1,0,1,0,1,1,0,1,1,0,1,0,1,1,1,1,1}, 
        {0,0,0,0,0,0,0,0,1,0,0,0,1,0,0,0,0,0,0,0,0}, 
        {1,1,1,1,1,0,1,0,1,1,1,1,1,0,1,0,1,1,1,1,1},
        {0,0,0,0,0,0,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0},
        {1,1,1,1,1,0,1,0,1,1,1,1,1,0,1,0,1,1,1,1,1},
        {1,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,1},
        {1,0,1,1,1,0,1,1,1,0,1,0,1,1,1,0,1,1,1,0,1},
        {1,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,1},
        {1,1,1,0,1,0,1,0,1,1,1,1,1,0,1,0,1,0,1,1,1},
        {1,0,0,0,0,0,1,0,0,0,1,0,0,0,1,0,0,0,0,0,1},
        {1,0,1,1,1,1,1,1,1,0,1,0,1,1,1,1,1,1,1,0,1},
        {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}
    };

    int puntos[filas][columnas] = {};
    int frutas[filas][columnas] = {};

    for (int i = 0; i < filas; ++i)
        for (int j = 0; j < columnas; ++j)
            if (mapa[i][j] == 0) puntos[i][j] = 1;

    for (int i = 0; i < 5; ++i) {
        int r = rand() % filas;
        int c = rand() % columnas;
        if (mapa[r][c] == 0) frutas[r][c] = (rand() % 2) + 1;
    }

    Pacman pacman;
    Ghost ghost;
    sf::Font font;
    font.loadFromFile("assets/fonts/Minecraft.ttf");

    sf::Text textoPacman, textoGhost;
    textoPacman.setFont(font);
    textoGhost.setFont(font);
    textoPacman.setCharacterSize(20);
    textoGhost.setCharacterSize(20);
    textoPacman.setFillColor(sf::Color::Magenta);
    textoGhost.setFillColor(sf::Color::Red);
    textoPacman.setPosition(10, 5);
    textoGhost.setPosition(400, 5);

    sf::SoundBuffer bufferEat;
    bufferEat.loadFromFile("assets/music/eating.ogg");
    sf::Sound sonidoEat(bufferEat);

    Direccion dirP, bufferP;
    Direccion dirG, bufferG;

    sf::Clock clock;       
    float moveTimer = 0.0f;
    float moveDelay = 0.05f; 

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        float deltaTime = clock.restart().asSeconds();
        moveTimer += deltaTime;

        pacman.updateAnimation(); 

        if (moveTimer > moveDelay) {
            
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))  bufferP = {-1, 0};
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) bufferP = {1, 0};
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))    bufferP = {0, -1};
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))  bufferP = {0, 1};

            if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) bufferG = {-1, 0};
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) bufferG = {1, 0};
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) bufferG = {0, -1};
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) bufferG = {0, 1};

            // Aquí se pasan las funciones corregidas que aceptan mapa[21][21]
            moverPorBuffer(pacman.getSprite(), dirP, bufferP, tileSize, mapa, pacman.getVelocidad());
            moverPorBuffer(ghost.getSprite(), dirG, bufferG, tileSize, mapa, ghost.getVelocidad());

            pacman.actualizarVelocidad();
            ghost.actualizarVelocidad();

            sf::Vector2f posP = pacman.getPosition();
            int filaP = posP.y / tileSize;
            int colP = posP.x / tileSize;
            // Verificar limites antes de acceder al array
            if (filaP >= 0 && filaP < filas && colP >= 0 && colP < columnas) {
                if (puntos[filaP][colP] == 1) {
                    puntos[filaP][colP] = 0;
                    pacman.sumarPunto();
                    sonidoEat.play();
                }
                if (frutas[filaP][colP] == 1) { frutas[filaP][colP] = 0; pacman.agregarVida(); }
                if (frutas[filaP][colP] == 2) { frutas[filaP][colP] = 0; pacman.activarVelocidad(); }
            }

            sf::Vector2f posG = ghost.getPosition();
            int filaG = posG.y / tileSize;
            int colG = posG.x / tileSize;
            if (filaG >= 0 && filaG < filas && colG >= 0 && colG < columnas) {
                if (puntos[filaG][colG] == 1) {
                    puntos[filaG][colG] = 0;
                    ghost.sumarPunto();
                    sonidoEat.play();
                }
                if (frutas[filaG][colG] == 1) { frutas[filaG][colG] = 0; ghost.agregarVida(); }
                if (frutas[filaG][colG] == 2) { frutas[filaG][colG] = 0; ghost.activarVelocidad(); }
            }

            if (pacman.getBounds().intersects(ghost.getBounds())) {
                sf::Text resultado;
                resultado.setFont(font);
                resultado.setCharacterSize(40);
                resultado.setPosition(120, 200);
                
                if (pacman.getPuntos() >= ghost.getPuntos()) {
                    resultado.setString("¡Pacman gana!");
                    resultado.setFillColor(sf::Color::Yellow);
                } else {
                    resultado.setString("¡Ghost gana!");
                    resultado.setFillColor(sf::Color::Red);
                }
                window.clear();
                window.draw(fondo);
                window.draw(resultado);
                window.display();
                sf::sleep(sf::seconds(2));
                window.close();
                break;
            }

            moveTimer = 0;
        }

        window.clear();
        window.draw(fondo);

        for (int i = 0; i < filas; ++i)
            for (int j = 0; j < columnas; ++j)
                if (puntos[i][j] == 1) {
                    sf::CircleShape punto(3);
                    punto.setFillColor(sf::Color::Yellow);
                    punto.setPosition(j * tileSize + 14, i * tileSize + 14);
                    window.draw(punto);
                }

        for (int i = 0; i < filas; ++i)
            for (int j = 0; j < columnas; ++j) {
                if (frutas[i][j] == 1) {
                    sf::CircleShape fruta(6);
                    fruta.setFillColor(sf::Color::Red);
                    fruta.setPosition(j * tileSize + 13, i * tileSize + 13);
                    window.draw(fruta);
                } else if (frutas[i][j] == 2) {
                    sf::CircleShape fruta(6);
                    fruta.setFillColor(sf::Color::Green);
                    fruta.setPosition(j * tileSize + 13, i * tileSize + 13);
                    window.draw(fruta);
                }
            }

        textoPacman.setString("Pacman: " + std::to_string(pacman.getPuntos()) + " pts | Vidas: " + std::to_string(pacman.getVidas()));
        textoGhost.setString("Ghost: " + std::to_string(ghost.getPuntos()) + " pts | Vidas: " + std::to_string(ghost.getVidas()));
        window.draw(textoPacman);
        window.draw(textoGhost);

        pacman.draw(window);
        ghost.draw(window);

        window.display();
    }
}