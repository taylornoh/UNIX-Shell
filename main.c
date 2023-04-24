#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>

// Parsing Function
char** parse(char* s) {
  static char* words[500];
  memset(words, 0, sizeof(words));

  char break_chars[] = " \t";

  int i = 0;
  char* p = strtok(s, break_chars);
  words[i++] = p;
  while (p != NULL) {
    p = strtok(NULL, break_chars);
    words[i++] = p;
  }
  return words;
}

// Main Function
int main(int argc, const char * argv[]) {
  char input[BUFSIZ];
  char last_command[BUFSIZ];
  int i;

  memset(input, 0, BUFSIZ * sizeof(char));
  memset(input, 0, BUFSIZ * sizeof(char));
  bool finished = false;

  // Start of main while loop
  while (!finished) {
    printf("osh> ");                                // Prompting user
    fflush(stdout);

    if ((fgets(input, BUFSIZ, stdin)) == NULL) {    // Checking for null input
      fprintf(stderr, "no command entered\n");
      exit(1);
    }
    input[strlen(input) - 1] = '\0';                // Wiping out newline at end of string
    printf("input was: \n'%s'\n", input);

    // History section of main while loop
    if (strncmp(input, "!!", 2) == 0) {
      if (strlen(last_command) == 0) {
        fprintf(stderr, "no last command to execute\n");
      }
      printf("last command was: %s\n", last_command);
    } else if (strncmp(input, "exit", 4) == 0) {
      finished = true;
    } else {
      strcpy(last_command, input);
      printf("You entered: %s\n", input);

      // Forking Child Process section of main while loop
      char** words = parse(input);
      pid_t pid = fork();
      if (pid < 0) {
        printf("\nFailed forking child..");
        return 0;
      } else if (pid == 0) {
        // Redirection checking
        int fd_in = 0;
        int fd_out = 1;
        for (int i = 0; words[i] != NULL; i++) {
          if (strcmp(words[i], "<") == 0) {
            fd_in = open(words[i+1], O_RDONLY);
            dup2(fd_in, STDIN_FILENO);
            close(fd_in);
            words[i] = NULL;
            words[i+1] = NULL;
          } else if (strcmp(words[i], ">") == 0) {
            fd_out = creat(words[i+1], 0644);
            dup2(fd_out, STDOUT_FILENO);
            close(fd_out);
            words[i] = NULL;
            words[i+1] = NULL;
          }
        }
        // Invoking execvp in child process after Redirection
        execvp(words[0],words);
        printf("\n");
        return 0;

      } else {
        wait(NULL);                             // Waiting for child to terminate
      }
      printf("\n");
    }
  }

  printf("osh exited\n");
  printf("program finished\n");

  return 0;
}
