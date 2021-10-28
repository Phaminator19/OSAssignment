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
#include <stdlib.h>

#define MAXLIST 10000 //max number of letters to be supported
#define MAXCOM 100 // max number of commands to be supported
#define clear() printf("\033[H\033[J")

char currentdir[1024];

//Function to do a greeting shell
void init_shell() {
    clear();

    printf("\n\n\n\t ***MY SHELL***");
    printf("\n\n\n\t- Welcome to COP4600- ASSIGNMENT #2 ");
    printf("\n\n\n\n*******************************************");
    
    printf("\n");
    sleep(1);
    clear();
}

char *read_line(void) {
    int bufsize = MAXCOM;
    char *buffer = malloc(bufsize * sizeof(char));
    int position = 0;
    int c;
    
    //if allocation is bad, it will print a bad statement
    if (!buffer) {
        fprintf(stderr, "allocation error\n");
        exit(EXIT_FAILURE);
    }
    
    while(1) {
       
        c = getc(stdin);
        
        //if we hit EOF, replace it with a null character and return.
        if (c==EOF || c == '\n') {
            buffer[position] = '\0';
            // printf("%s", buf);
            return buffer;
        }
        else {
            buffer[position] = c;
        }
        position++;
    
        if (position >= MAXCOM) {
            bufsize += MAXCOM;
            buffer = realloc(buffer, bufsize);

            if (!buffer) {
                fprintf(stderr, "Fail to Re-allocate more blocks\n");
                exit(EXIT_FAILURE);
            }
        }
    }

}

//Parse the command line
char** parse_command_line(char *line) {
    int bufsize = MAXLIST; 
    int position = 0;
    char **tokens = malloc(bufsize * sizeof(char*));
    char *token = NULL; 
    int i;
    
    token = strtok(line, " ");

    while (token != NULL)
    {
        tokens[position] = token;
        position++;

        if (position >= MAXCOM) {
            bufsize += MAXCOM;
            tokens = realloc(tokens, bufsize * sizeof(char*));

            if (!tokens) {
                fprintf(stderr, "Fail to Re-allocate more blocks\n");
                exit(EXIT_FAILURE);
            }
        }

        token = strtok(NULL, " ");
    }

    tokens[position] = NULL;
    return tokens;
}

//Function that the system command is executed
void execArgs(char** parsed) {
    int pid = fork();

    if(pid == -1) {
        printf("\nFailed Forking child...\n");
        return;
        
    }else if (pid == 0) {
        //execution function in an if case. It will return -1 if not found or 1 if found
        if (execvp(parsed[0], parsed) < 0) {
            printf("\nCould not execute command\n");
        }
        exit(0);
        
    } else {
        wait(NULL);
        return;
    }
}



void process_commands(void) {
    char line[1000]; 

    int bufsize = MAXCOM;
    char *buffer = malloc(sizeof(char) * bufsize);
    char **command = malloc(bufsize * sizeof(char*));

    while(1) {
        printf("# ");
        buffer = read_line();
        
        command = parse_command_line(buffer);
        execArgs(command);
    }
}

void movetoDir(char *Directory) {
    // Check input string
    if((Directory != NULL) && (Directory[0] == '\0')) {
        fprintf(stderr, "Directory does not exist.\n");
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

//Prints the value of the currentdir variable
void whereami() {
    printf("%s\n", currentdir);
}

struct stack_struct {
    int top;
    unsigned capacity;
    char* commands;
};

typedef struct stack_struct stack;

stack *createStack(unsigned capacity) {
    stack *st = malloc(sizeof(stack));

    st->capacity = capacity;
    st->top = -1;
    st->commands = (char*)malloc(st->capacity * sizeof(char));

    return st;
}

int is_empty(stack *st) {
    return st->top == -1; 
}

int is_full(stack *st) {
    return st->top == st->capacity - 1;
}

stack *pop (stack *st) {
    if(is_empty(st)) {
        return INT_MIN;
    }

    return st->commands[st->top--];

    
}

void push(stack *st, char* commands) {
    
    if(is_full(st)) {
        printf("the stack is full\n");
        return;
    }
    else {
        increment = st->top + 1;
        st->commands[increment] = commands;
    }
}



//prints out the recently typed commands in reverse order with numbers.
void history() {

}

int main() {
    init_shell();
    char line[MAXLIST];

    //this line will process the command after initialize the shell
    process_commands();

    return 0;
}