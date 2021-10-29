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
#include <signal.h>
#include <stdbool.h>

#define MAXLIST 10000 //max number of letters to be supported
#define MAXCOM 100 // max number of commands to be supported
#define clear() printf("\033[H\033[J")
       
char* stack[MAXCOM];     
int top = -1;
FILE *historyFile;

//char* directory_stack[MAXCOM];
//int d_top = -1;

int isempty();
int isfull();
char* replay(int num);
char* popCommand();
void pushCommand(char* data);
char** parse_command_line(char *line);
void init_shell();
char *read_line(void);
void execArgs(char** parsed);
int ownCmdHandler(char *buffer);
void process_commands(void);
void movetoDir(char *Directory);
void whereami();
void clearHistory();
void checkHistory(bool flag);
void byebye();
void dalek(int pid);
void start(char **command_tokens, int length);
void background(char **command_tokens, int length);
int isFullPath(char *path);

char currentdir[1024];

// Made by Zac
int isempty() {

   if(top == -1)
      return 1;
   else
      return 0;
}

// Made by Zac   
int isfull() {

   if(top == MAXCOM)
      return 1;
   else
      return 0;
}

// Made by Zac
char* replay(int num) {
    if(num > top || num < 0)
    {
        printf("Invalid history number.");
        return NULL;
    }
    return stack[top - num];
}

// Made by Zac
char* popCommand() {
    char* data;
	
    if(!isempty()) 
    {
        data = stack[top];
        top = top - 1;   
        return data;
    } 
    else 
    {
        printf("Could not retrieve data, Stack is empty.\n");
    }
}

// Made by Zac
void pushCommand(char* data) {
    if(!isfull()) 
    {
        top = top + 1;
        stack[top] = malloc(sizeof(data));   
        strcpy(stack[top], data);
    } 
    else 
    {
        printf("Maximum number of commands met. Please clear your history\n");
    }
}

//Parse the command line
char** parse_command_line(char *line) {
    int bufsize = MAXLIST; 
    int position = 0;
    char **tokens = malloc(bufsize * sizeof(char*));
    char *token = NULL; 
    int i;
    char linetoread[MAXLIST];

    strcpy(linetoread, line);

    token = strtok(linetoread, " ");

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

//Function to do a greeting shell
void init_shell() {
    clear();
    
    printf("\n\n\n\t ***MY SHELL***");
    printf("\n\n\n\t- Welcome to COP4600 - ASSIGNMENT #2 -");
    printf("\n\n\n\n*******************************************");
    
    printf("\n");
    sleep(1);

    int NoOfOwnCmds = 8;
    char* ListOfOwnCmds[NoOfOwnCmds];
    ListOfOwnCmds[0] = "byebye";
    ListOfOwnCmds[1] = "movetodir";
    ListOfOwnCmds[2] = "background";
    ListOfOwnCmds[3] = "start";
    ListOfOwnCmds[4] = "history";
    ListOfOwnCmds[5] = "replay";
    ListOfOwnCmds[6] = "dalek";
    ListOfOwnCmds[7] = "whereami";

    printf("List of commands:\n");
    for(int i = 0; i < NoOfOwnCmds; i++) {
        printf("# %d: %s\n", i, ListOfOwnCmds[i]);
    }

    printf("\n");

    char line[MAXLIST];
    int i = 0;

    if(getcwd(currentdir, sizeof(currentdir)) == NULL) 
    {
        printf("Could not get current directory");
        return;
    }

    if(historyFile = fopen("history.txt", "r+"))
    {
        while (fscanf(historyFile, "%s", line) != EOF) 
        {
            pushCommand(line);
        }
    }
    else
    {
        historyFile = fopen("history.txt", "w+");
    }
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

int ownCmdHandler(char *buffer) {
    int NoOfOwnCmds = 8, i, switchOwnArg = 0;
    char* ListOfOwnCmds[NoOfOwnCmds];
    //char* username;
    char* cmd = NULL;
    int parsedLength = -1;
    char **parsed;

    parsed = parse_command_line(buffer);

    while(parsed[++parsedLength] != NULL) {
    }

    ListOfOwnCmds[0] = "byebye";
    ListOfOwnCmds[1] = "movetodir";
    ListOfOwnCmds[2] = "background";
    ListOfOwnCmds[3] = "start";
    ListOfOwnCmds[4] = "history";
    ListOfOwnCmds[5] = "replay";
    ListOfOwnCmds[6] = "dalek";
    ListOfOwnCmds[7] = "whereami";

    for (i = 0; i < NoOfOwnCmds; i++) {
        if (strcmp(parsed[0], ListOfOwnCmds[i]) == 0) {
            switchOwnArg = i + 1;
            break;
        }
    }
  
    switch (switchOwnArg) {
    case 1:
        pushCommand(buffer);
        byebye();
    case 2:
        movetoDir(parsed[1]);
        return 1;
    case 3:
        background(parsed, parsedLength);
        return 1;
    case 4:
        start(parsed, parsedLength);
        return 1;
    case 5:
        if(parsedLength > 1 && strcmp(parsed[1], "-c") == 0)
        {
            checkHistory(true);
        }
        else
        {
            checkHistory(false);
        }
        return 1;
    case 6:
        if (parsedLength > 1 && atoi(parsed[1]) >= 0)
        {
            cmd = replay(atoi(parsed[1]));
        }
        if(cmd != NULL)
        {
            ownCmdHandler(cmd);
        }
        return 1;
    case 7:
        if (parsedLength && atoi(parsed[1]))
        {
            dalek(atoi(parsed[1]));
        }
        else
        {
            printf("Incorrect arguments. Please input a valid pid.\n");
        }
        return 1;
    case 8:
        whereami();
        return 1;
    default:
        break;
    }
    return 0;
}

void process_commands(void) {
    char line[1000]; 

    int bufsize = MAXCOM;
    char *buffer = malloc(sizeof(char) * bufsize);
    char **command = malloc(bufsize * sizeof(char*));

    while(1) {
        printf("# ");
        buffer = read_line();
        
        ownCmdHandler(buffer);
        pushCommand(buffer);
    }
}

// Edited by Joey
// getting ../ to work and 
// changing the directory
void movetoDir(char *Directory) {
    char path[MAXCOM];
    char temp[MAXCOM];

    // Check input string
    if((Directory == NULL) || (Directory[0] == '\0')) {
        fprintf(stderr, "No directory entered.\n");
        return;
    }

    // Check if full path or relative
    if(isFullPath(Directory) == 0) {
        // Append to current dir
        strcpy(temp, currentdir);
        strcat(temp, "/");
        strcat(temp, Directory);
    }
    else {
        // Change the current directory
        strcpy(temp, Directory); 
    }
    // Check if path exists
    if (realpath(temp, path) == NULL) {
        printf("Error: Directory name doesn't exist.\n");
        return; 
    }
    else {
        // Copy temp into currentdir
        strcpy(currentdir, path);
    }

    //DIR *dir;
    //dir = opendir(Directory);
    // Check whether the directory exists
    // If directory exists, save in global variable
    //if (getcwd(Directory, sizeof(Directory)) == NULL)
    // if (dir == NULL) {    
    //     fprintf(stderr, "Please enter an existing directory.\n");
    //     return;
    // }
    // else {
    //     // add / before adding next part of path
    //     path = strcat(currentdir, "/");
    //     strcat(path, Directory);

    //     strcpy(currentdir, path);
        
    //     return;
    // }
}

//Prints the value of the currentdir variable
void whereami() {
    if(currentdir == NULL) {
        printf("No where.\n");
        return;
    }
    else {
        printf("%s\n", currentdir);
    }
    
}

// Made by Zac
// Clears history when -c flag is given
void clearHistory()
{
    while(!isempty())
    {
        popCommand();
    }
    historyFile = fopen("history.txt", "w");
}

// Made by Zac
//prints out the recently typed commands in reverse order with numbers.
void checkHistory(bool flag) {
    
    if (flag)
    {
        clearHistory();
        return;
    }
    
    int i = 0;
    while(i <= top)
    {
        printf("%d: %s\n", i, replay(i));
        i++;
    }
    return;
}

// Made by Zac
// Terminates shell and saves history file
void byebye()
{
    int i = 0;
    while(i <= top)
    {  
      fprintf(historyFile, "%s\n", stack[i]);
      free(stack[i]);
      i++;
    }
    fclose(historyFile);
    exit(0);
}

void dalek(int pid)
{
    int status = kill(pid, SIGKILL);
    if (status == -1)
    {
        printf("Unable to kill the specified process.\n");
    } else {
        printf("Process killed.\n");
    }
}

//void dalekall() {
    
//    printf("Exterminating %d processes: %d %d %d", )
//}

//made by Quang
// is this relative or direct path?
void start(char **command_tokens, int length) 
{
    pid_t pid = fork();
    int i;
    char path[MAXLIST];
    char* command[length+1];
    FILE * file;

    // Find result of fork call.
    if(pid < 0)
    {
        printf("Fork() failed.\n");
        return;
    }

    //check to see if we are in the parent process
    if (pid > 0) 
    {
        waitpid(pid, NULL, 0);
    }

    for (i = 1; i < length; i++)
    {
        command[i - 1] = malloc(sizeof(command_tokens[i]));
        strcpy(command[i - 1], command_tokens[i]);
    }
    
    command[length] = NULL;

    strcpy(path, command[0]);
    
    // Check if we are in child
    if (pid == 0) 
    {
         //if the argument isn't start with "/ the shell should interpreted as a relative path.
        if (isFullPath(path) == 0) 
        {
            strcpy(path, currentdir);
            strcat(path, "/");
            strcat(path, command[0]);
        }

        else if(isFullPath(path) == 2)
        {
            printf("Please enter a program path.\n");
        }

        if (!(file = fopen(path, "r")))
        {
            printf("Program could not be found.\n");
        }
        else if(execv(path, command) == -1)
        {
            printf("Program could not be executed.\n");
        }
    }

    for (i = 0; i < length; i++)
    {
        free(command[i]);
    }
}

//It is similar to the run command, but it immediately prints the PID of the program it 
// started, and returns the prompt. 
// Execute a program in the background
// Made by Joey
void background(char **command_tokens, int length) {
    pid_t pid = fork();
    FILE *file;
    int i;
    char path[MAXLIST];
    char* command[length+1];

    for (i = 1; i < length; i++)
    {
        command[i - 1] = malloc(sizeof(command_tokens[i]));
        strcpy(command[i - 1], command_tokens[i]);
    }

    if (pid > 0)
    {
        printf("PID: %d\n", pid);
    }
    
    command[length] = NULL;

    strcpy(path, command[0]);

    // Find result of fork call.
    if(pid < 0)
    {
        printf("Fork() failed.\n");
        return;
    }
    else if (pid == 0) 
    {
        //if the argument isn't start with "/ the shell should interpreted as a relative path.
        if (isFullPath(path) == 0) 
        {
            strcpy(path, currentdir);
            strcat(path, "/");
            strcat(path, command[0]);
        }
        else if(isFullPath(path) == 2)
        {
            printf("Please enter a program path.\n");
        }

        if (!(file = fopen(path, "r")))
        {
            printf("Program could not be found.\n");
        }
        else if(execv(path, command) == -1)
        {
            printf("Program could not be executed.\n");
        }
    }

    for (i = 0; i < length; i++)
    {
        free(command[i]);
    }
    
    if (pid == 0) 
    {
        printf("the given program could not be found\n");
        return;
    }

}

// Made by Joey
int isFullPath(char *path) {
    if (path == NULL)
    {
        return 2;
    }
    // Indicate whether the path
    // is full or relative.
    if (path[0] == 47)
        return 1;
    else 
        return 0;
}

int main() {
    init_shell();
    char line[MAXLIST];

    //this line will process the command after initialize the shell
    process_commands();

    return 0;
}