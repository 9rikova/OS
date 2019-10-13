#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main()
{
    int fd[2];
    if (pipe(fd) == -1)
    {
        write(STDERR_FILENO, "ERROR: no pipe\n", 8);
        exit(EXIT_FAILURE);
    }

    pid_t pid = fork();
    
    if (pid < 0)
    {
        write(STDERR_FILENO, "ERROR: no fork\n", 15);
        exit(EXIT_FAILURE);
    }
    else if (pid == 0)
    {
        close(fd[0]);
        dup2(fd[1], STDOUT_FILENO);
        dup2(fd[1], STDERR_FILENO);
        close(fd[1]);
        execl("/bin/sh", "/bin/sh", NULL);
        write(STDERR_FILENO, "ERROR: no exec\n", 14);
    }
    else
    {
        close(fd[1]);
        char c;
        while(read(fd[0], &c, sizeof(char)) > 0)
        {
            if (c >= 'a' && c <= 'z') c -= 32;
            write(STDOUT_FILENO, &c, sizeof(char));
        }
    }
}