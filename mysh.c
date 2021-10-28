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
        printf("Command does not exist. Please use a valid command.");
        return NULL;
    }
   return stack[top - num];
}

// Made by Zac
char* popCommand() {
   char* data;
	
   if(!isempty()) {
      data = stack[top];
      top = top - 1;   
      return data;
   } else {
      printf("Could not retrieve data, Stack is empty.\n");
   }
}

// Made by Zac
char* pushCommand(char* data) {

   if(!isfull()) {
      top = top + 1;   
      stack[top] = data;
   } else {
      printf("Maximum number of commands met. Please clear your history\n");
   }
}

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
    char * line = NULL;
    size_t len = 0;
    ssize_t read;    

    if(historyFile = fopen("history.txt", "r+"))
    {
        while ((read = getline(&line, &len, historyFile)) != -1) 
        {
            pushCommand(line);
        }
    }
    else
    {
        historyFile = fopen("history.txt", "w+");
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
        pushCommand(buffer);
        
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
        printf("%d: %s", i, replay(i));
        i++;
    }
}

// Made by Zac
// Terminates shell and saves history file
void byebye()
{
    while(!isempty())
    {
        fprintf(historyFile, "%s\n", popCommand());
    }
    fclose(historyFile);
    exit(0);
}

void dalek(int pid)
{
    kill(pid, SIGKILL);
}

//made by Quang
// is this relative or direct path?
void start(char *program_name) {
    int pid = fork();
    char *argv[] = {program_name, NULL};
    
    // Find result of fork call.
    if(pid < 0)
    {
        perror("FORK FAILED\n");
        return;
    }
    
    else if (pid == 0) {
    //this would check if the argument starts with "/" the shell should interpret it as full path then.
    if (strchr(program_name, '/') != NULL) {
            // Edits made by Joey
            printf("Starting program: %s\n", program_name);
            execv(argv[0], argv);
        }
    }
    //if the argument isn't start with "/ the shell should interpreted as a relative path."
    else if (strchr(program_name, '/') == NULL) {
        printf("Starting program: %s\n", program_name);
        execv(argv[0], argv);
    }
    else {
        perror("the execution failed.\n");
    }

    //check to see if we are in the parent process
    if (pid>0) {
        waitpid(pid, NULL, 0);
    }
    if (pid == 0) {
        perror("the given program could not be found\n");

        return;
    }
}

//It is similar to the run command, but it immediately prints the PID of the program it 
// started, and returns the prompt. 
// Execute a program in the background



int ownCmdHandler(char **parsed) {
    int NoOfOwnCmds = 5, i, switchOwnArg = 0;
    char* ListOfOwnCmds[NoOfOwnCmds];
    char* username;  
  
    ListOfOwnCmds[0] = "byebye";
    ListOfOwnCmds[1] = "movetodir";
    ListOfOwnCmds[2] = "help";
    ListOfOwnCmds[3] = "hello";
    ListOfOwnCmds[4] = "history";
    ListOfOwnCmds[5] = "replay";
    ListOfOwnCmds[6] = "";
  
    for (i = 0; i < NoOfOwnCmds; i++) {
        if (strcmp(parsed[0], ListOfOwnCmds[i]) == 0) {
            switchOwnArg = i + 1;
            break;
        }
    }
  
    switch (switchOwnArg) {
    case 1:
        printf("\nGoodbye\n");
        exit(0);
    case 2:
        if (sizeof(parsed) / sizeof(parsed[0]) > 1)
        {
            chdir(parsed[1]);
        }
        else
        {
            printf("Incorrect number of arguments.");
        }
        return 1;
    case 3:
        // openHelp();
        return 1;
    case 4:
        // username = getenv("USER");
        // printf("\nHello %s.\nMind that this is "
        //     "not a place to play around."
        //     "\nUse help to know more..\n",
        //     username);
        return 1;
    case 5:
        if(sizeof(parsed) / sizeof(parsed[0]) > 1 && strcmp(parsed[1], "-c") == 0)
        {
            checkHistory(true);
        }
        else
        {
            checkHistory(false);
        }
    default:
        break;
    }
  
    return 0;
}

int main() {
    init_shell();
    char line[MAXLIST];

    //this line will process the command after initialize the shell
    process_commands();

    return 0;
}