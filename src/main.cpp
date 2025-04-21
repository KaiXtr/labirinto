#include <SDL2/SDL.h>
#include <iostream>

#include "janela.cpp"

int main(int argc, char* args[]) {
    unsigned int janelaLargura = 300;
    unsigned int janelaAltura = 300;
    Janela janela = Janela("Labirinto", 10, 10, janelaLargura, janelaAltura);
    Jogador jogador = Jogador();

    janela.gerarLabirinto(5,5);

    // FPS
	static Uint32 tintervalo=30, prox_f, atual_f, fps_atual, fps_contar=60;
	prox_f = SDL_GetTicks() + tintervalo;

    while (janela.getAtiva()) {
		SDL_Event evento;
		while (SDL_PollEvent(&evento)){
            janela.eventos(evento);
            jogador.eventos(evento);
        }
        janela.limpar();
        janela.desenharLabirinto();
        jogador.desenhar(janela.getRenderer());
        janela.atualizar();

        // FPS
		atual_f = SDL_GetTicks();
		if (prox_f > atual_f)
			SDL_Delay(prox_f - atual_f);
		prox_f += tintervalo;
		fps_contar--;
		if (fps_contar == 0){
			fps_atual = prox_f - atual_f;
			fps_contar = 30;
		}
    }

    janela.fechar();

    return 0;
};
