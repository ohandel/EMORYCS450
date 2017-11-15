/* THIS CODE IS MY OWN WORK, IT WAS WRITTEN WITHOUT CONSULTING
A TUTOR OR CODE WRITTEN BY OTHER STUDENTS - OWEN HANDEL */
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <memory.h>
#include <ctype.h>

int main(int argc, char **argv) {

    int fds[2]; //fds for pipe A
    int pds[2]; //fds for pipe B
    pid_t pid, gid; //process IDS
    FILE *stream; 
    char buffer[512];
    int chars;
    char c;
    pipe(fds); //making pipe A
    pipe(pds); //making pipe B

    pid = fork(); //making first child

    if (pid == 0) {
        close(fds[1]); //close write end of pipe A
        close(pds[0]); //closing read end of pipe B
        dup2(fds[0], STDIN_FILENO); //redirecting the standard input to the the read end of pipe A
        dup2(pds[1], STDOUT_FILENO); //redirecting the standard output to the write end of pipe B
        close(pds[1]); //closing the write of pipe B
        close(fds[0]); //closing the read end of pipe A
        execlp("sort", "sort", NULL); //exec sort 0 and 1 remain open as they are open when exec was called
        }

    gid = fork(); //child number 2

    if (gid == 0) {
        close(fds[1]); //closing the write end of pipe A
        close(fds[0]); //closing the read end of pipe A
        close(pds[1]); //closing the write end of pipe B
        dup2(pds[0], STDIN_FILENO); //redirecting the standard input to read end of pipe B
        close(pds[0]); //close the old pipe B read
        stream = fdopen(STDIN_FILENO, "r"); //open stream
        int count = 0;
        int length;
        char *str[500000];
        while (fgets(buffer, 512, stream) != NULL) {
                str[count] = strndup(buffer, 35); //copy str from buffer to new array of in which each index holds a string from the buffer
                count++;
            }
        str[count] = "";
        int mult[count]; //array for word multiplicty
        int countMult = 1; //starts at one as every word recorded has a multiplicity of at least one
        for (int i = 0; i <= count; i++) {
            if ((i > 0) && ((strcmp(str[i], str[i - 1])) == 0) && str[i] != "") {
                str[i - 1] = "";
                countMult++; //each time duplicate recorded count up
            }else {
                mult[i-1] = countMult;
                countMult = 1;
            }
        }
        printf("Word Conditions: <5 have been discarded, >35 have been truncated, words alphabetized, any special characters removed, word multiplicity recorded\n\n");
        printf("The constructed list of words from the input given: \n\n");
        for (int i = 0; i < count; i++) { //iterating through the array containing the strings
            length = strlen(str[i]);
           if(length > 5 && length < 35 && (str[i] != "")){
            printf("%sMultiplicity: %d\n\n", str[i],mult[i]); //print multiplicity under word
           }else if(length == 35){
               printf("%s\nMultiplicity: %d\n\n", str[i],mult[i]); //printing multiplicity if word was truncated formatting
           }
        }
     fclose(stream); //close input stream
    }
  else {
        close(pds[1]); //close write end of pipe B
        close(pds[0]); //close read end of pipe B
        close(fds[0]); //close read end of pipe A
        stream = fdopen(fds[1], "w"); //input stream to write end of pipe A
        while ((chars = fgetc(stdin)) != EOF) { //parsing
            c = (char) tolower(chars); //to lowercase
            if (c == ' ' || c == '\n') {
                fputc('\n', stream);
            } else if (isalpha(chars) !=0) {
                fputc(c, stream);
            }
        }
        fclose(stream); //close input stream
        close(fds[1]); // close write end of pipe A
         while (wait(NULL) > 0) {   //loop will hang until all children are complete
            }
        }
    }
