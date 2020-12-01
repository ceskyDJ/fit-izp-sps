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
 * Start capacity (max number of cells) for the row
 */
#define ROW_START_CAPACITY 1
/**
 * Start capacity (max number of rows) for the table
 */
#define TABLE_START_CAPACITY 1

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
 * @field capacity How many cells can be in the row
 */
typedef struct row {
    Cell *cells;
    int size;
    int capacity;
} Row;
/**
 * The whole table
 * @field rows Rows in the table
 * @field size Number of rows in the table
 * @field capacity How many cells can be in the row
 */
typedef struct table {
    Row *rows;
    int size;
    int capacity;
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
ErrorInfo copyCell(Cell *newCell, const Cell *sourceCell);
ErrorInfo addRowToTable(Table *table, const Row *row, int position);
ErrorInfo addColumnToTable(Table *table, const Cell *cell, int position);
ErrorInfo addCellToRow(Row *row, const Cell *cell, int position);
void deleteRowFromTable(Table *table, unsigned int position);
void deleteColumnFromTable(Table *table, unsigned int columnNumber);
void deleteCellFromTable(Table *table, unsigned int position);
ErrorInfo alignRowSizes(Table *table);
void destructTable(Table *table);
void destructRow(Row *row);
void destructCell(Cell *cell);
ErrorInfo setCellValue(Table *table, unsigned int row, unsigned int column, const char *newValue);
char *getCellValue(Table *table, unsigned int row, unsigned int column);

/**
 * The main function
 * @param argc Number of input arguments
 * @param argv Input arguments "array"
 * @return Exit code
 */
int main(int argc, char **argv) {
    ErrorInfo err;

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

    if((table = malloc(sizeof(Table))) == NULL) {
        err.error = true;
        err.message = "Nepodarilo se alokovat pamet pro novou tabulku.";

        return err;
    }

    if ((table->rows = malloc(TABLE_START_CAPACITY * sizeof(Row))) == NULL) {
        free(table);

        err.error = true;
        err.message = "Nepodarilo se alokovat pamet pro data tabulky.";

        return err;
    }

    table->size = 0;
    table->capacity = TABLE_START_CAPACITY;

    return err;
}

/**
 * Creates a new row
 * @param row Where to save the row
 * @return Error information
 */
ErrorInfo createRow(Row *row) {
    ErrorInfo err = {.error = false};

    if ((row = malloc(sizeof(Row))) == NULL) {
        err.error = true;
        err.message = "Nepodarilo se alokovat pamet pro novy radek.";

        return err;
    }

    if ((row->cells = malloc(ROW_START_CAPACITY * sizeof(Cell))) == NULL) {
        free(row);

        err.error = true;
        err.message = "Nepodarilo se alokovat pamet pro data radku.";

        return err;
    }

    row->size = 0;
    row->capacity = ROW_START_CAPACITY;

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

    if ((cell = malloc(sizeof(Cell))) == NULL) {
        err.error = true;
        err.message = "Nepodarilo se alokovat pamet pro novou bunku.";

        return err;
    }

    if ((cell->data = malloc(strlen(content))) == NULL) {
        err.error = true;
        err.message = "Nepodarilo se alokovat pamet pro data bunky.";

        return err;
    }

    strcpy(cell->data, content);
    cell->size = (int)strlen(cell->data);

    return err;
}

/**
 * Makes a copy of the cell
 * @param newCell Where to save the new cell
 * @param sourceCell Source cell to copy
 * @return Error information
 */
ErrorInfo copyCell(Cell *newCell, const Cell *sourceCell) {
    return createCell(newCell, sourceCell->data);
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

    // Resizing table if needed
    if (table->capacity < (table->size + 1)) {
        if ((table->rows = realloc(table->rows, sizeof(Row) * table->capacity * 2)) == NULL) {
            destructTable(table);

            err.error = true;
            err.message = "Nepodarilo se rozsirit pametovy prostor pro tabulku.";

            return err;
        }
    }

    // Free up space on specified position
    for (int i = table->size; i >= position; i--) {
        table->rows[i + 1] = table->rows[i];
    }

    // Insert the row to the specified position
    table->rows[position] = *row;

    return err;
}

/**
 * Adds a column to the table (inserts cell with the same data to all of the rows)
 * @param table Table to edit
 * @param cell Cell to insert
 * @param position Position in the table (0 = first)
 * @return Error information
 */
ErrorInfo addColumnToTable(Table *table, const Cell *cell, int position) {
    ErrorInfo err = {.error = false};

    // Add cell to every row at specified position
    for (int i = 0; i < table->size; i++) {
        if ((err = addCellToRow(&table->rows[i], cell, position)).error) {
            destructTable(table);

            return err;
        }
    }

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

    // Resizing the row if needed
    if (row->capacity < (row->size + 1)) {
        if ((row->cells = realloc(row->cells, sizeof(Cell) * row->capacity * 2)) == NULL) {
            destructRow(row);

            err.error = true;
            err.message = "Nepodarilo se rozsirit pametovy prostor pro radek.";

            return err;
        }
    }

    // Free up the space on specified position
    for (int i = row->size; i > position; i--) {
        row->cells[i + 1] = row->cells[i];
    }

    // Insert the cell to the specified position
    row->cells[position] = *cell;

    return err;
}

/**
 * Deletes the row from the table
 * @param table Table to edit
 * @param position Position with the row to delete
 */
void deleteRowFromTable(Table *table, unsigned int position) {
    // Move rows to replace and fill the deleted position
    for (int i = (int)position; i < table->size - 1; i++) {
        table->rows[i] = table->rows[i + 1];
    }

    // The size has been changed
    table->size--;
}

/**
 * Deletes the column from the table
 * @param table Table to edit
 * @param columnNumber Number of column to delete
 */
void deleteColumnFromTable(Table *table, unsigned int columnNumber) {
    // Delete the cell on position columnNumber from every row of the table
    for (int i = 0; i < table->size; i++) {
        // Move cells to replace and fill the deleted position
        for (int j = (int)columnNumber; j < table->rows[i].size - 1; j++) {
            table->rows[i].cells[j] = table->rows[i].cells[j + 1];
        }

        // The size has been changed
        table->rows[i].size--;
    }
}

/**
 * Aligns all rows of the table to the same size
 * @param table Table to edit
 * @return Error information
 */
ErrorInfo alignRowSizes(Table *table) {
    ErrorInfo err = {.error = false};

    // Find number of cells in the biggest row (row with the most cells)
    int biggestRow = 0;
    for (int i = biggestRow + 1; i < table->size; i++) {
        if (table->rows[i].size > table->rows[biggestRow].size) {
            biggestRow = i;
        }
    }

    // Set number of cells in each row by the row with the most cells
    for (int i = 0; i < table->size; i++) {
        for (int j = table->rows[i].size; j < biggestRow; j++) {
            // Prepare empty cell
            Cell cell;
            if ((err = createCell(&cell, "")).error) {
                destructTable(table);

                return err;
            }

            if ((err = addCellToRow(&table->rows[i], &cell, j)).error) {
                destructTable(table);

                return err;
            }
        }
    }

    return err;
}

/**
 * Resizes the table to a new size
 * <strong>Warning! The table must be already aligned using alignRowSizes()</strong>
 * @param table Table to resize
 * @param rows New number of rows
 * @param columns New number of columns
 * @return Error information
 */
ErrorInfo resizeTable(Table *table, unsigned int rows, unsigned int columns) {
    ErrorInfo err = {.error = false};

    // Add missing columns to the first row (it will be distributed automatically by calling alignRowSizes() function)
    for (int i = table->rows[0].size; i < (int)columns; i++) {
        // Prepare the new cell
        Cell cell;
        if ((err = createCell(&cell, "")).error) {
            destructTable(table);

            return err;
        }

        // Add the cell to the row
        if ((err = addCellToRow(&table->rows[0], &cell, i)).error) {
            destructTable(table);

            return err;
        }
    }

    // Add missing rows
    for (int i = table->size; i < (int)rows; i++) {
        // Prepare the new row
        Row row;
        if ((err = createRow(&row)).error) {
            destructTable(table);

            return err;
        }

        // Add the row into table
        if ((err = addRowToTable(table, &row, i)).error) {
            destructTable(table);

            return err;
        }
    }

    // Add missing empty cells
    alignRowSizes(table);

    return err;
}

/**
 * Destructs table (= deallocates all of its allocated memory)
 * @param table Table to be destructed
 */
void destructTable(Table *table) {
    // Content
    for (int i = 0; i < table->size; i++) {
        destructRow(&table->rows[i]);
    }

    free(table->rows);
    table->capacity = 0;
    table->size = 0;

    // Table
    free(table);
}

/**
 * Destruct row (= deallocates all of its allocated memory)
 * @param row Row to be destructed
 */
void destructRow(Row *row) {
    // Content
    for (int i = 0; i < row->size; i++) {
        destructCell(&row->cells[i]);
    }

    free(row->cells);
    row->capacity = 0;
    row->size = 0;

    // Row
    free(row);
}

/**
 * Destruct cell (= deallocates all of its allocated memory)
 * @param cell Cell to be destructed
 */
void destructCell(Cell *cell) {
    // Content
    free(cell->data);
    cell->size = 0;

    // Cell
    free(cell);
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

    // Get cell and new value's size for easier manipulation
    Cell *cell = &table->rows[row].cells[column];
    int newSize = (int)strlen(newValue);

    // Resize for the new value
    if ((realloc(cell->data, newSize)) == NULL) {
        destructTable(table);

        err.error = true;
        err.message = "Nepodarilo se rozsirit pametovy prostor bunky.";

        return err;
    }

    // Set the new value
    memcpy(cell->data, newValue, newSize);
    cell->size = newSize;

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
    return table->rows[row].cells[column].data;
}