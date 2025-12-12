// === src/Juego.cpp ===
#include "../include/ghost.h" 
#include "../include/pacman.h"
#include "../include/Juego.h"
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <iostream>
#include <string>
#include <cmath> 

// Funciones de ayuda
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
    int x = static_cast<int>(std::round(pos.x));
    int y = static_cast<int>(std::round(pos.y));
    bool centrado = (x % tileSize == 0 && y % tileSize == 0);

    if (centrado) {
        sprite.setPosition(x, y);
        if (puedeMover(x, y, buffer.dx, buffer.dy, tileSize, mapa)) actual = buffer;
        if (!puedeMover(x, y, actual.dx, actual.dy, tileSize, mapa)) actual = {0, 0};
    }

    if (actual.dx != 0 || actual.dy != 0) {
        float distParaLlegar = 0;
        if (actual.dx > 0) distParaLlegar = tileSize - (x % tileSize);
        else if (actual.dx < 0) distParaLlegar = (x % tileSize == 0 ? tileSize : x % tileSize);
        else if (actual.dy > 0) distParaLlegar = tileSize - (y % tileSize);
        else if (actual.dy < 0) distParaLlegar = (y % tileSize == 0 ? tileSize : y % tileSize);

        if (velocidad > distParaLlegar) {
            sprite.move(actual.dx * distParaLlegar, actual.dy * distParaLlegar);
            float sobrante = velocidad - distParaLlegar;
            sf::Vector2f nuevaPos = sprite.getPosition();
            if (puedeMover(nuevaPos.x, nuevaPos.y, buffer.dx, buffer.dy, tileSize, mapa)) actual = buffer;
            if (!puedeMover(nuevaPos.x, nuevaPos.y, actual.dx, actual.dy, tileSize, mapa)) actual = {0, 0};
            else sprite.move(actual.dx * sobrante, actual.dy * sobrante);
        } else {
            sprite.move(actual.dx * velocidad, actual.dy * velocidad);
        }
    }
}

float obtenerDistancia(sf::Vector2f p1, sf::Vector2f p2) {
    return std::sqrt(std::pow(p2.x - p1.x, 2) + std::pow(p2.y - p1.y, 2));
}

void GameManager::run() {
    const int filas = 21, columnas = 21, tileSize = 32;
    sf::RenderWindow window(sf::VideoMode(columnas * tileSize, filas * tileSize), "PACMAN");
    window.setFramerateLimit(60);

    // --- CARGA DE IMÁGENES ---
    sf::Texture inicioTexture, fondoTexture, winPacmanTex, winGhostTex;
    
    if (!inicioTexture.loadFromFile("assets/images/inicio.png")) std::cerr << "Error inicio" << std::endl;
    if (!fondoTexture.loadFromFile("assets/images/background.png")) std::cerr << "Error bg" << std::endl;
    
    // Cargar imágenes de victoria
    if (!winPacmanTex.loadFromFile("assets/images/win_pacman.png")) std::cerr << "Error win_pacman" << std::endl;
    if (!winGhostTex.loadFromFile("assets/images/win_ghost.png")) std::cerr << "Error win_ghost" << std::endl;

    // Configurar Sprites
    sf::Sprite inicioSprite(inicioTexture), fondo(fondoTexture);
    sf::Sprite spriteWinPacman(winPacmanTex), spriteWinGhost(winGhostTex);

    // Escalar imágenes al tamaño de la ventana
    float scaleX = (float)(columnas * tileSize);
    float scaleY = (float)(filas * tileSize);
    
    inicioSprite.setScale(scaleX / inicioTexture.getSize().x, scaleY / inicioTexture.getSize().y);
    fondo.setScale(scaleX / fondoTexture.getSize().x, scaleY / fondoTexture.getSize().y);
    fondo.setColor(sf::Color(100, 100, 100)); 
    
    spriteWinPacman.setScale(scaleX / winPacmanTex.getSize().x, scaleY / winPacmanTex.getSize().y);
    spriteWinGhost.setScale(scaleX / winGhostTex.getSize().x, scaleY / winGhostTex.getSize().y);

    // --- SONIDOS ---
    sf::SoundBuffer inicioBuffer, eatBuffer;
    inicioBuffer.loadFromFile("assets/music/inicio.ogg");
    eatBuffer.loadFromFile("assets/music/eating.ogg");
    sf::Sound inicioSound(inicioBuffer), sonidoEat(eatBuffer);

    // --- TEXTOS ---
    sf::Font font;
    font.loadFromFile("assets/fonts/Minecraft.ttf");
    
    sf::Text textoPacman, textoGhost, textoHabilidadP, textoHabilidadG, textoApagon;
    textoPacman.setFont(font); textoGhost.setFont(font); 
    textoHabilidadP.setFont(font); textoHabilidadG.setFont(font); textoApagon.setFont(font);

    textoPacman.setCharacterSize(18); textoGhost.setCharacterSize(18);
    textoHabilidadP.setCharacterSize(15); textoHabilidadG.setCharacterSize(15);
    textoApagon.setCharacterSize(40); textoApagon.setFillColor(sf::Color::Red);

    textoPacman.setFillColor(sf::Color::White); textoGhost.setFillColor(sf::Color::Red);
    textoHabilidadP.setFillColor(sf::Color::Cyan); textoHabilidadG.setFillColor(sf::Color::Magenta);
    
    textoPacman.setPosition(10, 5); textoGhost.setPosition(420, 5);
    textoHabilidadP.setPosition(10, 650); textoHabilidadG.setPosition(420, 650);

    // Muros
    sf::RectangleShape wallShape(sf::Vector2f(tileSize, tileSize));
    wallShape.setFillColor(sf::Color(80, 0, 120));      
    wallShape.setOutlineThickness(-2.0f);                
    wallShape.setOutlineColor(sf::Color(179, 0, 255));   
    sf::RectangleShape wallInner(sf::Vector2f(tileSize - 12, tileSize - 12));
    wallInner.setFillColor(sf::Color(200, 50, 255, 100)); 
    wallInner.setOrigin((tileSize - 12)/2, (tileSize - 12)/2);

    // Niebla
    sf::RectangleShape nieblaShape(sf::Vector2f(tileSize, tileSize));
    nieblaShape.setFillColor(sf::Color::Black);

    // Pantalla Inicio
    inicioSound.setLoop(true); inicioSound.play();
    bool juegoIniciado = false;
    while (window.isOpen() && !juegoIniciado) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) window.close();
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Space) juegoIniciado = true;
        }
        window.clear(); window.draw(inicioSprite); window.display();
    }
    inicioSound.stop();

    // matrz MAPA
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
    int puntosRestantes = 0;
    for (int i = 0; i < filas; ++i)
        for (int j = 0; j < columnas; ++j)
            if (mapa[i][j] == 0) { puntos[i][j] = 1; puntosRestantes++; }
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

    sf::Clock cicloNiebla;
    bool hayNiebla = true; 
    float tiempoLuz = 10.0f;
    float tiempoOscuridad = 5.0f;

    sf::Clock cooldownDashP;
    bool dashActivo = false;
    sf::Clock duracionDashP;

    sf::Clock cooldownInvisG;
    bool invisibleActivo = false;
    sf::Clock duracionInvisG;

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) window.close();
            
            if (event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::Space) {
                    if (cooldownDashP.getElapsedTime().asSeconds() > 5.0f) {
                        dashActivo = true; duracionDashP.restart(); cooldownDashP.restart();
                    }
                }
                if (event.key.code == sf::Keyboard::Enter) {
                    if (cooldownInvisG.getElapsedTime().asSeconds() > 10.0f) {
                        invisibleActivo = true; duracionInvisG.restart(); cooldownInvisG.restart();
                        ghost.getSprite().setColor(sf::Color(255, 255, 255, 50));
                    }
                }
            }
        }

        float deltaTime = clock.restart().asSeconds();
        moveTimer += deltaTime;
        pacman.updateAnimation(); 

        float tiempoCiclo = cicloNiebla.getElapsedTime().asSeconds();
        if (tiempoCiclo > tiempoLuz && tiempoCiclo < (tiempoLuz + tiempoOscuridad)) hayNiebla = true;
        else if (tiempoCiclo >= (tiempoLuz + tiempoOscuridad)) { cicloNiebla.restart(); hayNiebla = false; }
        else hayNiebla = false;

        float velocidadP = pacman.getVelocidad();
        if (dashActivo) {
            if (duracionDashP.getElapsedTime().asSeconds() < 2.0f) velocidadP *= 2.0f;
            else dashActivo = false;
        }
        if (invisibleActivo) {
            if (duracionInvisG.getElapsedTime().asSeconds() > 3.0f) {
                invisibleActivo = false; ghost.getSprite().setColor(sf::Color(255, 255, 255, 255));
            }
        }

        if (moveTimer > moveDelay) {
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))  bufferP = {-1, 0};
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) bufferP = {1, 0};
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))    bufferP = {0, -1};
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))  bufferP = {0, 1};

            if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) bufferG = {-1, 0};
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) bufferG = {1, 0};
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) bufferG = {0, -1};
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) bufferG = {0, 1};

            moverPorBuffer(pacman.getSprite(), dirP, bufferP, tileSize, mapa, velocidadP);
            moverPorBuffer(ghost.getSprite(), dirG, bufferG, tileSize, mapa, ghost.getVelocidad());
            pacman.actualizarVelocidad(); ghost.actualizarVelocidad();

            sf::Vector2f posP = pacman.getPosition();
            int filaP = posP.y / tileSize; int colP = posP.x / tileSize;
            if (filaP >= 0 && filaP < filas && colP >= 0 && colP < columnas) {
                if (puntos[filaP][colP] == 1) { puntos[filaP][colP] = 0; pacman.sumarPunto(); sonidoEat.play(); puntosRestantes--; }
                if (frutas[filaP][colP] == 1) { frutas[filaP][colP] = 0; pacman.agregarVida(); }
                if (frutas[filaP][colP] == 2) { frutas[filaP][colP] = 0; pacman.activarVelocidad(); }
            }
            sf::Vector2f posG = ghost.getPosition();
            int filaG = posG.y / tileSize; int colG = posG.x / tileSize;
            if (filaG >= 0 && filaG < filas && colG >= 0 && colG < columnas) {
                if (puntos[filaG][colG] == 1) { puntos[filaG][colG] = 0; ghost.sumarPunto(); sonidoEat.play(); puntosRestantes--; }
            }

            // VICTORIA
            if (puntosRestantes <= 0) {
                 window.clear();
                 
                 if (pacman.getPuntos() > ghost.getPuntos()) {
                     window.draw(spriteWinPacman);
                 } else {
                     window.draw(spriteWinGhost);
                 }
                 window.display();
                 sf::sleep(sf::seconds(5));
                 window.close();
                 break;
            }

            // --- VICTORIA POR MUERTE 
            if (pacman.getBounds().intersects(ghost.getBounds())) {
                pacman.restarVida();
                sf::Text aviso; aviso.setFont(font); aviso.setCharacterSize(50);
                
                if (pacman.getVidas() <= 0) {
                    
                    window.clear();
                    window.draw(spriteWinGhost);
                    window.display();
                    sf::sleep(sf::seconds(5));
                    window.close();
                    break;
                } else {
                    aviso.setString("ATRAPADO!"); aviso.setFillColor(sf::Color::Yellow); aviso.setPosition(220, 300);
                    window.clear(); window.draw(fondo); window.draw(aviso); window.display();
                    sf::sleep(sf::seconds(1.5f));
                    pacman.resetPosition(); ghost.resetPosition();
                    dirP = {0,0}; bufferP = {0,0}; dirG = {0,0}; bufferG = {0,0};
                }
            }
            moveTimer = 0;
        }

        window.clear();
        window.draw(fondo); 

        for (int i = 0; i < filas; ++i) {
            for (int j = 0; j < columnas; ++j) {
                bool visible = true;
                if (hayNiebla) {
                    float distP = obtenerDistancia(sf::Vector2f(j*tileSize, i*tileSize), pacman.getPosition());
                    float distG = obtenerDistancia(sf::Vector2f(j*tileSize, i*tileSize), ghost.getPosition());
                    if (distP > 30 && distG > 30) visible = false;
                }

                if (visible) {
                    if (mapa[i][j] == 1) {
                        wallShape.setPosition(j * tileSize, i * tileSize); window.draw(wallShape);
                        wallInner.setPosition(j * tileSize + 6, i * tileSize + 6); window.draw(wallInner);
                    } else {
                        if (puntos[i][j] == 1) {
                            sf::CircleShape punto(3); punto.setFillColor(sf::Color(255, 184, 151)); 
                            punto.setPosition(j * tileSize + 14, i * tileSize + 14); window.draw(punto);
                        }
                        if (frutas[i][j] > 0) {
                            sf::CircleShape fruta(6);
                            fruta.setFillColor(frutas[i][j] == 1 ? sf::Color::Red : sf::Color::Green);
                            fruta.setPosition(j * tileSize + 13, i * tileSize + 13); window.draw(fruta);
                        }
                    }
                } else {
                    nieblaShape.setPosition(j * tileSize, i * tileSize);
                    window.draw(nieblaShape);
                }
            }
        }

        textoPacman.setString("Pacman: " + std::to_string(pacman.getPuntos()) + " | Vidas: " + std::to_string(pacman.getVidas()));
        textoGhost.setString("Ghost: " + std::to_string(ghost.getPuntos()) + " | Vidas: " + std::to_string(ghost.getVidas()));
        window.draw(textoPacman); window.draw(textoGhost);

        std::string dashStatus = (cooldownDashP.getElapsedTime().asSeconds() > 5.0f) ? "[SPACE] DASH LISTO" : "DASH: " + std::to_string(5 - (int)cooldownDashP.getElapsedTime().asSeconds());
        std::string invisStatus = (cooldownInvisG.getElapsedTime().asSeconds() > 10.0f) ? "[ENTER] INVIS LISTO" : "INVIS: " + std::to_string(10 - (int)cooldownInvisG.getElapsedTime().asSeconds());
        textoHabilidadP.setString(dashStatus);
        textoHabilidadG.setString(invisStatus);
        window.draw(textoHabilidadP); window.draw(textoHabilidadG);

        if (hayNiebla) {
            textoApagon.setString("! APAGON !");
            textoApagon.setPosition(220, 640);
            window.draw(textoApagon);
        }

        pacman.draw(window);
        ghost.draw(window);

        window.display();
    }
}