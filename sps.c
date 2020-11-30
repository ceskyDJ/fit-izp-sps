/*
 * SPS
 *
 * Simple spreadsheet editor
 *
 * @author Michal Šmahel <xsmahe01@stud.fit.vutbr.cz>
 * @date November-December 2020
 * @version 1.0
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * @def DEFAULT_DELIMITER Default delimiter for case user didn't set different
 */
#define DEFAULT_DELIMITER " "

/**
 * @def streq(first, second) Check if first equals second
 */
#define streq(first, second) (strcmp(first, second) == 0)

/**
 * Individual table cell
 * @field data Cell's content
 * @field size Size of the cell's content
 */
typedef struct cell {
    char *data;
    int size;
} Cell;
/**
 * Individual table row
 * @field cells Cells in the row
 * @field size Number of cells in the row
 */
typedef struct row {
    Cell *cells;
    int size;
} Row;
/**
 * The whole table
 * @field rows Rows in the table
 * @field size Number of rows in the table
 */
typedef struct table {
    Row *rows;
    int size;
} Table;

// Input/output functions
void writeErrorMessage(const char *message);

int main(int argc, char **argv) {
    /* ARGUMENTS PARSING */
    // Valid arguments: ./sps [-d DELIMITERS] <CMD_SEQUENCE> <FILE>
    // Check arguments count
    if (argc < 3) {
        writeErrorMessage("Nedostatecny pocet vstupnich argumentu.");

        return EXIT_FAILURE;
    } else if (argc > 5) {
        writeErrorMessage("Prekrocen maximalni pocet vstupnich argumentu.");

        return EXIT_FAILURE;
    }

    // Get delimiter from arguments
    int skippedArgs = 1;
    char **delimiters;
    if (argc == 5 && streq(argv[skippedArgs], "-d")) {
        delimiters = &argv[skippedArgs + 1];
        skippedArgs += 2;
    } else {
        const char *DELIMITER = DEFAULT_DELIMITER;
        delimiters = (char **) &DELIMITER;
    }

    // Get commands from arguments
    // TODO: Implement this...
    skippedArgs += 1;

    // Get file from arguments
    FILE *inputFile;
    if ((inputFile = fopen(argv[skippedArgs], "r+")) == NULL) {
        writeErrorMessage("Soubor predany ve vstupnich argumentech se nepodarilo otevrit.");

        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

/**
 * Writes error message to standard error output
 * @param message Error message
 */
void writeErrorMessage(const char *message) {
    fprintf(stderr, "sps: %s", message);
}