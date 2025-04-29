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
                vector<int> dirs = {0,0,0,0,0};
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

        // MOSTRANDO DIREÇÃO ATUAL EM QUE O ESCAVADOR NAVEGOU
        char directions[] = {'R','D','L','U','?'};
        //cout << "= [" << x << ", " << y << "]: " << directions[dir] << endl;

        // O BLOCO ATUAL FOI VISITADO
        vector<vector<vector<int>>> m = *matrix;
        m[x][y][4] = 1;

        // REMOVENDO PAREDE ENTRE O BLOCO ANTERIOR E O BLOCO ATUAL
        if (dir == 0) {
            m[x][y][2] = 1; // VEIO DA DIREITA, APAGA A ESQUERDA
            m[x - steps][y][0] = 1;
        }
        if (dir == 1) {
            m[x][y][3] = 1; // VEIO DE BAIXO, APAGA ACIMA
            m[x][y - steps][1] = 1;
        }
        if (dir == 2) {
            m[x][y][0] = 1; // VEIO DA ESQUERDA, APAGA A DIREITA
            m[x + steps][y][2] = 1;
        }
        if (dir == 3) {
            m[x][y][1] = 1; // VEIO DE CIMA, APAGA ABAIXO
            m[x][y + steps][3] = 1;
        }
        
        // ATUALIZANDO DADOS DO PONTEIRO
        *matrix = m;

        // EXIBINDO AS MUDANÇAS EM TEMPO REAL COM UM TEMPO DE DELAY
        atualizarAlgoritmo(x, y);

        // VERIFICANDO VIZINHOS POSSÍVEIS
        unsigned int v[4];
        unsigned int* vizinhos = vizinhosBloco(v, m, x, y, dir, largura, altura);
        unsigned int vizinhosAVisitar = checkVizinhos(vizinhos);

        // O ESCAVADOR VAI VISITAR TODOS OS VIZINHOS POSSÍVEIS
        while (vizinhosAVisitar > 0) {
            int d = rand() % 4;

            cout << "Direção: " << directions[d] << endl;;
            atualizarAlgoritmo(x,y);

            // VISITANDO A DIREITA
            if (d == 0) {
                if ((dir != 2)&&(vizinhos[d] == 1)) {
                    vizinhos[d] = 2;
                    visitarBloco(matrix, x + steps, y, d, largura, altura);
                    vizinhos = vizinhosBloco(vizinhos, *matrix, x, y, dir, largura, altura);
                    vizinhosAVisitar = checkVizinhos(vizinhos);
                } else cout << "Inacessível, ";
            }
            // VISITANDO PARA BAIXO
            if (d == 1) {
                if ((dir != 3)&&(vizinhos[d] == 1)) {
                    vizinhos[d] = 2;
                    visitarBloco(matrix, x, y + steps, d, largura, altura);
                    vizinhos = vizinhosBloco(vizinhos, *matrix, x, y, dir, largura, altura);
                    vizinhosAVisitar = checkVizinhos(vizinhos);
                } else cout << "Inacessível, ";
            }
            // VISITANDO A ESQUERDA
            if (d == 2) {
                if ((dir != 0)&&(vizinhos[d] == 1)) {
                    vizinhos[d] = 2;
                    visitarBloco(matrix, x - steps, y, d, largura, altura);
                    vizinhos = vizinhosBloco(vizinhos, *matrix, x, y, dir, largura, altura);
                    vizinhosAVisitar = checkVizinhos(vizinhos);
                } else cout << "Inacessível, ";
            }
            // VISITANDO PARA CIMA
            if (d == 3) {
                if ((dir != 1)&&(vizinhos[d] == 1)) {
                    vizinhos[d] = 2;
                    visitarBloco(matrix, x, y - steps, d, largura, altura);
                    vizinhos = vizinhosBloco(vizinhos, *matrix, x, y, dir, largura, altura);
                    vizinhosAVisitar = checkVizinhos(vizinhos);
                } else cout << "Inacessível, ";
            }
        }

        if (vizinhosAVisitar == 0)
            cout << "Todos os vizinhos visitados." << endl;
        
        atualizarAlgoritmo(x,y);
    }

    void gerarLabirinto(unsigned int largura, unsigned int altura){
        // DEFININDO DIMENSÕES DO LABIRINTO
        labirintoLargura = largura;
        labirintoAltura = altura;

        // GERANDO UMA MATRIZ VAZIA DE CÉLULAS COM 4 PAREDES
        gerarLabirintoVazio(&labirintoMatrix, largura, altura);

        // POSIÇÃO INICIAL DO "ESCAVADOR"
        unsigned int x = SDL_round(largura/2);
        unsigned int y = SDL_round(altura/2);

        cout << "Pressione qualquer tecla para gerar o labirinto..." << endl;
        atualizarAlgoritmo(x, y, 0, true);

        // COMEÇANDO A CAVAR A PARTIR DO CENTRO DO LABIRINTO
        visitarBloco(&labirintoMatrix, x, y, 4, largura, altura);
    }

    unsigned int* vizinhosBloco(unsigned int v[], vector<vector<vector<int>>> m, int x, int y, int d, int largura, int altura, int steps=1) {
        // DETERMINANDO OS VIZINHOS POSSÍVEIS (QUAIS BLOCOS ELE PODE VISITAR), SE:
        // 1. ESTIVER DENTRO DOS LIMITES DAS DIMENSÕES DE LARGURA E ALTURA
        // 2. A PAREDE AINDA NÃO FOI CAVUCADA
        // 3. O BLOCO NÃO FOI VISITADO

        v[0] = 0;
        v[1] = 0;
        v[2] = 0;
        v[3] = 0;

        // CAVAR PARA A DIREITA
        if (x + steps < largura) {
            if ((m[x + steps][y][2] == 0)&&(m[x + steps][y][4] == 0)) {
                v[0] = 1;
            }
        }
        // CAVAR PARA BAIXO
        if (y + steps < altura) {
            if ((m[x][y + steps][3] == 0)&&(m[x][y + steps][4] == 0)) {
                v[1] = 1;
            }
        }
        // CAVAR PARA A ESQUERDA
        if (x - steps >= 0) {
            if ((m[x - steps][y][0] == 0)&&(m[x - steps][y][4] == 0)) {
                v[2] = 1;
            }
        }
        // CAVAR PARA CIMA
        if (y - steps >= 0) {
            if ((m[x][y - steps][1] == 0)&&(m[x][y - steps][4] == 0)) {
                v[3] = 1;
            }
        }
        
        char directions[] = {'R','D','L','U','?'};
        cout << directions[d] << "= [" << v[0] << ", " << v[1] << ", " << v[2] << ", " << v[3] << "]" << endl;;
        atualizarAlgoritmo(x, y);

        return v;
    }

    unsigned int checkVizinhos(unsigned int v[]) {
        unsigned int vizinhosAVisitar = 0;

        if (v[0] == 1)
            vizinhosAVisitar++;
        if (v[1] == 1)
            vizinhosAVisitar++;
        if (v[2] == 1)
            vizinhosAVisitar++;
        if (v[3] == 1)
            vizinhosAVisitar++;

        return vizinhosAVisitar;
    }

    void atualizarAlgoritmo(int x, int y, int s=10, bool pausar=false) {
        // ATUALIZANDO POSIÇÃO ATUAL DO ESCAVADOR APENAS PARA VISUALIZAÇÃO
        curPosX = x;
        curPosY = y;

		/*SDL_Event evento;
        while (pause(&evento)) {
		    while (SDL_PollEvent(&evento)){
                limpar();
                desenharLabirinto();
                atualizar();
            }
        }*/

        limpar();
        desenharLabirinto();
        atualizar();

        if (pausar)
            getchar();
        else
            usleep(s * 1000);
    }

    void desenharLabirinto(){
        /*SDL_Surface* tela = SDL_GetWindowSurface(window);
        Uint32 corFundo = SDL_MapRGB(tela -> format, 0x00, 0x00, 0x00);
        SDL_FillRect(tela, NULL, corFundo);*/
        int iniX = 50;
        int iniY = 50;
        int qLado = 10;
        int qDist = qLado + 1;

        for (int i=0; i < labirintoAltura; i++) {
            for (int j=0; j < labirintoLargura; j++) {
                if ((curPosX == j)&&(curPosY == i)) {
                    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);  
                } else if (labirintoMatrix[j][i][4] == 1) {
                    SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
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
                
                // PAREDE DIREITA
                if (labirintoMatrix[j][i][0] == 0)
                    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
                else
                    SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
                SDL_RenderDrawLine(
                    renderer,
                    iniX + (j * qDist) + qLado,
                    iniY + (i * qDist),
                    iniX + (j * qDist) + qLado,
                    iniY + (i * qDist) + qLado
                );
                // PAREDE DE BAIXO
                if (labirintoMatrix[j][i][1] == 0)
                    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
                else
                    SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
                SDL_RenderDrawLine(
                    renderer,
                    iniX + (j * qDist),
                    iniY + (i * qDist) + qLado,
                    iniX + (j * qDist) + qLado,
                    iniY + (i * qDist) + qLado
                );
                // PAREDE ESQUERDA
                if (labirintoMatrix[j][i][2] == 0)
                    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
                else
                    SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
                SDL_RenderDrawLine(
                    renderer,
                    iniX + (j * qDist),
                    iniY + (i * qDist),
                    iniX + (j * qDist),
                    iniY + (i * qDist) + qLado
                );
                // PAREDE ACIMA
                if (labirintoMatrix[j][i][3] == 0)
                    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
                else
                    SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
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

    void eventos(SDL_Event evt) {
        if (evt.type == SDL_QUIT) {
            isJanelaAtiva = false;
            fechar();
        }
    }

    bool pause(SDL_Event* evt) {
        eventos(*evt);

        if (evt->type == SDL_KEYDOWN)
            return false;
        else
            return true;
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
