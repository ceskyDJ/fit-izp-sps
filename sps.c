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
#include <stdbool.h>

/**
 * @def DEFAULT_DELIMITER Default delimiter for case user didn't set different
 */
#define DEFAULT_DELIMITER " "
/**
 * Flag for the last item
 */
#define LAST_ITEM "LAST_ITEM"

/**
 * @def streq(first, second) Check if first equals second
 */
#define streq(first, second) (strcmp(first, second) == 0)

/**
 * @typedef Error information tells how some action ended
 * @field error Did it end with error? (=> if true, something bad happened; otherwise the operation was successful)
 * @field message Description message in case of error = true
 */
typedef struct errorInfo {
    bool error;
    char *message;
} ErrorInfo;
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
ErrorInfo loadTableFromFile(Table * table, char *file);
ErrorInfo loadRowFromFile(Row *row, char *file);
ErrorInfo loadCellFromFile(Cell *cell, char *file);
void writeErrorMessage(const char *message);

int main(int argc, char **argv) {
    ErrorInfo err = {.error = false};

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
    char *inputFile = argv[skippedArgs];

    /* DATA PARSING */
    Table table;
    if ((err = loadTableFromFile(&table, inputFile)).error) {
        writeErrorMessage(err.message);

        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

/**
 * Constructs table with data from a file
 * @param table Where to save the table
 * @param file The file with data for table
 * @return Error information
 */
ErrorInfo loadTableFromFile(Table *table, char *file) {
    ErrorInfo err = {.error = false};

    return err;
}

/**
 * Constructs row with data from a file
 * @param row Where to save the row
 * @param file The file with data for the row
 * @return Error information or special state for the last row in the table
 */
ErrorInfo loadRowFromFile(Row *row, char *file) {
    ErrorInfo err = {.error = false};

    return err;
}

/**
 * Constructs cell with data from a file
 * @param cell Where to save the cell
 * @param file The file with data for the row
 * @return Error information or special state for the last cell in the row
 */
ErrorInfo loadCellFromFile(Cell *cell, char *file) {
    ErrorInfo err = {.error = false};

    FILE *source;
    if ((source = fopen(file, "r")) == NULL) {
        writeErrorMessage("Soubor predany ve vstupnich argumentech se nepodarilo otevrit.");

    }

    return err;
}

/**
 * Writes error message to standard error output
 * @param message Error message
 */
void writeErrorMessage(const char *message) {
    fprintf(stderr, "sps: %s", message);
}