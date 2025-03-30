#include <SDL2/SDL.h>
#include <iostream>

#include "controles.cpp"

class Jogador {
private:
    int posX = 0;
    int posY = 0;
public:
    void eventos(SDL_Event evt){
        if (evt.type == SDL_KEYDOWN) {
            if (evt.key.keysym.sym == Controles.cima) {
                mover(0,-1);
            }
            if (evt.key.keysym.sym == Controles.baixo) {
                mover(0,1);
            }
            if (evt.key.keysym.sym == Controles.esquerda) {
                mover(-1,0);
            }
            if (evt.key.keysym.sym == Controles.direita) {
                mover(1,0);
            }
        }
    }

    void mover(int x, int y){
        posX += x;
        posY += y;
    }

    int getX(){
        return posX;
    }

    int getY(){
        return posY;
    }

    void desenhar(SDL_Renderer* renderer) {
		SDL_SetRenderDrawColor(renderer, 200, 0, 0, 255);
        
        int spac = 2;
        int qLado = 10 - spac;
        int qDist = 10;

        SDL_Rect quadrado = {
            (posX * qDist),
            (posY * qDist),
            qLado,
            qLado
        };
        SDL_Rect quadradoFill = {
            quadrado.x + 1, quadrado.y + 1,
            quadrado.w - 2, quadrado.h - 2
        };

        SDL_RenderDrawRect(renderer, &quadrado);
        SDL_RenderFillRect(renderer, &quadradoFill);
    }
};
