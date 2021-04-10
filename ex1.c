/*
 * ex1.c
 * Ilana Sivan
 * ID: 205634272
 */

#include <sys/types.h>
#include <sys/stat.h>

#include <fcntl.h>
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define MAX_BUFFER_SIZE 65536
#define DESTINATION_FILE_MODE S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH

extern int opterr, optind;

void exit_with_usage(const char *message) {
    fprintf(stderr, "%s\n", message);
    fprintf(stderr, "Usage:\n\tex1 [-f] BUFFER_SIZE SOURCE DEST\n");
    exit(EXIT_FAILURE);
}

void append_file(const char *source_file, const char *dest_file, int buffer_size, int force_flag) {
    /*
     * Append source_file content to dest_file, buffer_size bytes at a time.
     * If force_flag is true, then also create dest_file if does not exist. Otherwise print error, and exit.
     *
     * TODO:
     * 	1. Open source_file for reading
     * 	2. Open dest_file for writing (Hint: is force_flag true?)
     * 	3. Loop reading from source and writing to the destination buffer_size bytes each time
     * 	4. Close source_file and dest_file
     *
     *  ALWAYS check the return values of syscalls for errors!
     *  If an error was found, use perror(3) to print it with a message, and then exit(EXIT_FAILURE)
     */

    int input_file, output_file; /* Input and output files */
    int read_in, write_out; /* Read and write */
    char buffer[MAX_BUFFER_SIZE]; /* Create buffer */

    // Open source file
    input_file = open(source_file, O_RDONLY);

    if (input_file == -1) {
        perror("Unable to open source file for reading");
        exit(EXIT_FAILURE);
    }


    // Open destination file or create if it doesn't exist

    if (force_flag) {
        output_file = open(dest_file, O_WRONLY | O_APPEND | O_CREAT | O_TRUNC, DESTINATION_FILE_MODE);
    } else {
        output_file = open(dest_file, O_WRONLY | O_APPEND);
    }

    if (output_file == -1) {
        close(input_file);
        perror("Unable to open destination file for writing");
        exit(EXIT_FAILURE);
    }



    // Append buffer content
    while ((read_in = read(input_file, buffer, buffer_size)) > 0) {

        write_out = write(output_file, buffer, read_in);

        if (write_out == -1) {
            close(input_file);
            close(output_file);
            perror("Unable to append to destination file");
            exit(EXIT_FAILURE);
        }

        if (read_in != write_out) {
            close(input_file);
            close(output_file);
            perror("Unable to append buffer content to destination file");
            exit(EXIT_FAILURE);
        }
    }

    if (read_in < 0) {
        close(input_file);
        close(output_file);
        perror("Unable to read source file");
        exit(EXIT_FAILURE);
    }


    /* Close input and destination files file */
    if (close(input_file) == -1) {
        close(output_file);
        perror("Unable to close source file");
        exit(EXIT_FAILURE);
    }

    if (close(output_file) == -1) {
        perror("Unable to close destination file");
        exit(EXIT_FAILURE);
    }


    printf("Content from file %s was successfully appended to %s\n", source_file, dest_file);
    exit(EXIT_SUCCESS);
}

void parse_arguments(
        int argc, char **argv,
        char **source_file, char **dest_file, int *buffer_size, int *force_flag) {
    /*
     * parses command line arguments and set the arguments required for append_file
     */
    int option_character;

    opterr = 0; /* Prevent getopt() from printing an error message to stderr */

    while ((option_character = getopt(argc, argv, "f")) != -1) {
        switch (option_character) {
            case 'f':
                *force_flag = 1;
                break;
            default:  /* '?' */
                exit_with_usage("Unknown option specified");
        }
    }

    if (argc - optind != 3) {
        exit_with_usage("Invalid number of arguments");
    } else {
        *source_file = argv[argc - 2];
        *dest_file = argv[argc - 1];
        *buffer_size = atoi(argv[argc - 3]);

        if (strlen(*source_file) == 0 || strlen(*dest_file) == 0) {
            exit_with_usage("Invalid source / destination file name");
        } else if (*buffer_size < 1 || *buffer_size > MAX_BUFFER_SIZE) {
            exit_with_usage("Invalid buffer size");
        }
    }
}

int main(int argc, char **argv) {
    int force_flag = 0; /* force flag default: false */
    char *source_file = NULL;
    char *dest_file = NULL;
    int buffer_size = MAX_BUFFER_SIZE;

    parse_arguments(argc, argv, &source_file, &dest_file, &buffer_size, &force_flag);

    append_file(source_file, dest_file, buffer_size, force_flag);

    return EXIT_SUCCESS;
}
