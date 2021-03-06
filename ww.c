#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <ctype.h>
#include <string.h>
#include <dirent.h>
#include "ww.h"
#include "strbuf.h"

// Global variables for find_token
int isNewLine = 0;
int paragraphBreakPrinted = 1;
int EndOfFile = 0;

void exit_error(char *reason){
    perror(reason);
    exit(EXIT_FAILURE);
}

int find_token(int width, int fd, strbuf_t *tok){
    char *buf = malloc(sizeof(char));
    if (!buf){
        exit_error("Malloc failed to aquire space.\n");
    }

    // Begin reading through doc
    while (read(fd, buf, sizeof(char)) > 0){
        // The only whitespace that matters is newline, everything else ends the word
        if (isspace(*buf)){
            if (*buf == '\n'){
                isNewLine++;
                free(buf);
                return EXIT_SUCCESS;
            }
            else{
                free(buf);
                return EXIT_SUCCESS;
            }
        }
        else{
            // Append the char to the strbuf
            sb_append(tok, *buf);
            // Reset both flags to false
            isNewLine = 0;
            paragraphBreakPrinted = 1;
        }
    }

    // If reaching here EOF
    EndOfFile = 1;
    free(buf);
    return EXIT_SUCCESS;
}

int read_file(int width, int fd, int fd_out){
    // Current pointer to pos in line
    int pos = 0;
    strbuf_t tok;
    sb_init(&tok, width);
    // Static variables for convenience 
    char *newLine = "\n";
    char *space = " ";
    int out = fd_out > 0 ? fd_out : 1;
    // Parse through the words in the input
    while(find_token(width, fd, &tok) != EXIT_FAILURE){
        // check if EOF was hit
        if (EndOfFile){
            break;
        }
        // Check if tok->length > width
        if (tok.used > width){
            // Print out word and return exit_failure
            int res = write(out, newLine, 1);
            if (res < 0)
                exit_error("Write error.\n");
            res = write(out, tok.data, tok.used);
            if (res < 0)
                exit_error("Write error.\n");
            // Return exit failure
            exit_error("Word does not fit width/\n");
        }
        // Check if pos + tok->length is greater than width
        else if ( (pos+tok.used) > width){
            // Proceed to next line
            int res = write(out, newLine, 1);
            if (res < 0)
                exit_error("Write error.\n");
            // Write word in new line
            res = write(out, tok.data, tok.used);
            if (res < 0)
                exit_error("Write error.\n");
            // Update pos
            pos = tok.used;
        }
        else{
            // Write word in line
            int res = write(out, tok.data, tok.used);
            if (res < 0)
                exit_error("Write error.\n");
            // Update pos
            pos += tok.used;
        }
        // Check for paragraph break
        if (isNewLine == 2 && paragraphBreakPrinted && pos != 0){
            int res = write(out, newLine, 1);
            if (res < 0)
                exit_error("Write error.\n");
            res = write(out, newLine, 1);
            if (res < 0)
                exit_error("Write error.\n");
            pos = 0;
            paragraphBreakPrinted = 0;
            isNewLine = 0;
        }
        // Print a space after word
        if (pos + 1 >= width){
            // Print newline instead
            int res = write(out, newLine, 1);
            if (res < 0)
                exit_error("Write error.\n");
            pos = 0;
        }
        else{
            if (tok.used > 0){
                int res = write(out, space, 1);
                if (res < 0)
                    exit_error("Write error.\n");
                pos++;
            }
        }

        // Reset the buffer
        tok.length = width;
        tok.used = 0;
    }

    // Free token and return
    write(out, newLine, 1);
    sb_destroy(&tok);
    return EXIT_SUCCESS;
}

int ww(int width, char *file, int fd_out){
    // Attempt to open given file
    int fd = open(file, O_RDONLY);
    int out = fd_out > 0 ? fd_out : 0;
    if (fd == -1)
        exit_error("Invalid file path.\n");
    if (read_file(width, fd, out) == EXIT_FAILURE)
        exit_error("Read file error.\n");
    return EXIT_SUCCESS;
}

int ww_dir(int width, char *directory){
    DIR *dirp = opendir(directory); // open the current directory
    // If dirp is null error
    if (dirp == NULL)
        exit_error("Failed to open directory.\n");

    struct dirent *de;
    char *file_name = malloc(256*sizeof(char));
    char *file_out = malloc(256*sizeof(char));
    if (!(file_name&&file_out))
        exit_error("Malloc failed to acquire enough memory.\n");

    // Iterate over everything in the directory
    while ((de = readdir(dirp))){
        // create filepath for the file
        char *wrap = "wrap.";
        char *name = de->d_name;
        strcat(file_name, directory);
        strcat(file_name, name);
        int res = 1;
        // Check if file starts with "wrap."
        for (int i = 0;i<5;i++){
            if (*(wrap + i) != *(name+i)){
                res = 0;
                break;
            }
        }
        if (res == 1){
            strcat(file_out, directory);
            strcat(file_out, name);
        }
        else{
            strcat(file_out, directory);
            strcat(file_out, wrap);
            strcat(file_out, name);
        }

        // if d_type == 4, it's a directory, skip
        if (de->d_type == 4){
            // Reset the file_name string
            memset(file_name, '\0', 256 * sizeof(char));
            memset(file_out, '\0', 256 * sizeof(char));
            continue;
        }

        // if d_type == 8, it's a file
        if (de->d_type == 8){
            // use file
            int fd_out = open(file_out, O_WRONLY | O_CREAT);
            if (fd_out == -1)
                exit_error("Out file failed to open.\n");
            if (ww(width, file_name, fd_out) == EXIT_FAILURE)
                exit(EXIT_FAILURE);
        }
        // Reset the file_name string
        memset(file_name, '\0', 256*sizeof(char));
        memset(file_out, '\0', 256*sizeof(char));
    }

    if (closedir(dirp) < 0)
        exit_error("Failed to close directory.\n");

    free(file_name);
    free(file_out);
    return EXIT_SUCCESS;
}

int ww_cmd(int width){
    // Call read_file with width and stdin
    if (read_file(width, 0, 0) == EXIT_FAILURE)
        exit_error("Read_file error.\n");

    return EXIT_SUCCESS;
}