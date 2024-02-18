#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <errno.h>
#include <string.h>

/*
NOTE: this program runs commands with one pipe only, for example:
command:
    ls -1 | cut -f1 -d.
result:
    file1
    file1
*/
int main(int argc, char* argv[]){
    int fd[2]; //fd[0] -> read, fd[1] -> write
    if (pipe(fd) == -1){
        printf("Failed to create a pipe");
    }

    char *args[10];
    char *FirstArray[10];
    char *SecondArray[10];
    int i=0;
    char str[100];
    printf("Please enter a command:\n");
    // this thing is used to store spaces using scanf
    scanf("%[^\n]100s", str);

    char* token = strtok(str, "|");
    while (token != NULL) {
        args[i++] = token;
        token = strtok(NULL, "|");
    }

    token = strtok(args[0], " ");
    i = 0;
    while (token != NULL) {
        FirstArray[i] = token;
        token = strtok(NULL, " ");
        i++;
    }
    FirstArray[i] = NULL;


    token = strtok(args[1], " ");
    i = 0;
    while (token != NULL) {
        SecondArray[i] = token;
        token = strtok(NULL, " ");
        i++;
    }
    SecondArray[i] = NULL;

    int pid1 = fork();
    if(pid1 == -1){
        printf("Failed to fork");
    }
    if (pid1 == 0){ //Child process 1
        dup2(fd[1], STDOUT_FILENO);
        close(fd[1]);
        close(fd[0]);
        int ret = execvp(FirstArray[0], FirstArray);
        if(ret == -1){
            printf("%s: command not found\n", FirstArray[0]);
            return 0;
        }
    }

    int pid2 = fork();
    if(pid2 == -1){
        printf("Failed to fork");
    }

    if (pid2 == 0){ //Child process 2
        dup2(fd[0], STDIN_FILENO);
        close(fd[1]);
        close(fd[0]);
        int ret2 = execvp(SecondArray[0], SecondArray);
        if(ret2 == -1){
            printf("%s: command not found\n", SecondArray[0]);
            return 0;
        }
    }
    waitpid(-1);
    return 0;
}