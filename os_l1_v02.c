/*
 *На вход программе подается команда интерпретатора команд. Программа должна произвести вывод команды в верхнем регистре.
 */
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

#define SIZE_STR 128

int main()
{
    int PtC[2], // от родителя к ребенку
        CtP[2]; // от ребенка к родителю

    if (pipe(PtC) == -1 || pipe(CtP) == -1)
    {
        write(STDERR_FILENO, "Oops...\n", 8);
        exit(EXIT_FAILURE);
    }

    pid_t pid = fork();

    if (pid > 0) // РОДИТЕЛЬСКИЙ ПРОЦЕСС
    {
        close(PtC[0]); // 0 - чтение, 1 - запись
        close(CtP[1]);
        char comand[SIZE_STR];
        char c; // для считывания посимвольно
        int i = 0;

        while (read(STDIN_FILENO, &comand[i], sizeof(char)) > 0 && comand[i] != '\n') i++;

        comand[i++] = '\0';

        write(PtC[1], &i, sizeof(int));
        write(PtC[1], comand, i);

        close(PtC[1]);

        waitpid(pid, NULL, 0);

        while (read(CtP[0], &c, sizeof(char)) > 0)
        {
            if (c >= 'a' && c <= 'z')
            {
                c -= 32;
            }
            write(STDOUT_FILENO, &c, sizeof(char));
        }
        close(CtP[0]);
    }

    else if (pid == 0) // ДОЧЕРНИЙ ПРОЦЕСС
    {
        close(PtC[1]);
        close(CtP[0]);
        int size;
        char comand[SIZE_STR];
        read(PtC[0], &size, sizeof(int));
        read(PtC[0], comand, size);
        close(PtC[0]);
        dup2(CtP[1], STDOUT_FILENO);
        dup2(CtP[1], STDERR_FILENO);
        close(CtP[1]);
        char *argv[] = {"sh", "-c", comand, NULL};
        execv("/bin/sh", argv);
    }
    else
    {
        write(STDERR_FILENO, "ERROR: no fork\n", 15);
        exit(EXIT_FAILURE);
    }
    return EXIT_SUCCESS;
}
