#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <string.h>
#include <errno.h>

int main(int argc, char* argv[]){
    if (argc <= 1) {
        printf("You need to provide at least one .txt file to be converted to .epub\n");
        return EXIT_FAILURE;
    }

    int n = argc - 1; //Number of files to be converted
    int pid = 0; //Process id
    int status = 0; //Status of the process

    char** zipArguments;
    zipArguments = malloc(16*n*sizeof(char));
    zipArguments[0] = malloc(5); zipArguments[1] = malloc(12);
    strcpy(zipArguments[0], "zip"); strcpy(zipArguments[1], "ebooks.zip"); zipArguments[argc+1] = NULL;

    for(int i=0 ; i<n ; i++){
        char* outputName = malloc(strlen(argv[i+1]) + 5);
        strcpy(outputName, argv[i+1]);
        outputName[strlen(argv[i+1]) - 4] = '\0';
        strcat(outputName, ".epub");

        zipArguments[i+2] = malloc(strlen(outputName));
        strcpy(zipArguments[i+2], outputName);

        pid = fork();
        if(pid == 0){
            printf("[pid%d] converting %s\n", getpid(), argv[i+1]);
            char *args[] = {"pandoc","--quiet", argv[i+1], "-o", outputName, NULL};
            if(execvp("pandoc", args)<0){ printf("Error in execvp converting to .epub\n"); return EXIT_FAILURE;}
            free(outputName);
        }
        else{
            for(int i=0 ; i<n ; i++){
                wait(&status);
            }
        }
    }

    if(execvp("zip", zipArguments)<0){ printf("Error in execvp while zipping"); return EXIT_FAILURE;}

    for(int i=0 ; i<16*n ; i++){
        free(zipArguments[i]);
    }    
    free(zipArguments);
    return EXIT_SUCCESS;
}