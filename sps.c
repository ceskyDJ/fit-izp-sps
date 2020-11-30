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
 * Selection for the end of the collection (array)
 */
#define END_OF_COLLECTION -1

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
ErrorInfo loadTableFromFile(Table * table, const char *file);
ErrorInfo loadRowFromFile(Row *row, const char *file);
ErrorInfo loadCellFromFile(Cell *cell, const char *file);
void saveTableToFile(Table *table, const char *file);
void writeErrorMessage(const char *message);
// Functions for working with table and its components
ErrorInfo createTable(Table *table);
ErrorInfo createRow(Row *row);
ErrorInfo createCell(Cell *cell, const char *content);
ErrorInfo addRowToTable(Table *table, const Row *row, int position);
ErrorInfo addCellToRow(Row *row, const Cell *cell, int position);
ErrorInfo deleteRowFromTable(Table *table, unsigned int position);
ErrorInfo deleteColumnFromTable(Table *table, unsigned int columnNumber);
ErrorInfo deleteCellFromTable(Table *table, unsigned int position);
ErrorInfo alignRowSizes(Table *table);
ErrorInfo setCellValue(Table *table, unsigned int row, unsigned int column, const char *newValue);
char *getCellValue(Table *table, unsigned int row, unsigned int column);

/**
 * The main function
 * @param argc Number of input arguments
 * @param argv Input arguments "array"
 * @return Exit code
 */
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
    unsigned int skippedArgs = 1;
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
ErrorInfo loadTableFromFile(Table *table, const char *file) {
    ErrorInfo err = {.error = false};

    // TODO: implement the function...

    return err;
}

/**
 * Constructs row with data from a file
 * @param row Where to save the row
 * @param file The file with data for the row
 * @return Error information or special state for the last row in the table
 */
ErrorInfo loadRowFromFile(Row *row, const char *file) {
    ErrorInfo err = {.error = false};

    // TODO: implement the function...

    return err;
}

/**
 * Constructs cell with data from a file
 * @param cell Where to save the cell
 * @param file The file with data for the row
 * @return Error information or special state for the last cell in the row
 */
ErrorInfo loadCellFromFile(Cell *cell, const char *file) {
    ErrorInfo err = {.error = false};

    FILE *source;
    if ((source = fopen(file, "r")) == NULL) {
        writeErrorMessage("Soubor predany ve vstupnich argumentech se nepodarilo otevrit.");

        // TODO: implement the function...
    }

    return err;
}

/**
 * Saves table data to the file
 * @param table Table to save
 * @param file The file to save the table into
 */
void saveTableToFile(Table *table, const char *file) {
    // TODO: implement the function...
}

/**
 * Writes error message to standard error output
 * @param message Error message
 */
void writeErrorMessage(const char *message) {
    fprintf(stderr, "sps: %s", message);
}

/**
 * Creates a new table
 * @param table Where to save the table
 * @return Error information
 */
ErrorInfo createTable(Table *table) {
    ErrorInfo err = {.error = false};

    // TODO: implement the function...

    return err;
}

/**
 * Creates a new row
 * @param row Where to save the row
 * @return Error information
 */
ErrorInfo createRow(Row *row) {
    ErrorInfo err = {.error = false};

    // TODO: implement the function...

    return err;
}

/**
 * Creates a new cell
 * @param cell Where to save the cell
 * @param content Cells' content
 * @return Error information
 */
ErrorInfo createCell(Cell *cell, const char *content) {
    ErrorInfo err = {.error = false};

    // TODO: implement the function...

    return err;
}

/**
 * Adds a row to a table
 * @param table Table to edit
 * @param row Row to add to the table
 * @param position Position in the table (0 = first)
 * @return Error information
 */
ErrorInfo addRowToTable(Table *table, const Row *row, int position) {
    ErrorInfo err = {.error = false};

    // TODO: implement the function...

    return err;
}

/**
 * Adds a cell to a row
 * @param row Row to edit
 * @param cell Cell to add to the row
 * @param position Position in the row (0 = first)
 * @return Error information
 */
ErrorInfo addCellToRow(Row *row, const Cell *cell, int position) {
    ErrorInfo err = {.error = false};

    // TODO: implement the function...

    return err;
}

/**
 * Deletes the row from the table
 * @param table Table to edit
 * @param position Position with the row to delete
 * @return Error information
 */
ErrorInfo deleteRowFromTable(Table *table, unsigned int position) {
    ErrorInfo err = {.error = false};

    // TODO: implement the function...

    return err;
}

/**
 * Deletes the column from the table
 * @param table Table to edit
 * @param columnNumber Number of column to delete
 * @return Error information
 */
ErrorInfo deleteColumnFromTable(Table *table, unsigned int columnNumber) {
    ErrorInfo err = {.error = false};

    // TODO: implement the function...

    return err;
}

/**
 * Deletes the cell from the table
 * @param table Table to edit
 * @param position Position of the cell to delete
 * @return Error information
 */
ErrorInfo deleteCellFromTable(Table *table, unsigned int position) {
    ErrorInfo err = {.error = false};

    // TODO: implement the function...

    return err;
}

/**
 * Aligns all rows of the table to the same size
 * @param table Table to edit
 * @return Error information
 */
ErrorInfo alignRowSizes(Table *table) {
    ErrorInfo err = {.error = false};

    // TODO: implement the function...

    return err;
}

/**
 * Sets a new value to the selected cell of the table
 * @param table Table to edit
 * @param row Row selection
 * @param column Column selection
 * @param newValue New value
 * @return Error information
 */
ErrorInfo setCellValue(Table *table, unsigned int row, unsigned int column, const char *newValue) {
    ErrorInfo err = {.error = false};

    // TODO: implement the function...

    return err;
}

/**
 * Returns value of the selected cell of the table
 * @param table Table contains the selected cell
 * @param row Selected row
 * @param column Selected column
 * @return Value of the cell
 */
char *getCellValue(Table *table, unsigned int row, unsigned int column) {
    // TODO: implement the function...

    return "";
}