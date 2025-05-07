#include <SDL2/SDL.h>
#include <iostream>

#include "janela.cpp"

void userInput(const char* txt, int v) {
    char u;
    cout << txt << ": ";
    scanf("%c", &u);

    if (u == NULL) {
        v = 20;
    } else {
        v = u;
    }
    cout << endl;
}

int main(int argc, char* args[]) {
    unsigned int janelaLargura = 300;
    unsigned int janelaAltura = 300;
    Janela janela = Janela("Labirinto", 10, 10, janelaLargura, janelaAltura);
    Jogador jogador = Jogador();
    int labirintoLargura = 10;
    int labirintoAltura = 10;

    cout <<
    "░▒▓█▓▒░       ░▒▓██████▓▒░░▒▓███████▓▒░░▒▓█▓▒░▒▓███████▓▒░░▒▓█▓▒░▒▓███████▓▒░▒▓████████▓▒░▒▓██████▓▒░  " << endl <<
    "░▒▓█▓▒░      ░▒▓█▓▒░░▒▓█▓▒░▒▓█▓▒░░▒▓█▓▒░▒▓█▓▒░▒▓█▓▒░░▒▓█▓▒░▒▓█▓▒░▒▓█▓▒░░▒▓█▓▒░ ░▒▓█▓▒░  ░▒▓█▓▒░░▒▓█▓▒░ " << endl <<
    "░▒▓█▓▒░      ░▒▓█▓▒░░▒▓█▓▒░▒▓█▓▒░░▒▓█▓▒░▒▓█▓▒░▒▓█▓▒░░▒▓█▓▒░▒▓█▓▒░▒▓█▓▒░░▒▓█▓▒░ ░▒▓█▓▒░  ░▒▓█▓▒░░▒▓█▓▒░ " << endl <<
    "░▒▓█▓▒░      ░▒▓████████▓▒░▒▓███████▓▒░░▒▓█▓▒░▒▓███████▓▒░░▒▓█▓▒░▒▓█▓▒░░▒▓█▓▒░ ░▒▓█▓▒░  ░▒▓█▓▒░░▒▓█▓▒░ " << endl <<
    "░▒▓█▓▒░      ░▒▓█▓▒░░▒▓█▓▒░▒▓█▓▒░░▒▓█▓▒░▒▓█▓▒░▒▓█▓▒░░▒▓█▓▒░▒▓█▓▒░▒▓█▓▒░░▒▓█▓▒░ ░▒▓█▓▒░  ░▒▓█▓▒░░▒▓█▓▒░ " << endl <<
    "░▒▓█▓▒░      ░▒▓█▓▒░░▒▓█▓▒░▒▓█▓▒░░▒▓█▓▒░▒▓█▓▒░▒▓█▓▒░░▒▓█▓▒░▒▓█▓▒░▒▓█▓▒░░▒▓█▓▒░ ░▒▓█▓▒░  ░▒▓█▓▒░░▒▓█▓▒░ " << endl <<
    "░▒▓████████▓▒░▒▓█▓▒░░▒▓█▓▒░▒▓███████▓▒░░▒▓█▓▒░▒▓█▓▒░░▒▓█▓▒░▒▓█▓▒░▒▓█▓▒░░▒▓█▓▒░ ░▒▓█▓▒░   ░▒▓██████▓▒░  " << endl;

    //userInput("Largura do labirinto (20)", labirintoLargura);
    //userInput("Altura do labirinto (20)", labirintoAltura);

    while (janela.getAtiva()) {
        janela.gerarLabirinto(labirintoLargura, labirintoAltura);
        if (janela.getAtiva())
            janela.buscarLabirinto();

        if (janela.getAtiva()) {
            // FINALIZANDO
            cout << "Reiniciar? [y/N]: ";

            char r;
            scanf("%c", &r);
            if (r == 'N')
                break;
        }
    }

    janela.fechar();

    cout << "Fim do programa =)" << endl;

    return 0;
};
