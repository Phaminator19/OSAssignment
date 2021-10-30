/*  Contributers:
    Quang Pham
    Zachary Kirksey
    Lauren Bravine
    
    COP4600 - Homework_2 
    myshell - making a custom linux shell in C 
*/
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

#define MAXLIST 100000 //max number of letters to be supported
#define MAXCOM 100 // max number of commands to be supported
#define clear() printf("\033[H\033[J")
       
char* stack[MAXCOM];     
int top = -1;
FILE *historyFile;

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
void movetoDir(char *parsed_arg);
void whereami();
void clearHistory();
void checkHistory(bool flag);
void byebye();
void dalek(int pid);
void start(char **command_tokens, int length);
void background(char **command_tokens, int length);
int isFullPath(char *path);

char currentdir[MAXLIST];

/*
Contributers:
Zac
*/
int isempty() {

   if(top == -1)
      return 1;
   else
      return 0;
}

/*
Contributers:
Zac
*/   
int isfull() {

   if(top == MAXCOM)
      return 1;
   else
      return 0;
}

/*
Contributers:
Zac
Lauren
*/
char* replay(int num) {
    if(num > top || num < 0)
    {
        printf("Invalid history number.");
        return NULL;
    }
    return stack[top - num];
}

/*
Contributers:
Zac
Lauren
*/
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

/*
Contributers:
Zac
Lauren
*/
void pushCommand(char* data) {
    if(!isfull()) 
    {
        top = top + 1;
        stack[top] = (char *)malloc(sizeof(data));   
        strcpy(stack[top], data);
    } 
    else 
    {
        printf("Maximum number of commands met. Please clear your history\n");
    }
}

/*
Contributers:
Quang
*/
char** parse_command_line(char *line) {
    int bufsize = MAXLIST; 
    int position = 0;

    char **tokens = (char **)malloc(bufsize * sizeof(char*));
    char *token = (char *)malloc(sizeof(char) * bufsize); 
    int i, j;

    token = strtok(line, " ");

    while (token != NULL)
    {
        tokens[position] = (char *)malloc(sizeof(token));
        // NOTE: changed to strcpy.
        strcpy(tokens[position], token);
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
    //tokens[position] = (char *)malloc(sizeof(token));
    free(token);
    tokens[position] = NULL;
    // for (i = 0; i < position; i++)
    // {
    //     printf("token: %s\n", tokens[i]);
    // }
    return tokens;
}

/*
Contributers:
Zac
Quang
*/
//Function to do a greeting shell
void init_shell() {
    clear();
    
    printf("\n\n\n\t ***MY SHELL***");
    printf("\n\n\n\t- Welcome to COP4600 - ASSIGNMENT #2 -");
    printf("\n\n\n\n*******************************************");
    
    printf("\n");
    sleep(1);
    
    char line[MAXLIST];
    ssize_t line_len;
    int i = 0;

    if(getcwd(currentdir, sizeof(currentdir)) == NULL) 
    {
        printf("Could not get current directory");
        return;
    }

    if(historyFile = fopen("history.txt", "r+"))
    {
        while ( fgets(line, MAXLIST, historyFile)) 
        {
            line[strcspn(line, "\n")] = 0;
            pushCommand(line);
        }
    }
    else
    {
        historyFile = fopen("history.txt", "w+");
    }
}

/*
Contributers:
Quang
*/
char *read_line(void) {
    int bufsize = MAXCOM;
    char *buffer = (char *)malloc(bufsize * sizeof(char));
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
        
        //if the length of the string is over our limit, then we have a realloc to create more blocks for them. 
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

/*
Contributers:
Zac
Lauren
Joey
*/
int ownCmdHandler(char *buffer) {
    int NoOfOwnCmds = 8, i, switchOwnArg = 0;

    char *ListOfOwnCmds[] = {"byebye", "movetodir", "background", "start",
                             "history", "replay", "dalek", "whereami"};
    //char* ListOfOwnCmds[NoOfOwnCmds];
    char* cmd = (char *)malloc(sizeof(char) * MAXLIST); // was set to NULL
    int parsedLength = -1;
    char **parsed;

    parsed = parse_command_line(buffer);

    // Get the parsed length.
    while(parsed[++parsedLength] != NULL) {
    }

    for (i = 0; i < NoOfOwnCmds; i++) {
        if (strcmp(parsed[0], ListOfOwnCmds[i]) == 0) {
            switchOwnArg = i + 1;
            break;
        }
    }
  
    switch (switchOwnArg) {
    case 1:
        pushCommand(buffer);
        for (i = 0; i < parsedLength; i++)
           free(parsed[i]);
        free(parsed);
        byebye();
    case 2:
        movetoDir(parsed[1]);
        for (i = 0; i < parsedLength; i++)
           free(parsed[i]);
        free(parsed);
        return 1;
    case 3:
        background(parsed, parsedLength);
        for (i = 0; i < parsedLength; i++)
           free(parsed[i]);
        free(parsed);
        return 1;
    case 4:
        start(parsed, parsedLength);
        for (i = 0; i < parsedLength; i++)
            free(parsed[i]);
        free(parsed);
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
        for (i = 0; i < parsedLength; i++)
            free(parsed[i]);
        free(parsed);
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
        for (i = 0; i < parsedLength; i++)
           free(parsed[i]);
        free(parsed);
        free(cmd);
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
        for (i = 0; i < parsedLength; i++)
            free(parsed[i]);
        free(parsed);
        return 1;
    case 8:
        whereami();
        for (i = 0; i < parsedLength; i++)
            free(parsed[i]);
        free(parsed);
        return 1;
    default:
        break;
    }
    return 0;
}

/*
Contributers:
Zac
Quang
Joey
*/
void process_commands(void) {
    int bufsize = MAXCOM;
    //char *buffer = malloc(sizeof(char) * bufsize);
    char * buffer;

    while(1) {
        // printf("# ");
        buffer = readline("# ");
        
        ownCmdHandler(buffer);
        pushCommand(buffer);
        free(buffer);
    }
}

/*
Contributers:
Zac
Joey
*/
// getting ../ to work and 
// changing the directory
void movetoDir(char *parsed_arg) {
    char path[MAXLIST];
    char temp[MAXLIST];
    char directory[MAXLIST];

    strcpy(directory, parsed_arg);

    // Lauren
    // Check input string
    if((directory == NULL) || (directory[0] == '\0')) {
        printf("No directory entered.\n");
        return;
    }

    // Check if full path or relative
    if(isFullPath(directory) == 0) {
        // Append to current dir
        strcpy(temp, currentdir);
        strcat(temp, "/");
        strcat(temp, directory);
    }
    else {
        // Change the current directory
        strcpy(temp, directory); 
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

}

/*
Contributers:
Quang or Joey?
*/
//Prints the value of the currentdir variable
void whereami() {
    if(currentdir == NULL) {
        printf("Nowhere.\n");
        return;
    }
    else {
        printf("%s\n", currentdir);
    }
    
}

/*
Contributers:
Zac
*/
// Clears history when -c flag is given
void clearHistory()
{
    while(!isempty())
    {
        popCommand();
    }
    historyFile = fopen("history.txt", "w");
}

/*
Contributers:
Zac
*/
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

/*
Contributers:
Zac
Lauren
*/
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

/*
Contributers:
Quang
Zac
Lauren
*/
//the start function.
void start(char **command_tokens, int length) 
{
    pid_t pid = fork();
    int i;
    char path[MAXLIST];
    char** command = (char **)malloc(sizeof(char *) * length);
    FILE * file;
    // printf("length: %d\n", length);
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
        command[i - 1] = (char *)malloc(sizeof(command_tokens[i]));
        strcpy(command[i - 1], command_tokens[i]);
        //printf("cmd[i-1]: %s\n", command[i-1]);
    }
    
    //command[length] = (char *)malloc(sizeof(char) * MAXLIST);
    command[length - 1] = NULL;

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
        else
        {
            // Close the open file.
            fclose(file);
            if (execv(path, command) == -1)
            {
                // printf("path: %s\n", path);
                //printf("com: %s\n", command);
                printf("Program could not be executed.\n");
            }
        }
    }

    for (i = 0; i < length - 1; i++)
    {
        printf("i %d\n", i);
        free(command[i]);
        // printf("freed\n");
    }

    free(command);
    
}

//It is similar to the run command, but it immediately prints the PID of the program it 
// started, and returns the prompt. 
// Execute a program in the background
/*
Contributers:
Joey
Zac
*/
void background(char **command_tokens, int length) {
    pid_t pid = fork();
    FILE *file;
    int i;
    char path[MAXLIST];
    char** command = (char **)malloc(sizeof(char *) * length);

    for (i = 1; i < length; i++)
    {
        command[i - 1] = (char *)malloc(sizeof(command_tokens[i]));
        strcpy(command[i - 1], command_tokens[i]);
    }

    if (pid > 0)
    {
        printf("PID: %d\n", pid);
    }
    
    //command[length] = (char *)malloc(sizeof(char) * MAXLIST);
    command[length - 1] = NULL;

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
        else
        {
            // Close the open file.
            fclose(file);
            if (execv(path, command) == -1)
            {
                // printf("path: %s\n", path);
                // //printf("com: %s\n", command);
                // printf("%s\n", command[0]);
                printf("Program could not be executed.\n");
            }
        }
    }

    // printf("%s\n", command[0]);

    for (i = 0; i < length - 1; i++)
    {
        free(command[i]);
    }

    // printf("%s\n", command[0]);

    free(command);

}

/*
Contributers:
Joey
*/
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
    // currentdir = malloc(sizeof(char) * MAXLIST);
    init_shell(); //initialize the shell
    char line[MAXLIST];

    //this line will process the command after initialize the shell
    process_commands();

    return 0;
}