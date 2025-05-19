#include "../include/bfc.h"
#include <stdio.h>
#include <stdlib.h>
#include <wchar.h>
#include <locale.h>
#include <stdlib.h>
#include <string.h>

#define char32 __uint32_t

int main(int argc, char *argv[]){
    // Configura o locale para suportart UTF-8 pegando a
    // configuração do sistema
    if(setlocale(LC_ALL, "") == NULL){
        fprintf(stderr, "Erro ao configurar o locale.\n");
        return 1;
    }

    tratar_entrada(argv, argc);
}

/*
    tratar_entrada: Processa a linha entrada como parametro, intera sobre a
                    string tentando converter uma sequência de bytes UTF-8
                    para um caractere largo.
*/
void tratar_entrada(char * entradas, int quantidade){
    for(int i = 1; i < quantidade; i++){
        char * comando = entradas[i];
        printf("Analisando: %s\n", entradas[i]);
        
        mbstate_t state;                    // Estado de conversão para mbrtowc
        memset(&state, 0, sizeof(state));   // Inicializa o estado
        const char *ptr = comando;          // Ponteiro para percorrer a string UTF-8
        wchar_t wc;                         // Para armazenar o caracter largo convertido
        size_t byte_processed;              // Número de bytes processados por mbrtowc

        while(*ptr != '\0'){
            byte_processed = mbrtowc(&wc, ptr, strlen(ptr), &state);    // Pega a nova nova sequência de bytes começando em 'ptr' e armazena em 'wc'

            switch (byte_processed){
                default:                            // Conversão bem-sucedida, 'wc' contém um caracter largo
                    tratar_caracter((char32)wc);
                    ptr += byte_processed;          // Avança o ponteiro pelo número de bytes processados
                    break;

                case (size_t)-1:
                    perror("Erro de codificação UTF-8 inválida");
                    break;

                case (size_t)-2:
                    fprintf(stderr, "Sequência UTF-8 incompleta no final da string.\n");
                    break;

                case 0: break;                      // Caracter nulo
            }
        }
        printf("\n");
    }
}

int tratar_caracter(__uint32_t caracter){
    printf("%lc -> %u\n", (wint_t)caracter, caracter);
}