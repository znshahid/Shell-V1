/******************************************************************************

Zainab Shahid
OPSYS Jochen
Extended Shell

*******************************************************************************/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <ctype.h>
#include <errno.h>
#include <dirent.h>

#define MAX_STRING 2058
#define DELIMITER " "
#define MAX_TOKENS 100
#define MAX_TOKEN_LENGTH 100

extern char **environ; //using global variable environ that contains all env variables
int is_background = 0;

//function to tokenize input and store the tokens into an array
char **Tokenize(char *input) 
{
    //allocate dynamic memory for the array using malloc
    char **tokens = malloc(MAX_TOKENS * sizeof(char *));
    if (tokens == NULL){
        perror("no input found.");
        exit(EXIT_FAILURE);
    }
    
    int token_count = 0; //initialize token_count at 0
    
    //removing any leading spaces that are not a delimiter
    while (isspace(*input)){
        input++;
    }
    
   //using strtok to tokenize input
    char *token = strtok(input, DELIMITER);
    while (token != NULL && token_count < MAX_TOKENS)
    {
        //create buffer array to allocate memory
        tokens[token_count] = malloc(MAX_TOKEN_LENGTH * sizeof(char));
        
        //using built in command strcpy to copy tokens into buffer array
        strcpy(tokens[token_count], token);
    
       //increment token count and increment pointer until end of string
       token_count++;
       
       while (isspace(*++input)) {
           //no body needed
           }
        
        if (token_count >= MAX_TOKENS) { //handle if tokens are more than max
            break;
        }
        
    //get next token
        token = strtok(NULL, DELIMITER);
    }
    //set array to null before loop terminates
    tokens[token_count] = NULL;
    
     for (int i = 0; tokens[i] != NULL; i++) {
        if (strcmp(tokens[i], "&") == 0) {
            is_background = 1;
            tokens[i] = NULL;
            break;
        }
     }    
    //return values in array
    return tokens;
}

//function to terminate program using exit commands
void ExitFunc (int terminate)
  {
    printf ("exiting shell program.");
    exit (terminate); //terminate program
  }

//function to change directory and specify path
void ChangeDirectory (char **tokens, int num_tokens)
  {
    //changing to home directory if no path is specified
    if (tokens[1] == NULL){
        char *homeDIR = getenv("HOME"); //get home directory using getenv
        int change = chdir(homeDIR);
        printf ("changed directory to %s\n", homeDIR);
    }
    //else change to specified directory
    else {
    //using built in command stat to check file type to check if 2nd input is a directory
    struct stat path; 
    int stat_check = stat(tokens[1], &path); //check second token

    if (stat_check != 0) { //if the path does not exist
      printf("Error: Directory does not exist.\n");
    } else if (!S_ISDIR(path.st_mode)) { //if path exists but is not a directory
      printf("Error: %s is not a directory.\n", tokens[1]);
    } else { //else if path exists and is a directory
      int change = chdir(tokens[1]); //change directory to specified path
      if (change < 0){
        printf ("not successful.");
      } else{
        printf ("changed directory to %s\n", tokens[1]);
      }
    }
} //the cd function only checks the first two tokens and disregards the rest
}

//function to get the print current working directory
void printCWD() {
    char cwd[MAX_STRING];
  if (getcwd (cwd, sizeof (cwd)) != NULL)
    {
     printf ("< @ %s > " , cwd);
    }
}

//function to get the current pid
void getPID() {
    pid_t pid = getpid ();
    printf ("current PID is %d\n", pid);
}

//function to handle the kill command
void kill_command(char **tokens, int num_tokens) {
       //determine the signal to send
    int signal_num = SIGTERM; // default signal is SIGTERM
    if (num_tokens == 3) { //if 3 inputs
        //get the signal number from the second token
        char *signal_str = tokens[1] + 1; //disregard the -
        signal_num = atoi(signal_str); //using atoi to convert char into int
    }

    //extract the pid from the last token
    pid_t pid = atoi(tokens[num_tokens - 1]);

    //check if pid exists
    if (kill(pid, signal_num) == -1) { //if kill returns -1
        if (errno == ESRCH) { //use error code ESRCH
            fprintf(stderr, "kill: no such process\n");
        } else {
            perror("kill");
        }
    }
}

//function that uses built in command setenv to set environment variables
void set_env(char **tokens, int num_tokens) { //takes string as argumument
    //if no variable input
    if (num_tokens == 1) { //using built in command setenv
        setenv(tokens[0], "", 1); //sets variable to empty string
    }
    //if one variable input
    if (num_tokens == 2) {
        setenv(tokens[1], tokens[1], 1); //set variable to specified variable
    }
    //if two variable input
    if (num_tokens == 3) { 
        setenv(tokens[2], tokens[1], 1); //sets variable to value of first
    }
    
    //if > two tokens 
    else if (num_tokens > 2) {
    fprintf(stderr, "Error: too many arguments.\n"); //print error message
    }
   
}

//function to print the environment variables
void print_env() {
    char **env = environ; 
    while (*env != NULL) {
        printf("%s\n", *env); //print each element until null
        env++;
    }
}

//function to handle which command
//at first i used malloc to allocate memory for a separate string
//that stored the path command, however i kept getting a malloc corrupt message
//so i tried it this way
void which(char **tokens, int num_tokens) {
    if (num_tokens < 2) { //if no file path is specified
        printf("Enter file name\n");
        return;
    }
    char **path_tokens = Tokenize(getenv("PATH")); //tokenize the path variable

    //loop through each directory in the path variable
    for (int i = 0; path_tokens[i] != NULL; i++) {
        //create string that contains path to the executable file
        char file_path[MAX_TOKENS];
        snprintf(file_path, MAX_TOKENS, "%s/%s", path_tokens[i], tokens[1]);

        //check if the file is executable
        if (access(file_path, X_OK) == 0) { //using access
            printf("%s\n", file_path); //print the file path
            return;
        }
    }
 
    //free the memory allocated for the path tokens
    for (int i = 0; i < MAX_TOKENS; i++) {
        free(path_tokens[i]);
    }
    free(path_tokens);
}


//function to list directories
void list(char **tokens, int num_tokens) {
    DIR *dir;
    struct dirent *ent;
//using built in commands opendir and readdir by including dirrent.h
    if (num_tokens == 1) {
        //if no arguments list files in cwd
        dir = opendir("."); //open current directory
        if (dir == NULL) {
            perror("opendir"); //error message if it fails
            return;
        }
        while ((ent = readdir(dir)) != NULL) { //read directory once its opened
            printf("%s\n", ent->d_name); //prints files in directory
        }

        closedir(dir); //close the directory
    } else { //if more than one token
        //list files in each specified directory
        for (int i = 1; i < num_tokens; i++) {
            printf("\n%s:\n", tokens[i]);
            dir = opendir(tokens[i]); //open each directory
            if (dir == NULL) {
                perror("opendir");
                continue;
            }

            while ((ent = readdir(dir)) != NULL) { //read each directory
                printf("%s\n", ent->d_name);
            }

            closedir(dir); //close directory
        }
    }
}

//moved execution implementation out of main and created a new function to handle commands
pid_t executeCommand(char **tokens, int num_tokens) {
    char *first_token = tokens[0];
    pid_t pid = fork();
    if (pid == -1) {
        fprintf(stderr, "Error: fork failed\n");
        exit(EXIT_FAILURE);
        //child process
    } else if (pid == 0) {
        //build in command handling
        if (strcmp(first_token, "cd") == 0) {
            ChangeDirectory(tokens, num_tokens);
            exit(0);
        } else if (strcmp(first_token, "setenv") == 0) {
            set_env(tokens, num_tokens);
            exit(0);
        } else if (strcmp(first_token, "pid") == 0) {
            getPID();
            exit(0);
        } else if (strcmp(first_token, "pwd") == 0) {
            printCWD();
            exit(0);
        } else if (strcmp(first_token, "kill") == 0) {
            kill_command(tokens, num_tokens);
            exit(0);
        } else if (strcmp(first_token, "which") == 0) {
            which(tokens, num_tokens);
            exit(0);
        } else if (strcmp(first_token, "printenv") == 0) {
            print_env();
            exit(0);
        } else if (strcmp(first_token, "list") == 0 || strcmp(first_token, "ls") == 0) {
            list(tokens, num_tokens);
            exit(0);
        } else {
            //check if command is an absolute or relative path
            if (first_token[0] == '/' || strncmp(first_token, "./", 2) == 0 || strncmp(first_token, "../", 3) == 0) {
                if (access(first_token, X_OK) == 0) { //using access to check if file is executable
                    execve(first_token, tokens, environ); //execute command using execve
                    perror("execve"); //error if fails
                    exit(EXIT_FAILURE);
                } else { //if command is not found
                    fprintf(stderr, "Error: command not executable or not found\n");
                    exit(EXIT_FAILURE);
                }
        } else {
            // Execute other commands
            execvp(tokens[0], tokens); //execvp replaces current process image with new
            perror("execvp");
            exit(EXIT_FAILURE);
        }
        }
    } else {
         if (!is_background) {
            int status;
            waitpid(pid, &status, 0);
        } else {
            printf("Background job started. PID: %d\n", pid);
            return 0;
        }
   // Reset the background flag for the next command
    is_background = 0;
    return pid;
    }
}
 
//main function  
int main()
{
    char input[MAX_STRING];
    int pid;
//loop to keep prompting user for input and not exit after each command
 while (1) {
     is_background = 0;
    printCWD();
  fflush(stdout); //i noticed prompt was delayed so im flushing the output stream
  fgets (input, MAX_STRING, stdin); 
  input[strcspn(input, "\n")] = '\0'; //using strcspn to remove newline character from input
  
  char **tokens = Tokenize(input);

    //loop through each value in array and count the number of tokens
        int i = 0;
        while (tokens[i] != NULL) {
            i++;
        }
       
        //check for exit function before executing other commands that require fork
        if (strcmp(tokens[0], "quit") == 0 || strcmp(tokens[0], "exit") == 0 || strcmp(tokens[0], "logout") == 0) {
            ExitFunc(0);
        }
       executeCommand(tokens, i);
    
   // Reset the background flag for the next command
    is_background = 0;
    
    // Check for any completed background processes
    pid_t bg_pid;
    int bg_status;
    bg_pid = waitpid(-1, &bg_status, WNOHANG);
    if (bg_pid > 0) {
        printf("Background job with PID %d has completed.\n", bg_pid);
    }
   
          //check if input is empty
        if (tokens[0] == NULL) {
            free(tokens); //free memory if it is
            continue; //and continue prompting user for input
        }
 
//free memory allocated for tokens
        for (i = 0; i < MAX_TOKENS; i++) {
            free(tokens[i]);
        }
        free(tokens);
    }
    return 0;
}

