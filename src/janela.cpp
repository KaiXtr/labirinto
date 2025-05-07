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
    unsigned int labirintoMode = 0;
    unsigned int labirintoPassos = 0;
    unsigned int startPosX = 0;
    unsigned int startPosY = 0;
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

    void gerarLabirinto(unsigned int largura, unsigned int altura){
        // DEFININDO DIMENSÕES DO LABIRINTO
        labirintoLargura = largura;
        labirintoAltura = altura;
        labirintoMode = 0;
        labirintoPassos = 0;
        labirintoMatrix = {};

        // GERANDO UMA MATRIZ VAZIA DE CÉLULAS COM 4 PAREDES
        gerarLabirintoVazio(&labirintoMatrix);

        // POSIÇÃO INICIAL DO "ESCAVADOR"
        startPosX = SDL_round(largura/2);
        startPosY = SDL_round(altura/2);

        cout << "Pressione qualquer tecla para gerar o labirinto..." << endl;
        atualizarAlgoritmo(startPosX, startPosY, 0, true);

        // COMEÇANDO A CAVAR A PARTIR DO CENTRO DO LABIRINTO
        cavarBloco(&labirintoMatrix, startPosX, startPosY, 4);

        if (isJanelaAtiva) {
            // REINICIANDO POSIÇÃO E LIMPANDO BLOCOS VISITADOS
            limparLabirinto();

            cout << endl << "TOTAL DE PASSOS: " << labirintoPassos << endl;
        }
    }

    void limparLabirinto() {
        labirintoMode = 1;
        curPosX = 0;
        curPosY = 0;

        for (int i=0; i < labirintoAltura; i++) {
            for (int j=0; j < labirintoLargura; j++) {
                labirintoMatrix[i][j][4] = 0;
            }
        }

        limpar();
        desenharLabirinto();
        atualizar();
    }

    void gerarLabirintoVazio(vector<vector<vector<int>>>* matrix) {
        vector<vector<vector<int>>> m = *matrix;

        for (int i=0; i < labirintoAltura; i++) {
            vector<vector<int>> linha = {};

            for (int j=0; j < labirintoLargura; j++) {
                vector<int> dirs = {0,0,0,0,0};
                linha.push_back(dirs);
            }
            m.push_back(linha);
        }

        *matrix = m;
    }

    void cavarBloco(vector<vector<vector<int>>>* matrix, int x, int y, unsigned int dir) {
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
        labirintoPassos++;

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
        if (!atualizarAlgoritmo(x, y))
            return;

        // VERIFICANDO VIZINHOS POSSÍVEIS
        unsigned int v[4];
        unsigned int* vizinhos = vizinhosBloco(v, m, x, y, dir);
        vector<int> vizinhosAVisitar = checkVizinhos(vizinhos);

        // O ESCAVADOR VAI VISITAR TODOS OS VIZINHOS POSSÍVEIS
        while (vizinhosAVisitar.size() > 0) {
            int d = vizinhosAVisitar[rand() % vizinhosAVisitar.size()];

            cout << "Direção: " << directions[d] << endl;;
            if (!atualizarAlgoritmo(x,y))
                break;

            // VISITANDO A DIREITA
            if (d == 0) {
                if ((dir != 2)&&(vizinhos[d] == 1)) {
                    vizinhos[d] = 2;
                    cavarBloco(matrix, x + steps, y, d);
                    vizinhos = vizinhosBloco(vizinhos, *matrix, x, y, dir);
                    vizinhosAVisitar = checkVizinhos(vizinhos);
                } else cout << "Inacessível, ";
            }
            // VISITANDO PARA BAIXO
            if (d == 1) {
                if ((dir != 3)&&(vizinhos[d] == 1)) {
                    vizinhos[d] = 2;
                    cavarBloco(matrix, x, y + steps, d);
                    vizinhos = vizinhosBloco(vizinhos, *matrix, x, y, dir);
                    vizinhosAVisitar = checkVizinhos(vizinhos);
                } else cout << "Inacessível, ";
            }
            // VISITANDO A ESQUERDA
            if (d == 2) {
                if ((dir != 0)&&(vizinhos[d] == 1)) {
                    vizinhos[d] = 2;
                    cavarBloco(matrix, x - steps, y, d);
                    vizinhos = vizinhosBloco(vizinhos, *matrix, x, y, dir);
                    vizinhosAVisitar = checkVizinhos(vizinhos);
                } else cout << "Inacessível, ";
            }
            // VISITANDO PARA CIMA
            if (d == 3) {
                if ((dir != 1)&&(vizinhos[d] == 1)) {
                    vizinhos[d] = 2;
                    cavarBloco(matrix, x, y - steps, d);
                    vizinhos = vizinhosBloco(vizinhos, *matrix, x, y, dir);
                    vizinhosAVisitar = checkVizinhos(vizinhos);
                } else cout << "Inacessível, ";
            }
        }

        // TODOS OS VIZINHOS FORAM VISITADOS
        if (vizinhosAVisitar.size() == 0)
            cout << "Todos os vizinhos visitados." << endl;
        
        atualizarAlgoritmo(x,y);
    }

    unsigned int* vizinhosBloco(unsigned int v[], vector<vector<vector<int>>> m, int x, int y, int d, int w=0, int steps=1) {
        // DETERMINANDO OS VIZINHOS POSSÍVEIS (QUAIS BLOCOS ELE PODE VISITAR), SE:
        // 1. ESTIVER DENTRO DOS LIMITES DAS DIMENSÕES DE LARGURA E ALTURA
        // 2. A PAREDE AINDA NÃO FOI CAVUCADA
        // 3. O BLOCO NÃO FOI VISITADO

        v[0] = 0;
        v[1] = 0;
        v[2] = 0;
        v[3] = 0;

        // CAVANDO BLOCOS: w=0, pois os vizinhos são blocos com paredes que podem ser escavadas
        // VISITANDO BLOCOS: w=1, pois os vizinhos são os blocos que foram escavados

        // CAVAR PARA A DIREITA
        if (x + steps < labirintoLargura) {
            if ((m[x + steps][y][2] == w)&&(m[x + steps][y][4] == 0)) {
                v[0] = 1;
            }
        }
        // CAVAR PARA BAIXO
        if (y + steps < labirintoAltura) {
            if ((m[x][y + steps][3] == w)&&(m[x][y + steps][4] == 0)) {
                v[1] = 1;
            }
        }
        // CAVAR PARA A ESQUERDA
        if (x - steps >= 0) {
            if ((m[x - steps][y][0] == w)&&(m[x - steps][y][4] == 0)) {
                v[2] = 1;
            }
        }
        // CAVAR PARA CIMA
        if (y - steps >= 0) {
            if ((m[x][y - steps][1] == w)&&(m[x][y - steps][4] == 0)) {
                v[3] = 1;
            }
        }
        
        char directions[] = {'R','D','L','U','?'};
        cout << directions[d] << "= [" << v[0] << ", " << v[1] << ", " << v[2] << ", " << v[3] << "]" << endl;;
        atualizarAlgoritmo(x, y);

        return v;
    }

    vector<int> checkVizinhos(unsigned int v[]) {
        vector<int> vizinhosAVisitar;

        if (v[0] == 1)
            vizinhosAVisitar.push_back(0);
        if (v[1] == 1)
            vizinhosAVisitar.push_back(1);
        if (v[2] == 1)
            vizinhosAVisitar.push_back(2);
        if (v[3] == 1)
            vizinhosAVisitar.push_back(3);

        return vizinhosAVisitar;
    }

    void buscarLabirinto() {
        // INICIANDO BUSCA NO LABIRINTO
        labirintoPassos = 0;
        cout << "Pressione qualquer tecla para buscar pelo destino do labirinto..." << endl;
        atualizarAlgoritmo(0, 0, 0, true);
        labirintoMode = 2;

        // COMEÇANDO A CAVAR A PARTIR DO CENTRO DO LABIRINTO
        bool encontrado = visitarBloco(&labirintoMatrix, curPosX, curPosY);

        limparLabirinto();
        labirintoMode = 3;
    }

    bool visitarBloco(vector<vector<vector<int>>>* matrix, int x, int y) {
        unsigned int steps = 1;

        // O BLOCO ATUAL FOI VISITADO
        vector<vector<vector<int>>> m = *matrix;
        labirintoPassos++;

        // EXIBINDO AS MUDANÇAS EM TEMPO REAL COM UM TEMPO DE DELAY
        atualizarAlgoritmo(x, y, 0, true);

        // VERIFICANDO VIZINHOS POSSÍVEIS
        unsigned int v[4];
        unsigned int* vizinhos = vizinhosBloco(v, m, x, y, 4, 1);
        vector<int> vizinhosAVisitar = checkVizinhos(vizinhos);
        bool encontrado = NULL;

        // CALCULANDO G + H DE CADA VIZINHO
        unsigned int minF = 99;
        for (int i=0; i<vizinhosAVisitar.size(); i++){
            if (encontrado || !isJanelaAtiva)
                break;

            unsigned int nx;
            unsigned int ny;

            if (vizinhosAVisitar[i] == 0){
                nx = x + steps;
                ny = y;
            }
            if (vizinhosAVisitar[i] == 1){
                nx = x;
                ny = y + steps;
            }
            if (vizinhosAVisitar[i] == 2){
                nx = x - steps;
                ny = y;
            }
            if (vizinhosAVisitar[i] == 3){
                nx = x;
                ny = y - steps;
            }

            if ((nx == startPosX)&&(ny == startPosY)){
                cout << "ACHOU" << endl;
                encontrado = true;
            }

            unsigned int h = manhattanDistance(x, y, nx, ny);
            unsigned int f = 1 + h;
        
            // ATUALIZANDO DADOS DO PONTEIRO
            m[nx][ny][4] = 1;
            *matrix = m;

            if (f < minF)
                minF = f;

            encontrado = visitarBloco(matrix, nx, ny);
        }

        return encontrado;
    }

    unsigned int manhattanDistance(unsigned int x, unsigned int y, unsigned int nx, unsigned int ny) {
        double mx = x - nx;
        double my = y - ny;
        unsigned int h = abs(mx) + abs(my);
        return h;
    }

    bool atualizarAlgoritmo(int x, int y, int s=10, bool p=false) {
        // ATUALIZANDO POSIÇÃO ATUAL DO ESCAVADOR APENAS PARA VISUALIZAÇÃO
        curPosX = x;
        curPosY = y;

        bool sleep = !p;
        if (isJanelaAtiva) {
            bool pausado = p;
            do {
		        SDL_Event evento;
                while (SDL_PollEvent(&evento)) {
                    eventos(evento);
                    if (!sleep)
                        pausado = pausar(evento);
                }
                limpar();
                desenharLabirinto();
                atualizar();
            } while (pausado);

            if (sleep)
                usleep(s * 1000);
            return true;
        } else {
            return false;
        }
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
                } else if ((labirintoMode > 0)&&(startPosX == j)&&(startPosY == i)) {
                    SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);
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
                changeWallColor(labirintoMatrix[j][i][0], labirintoMatrix[j][i][4]);
                SDL_RenderDrawLine(
                    renderer,
                    iniX + (j * qDist) + qLado,
                    iniY + (i * qDist),
                    iniX + (j * qDist) + qLado,
                    iniY + (i * qDist) + qLado
                );
                // PAREDE DE BAIXO
                changeWallColor(labirintoMatrix[j][i][1], labirintoMatrix[j][i][4]);
                SDL_RenderDrawLine(
                    renderer,
                    iniX + (j * qDist),
                    iniY + (i * qDist) + qLado,
                    iniX + (j * qDist) + qLado,
                    iniY + (i * qDist) + qLado
                );
                // PAREDE ESQUERDA
                changeWallColor(labirintoMatrix[j][i][2], labirintoMatrix[j][i][4]);
                SDL_RenderDrawLine(
                    renderer,
                    iniX + (j * qDist),
                    iniY + (i * qDist),
                    iniX + (j * qDist),
                    iniY + (i * qDist) + qLado
                );
                // PAREDE ACIMA
                changeWallColor(labirintoMatrix[j][i][3], labirintoMatrix[j][i][4]);
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

    void changeWallColor(unsigned int w, unsigned int v) {
        if (w == 0)
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        else if (v == 1)
            SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
        else if (labirintoMode > 0)
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    }

    void eventos(SDL_Event evt) {
        if (evt.type == SDL_QUIT) {
            isJanelaAtiva = false;
            fechar();
        }
    }
    
    bool pausar(SDL_Event evt) {
        if (evt.type == SDL_KEYDOWN)
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
