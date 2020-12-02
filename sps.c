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
 * @def LAST_CELL Flag for the last cell in the row
 */
#define LAST_CELL 0
/**
 * @def LAST_ROW Flag for the last row in the file
 */
#define LAST_ROW 1
/**
 * @def CELL_START_CAPACITY Start capacity (number of chars) for the cell
 */
#define CELL_START_CAPACITY 1
/**
 * @def ROW_START_CAPACITY Start capacity (max number of cells) for the row
 */
#define ROW_START_CAPACITY 1
/**
 * @def TABLE_START_CAPACITY Start capacity (max number of rows) for the table
 */
#define TABLE_START_CAPACITY 1
/**
 * @def SPECIAL_CHARS List of special characters (they must be escaped)
 */
#define SPECIAL_CHARS "\"\\"

/**
 * @def streq(first, second) Check if first equals second
 */
#define streq(first, second) (strcmp(first, second) == 0)
/**
 * @def strc(str, c) Check if string (str) contains char (c)
 */
#define strc(str, c) (strchr(str, c) != NULL)

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
 * @typedef Individual table cell
 * @field data Cell's content
 * @field size Size of the cell's content
 * @field capacity How many chars can be in the cell
 */
typedef struct cell {
    char *data;
    unsigned int size;
    unsigned int capacity;
} Cell;
/**
 * @typedef Individual table row
 * @field cells Cells in the row
 * @field size Number of cells in the row
 * @field capacity How many cells can be in the row
 */
typedef struct row {
    Cell *cells;
    unsigned int size;
    unsigned int capacity;
} Row;
/**
 * @typedef The whole table
 * @field rows Rows in the table
 * @field size Number of rows in the table
 * @field capacity How many cells can be in the row
 */
typedef struct table {
    Row *rows;
    unsigned int size;
    unsigned int capacity;
} Table;
/**
 * @typedef Command for data selection or manipulating with them
 * @field name Command's name (selections have the same name "select")
 * @field intParams Parameters of type integer
 * @field strParams Parameters of type string
 */
typedef struct command {
    char *name;
    int intParams[4];
    char *strParams[4];
} Command;

// Input/output functions
Table *loadTableFromFile(FILE *file, char *delimiters);
Row *loadRowFromFile(FILE *file, char *delimiters, char *flag);
Cell *loadCellFromFile(FILE *file, char *delimiters, char *flag);
void saveTableToFile(Table *table, FILE *file, char *delimiters);
void writeErrorMessage(const char *message);
// Functions for working with table and its components
Table *createTable();
Row *createRow();
Cell *createCell();
Cell *copyCell(const Cell *sourceCell);
ErrorInfo addRowToTable(Table *table, Row *row, unsigned int position);
ErrorInfo addColumnToTable(Table *table, Cell *cell, unsigned int position);
ErrorInfo addCellToRow(Row *row, Cell *cell, unsigned int position);
ErrorInfo addCharToCell(Cell *cell, char c, unsigned int position);
void deleteRowFromTable(Table *table, unsigned int position);
void deleteColumnFromTable(Table *table, unsigned int columnNumber);
void deleteCellFromTable(Table *table, unsigned int position);
ErrorInfo alignRowSizes(Table *table);
void trimRows(Table *table);
ErrorInfo resizeTable(Table *table, unsigned int rows, unsigned int columns);
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
    // Open the file for reading
    FILE *fileRead;
    if ((fileRead = fopen(inputFile, "r")) == NULL) {
        writeErrorMessage("Zadany soubor se nepodarilo otevrit pro cteni.");

        return EXIT_FAILURE;
    }

    // Load data from file
    Table *table;
    if ((table = loadTableFromFile(fileRead, *delimiters)) == NULL) {
        writeErrorMessage("Nepodarilo se nacist tabulku z duvodu chyby pri alokaci pameti.");

        destructTable(table);
        fclose(fileRead);
        return EXIT_FAILURE;
    }

    // Close the read file
    fclose(fileRead);

    /* OUTPUT SAVING */
    // Open the file for writing
    FILE *fileWrite;
    if ((fileWrite = fopen(inputFile, "w")) == NULL) {
        writeErrorMessage("Zadany soubor se nepodarilo otevrit pro zapis.");

        return EXIT_FAILURE;
    }

    // Write output to the file
    saveTableToFile(table, fileWrite, *delimiters);

    // Deallocate table and close the write file
    destructTable(table);
    fclose(fileWrite);

    return EXIT_SUCCESS;
}

/**
 * Constructs table with data from a file
 * @param fileName Name of the input file
 * @param delimiters Column delimiters
 * @return Loaded table
 */
Table *loadTableFromFile(FILE *file, char *delimiters) {
    // Prepare new table
    Table *table;
    if ((table = createTable()) == NULL) {
        return NULL;
    }

    // Load table data
    char flag = -1;
    while (flag != LAST_ROW) {
        // Get the row data
        Row *row;
        if ((row = loadRowFromFile(file, delimiters, &flag)) == NULL) {
            return NULL;
        }

        // Add the row at the end of the table (table->size == last index + 1)
        if ((addRowToTable(table, row, table->size)).error) {
            return NULL;
        }
    }

    // Align rows to the same number of columns
    if (alignRowSizes(table).error) {
        return NULL;
    }

    return table;
}

/**
 * Constructs row with data from a file
 * @param file The file with data for the row
 * @param delimiters Column delimiters
 * @param flag Flag for returning special states
 * @return Loaded row
 */
Row *loadRowFromFile(FILE *file, char *delimiters, char *flag) {
    // Prepare new row
    Row *row;
    if ((row = createRow()) == NULL) {
        return NULL;
    }

    // Load row data
    while (*flag != LAST_ROW && *flag != LAST_CELL) {
        // Get the cell data
        Cell *cell;
        if ((cell = loadCellFromFile(file, delimiters, flag)) == NULL) {
            return NULL;
        }

        // Add the cell to the end of the row (row->size == last index + 1)
        if ((addCellToRow(row, cell, row->size)).error) {
            return NULL;
        }
    }

    if (*flag == LAST_CELL) {
        *flag = -1;
    }

    return row;
}

/**
 * Constructs cell with data from a file
 * @param file The file with data for the row
 * @param delimiters Column delimiters
 * @param flag Flag for returning special states
 * @return Loaded cell
 */
Cell *loadCellFromFile(FILE *file, char *delimiters, char *flag) {
    // Prepare the cell
    Cell *cell;
    if ((cell = createCell()) == NULL) {
        return NULL;
    }

    // Load data from file
    int prevC = '\0'; // Previous loaded char
    int c; // Loaded char
    bool ignoreDelimiters = false;
    while ((c = getc(file)) != EOF && c != '\n' && (!strc(delimiters, c) || ignoreDelimiters)) {
        if (c == '"' && prevC != '\\') {
            ignoreDelimiters = !ignoreDelimiters;
        } else if (!strc(SPECIAL_CHARS, c) || prevC == '\\'){
            addCharToCell(cell, (char)c, cell->size);
        }

        prevC = c;
    }

    // Detect the last row and the last cell (by cause of the while end)
    if (c == '\n') {
        *flag = LAST_CELL;
    }

    if ((c = getc(file)) == EOF) {
        *flag = LAST_ROW;
    }
    ungetc(c, file); // Put the char back to the file

    return cell;
}

/**
 * Saves table data to the file
 * @param table Table to save
 * @param file The file to save the table into
 * @param delimiter Column delimiter
 */
void saveTableToFile(Table *table, FILE *file, char *delimiters) {
    // Trim rows of the table
    trimRows(table);

    // Main delimiter
    char mainDelimiter = delimiters[0];

    for (unsigned i = 0; i < table->size; i++) {
        for (unsigned j = 0; j < table->rows[i].size; j++) {
            Cell *cell = &(table->rows[i].cells[j]);

            // Check if borders for cell contains delimiter are required
            bool borders = false;
            for (unsigned k = 0; k < strlen(delimiters); k++) {
                if (strc(cell->data, delimiters[k])) {
                    borders = true;

                    break;
                }
            }

            // Print left border
            if (borders) {
                fputc('"', file);
            }

            for (unsigned k = 0; k < cell->size; k++) {
                // Add backslash before escaped characters
                if (strc(SPECIAL_CHARS, cell->data[k])) {
                    fputc('\\', file);
                }

                // Print char from cell data
                fputc(cell->data[k], file);
            }

            // Print right border
            if (borders) {
                fputc('"', file);
            }

            // Add delimiter if not last
            if (j + 1 < table->rows[i].size) {
                fputc(mainDelimiter, file);
            }
        }

        // Add line break
        fputc('\n', file);
    }
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
 * @return Pointer to the new table or NULL if error occurred
 */
Table *createTable() {
    Table *table;
    if ((table = malloc(sizeof(Table))) == NULL) {
        return NULL;
    }

    table->size = 0;
    table->capacity = TABLE_START_CAPACITY;

    if ((table->rows = malloc(TABLE_START_CAPACITY * sizeof(Row))) == NULL) {
        free(table);
        return NULL;
    }

    return table;
}

/**
 * Creates a new row
 * @return Pointer to the new row or NULL if error occurred
 */
Row *createRow() {
    Row *row;
    if ((row = malloc(sizeof(Row))) == NULL) {
        return NULL;
    }

    row->size = 0;
    row->capacity = ROW_START_CAPACITY;

    if ((row->cells = malloc(ROW_START_CAPACITY * sizeof(Cell))) == NULL) {
        free(row);
        return NULL;
    }

    return row;
}

/**
 * Creates a new cell
 * @return Pointer to the new cell or NULL if error occurred
 */
Cell *createCell() {
    Cell *cell;
    if ((cell = malloc(sizeof(Cell))) == NULL) {
        return NULL;
    }

    cell->size = 0;
    cell->capacity = CELL_START_CAPACITY;

    // The last '\0' --> + 1
    if ((cell->data = malloc((CELL_START_CAPACITY + 1) * sizeof(char))) == NULL) {
        free(cell);
        return NULL;
    }
    memset(cell->data, '\0', cell->capacity + 1);

    return cell;
}

/**
 * Makes a copy of the cell
 * @param sourceCell Source cell to copy
 * @return Deep copy of the provided cell or NULL if error occurred
 */
Cell *copyCell(const Cell *sourceCell) {
    Cell *cell;
    if ((cell = createCell()) == NULL) {
        return NULL;
    }

    cell->capacity = sourceCell->capacity;
    cell->size = sourceCell->size;

    // The last '\0' --> + 1
    if ((cell->data = malloc((cell->capacity + 1) * sizeof(char))) == NULL) {
        free(cell);
        return NULL;
    }
    memcpy(cell->data, sourceCell->data, cell->capacity + 1);

    return cell;
}

/**
 * Adds a row to a table
 * @param table Table to edit
 * @param row Row to add to the table
 * @param position Position in the table (0 = first)
 * @return Error information
 */
ErrorInfo addRowToTable(Table *table, Row *row, unsigned int position) {
    ErrorInfo err = {.error = false};

    // Resizing table if needed
    if (table->capacity < (table->size + 1)) {
        if ((table->rows = realloc(table->rows, table->capacity * 2 * sizeof(Row))) == NULL) {
            err.error = true;
            err.message = "Nepodarilo se rozsirit pametovy prostor pro tabulku.";

            return err;
        }

        table->capacity *= 2;
    }

    // Free up space on specified position
    for (unsigned i = table->size; i > position; i--) {
        table->rows[i + 1] = table->rows[i];
    }

    // Insert the row to the specified position
    table->rows[position] = *row;
    table->size++;

    // Row has been inserted into table, the pointer won't be needed
    free(row);
    return err;
}

/**
 * Adds a column to the table (inserts cell with the same data to all of the rows)
 * @param table Table to edit
 * @param cell Cell to insert
 * @param position Position in the table (0 = first)
 * @return Error information
 */
ErrorInfo addColumnToTable(Table *table, Cell *cell, unsigned int position) {
    ErrorInfo err = {.error = false};

    // Add cell to every row at specified position
    for (unsigned i = 0; i < table->size; i++) {
        Cell *cellCopy = copyCell(cell);
        if ((err = addCellToRow(&(table->rows[i]), cellCopy, position)).error) {
            return err;
        }
    }

    // Cell has been inserted into table, the pointer won't be needed
    free(cell);
    return err;
}

/**
 * Adds a cell to a row
 * @param row Row to edit
 * @param cell Cell to add to the row
 * @param position Position in the row (0 = first)
 * @return Error information
 */
ErrorInfo addCellToRow(Row *row, Cell *cell, unsigned int position) {
    ErrorInfo err = {.error = false};

    // Resizing the row if needed
    if (row->capacity < (row->size + 1)) {
        if ((row->cells = realloc(row->cells, row->capacity * 2 * sizeof(Cell))) == NULL) {
            err.error = true;
            err.message = "Nepodarilo se rozsirit pametovy prostor pro radek.";

            return err;
        }

        row->capacity *= 2;
    }

    // Free up the space on specified position
    for (unsigned i = row->size; i > position; i--) {
        row->cells[i + 1] = row->cells[i];
    }

    // Insert the cell to the specified position
    row->cells[position] = *cell;
    row->size++;

    // Cell has been inserted into row, the pointer won't be needed
    free(cell);
    return err;
}

/**
 * Adds a char to the cell
 * @param cell Cell to edit
 * @param c Char to insert
 * @param position Position in the cell (0 = first)
 * @return Error information
 */
ErrorInfo addCharToCell(Cell *cell, char c, unsigned int position) {
    ErrorInfo err = {.error = false};

    // Resize data for the cell if needed
    if (cell->capacity < (cell->size + 1)) {
        // The last '\0' --> + 1
        if ((cell->data = realloc(cell->data, (2 * cell->capacity + 1) * sizeof(char))) == NULL) {
            err.error = true;
            err.message = "Nepodarilo se rozsirit pametovy prostor pro bunku.";

            return err;
        }

        cell->capacity *= 2;
    }

    // Fill newly allocated space with zero bytes
    memset(&(cell->data[cell->size]), '\0', cell->capacity - cell->size + 1);

    // Free up the space on specified position
    for (unsigned i = cell->size; i > position; i--) {
        cell->data[i + 1] = cell->data[i];
    }

    // Append char to the cell data (cell.size == last index + 1)
    cell->data[position] = c;
    cell->size++;

    return err;
}

/**
 * Deletes the row from the table
 * @param table Table to edit
 * @param position Position with the row to delete
 */
void deleteRowFromTable(Table *table, unsigned int position) {
    // Move rows to replace and fill the deleted position
    for (unsigned i = (int)position; i < table->size - 1; i++) {
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
    for (unsigned i = 0; i < table->size; i++) {
        // Move cells to replace and fill the deleted position
        for (unsigned j = (int)columnNumber; j < table->rows[i].size - 1; j++) {
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
    unsigned biggestRow = 0;
    for (unsigned i = biggestRow + 1; i < table->size; i++) {
        if (table->rows[i].size > table->rows[biggestRow].size) {
            biggestRow = i;
        }
    }

    // Set number of cells in each row by the row with the most cells
    for (unsigned i = 0; i < table->size; i++) {
        for (unsigned j = table->rows[i].size; j < table->rows[biggestRow].size; j++) {
            // Prepare empty cell
            Cell *cell;
            if ((cell = createCell()) == NULL) {
                err.error = true;
                err.message = "Nepodarilo se alokovat pamet pro novou bunku.";

                return err;
            }

            if ((err = addCellToRow(&(table->rows[i]), cell, j)).error) {
                return err;
            }
        }
    }

    return err;
}

/**
 * Trims rows of the table (removes empty column at the end of the table)
 * @param table Table to edit
 */
void trimRows(Table *table) {
    // Get the maximum number of columns in the row
    unsigned mostColumns = 0;
    for (unsigned i = 0; i < table->size; i++) {
        unsigned validColumns = 0;
        for (unsigned j = 0; j < table->rows[i].size; j++) {
            if (table->rows[i].cells[j].size != 0) {
                validColumns = j + 1;
            }
        }

        if (validColumns > mostColumns) {
            mostColumns = validColumns;
        }
    }

    // Delete all unnecessary columns
    for (unsigned j = table->rows[0].size; j > mostColumns; j--) {
        deleteColumnFromTable(table, j);
    }
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
    for (unsigned i = table->rows[0].size; i < columns; i++) {
        // Prepare the new cell
        Cell *cell;
        if ((cell = createCell()) == NULL) {
            err.error = true;
            err.message = "Nepodarilo se alokovat pamet pro novou bunku.";

            return err;
        }

        // Add the cell to the row
        if ((err = addCellToRow(&(table->rows[0]), cell, i)).error) {
            return err;
        }
    }

    // Add missing rows
    for (unsigned i = table->size; i < rows; i++) {
        // Prepare the new row
        Row *row;
        if ((row = createRow()) == NULL) {
            err.error = true;
            err.message = "Nepodarilo se alokovat pamet pro novy radek.";

            return err;
        }

        // Add the row into table
        if ((err = addRowToTable(table, row, i)).error) {
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
    // In case the table has been already destructed
    if (table == NULL) {
        return;
    }

    for (unsigned i = 0; i < table->size; i++) {
        destructRow(&(table->rows[i]));
    }

    free(table->rows);
    table->capacity = 0;
    table->size = 0;

    free(table);
}

/**
 * Destruct row (= deallocates all of its allocated memory)
 * @param row Row to be destructed
 */
void destructRow(Row *row) {
    // In case the row has been already destructed
    if (row == NULL) {
        return;
    }

    for (unsigned i = 0; i < row->size; i++) {
        destructCell(&(row->cells[i]));
    }

    free(row->cells);
    row->capacity = 0;
    row->size = 0;
}

/**
 * Destruct cell (= deallocates all of its allocated memory)
 * @param cell Cell to be destructed
 */
void destructCell(Cell *cell) {
    // In case the cell has been already destructed
    if (cell == NULL) {
        return;
    }

    free(cell->data);
    cell->size = 0;
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
    Cell *cell = &(table->rows[row].cells[column]);
    int newSize = (int)strlen(newValue);

    // Resize for the new value
    // The last '\0' --> + 1
    if ((cell->data = realloc(cell->data, (newSize + 1) * sizeof(char))) == NULL) {
        err.error = true;
        err.message = "Nepodarilo se rozsirit pametovy prostor bunky.";

        return err;
    }
    cell->capacity = newSize + 1;

    // Set the new value
    memcpy(cell->data, newValue, newSize + 1);
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