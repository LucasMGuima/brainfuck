#include "../include/bfc.h"
#include <stdio.h>
#include <stdlib.h>
#include <wchar.h>
#include <locale.h>
#include <string.h>
#include <math.h>

#define TRUE 1
#define FALSE 0
#define char32 __uint32_t
#define BUFFER_SIZE 500
#define MAX_LINE_LENGTH 500

unsigned int numbers[100];                  // Armazena os números da operação
char numbers_index = 0;
char numbers_i = 0;
int buffer_number = 0;                      // Armazena temporáriamente o número
int number_size = 0;                        // Armazena o "tamanho" do número
int tratando_conta = FALSE;                 // Armazena se tratando uma conta
char operacoes[100];                        // Armazena as operaçãoes
char operacoes_index;
char buffer[BUFFER_SIZE];                   // Buffer para armazenar o código em Brainfuck
unsigned int buffer_index = 0;              // Indica a primeira posição vazia do buffer

void add_buffer(char * caracter, int qtd_carcacter);

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
    int multiplicador = 0;

    // Verifica se uma conta começou
    if(caracter == '=') tratando_conta = TRUE;
    // Verifica se uma conta terminou
    if((caracter < 30 || caracter > 39) && (caracter != '+') 
        && (caracter != '-') && (caracter != '*') && (caracter != '/')) tratando_conta = FALSE;
    
    if(tratando_conta){
        if(caracter >= 30 && caracter <= 39){
            buffer_number = buffer_number * pow(10, number_size) + (caracter - 30);
            number_size++;
        }else if((caracter == '+') && (caracter == '-') && (caracter == '*') && (caracter == '/')){
            numbers[numbers_index] = buffer_number;
            operacoes[operacoes_index] = caracter;
            number_size = 0;
            buffer_number = 0;
            operacoes_index++;
            numbers_index++;
        }
    }else{
        if(operacoes_index != 0){                                   // Tem uma conta para converter
            numbers_i = numbers_index;
            char precedencia[] = {'+', '-', '*', '/'};
            for(int i=0; i < 4; i++){
                char operacao = precedencia[i];
                for(int j=0; j < operacoes_index; j++){
                    if(operacoes[j] == operacao){
                        // Converte a operação
                        if(operacao == '*'){
                            int num1 = numbers[numbers_i];
                            numbers_i--;
                            int num2 = numbers[numbers_i];
                            numbers_i--;

                            printf(">");
                            while(num1 > 0){
                                printf("+");
                                num1--;
                            }
                            printf("[<");
                            while(num2 > 0){
                                printf("+");
                                num2--;
                            }
                            printf(">-]");
                            continue;
                        }
                        if(operacao == '+' || operacao == '-'){
                            int num = numbers[j];
                            while(num > 0){
                                printf("%s", operacao);
                                num--;
                            }
                            continue;
                        }
                    }
                }

            }
        }

        if(caracter%2 != 0){                                        // Caracter é impar
            caracter = caracter - 1;                                // Traz para o caracter par menor mais proximo
            
            add_buffer("+", 1);                                     // Adiciona 1 a conversão
        }

        if(caracter%2 == 0){                                        // Verifica se o caracter é um multiplo de 2
            unsigned int cociente = caracter;
            do{                                                     
                cociente = cociente/2;                              // Encontra o menor cociente par do valor
                multiplicador++;                                    // Armazena quantas vezes se precisa multiplicar o cociente por 2
            }while (cociente%2 == 0);
            
            // Escreve a equação no formato ">+...[<++..>-]<."
            add_buffer(">", 1);
            for(int i = 0; i < cociente; i++) add_buffer("+", 1);              // Escreve o cociente em BF
            add_buffer("[<", 2);
            for(int i = 0; i < (1 << multiplicador); i++) add_buffer("+", 1);  // Escreve o valor a se multiplicar o cociente para voltar ao caracter
            add_buffer(">-]<.>", 6);
            return 0;
        }
    }
}

void criar_arquivo(){
    printf("%s\n", buffer);
}

/*
    add_buffer: Adiciona a sequencia de caracteres enviados ao buffer,
                incrementa o buffer_index, se o tamanho do buffer estourar,
                encerra o programa.
*/
void add_buffer(char * caracter, int qtd_carcacter){
    if(buffer_index >= BUFFER_SIZE){    // Se o tamanho do buffer for ecedido encerra o programa
        printf("Tamanho do buffer ecedido, não foi pocível completar o processo.\n");
        exit(1);
    }

    for(int i = buffer_index, j = 0; buffer_index < qtd_carcacter + i; buffer_index++, j++){
        buffer[buffer_index] = caracter[j];
    }

}