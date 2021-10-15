/*COP4600 - Homework_2 
/*myshell - making my own linux shell in C */
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <dirent.h>

#define MAXLIST 100 //max number of commands to be supported
#define MAXCOM 10000 // max number of letters to be supported
#define clear() printf("\033[H\033[J")

char currentdir[1024];

//Function to do a greeting shell
void init_shell() {
    clear();

    printf("\n\n\n\t ***MY SHELL***");
    printf("\n\n\n\t-Welcome to COP4600-");
    printf("\n\n\n\n*************************************");
    
    char *username = getenv("USER");
    printf("\n\n\nUSER is: @%s", username)
    printf("\n")
    sleep(1);
    clear();
}

//Function to take input
int takeInput(char* str) {
    char* buf;

    buf = readline("\n# ");
    if (strlen(buf) != 0) {
        add_history(buf);
        strcpy(str, buf);
        return 0;
    }
    else {
        return 1;
    }
}

//Function that the system command is executed
void execArgs(char** parsed) {
    int pid = fork();

    if(pid == -1) {
        printf("\nFailed Forking child...")
        return;
    }

    else if (pid == 0) {
        //execution function in an if case. It will return -1 if not found or 1 if found
        if (execvp(parsed[0], parsed) < 0) {
            printf("\nCould not execute command")
        }

    }

}



void movetoDir(char *Directory) {
    // Check input string
    if((Directory != NULL) && (Directory[0] == '\0')) {
        fprintf(stderr, "Directory does not exist.\n")
        return;
    }

    // Check whether the directory is exist
    // If directory exists, save in global variable
    if (getcwd(Directory, sizeof(Directory)) == NULL) {    
        fprintf(stderr, "Please enter an existing directory.\n");
        return;
    }
    else {
        strcpy(currentdir, Directory);
        return;
    }
}



int main() {
    init_shell();
    return 0; 
}