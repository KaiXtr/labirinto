#include <SDL2/SDL.h>
#include <iostream>
#include <vector>
#include <ctime>
#include <unistd.h>

#include "jogador.cpp"

using namespace std;

class Janela {
private:
    SDL_Window* window = NULL;
	SDL_Renderer* renderer = NULL;
    bool isJanelaAtiva = false; 
    vector<vector<vector<int>>> labirintoMatrix;
    unsigned int labirintoLargura = 0;
    unsigned int labirintoAltura = 0;
    unsigned int curPosX = 0;
    unsigned int curPosY = 0;

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

    void gerarLabirintoVazio(vector<vector<vector<int>>>* matrix, unsigned int largura, unsigned int altura) {
        vector<vector<vector<int>>> m = *matrix;

        for (int i=0; i < altura; i++) {
            vector<vector<int>> linha = {};

            for (int j=0; j < largura; j++) {
                vector<int> dirs = {0,0,0,0};
                linha.push_back(dirs);
            }
            m.push_back(linha);
        }

        *matrix = m;
    }

    void visitarBloco(vector<vector<vector<int>>>* matrix, int x, int y, unsigned int dir, unsigned int largura, unsigned int altura) {
        /*
        * 0 - Não existe
        * 1 - Não visitado
        * 2 - Visitado
        */

        int steps = 1;

        limpar();
        desenharLabirinto();
        atualizar();
        usleep(100 * 10000);

        curPosX = x;
        curPosY = y;
        
        char directions[] = {'R','D','L','U'};
        cout << "= [" << x << ", " << y << "]: " << directions[dir] << endl;
        vector<vector<vector<int>>> m = *matrix;

        if (dir == 0) {
            m[x][y][2] = 1;
        }
        if (dir == 1) {
            m[x][y][3] = 1;
        }
        if (dir == 2) {
            m[x][y][0] = 1;
        }
        if (dir == 3) {
            m[x][y][1] = 1;
        }
        
        *matrix = m;

        unsigned int vizinhos[4] = {0,0,0,0};

        if (x + steps < largura) {
            if (m[x + steps][y][2] == 0) {
                vizinhos[0] = 1;
            }
        }
        if (y + steps < altura) {
            if (m[x][y + steps][3] == 0) {
                vizinhos[1] = 1;
            }
        }
        if (x - steps >= 0) {
            if (m[x - steps][y][0] == 0) {
                vizinhos[2] = 1;
            }
        }
        if (y - steps >= 0) {
            if (m[x][y - steps][1] == 0) {
                vizinhos[3] = 1;
            }
        }

        unsigned int vizinhosVisitados = 0;

        while (vizinhosVisitados < 4) {
            int d = rand() % 4;

            if ((d == 0)&&(dir != 2)&&(vizinhos[d] == 1)) {
                m[x][y][d] = 1;
                vizinhos[d] = 2;
                visitarBloco(matrix, x + steps, y, d, largura, altura);
            }
            if ((d == 1)&&(dir != 3)&&(vizinhos[d] == 1)) {
                m[x][y][d] = 1;
                vizinhos[d] = 2;
                visitarBloco(matrix, x, y + steps, d, largura, altura);
            }
            if ((d == 2)&&(dir != 0)&&(vizinhos[d] == 1)) {
                m[x][y][d] = 1;
                vizinhos[d] = 2;
                visitarBloco(matrix, x - steps, y, d, largura, altura);
            }
            if ((d == 3)&&(dir != 1)&&(vizinhos[d] == 1)) {
                m[x][y][d] = 1;
                vizinhos[d] = 2;
                visitarBloco(matrix, x, y - steps, d, largura, altura);
            }

            vizinhosVisitados++;
        }
    }

    void gerarLabirinto(unsigned int largura, unsigned int altura){
        labirintoLargura = largura;
        labirintoAltura = altura;
        gerarLabirintoVazio(&labirintoMatrix, largura, altura);

        unsigned int x = SDL_round(largura/2);
        unsigned int y = SDL_round(altura/2);

        visitarBloco(&labirintoMatrix, x, y, 4, largura, altura);
    }

    void desenharLabirinto(){
        /*SDL_Surface* tela = SDL_GetWindowSurface(window);
        Uint32 corFundo = SDL_MapRGB(tela -> format, 0x00, 0x00, 0x00);
        SDL_FillRect(tela, NULL, corFundo);*/
        int iniX = 50;
        int iniY = 50;
        int qLado = 10;
        int qDist = qLado + 2;

        for (int i=0; i < labirintoAltura; i++) {
            for (int j=0; j < labirintoLargura; j++) {
                if ((curPosX == j)&&(curPosY == i)) {
                    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);  
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

                SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
                
                if (labirintoMatrix[j][i][0] == 0){
                    SDL_RenderDrawLine(
                        renderer,
                        iniX + (j * qDist) + qLado,
                        iniY + (i * qDist),
                        iniX + (j * qDist) + qLado,
                        iniY + (i * qDist) + qLado
                    );
                }
                if (labirintoMatrix[j][i][1] == 0){
                    SDL_RenderDrawLine(
                        renderer,
                        iniX + (j * qDist),
                        iniY + (i * qDist) + qLado,
                        iniX + (j * qDist) + qLado,
                        iniY + (i * qDist) + qLado
                    );
                }
                if (labirintoMatrix[j][i][2] == 0){
                    SDL_RenderDrawLine(
                        renderer,
                        iniX + (j * qDist),
                        iniY + (i * qDist),
                        iniX + (j * qDist),
                        iniY + (i * qDist) + qLado
                    );
                }
                if (labirintoMatrix[j][i][3] == 0){
                    SDL_RenderDrawLine(
                        renderer,
                        iniX + (j * qDist),
                        iniY + (i * qDist),
                        iniX + (j * qDist) + qLado,
                        iniY + (i * qDist)
                    );
                }
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
