#include <SDL2/SDL.h>
#include <iostream>
#include <vector>
#include <ctime>
#include <unistd.h>

using namespace std;

void gerarLabirintoVazio(vector<vector<int>>* matrix, unsigned int* largura, unsigned int* altura) {
    for (int i=0; i < *altura; i++) {
        std::vector<int> linha = {};

        for (int j=0; j < *largura; j++) {
            linha.push_back(0);
        }
        vector<vector<int>> m = *matrix;
        m.push_back(linha);
    }
}

void visitarBloco(vector<vector<int>>* matrix, unsigned int x, unsigned int y, unsigned int* largura, unsigned int* altura) {
    /*
    * 0 - Não existe
    * 1 - Não visitado
    * 2 - Visitado
    */
    
    vector<vector<int>> m = *matrix;
    m[x][y] = 1;
    unsigned int vizinhos[3][3] = {{0,0,0},{0,2,0},{0,0,0}};

    for (int j=-1; j<2; j++) {
        if (x + j < 0)
            break;
        if (x + j >= *largura - 1)
            break;

        for (int i=-1; i<2; i++) {
            if (y + i < 0)
                break;
            if (y + i >= *altura - 1)
                break;

            vector<vector<int>> m = *matrix;
            if (m[i + x][j + y] == 0)
                vizinhos[i + 1][j + 1] = 1;
        }
    }

    unsigned int vizinhosVisitados = 0;

    while (vizinhosVisitados < 8) {
        cout << "[" << x << ", " << y << "]: " << vizinhosVisitados << endl;
        int d = rand() % (8 - vizinhosVisitados);

        if (d == 0)
            return visitarBloco(matrix, x - 1, y - 1, largura, altura);
        if (d == 1)
            return visitarBloco(matrix, x, y - 1, largura, altura);
        if (d == 2)
            return visitarBloco(matrix, x + 1, y - 1, largura, altura);
        if (d == 3)
            return visitarBloco(matrix, x - 1, y, largura, altura);
        if (d == 4)
            return visitarBloco(matrix, x + 1, y, largura, altura);
        if (d == 5)
            return visitarBloco(matrix, x - 1, y + 1, largura, altura);
        if (d == 6)
            return visitarBloco(matrix, x, y + 1, largura, altura);
        if (d == 7)
            return visitarBloco(matrix, x + 1, y + 1, largura, altura);

        vizinhosVisitados++;
    }
}

void gerarLabirintoFloodFill(vector<vector<int>>* matrix, unsigned int* largura, unsigned int* altura) {
    gerarLabirintoVazio(matrix, largura, altura);

    unsigned int x = SDL_round(*largura/2);
    unsigned int y = SDL_round(*altura/2);

    visitarBloco(matrix, x, y, largura, altura);
}