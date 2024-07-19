#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

#define MAX 256

void tokenizeInput(char comando[MAX], char *tokens[MAX]) {
    int i = 0;
    char *token = strtok(comando, " ");
    while (token != NULL) {
        tokens[i++] = token;
        token = strtok(NULL, " ");
    }
    tokens[i] = NULL;
}

void haveRedirect(char *tokens[MAX], char **inFile, char **outFile, char *inputRedirect, char *outputRedirect) {
    int i = 0;
    while (tokens[i] != NULL) {
        if (strcmp(tokens[i], "<") == 0) {
            *inputRedirect = 1;
            *inFile = tokens[i + 1];
            tokens[i] = NULL;
        }
        else if (strcmp(tokens[i], ">") == 0) {
            *outputRedirect = 1;
            *outFile = tokens[i + 1];
            tokens[i] = NULL;
        }
        i++;
    }
}

int main() {
    char comando[MAX], *tokens[MAX], *inFile, *outFile, inputRedirect = 0, outputRedirect = 0, background = 0;
    int pid;

    while (1) {
        inFile = NULL, outFile = NULL, inputRedirect = 0, outputRedirect = 0;
        printf("> ");
        if (!fgets(comando, MAX, stdin)) {
            break;
        }

        if (strlen(comando) <= 1) {
            continue;
        }

        comando[strlen(comando) - 1] = '\0';
        if (!strcmp(comando, "exit")) {
            exit(EXIT_SUCCESS);
        }

        background = 0;
        if (comando[strlen(comando) - 1] == '&') {
            background = 1;
            comando[strlen(comando) - 1] = '\0';
        }

        tokenizeInput(comando, tokens);
        haveRedirect(tokens, &inFile, &outFile, &inputRedirect, &outputRedirect);

        pid = fork();

        if (pid == 0) {
            if (inputRedirect) {
                FILE *in_fp = freopen(inFile, "r", stdin);
                if (in_fp == NULL) {
                    perror("Erro ao abrir arquivo de entrada");
                    exit(EXIT_FAILURE);
                }
            }

            if (outputRedirect) {
                FILE *out_fp = freopen(outFile, "w", stdout);
                if (out_fp == NULL) {
                    perror("Erro ao abrir arquivo de saída");
                    exit(EXIT_FAILURE);
                }
            }

            execvp(tokens[0], tokens);

            perror("Erro ao executar comando");
            exit(EXIT_FAILURE);
        }
        else if (pid > 0) {
            if (!background) {
                waitpid(pid, NULL, 0);
            }
        }
        else {
            perror("Erro ao criar processo");
            exit(EXIT_FAILURE);
        }
    }

    return 0;
}
