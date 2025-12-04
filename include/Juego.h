// === juego.h ===
#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

#ifndef JUEGO_H
#define JUEGO_H

#include <SFML/Graphics.hpp>

// MANTÉN esto si lo usas para el movimiento
struct Direccion {
    int dx = 0, dy = 0;
};

// Si tienes el mapa aquí, déjalo aquí.
// PERO NO escribas "class Pacman" ni "class Ghost" aquí.

#endif // JUEGO_H
class GameManager {
public:
    void run();
};