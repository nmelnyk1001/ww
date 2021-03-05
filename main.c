#include "ww.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

int main(int argc, char **argv){
    // ww can have one or two arguments
    if (argc > 3 || argc <= 1){
        perror("Incorrect number of arguments given.\n");
        return EXIT_FAILURE;
    }

    // Check page width (arg 1) is a positive number
    if (atoi(argv[1]) < 0){
        perror("Argument 1 must be a positive integer.\n");
        return EXIT_FAILURE;
    }

    // Determine what input method is given
    if (argc == 2){
        // Use commandline
        if (ww_cmd(atoi(argv[1])) == EXIT_FAILURE)
            return EXIT_FAILURE;
    }
    else{
        // Check if file is a directory
        struct stat statbuf;
        if (stat(argv[2], &statbuf) != 0){
            perror("Stat failed in main.\n");
            return EXIT_FAILURE;
        }

        if (S_ISREG(statbuf.st_mode)){
            // Handle regular file
            int width = atoi(argv[1]);
            if (ww(width, argv[2], 0) == EXIT_FAILURE){
                return EXIT_FAILURE;
            }
        }
        else if (S_ISDIR(statbuf.st_mode)){
            // Handle directories
            int width = atoi(argv[1]);
            if (ww_dir(width, argv[2]) == EXIT_FAILURE)
                return EXIT_FAILURE;
        }
        else{
            // Incompatible file type
            perror("Incompatible file type for ww.\n");
            return EXIT_FAILURE;
        }
    }
    return EXIT_SUCCESS;
}