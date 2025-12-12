// === src/Juego.cpp ===
#include "../include/ghost.h" 
#include "../include/pacman.h"
#include "../include/Juego.h"
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <iostream>
#include <string> // Necesario para std::to_string

// Funciones de ayuda (Mapa 21x21)
bool puedeMover(float x, float y, int dx, int dy, int tileSize, const int mapa[21][21]) {
    int fila = (y + dy * tileSize) / tileSize;
    int col = (x + dx * tileSize) / tileSize;
    return fila >= 0 && fila < 21 && col >= 0 && col < 21 && mapa[fila][col] == 0;
}

bool estaCentradoEnTile(sf::Vector2f pos, int tileSize) {
    return static_cast<int>(pos.x) % tileSize == 0 && static_cast<int>(pos.y) % tileSize == 0;
}

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
    const int filas = 21, columnas = 21, tileSize = 32;
    sf::RenderWindow window(sf::VideoMode(columnas * tileSize, filas * tileSize), "PACMAN NEON");
    window.setFramerateLimit(60);

    // --- Assets ---
    sf::Texture inicioTexture;
    if (!inicioTexture.loadFromFile("assets/images/inicio.png")) std::cerr << "Error loading inicio" << std::endl;
    sf::Sprite inicioSprite(inicioTexture);
    inicioSprite.setScale((float)(columnas * tileSize) / inicioTexture.getSize().x, (float)(filas * tileSize) / inicioTexture.getSize().y);

    sf::SoundBuffer inicioBuffer;
    if (!inicioBuffer.loadFromFile("assets/music/inicio.ogg")) std::cerr << "Error loading music" << std::endl;
    sf::Sound inicioSound(inicioBuffer);

    sf::Texture fondoTexture;
    if (!fondoTexture.loadFromFile("assets/images/background.png")) std::cerr << "Error loading bg" << std::endl;
    sf::Sprite fondo(fondoTexture);
    fondo.setScale((float)(columnas * tileSize) / fondoTexture.getSize().x, (float)(filas * tileSize) / fondoTexture.getSize().y);
    fondo.setColor(sf::Color(150, 150, 150)); 

    // --- CONFIGURACIÓN DE PUNTAJES (TEXTOS) ---
    sf::Font font;
    if (!font.loadFromFile("assets/fonts/Minecraft.ttf")) std::cerr << "Error loading font" << std::endl;
    
    sf::Text textoPacman, textoGhost;
    textoPacman.setFont(font); 
    textoGhost.setFont(font);
    
    textoPacman.setCharacterSize(18); // Tamaño legible
    textoGhost.setCharacterSize(18); 
    
    textoPacman.setFillColor(sf::Color::White); // Pacman Blanco
    textoGhost.setFillColor(sf::Color::Red);    // Ghost Rojo
    
    // Posiciones (Sobre el muro superior)
    textoPacman.setPosition(10, 5); 
    textoGhost.setPosition(420, 5); 

    sf::SoundBuffer bufferEat;
    if (!bufferEat.loadFromFile("assets/music/eating.ogg")) std::cerr << "Error loading eat" << std::endl;
    sf::Sound sonidoEat(bufferEat);

    // --- MUROS CON COLOR Y BORDE NEÓN ---
    sf::RectangleShape wallShape(sf::Vector2f(tileSize, tileSize));
    wallShape.setFillColor(sf::Color(100, 0, 150));      // Relleno Morado Oscuro
    wallShape.setOutlineThickness(-2.0f);                // Borde interior
    wallShape.setOutlineColor(sf::Color(179, 0, 255));   // Borde Neón Brillante

    // Brillo central opcional
    sf::RectangleShape wallInner(sf::Vector2f(tileSize - 12, tileSize - 12));
    wallInner.setFillColor(sf::Color(200, 50, 255, 100)); 
    wallInner.setOrigin((tileSize - 12)/2, (tileSize - 12)/2);

    // Loop Inicio
    inicioSound.setLoop(true);
    inicioSound.play();
    bool juegoIniciado = false;
    while (window.isOpen() && !juegoIniciado) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) window.close();
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Space) juegoIniciado = true;
        }
        window.clear();
        window.draw(inicioSprite);
        window.display();
    }
    inicioSound.stop();

    // MAPA
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
    
    // --- NUEVO: CONTADOR DE PUNTOS TOTALES ---
    int puntosRestantes = 0; 

    for (int i = 0; i < filas; ++i)
        for (int j = 0; j < columnas; ++j)
            if (mapa[i][j] == 0) {
                puntos[i][j] = 1;
                puntosRestantes++; // <--- Contamos cada punto creado
            }

    for (int i = 0; i < 5; ++i) {
        int r = rand() % filas; int c = rand() % columnas;
        if (mapa[r][c] == 0) frutas[r][c] = (rand() % 2) + 1;
    }

    Pacman pacman;
    Ghost ghost;
    Direccion dirP, bufferP, dirG, bufferG;
    sf::Clock clock;       
    float moveTimer = 0.0f;
    float moveDelay = 0.05f; 

    // === BUCLE PRINCIPAL 
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) window.close();
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

            moverPorBuffer(pacman.getSprite(), dirP, bufferP, tileSize, mapa, pacman.getVelocidad());
            moverPorBuffer(ghost.getSprite(), dirG, bufferG, tileSize, mapa, ghost.getVelocidad());
            pacman.actualizarVelocidad(); ghost.actualizarVelocidad();

            // COMER PUNTOS (PACMAN)
            sf::Vector2f posP = pacman.getPosition();
            int filaP = posP.y / tileSize; int colP = posP.x / tileSize;
            if (filaP >= 0 && filaP < filas && colP >= 0 && colP < columnas) {
                if (puntos[filaP][colP] == 1) { 
                    puntos[filaP][colP] = 0; 
                    pacman.sumarPunto(); 
                    sonidoEat.play(); 
                    puntosRestantes--; // <--- RESTAMOS PUNTOS
                }
                if (frutas[filaP][colP] == 1) { frutas[filaP][colP] = 0; pacman.agregarVida(); }
                if (frutas[filaP][colP] == 2) { frutas[filaP][colP] = 0; pacman.activarVelocidad(); }
            }

            // COMER PUNTOS (GHOST) - El fantasma también come para ganar
            sf::Vector2f posG = ghost.getPosition();
            int filaG = posG.y / tileSize; int colG = posG.x / tileSize;
            if (filaG >= 0 && filaG < filas && colG >= 0 && colG < columnas) {
                if (puntos[filaG][colG] == 1) { 
                    puntos[filaG][colG] = 0; 
                    ghost.sumarPunto(); 
                    sonidoEat.play(); 
                    puntosRestantes--; // <--- RESTAMOS PUNTOS
                }
            }

            // --- NUEVO: VERIFICAR SI SE ACABARON LOS PUNTOS (FINAL DEL JUEGO) ---
            if (puntosRestantes <= 0) {
                 sf::Text resultado; 
                 resultado.setFont(font); 
                 resultado.setCharacterSize(50); 
                 resultado.setPosition(150, 300);

                 // Decidir ganador por puntaje
                 if (pacman.getPuntos() > ghost.getPuntos()) {
                     resultado.setString("PACMAN GANA!"); resultado.setFillColor(sf::Color::Yellow);
                 } else if (ghost.getPuntos() > pacman.getPuntos()) {
                     resultado.setString("GHOST GANA!"); resultado.setFillColor(sf::Color::Red);
                 } else {
                     resultado.setString("EMPATE!"); resultado.setFillColor(sf::Color::White);
                 }
                 
                 window.clear();
                 window.draw(fondo);
                 
                 // Dibujar mapa vacío para que se vea bonito
                 for (int i = 0; i < filas; ++i) {
                     for (int j = 0; j < columnas; ++j) {
                         if (mapa[i][j] == 1) {
                             wallShape.setPosition(j * tileSize, i * tileSize);
                             window.draw(wallShape);
                         }
                     }
                 }
                 
                 window.draw(resultado);
                 
                 // Mostrar puntaje final
                 textoPacman.setPosition(200, 380); textoGhost.setPosition(200, 420);
                 window.draw(textoPacman); window.draw(textoGhost);
                 window.display();
                 
                 sf::sleep(sf::seconds(5));
                 window.close();
                 break;
            }

            // === COLISIÓN: PACMAN VS GHOST ===
            if (pacman.getBounds().intersects(ghost.getBounds())) {
                pacman.restarVida();

                sf::Text aviso; aviso.setFont(font); aviso.setCharacterSize(50);
                
                if (pacman.getVidas() <= 0) {
                    aviso.setString("GHOST GANA"); aviso.setFillColor(sf::Color::Red); aviso.setPosition(200, 300);
                    window.clear(); window.draw(fondo); window.draw(aviso);
                    textoPacman.setPosition(200, 380); textoGhost.setPosition(200, 420);
                    window.draw(textoPacman); window.draw(textoGhost);
                    window.display();
                    sf::sleep(sf::seconds(3)); window.close(); break;
                } else {
                    aviso.setString("ATRAPADO!"); aviso.setFillColor(sf::Color::Yellow); aviso.setPosition(220, 300);
                    window.clear(); window.draw(fondo);
                    // Redibujar mapa para que no se vea negro
                    for (int i = 0; i < filas; ++i) {
                        for (int j = 0; j < columnas; ++j) {
                            if (mapa[i][j] == 1) {
                                wallShape.setPosition(j * tileSize, i * tileSize);
                                window.draw(wallShape);
                                wallInner.setPosition(j * tileSize + 6, i * tileSize + 6);
                                window.draw(wallInner);
                            }
                        }
                    }
                    window.draw(aviso); window.display();
                    sf::sleep(sf::seconds(1.5f));

                    pacman.resetPosition();
                    ghost.resetPosition();
                    dirP = {0,0}; bufferP = {0,0};
                    dirG = {0,0}; bufferG = {0,0};
                }
            }
            moveTimer = 0;
        }

        
        window.clear();
        window.draw(fondo); 

        for (int i = 0; i < filas; ++i) {
            for (int j = 0; j < columnas; ++j) {
                if (mapa[i][j] == 1) {
                    wallShape.setPosition(j * tileSize, i * tileSize);
                    window.draw(wallShape);
                    // Brillo central
                    wallInner.setPosition(j * tileSize + 6, i * tileSize + 6);
                    window.draw(wallInner);
                } else {
                    if (puntos[i][j] == 1) {
                        sf::CircleShape punto(3); punto.setFillColor(sf::Color(255, 184, 151)); 
                        punto.setPosition(j * tileSize + 14, i * tileSize + 14);
                        window.draw(punto);
                    }
                    if (frutas[i][j] > 0) {
                        sf::CircleShape fruta(6);
                        fruta.setFillColor(frutas[i][j] == 1 ? sf::Color::Red : sf::Color::Green);
                        fruta.setPosition(j * tileSize + 13, i * tileSize + 13);
                        window.draw(fruta);
                    }
                }
            }
        }

        //PUNTAJES (Si se quere modfcar que sea desde el header .h)
        textoPacman.setString("Pacman: " + std::to_string(pacman.getPuntos()) + " | Vidas: " + std::to_string(pacman.getVidas()));
        textoGhost.setString("Ghost: " + std::to_string(ghost.getPuntos()) + " | Vidas: " + std::to_string(ghost.getVidas()));
        
        window.draw(textoPacman); 
        window.draw(textoGhost);

        pacman.draw(window);
        ghost.draw(window);

        window.display();
    }
}