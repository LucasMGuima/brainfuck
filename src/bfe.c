#include "../include/bfe.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TAPE_SIZE 500

char32 tape[TAPE_SIZE] = {0};
int tape_index = 0;

int main(int argc, char * argv[]){
    if(argc == 1){          // Codigo Brainfuck passado por fluxo
        char linha[MAX_LINE_LENGTH];

        // Loop para ler linhas da entrada padrão até o final do fluxo (EOF)
        while (fgets(linha, sizeof(linha), stdin) != NULL) {
            // Remover a quebra de linha, se presente
            linha[strcspn(linha, "\n")] = 0;
        }

        executor(linha);
    }else{                  // Código passado por arquivo
        if(strstr(argv[1], ".bf") == NULL){
            printf("O arquivo entrado deve ser do tipo '.bf'\n");
            exit(1);
        }
    }

    return 0;
}

void executor(char comandos[MAX_LINE_LENGTH]){
    for(int i = 0; i < MAX_LINE_LENGTH; i++){
        if(!comandos[i]) break;
        printf("Comando: [%d|%c]\n", comandos[i], comandos[i]);
        switch (comandos[i])
        {
            case '>': tape_index++; break;                      // Incrementa o ponteiro
            case '<': tape_index--; break;                      // Decrementa o ponteiro
            case '+': tape[tape_index]++; break;                // Incrementa em um o valor na tape na posição do ponteiro
            case '-': tape[tape_index]--; break;                // Decrementa em um o valor na tape na posição do ponteiro
            case '.': printf("."); printf("%c", tape[tape_index]); break;    // Imprime o dado no ponteiro da tape
            case ',': char c; scanf("%c", &c); tape[tape_index] = c; break;    // Aceita um input, armazena na tape onde o ponteiro esta
            
            case '[':       // Inicia um loop, se o valor no ponteiro é 0 passa para o próximo comando após o ']' par
                if(tape[tape_index] == 0){
                    int cont_loop_internos = 0; // Armazena quantos loops dentro deste
                    for(;i < MAX_LINE_LENGTH; i++){
                        if(comandos[i] == ']' && cont_loop_internos == 0) i++; break;   // Achou seu final de loop
                        if(comandos[i] == '[') cont_loop_internos++; continue;          // Encontrou o inicio de um loop interno
                        if(comandos[i] == ']') cont_loop_internos--; continue;          // Encontrou o final de um loop interno
                    }
                }
                break;
            
            case ']':       // Final de um loop, se o valor no ponteiro não for 0, volta para o comando apos o '[' correspondete
                if(tape[tape_index] != 0){
                    int cont_loop_internos = 0; // Armazena quantos loops dentro deste
                    for(;i >= 0; i--){
                        if(comandos[i] == '[' && cont_loop_internos == 0) i++; break;   // Achou seu inicio de loop
                        if(comandos[i] == '[') cont_loop_internos--; continue;          // Encontrou o inicio de um loop interno
                        if(comandos[i] == ']') cont_loop_internos++; continue;          // Encontrou o final de um loop interno
                    }
                }
                break;
        }
    }
}