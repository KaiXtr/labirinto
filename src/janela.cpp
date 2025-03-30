#include <SDL2/SDL.h>
#include <iostream>
#include <vector>
#include <ctime>
#include <unistd.h>

#include "jogador.cpp"

class Janela {
private:
    SDL_Window* window = NULL;
	SDL_Renderer* renderer = NULL;
    bool isJanelaAtiva = false; 
    std::vector<std::vector<int>> labirintoMatrix;
    unsigned int labirintoLargura = 0;
    unsigned int labirintoAltura = 0;

public:
    Janela(
        const char* janelaTitulo,
        unsigned int posX,
        unsigned int posY,
        unsigned int largura,
        unsigned int altura) {

        srand(time(0));

        std::cout << "Iniciando janela..." << std::endl;

        if (SDL_Init(SDL_INIT_VIDEO) < 0) {
            std::cout <<
            "Houve um problema ao inicializar a biblioteca SDL: " <<
            SDL_GetError() <<
            std::endl;
        }

        window = SDL_CreateWindow(janelaTitulo, posX, posY, largura, altura, SDL_WINDOW_SHOWN);

        if (window == NULL) {
            std::cout <<
            "Houve um problema ao criar a janela: " <<
            SDL_GetError() <<
            std::endl;
        }

		renderer = SDL_CreateRenderer(
            window, -1,
            SDL_RENDERER_ACCELERATED|SDL_RENDERER_TARGETTEXTURE
        );

        if (renderer == NULL) {
            std::cout <<
            "Houve um problema ao criar o renderizador da janela: " <<
            SDL_GetError() <<
            std::endl;
        }

        isJanelaAtiva = true;
    }

    SDL_Window* getWindow(){
        return window;
    }

    bool getAtiva(){
        return isJanelaAtiva;
    }

    SDL_Renderer* getRenderer(){
        return renderer;
    }

    void limpar(Uint8 vermelho=0, Uint8 verde=0, Uint8 azul=0, Uint8 alfa=255){
		SDL_SetRenderDrawColor(renderer, vermelho, verde, azul, alfa);
		SDL_RenderClear(renderer);
	}

    void gerarLabirinto(unsigned int largura, unsigned int altura){
        labirintoLargura = largura;
        labirintoAltura = altura;

        for (int i=0; i < labirintoAltura; i++) {
            std::vector<int> linha = {};

            for (int j=0; j < labirintoLargura; j++) {
                linha.push_back(1);
            }
            labirintoMatrix.push_back(linha);
        }

        unsigned int x = SDL_round(labirintoLargura/2);
        unsigned int y = SDL_round(labirintoAltura/2);
        
        labirintoMatrix[x][y] = 1;
    }

    void desenharLabirinto(){
        /*SDL_Surface* tela = SDL_GetWindowSurface(window);
        Uint32 corFundo = SDL_MapRGB(tela -> format, 0x00, 0x00, 0x00);
        SDL_FillRect(tela, NULL, corFundo);*/
        int iniX = 50;
        int iniY = 50;
        int spac = 2;
        int qLado = 10 - spac;
        int qDist = 10;

        for (int i=0; i < labirintoAltura; i++) {
            for (int j=0; j < labirintoLargura; j++) {
                if (labirintoMatrix[i][j] == 0){
		            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
                } else {
		            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
                }

                SDL_Rect quadrado = {
                    iniX + (j * qDist),
                    iniY + (i * qDist),
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
        }
    }

    void eventos(SDL_Event evt) {
        if (evt.type == SDL_QUIT) {
            isJanelaAtiva = false;
        }
    }

    void atualizar(){
        SDL_UpdateWindowSurface(window);
		SDL_RenderPresent(renderer);
    }

    void fechar() {
        std::cout << "Fechando janela..." << std::endl;
        isJanelaAtiva = false;
		SDL_DestroyWindow(window);
        SDL_Quit();
    }
};
