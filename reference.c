#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>

#define MAX 256

int main()
{
    char linha[MAX];
    char *tokens[MAX];
    char *token;
    int background = 0;
    int i = 0;
    int pid;

    while (1)
    {
        printf("> ");
        fflush(stdout);

        // Ler a linha de comando
        if (!fgets(linha, MAX, stdin))
        {
            break;
        }

        // Verificar se a linha está vazia
        if (strlen(linha) <= 1)
        {
            continue;
        }

        // Remover o newline no final
        linha[strlen(linha) - 1] = '\0';

        // Verificar se o comando deve ser executado em segundo plano
        background = 0;
        if (linha[strlen(linha) - 1] == '&')
        {
            background = 1;
            linha[strlen(linha) - 1] = '\0';
        }

        // Tokenizar a linha de comando
        token = strtok(linha, " ");
        while (token != NULL)
        {
            tokens[i++] = token;
            token = strtok(NULL, " ");
        }
        tokens[i] = NULL;

        // Verificar se o comando é 'exit'
        if (strcmp(tokens[0], "exit") == 0)
        {
            exit(EXIT_SUCCESS);
        }

        // Verificar se há redirecionamento de entrada ou saída
        int in_redirect = 0, out_redirect = 0;
        char *in_file, *out_file;

        i = 0;
        while (tokens[i] != NULL)
        {
            if (strcmp(tokens[i], "<") == 0)
            {
                in_redirect = 1;
                in_file = tokens[i + 1];
                tokens[i] = NULL;
            }
            else if (strcmp(tokens[i], ">") == 0)
            {
                out_redirect = 1;
                out_file = tokens[i + 1];
                tokens[i] = NULL;
            }
            i++;
        }

        // Criar processo filho
        pid = fork();

        if (pid == 0)
        {
            // Processo filho

            // Redirecionamento de entrada, se necessário
            if (in_redirect)
            {
                FILE *in_fp = fopen(in_file, "r");
                if (in_fp == NULL)
                {
                    perror("Erro ao abrir arquivo de entrada");
                    exit(EXIT_FAILURE);
                }
                // Redefinir stdin para o arquivo
                if (fgets(linha, MAX, in_fp) == NULL)
                {
                    perror("Erro ao ler do arquivo de entrada");
                    exit(EXIT_FAILURE);
                }
                fclose(in_fp);
            }

            // Redirecionamento de saída, se necessário
            if (out_redirect)
            {
                FILE *out_fp = freopen(out_file, "w", stdout);
                if (out_fp == NULL)
                {
                    perror("Erro ao abrir arquivo de saída");
                    exit(EXIT_FAILURE);
                }
            }

            // Executar o comando
            execvp(tokens[0], tokens);

            // Se execvp retornar, ocorreu um erro
            perror("Erro ao executar comando");
            exit(EXIT_FAILURE);
        }
        else if (pid > 0)
        {
            // Processo pai

            // Esperar pelo processo filho, a menos que esteja em segundo plano
            if (!background)
            {
                waitpid(pid, NULL, 0);
            }
        }
        else
        {
            // Erro ao criar o processo filho
            perror("Erro ao criar processo");
            exit(EXIT_FAILURE);
        }
    }

    return 0;
}
