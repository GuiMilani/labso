#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

#define MAX 256

int main() {
  char comando[MAX];
  int pid;
  int background = 0;
  char *tokens[MAX];
  char *token;
  int i = 0;

  while (1) {
    printf("> ");

    if (!fgets(comando, MAX, stdin)) {
        break;
    }

    background = 0;
    if (comando[strlen(comando)-1] == '&') {
        background = 1;
        comando[strlen(comando)-1] = '\0';
    }
    
    token = strtok(comando, " ");
    while (token != NULL) {
        tokens[i++] = token;
        token = strtok(NULL, " ");
    }
    tokens[i] = NULL;

    if (!strcmp(comando, "exit")) {
      exit(EXIT_SUCCESS);
    }

    int in_redirect = 0, out_redirect = 0;
    char *in_file, *out_file;
    
    i = 0;
    while (tokens[i] != NULL) {
        if (strcmp(tokens[i], "<") == 0) {
            in_redirect = 1;
            in_file = tokens[i+1];
            tokens[i] = NULL;
        } else if (strcmp(tokens[i], ">") == 0) {
            out_redirect = 1;
            out_file = tokens[i+1];
            tokens[i] = NULL;
        }
        i++;
    }    

    pid = fork();
    if (pid) {
      waitpid(pid, NULL, 0); 
    } else {
      execlp(comando, comando, NULL);
      printf("Erro ao executar comando!\n");
      exit(EXIT_FAILURE);
    }
  }
}
