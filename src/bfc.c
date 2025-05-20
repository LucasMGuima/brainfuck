#include "../include/bfc.h"
#include <stdio.h>
#include <stdlib.h>
#include <wchar.h>
#include <locale.h>
#include <string.h>

#define char32 __uint32_t
#define BUFFER_SIZE 500
#define MAX_LINE_LENGTH 500

char buffer[BUFFER_SIZE];                 // Buffer para armazenar o código em Brainfuck
unsigned int buffer_index = 0;              // Indica a primeira posição vazia do buffer

void add_buffer(char * caracter);

int main(int argc, char *argv[]){
    // Configura o locale para suportart UTF-8 pegando a
    // configuração do sistema
    if(setlocale(LC_ALL, "") == NULL){
        fprintf(stderr, "Erro ao configurar o locale.\n");
        return 1;
    }

    if(argc == 1){
        char linha[MAX_LINE_LENGTH];

        // Loop para ler linhas da entrada padrão até o final do fluxo (EOF)
        while (fgets(linha, sizeof(linha), stdin) != NULL) {
            // Remover a quebra de linha, se presente
            linha[strcspn(linha, "\n")] = 0;
        }

        tratar_entrada(linha);
    }else{
        tratar_entrada(argv[1]);
    }
    criar_arquivo();
}

/*
    tratar_entrada: Processa a linha entrada como parametro, intera sobre a
                    string tentando converter uma sequência de bytes UTF-8
                    para um caractere largo.
                    Se o caracter for válido chama 'tratar_caracter'
*/
void tratar_entrada(char * comando){
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
}

/*
    tratar_caracter: Transforma o caracter entrado em um comando Brainfuck,
                     adiciona ao buffer o novo seguimento de comando chamando
                     a função add_buffer.
*/
int tratar_caracter(__uint32_t caracter){
    //printf("\n%lc -> %u ", caracter, caracter);
    int multiplicador = 0;

    if(caracter%2 != 0){                                        // Caracter é impar
        caracter = caracter - 1;                                // Traz para o caracter par menor mais proximo
        //multiplicador++;
        add_buffer("+");                                            // Adiciona 1 a conversão
    }

    if(caracter%2 == 0){                                        // Verifica se o caracter é um multiplo de 2
        unsigned int cociente = caracter;
        do{                                                     
            cociente = cociente/2;                              // Encontra o menor cociente par do valor
            multiplicador++;                                    // Armazena quantas vezes se precisa multiplicar o cociente por 2
        }while (cociente%2 == 0);
        
        // Escreve a equação no formato ">+...[<++..>-]<."
        add_buffer(">");
        for(int i = 0; i < cociente; i++) add_buffer("+");              // Escreve o cociente em BF
        add_buffer("[<");
        for(int i = 0; i < (1 << multiplicador); i++) add_buffer("+");  // Escreve o valor a se multiplicar o cociente para voltar ao caracter
        add_buffer(">-]<.>");
        return 0;
    }
}

/*
    saida: Coloca o conteudo do buffer no stdout.
*/
void saida(){
    printf("%s\n", buffer);
}

/*
    add_buffer: Adiciona a sequencia de caracteres enviados ao buffer,
                incrementa o buffer_index, se o tamanho do buffer estourar,
                encerra o programa.
*/
void add_buffer(char * caracter){
    if(buffer_index >= BUFFER_SIZE){    // Se o tamanho do buffer for ecedido encerra o programa
        printf("Tamanho do buffer ecedido, não foi pocível completar o processo.\n");
        exit(1);
    }

    buffer[buffer_index++] = * caracter;
}