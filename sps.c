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
#include <ctype.h>

/**
 * @def DEFAULT_DELIMITER Default delimiter for case user didn't set different
 */
#define DEFAULT_DELIMITER " "
/**
 * @def EMPTY_FLAG Flag with no information
 */
#define EMPTY_FLAG -1
/**
 * @def LAST_CELL Flag for the last cell in the row
 */
#define LAST_CELL 0
/**
 * @def LAST_ROW Flag for the last row in the file
 */
#define LAST_ROW 1
/**
 * @def INVALID_INPUT_FORMAT Flag for the input with invalid format (for ex. '"' at forbidden place)
 */
#define INVALID_INPUT_FORMAT 2
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
 * @def COMMAND_NAME_SIZE Maximum string length of the command name (without \0)
 */
#define COMMAND_NAME_SIZE 6
/**
 * @def COMMAND_PARAMS_SIZE Size of array with command parameters (maximum number of parameters, resp.)
 */
#define COMMAND_PARAMS_SIZE 4
/**
 * @def LAST_ROW_COL_NUMBER Number represents the last row or column in selection
 */
#define LAST_ROW_COL_NUMBER -1
/**
 * @def BAD_ROW_COL_NUMBER Number represents bad number of row or column provided in input
 */
#define BAD_ROW_COL_NUMBER 0
/**
 * @def CLASSIC_COMMAND Classic data manipulation command
 */
#define CLASSIC_COMMAND true
/**
 * @def SELECTION_COMMAND Command for editing selection
 */
#define SELECTION_COMMAND false
/**
 * @def NUMBER_OF_VARIABLES Number of temporary data variables (_0 to _9)
 */
#define NUMBER_OF_VARIABLES 10

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
    Cell **cells;
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
    Row **rows;
    unsigned int size;
    unsigned int capacity;
} Table;
/**
 * @typedef Command for data selection or manipulating with them
 * @field type Type of the command (classic or selection)
 * @field name Command's name (selections have the same name "select")
 * @field intParams Parameters of type integer
 * @field strParams Parameters of type string
 * @field next Pointer to the next command in the linked-list
 */
typedef struct command {
    bool type;
    char name[COMMAND_NAME_SIZE + 1];
    int intParams[COMMAND_PARAMS_SIZE];
    char *strParams[COMMAND_PARAMS_SIZE];
    struct command *next;
} Command;
/**
 * @typedef Sequence of loaded commands
 * @field firstCmd Pointer to the first command of the linked-list
 * @field lastCmd Pointer to the last command of the linked-list
 */
typedef struct commandSequence {
    Command *firstCmd;
    Command *lastCmd;
} CommandSequence;
/**
 * @typedef Selection of the table cells
 * @field rowFrom First row coordinate
 * @field rowTo Second row coordinate
 * @field colFrom First column coordinate
 * @field colTo Second column coordinate
 * @field curRow Current row (for iterating over selection)
 * @field curCol Current column (for iterating over selection)
 */
typedef struct selection {
    unsigned int rowFrom;
    unsigned int rowTo;
    unsigned int colFrom;
    unsigned int colTo;
    unsigned int curRow;
    unsigned int curCol;
} Selection;
/**
 * @typedef Temporary variables
 * @field sel Selection variable (_)
 * @field data Data variables (_0 to _9)
 * @field number Program internal variable for storing number between iterations
 */
typedef struct variables {
    Selection *sel;
    char *data[NUMBER_OF_VARIABLES];
    double number;
} Variables;

// Input/output functions
Table *loadTableFromFile(FILE *file, char *delimiters, signed char *flag);
Row *loadRowFromFile(FILE *file, char *delimiters, signed char *flag);
Cell *loadCellFromFile(FILE *file, char *delimiters, signed char *flag);
CommandSequence *loadCommandsFromString(const char *string, signed char *flag);
void saveTableToFile(Table *table, FILE *file, char *delimiters);
void writeErrorMessage(const char *message);
// Functions for working with table and its components
Table *createTable();
Row *createRow();
Cell *createCell();
ErrorInfo addRowToTable(Table *table, Row *row, unsigned int position);
ErrorInfo addColumnToTable(Table *table, unsigned int position);
ErrorInfo addCellToRow(Row *row, Cell *cell, unsigned int position);
ErrorInfo addCharToCell(Cell *cell, char c, unsigned int position);
void deleteRowFromTable(Table *table, unsigned int position);
void deleteColumnFromTable(Table *table, unsigned int columnNumber);
ErrorInfo alignRowSizes(Table *table);
void trimRows(Table *table);
ErrorInfo resizeTable(Table *table, unsigned int rows, unsigned int columns);
void destructTable(Table *table);
void destructRow(Row *row);
void destructCell(Cell *cell);
ErrorInfo setCellValue(Table *table, unsigned int row, unsigned int column, const char *newValue);
char *getCellValue(Table *table, unsigned int row, unsigned int column);
// Functions for working with commands
CommandSequence *createCmdSeq();
Command *createCmd();
void addNewCmdToSeq(CommandSequence *cmdSeq, Command *cmd);
void convertTypesInCommandParams(CommandSequence *cmdSeq);
void destructCommandSequence(CommandSequence *cmdSeq);
void destructCommand(Command *cmd);
ErrorInfo processCommands(CommandSequence *cmdSeq, Table *table);
// Functions for working with selection
Selection *createSelection();
void updateSelectionBySelection(Selection *sel, Selection *pattern);
void destructSelection(Selection *sel);
// Functions for working with temporary variables
Variables *createVars();
void destructVars(Variables *vars);
// Selection functions (implementations of the commands)
ErrorInfo standardSelect(Command *cmd, Table *table, Selection *sel, Variables *vars);
ErrorInfo windowSelect(Command *cmd, Table *table, Selection *sel, Variables *vars);
ErrorInfo minMaxSelect(Command *cmd, Table *table, Selection *sel, Variables *vars);
ErrorInfo findSelect(Command *cmd, Table *table, Selection *sel, Variables *vars);
// Data manipulation functions (implementations of the commands)
ErrorInfo irow(Command *cmd, Table *table, Selection *sel, Variables *vars);
ErrorInfo arow(Command *cmd, Table *table, Selection *sel, Variables *vars);
ErrorInfo drow(Command *cmd, Table *table, Selection *sel, Variables *vars);
ErrorInfo icol(Command *cmd, Table *table, Selection *sel, Variables *vars);
ErrorInfo acol(Command *cmd, Table *table, Selection *sel, Variables *vars);
ErrorInfo dcol(Command *cmd, Table *table, Selection *sel, Variables *vars);
ErrorInfo setEdit(Command *cmd, Table *table, Selection *sel, Variables *vars);
ErrorInfo clearEdit(Command *cmd, Table *table, Selection *sel, Variables *vars);
ErrorInfo swapEdit(Command *cmd, Table *table, Selection *sel, Variables *vars);
ErrorInfo sumAvgEdit(Command *cmd, Table *table, Selection *sel, Variables *vars);
ErrorInfo countEdit(Command *cmd, Table *table, Selection *sel, Variables *vars);
ErrorInfo lenEdit(Command *cmd, Table *table, Selection *sel, Variables *vars);
// Variable using functions
ErrorInfo defVars(Command *cmd, Table *table, Selection *sel, Variables *vars);
ErrorInfo useVars(Command *cmd, Table *table, Selection *sel, Variables *vars);
ErrorInfo incVars(Command *cmd, Table *table, Selection *sel, Variables *vars);
ErrorInfo setVars(Command *cmd, Table *table, Selection *sel, Variables *vars);
// Help functions
bool isValidNumber(char *number);

/**
 * The main function
 * @param argc Number of input arguments
 * @param argv Input arguments "array"
 * @return Exit code
 */
int main(int argc, char **argv) {
    // Data structure for passing error information from functions
    ErrorInfo err;
    // Flag for passing additional information from some functions
    signed char flag;

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
    CommandSequence *cmdSeq;
    flag = EMPTY_FLAG;
    if ((cmdSeq = loadCommandsFromString(argv[skippedArgs], &flag)) == NULL) {
        if (flag != INVALID_INPUT_FORMAT) {
            writeErrorMessage("Nepodarilo se nacist prikazy z duvodu chyby pri alokaci pameti.");
        } else {
            writeErrorMessage("Format prikazu ve vstupnim argumentu je chybny.");
        }

        destructCommandSequence(cmdSeq);
        return EXIT_FAILURE;
    }
    skippedArgs += 1;

    // Get file from arguments
    char *inputFile = argv[skippedArgs];

    /* DATA LOADING */
    // Open the file for reading
    FILE *fileRead;
    if ((fileRead = fopen(inputFile, "r")) == NULL) {
        writeErrorMessage("Zadany soubor se nepodarilo otevrit pro cteni.");

        return EXIT_FAILURE;
    }

    // Load data from file
    Table *table;
    flag = EMPTY_FLAG;
    if ((table = loadTableFromFile(fileRead, *delimiters, &flag)) == NULL) {
        if (flag == INVALID_INPUT_FORMAT) {
            writeErrorMessage("Vstupni soubor obsahuje bunku v chybnem formatu.");
        } else {
            writeErrorMessage("Nepodarilo se nacist tabulku z duvodu chyby pri alokaci pameti.");
        }

        destructTable(table);
        fclose(fileRead);
        return EXIT_FAILURE;
    }

    // Close the read file
    fclose(fileRead);

    /* DATA PARSING */
    if ((err = processCommands(cmdSeq, table)).error) {
        writeErrorMessage(err.message);

        return EXIT_FAILURE;
    }

    /* HELP DATA DEALLOCATION */
    // Commands
    destructCommandSequence(cmdSeq);

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
Table *loadTableFromFile(FILE *file, char *delimiters, signed char *flag) {
    // Prepare new table
    Table *table;
    if ((table = createTable()) == NULL) {
        return NULL;
    }

    // Load table data
    while (*flag != LAST_ROW) {
        // Get the row data
        Row *row;
        if ((row = loadRowFromFile(file, delimiters, flag)) == NULL) {
            return NULL;
        }

        // Add the row at the end of the table (table->size == last index + 1)
        if ((addRowToTable(table, row, table->size + 1)).error) {
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
Row *loadRowFromFile(FILE *file, char *delimiters, signed char *flag) {
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
        if ((addCellToRow(row, cell, row->size + 1)).error) {
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
Cell *loadCellFromFile(FILE *file, char *delimiters, signed char *flag) {
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
            // Border char at the start of the cell
            if (prevC == '\0') {
                ignoreDelimiters = true;
            } else {
                // At the first position has been border char and it's the last char of the cell
                int nextC;
                if (((nextC = getc(file)) == '\n' || strc(delimiters, nextC)) && ignoreDelimiters) {
                    // Next delimiter will end the cell
                    ignoreDelimiters = false;
                } else {
                    *flag = INVALID_INPUT_FORMAT;

                    return NULL;
                }
                ungetc(nextC, file); // Put the char back to the scope
            }
        } else if (!strc(SPECIAL_CHARS, c) || prevC == '\\'){
            addCharToCell(cell, (char)c, cell->size + 1);
        }

        prevC = c;
    }

    // The cell doesn't have end border char
    if (ignoreDelimiters) {
        *flag = INVALID_INPUT_FORMAT;

        return NULL;
    }

    // Detect the last row and the last cell (by cause of the while end)
    if (c == '\n') {
        *flag = LAST_CELL;
    }

    if ((c = getc(file)) == EOF) {
        *flag = LAST_ROW;
    }
    ungetc(c, file); // Put the char back to the scope

    return cell;
}

/**
 * Loads commands from string into command sequence
 * @param string String with commands
 * @param flag Flag for returning additional information
 * @return Command sequence with loaded commands
 */
CommandSequence *loadCommandsFromString(const char *string, signed char *flag) {
    CommandSequence *cmdSeq;
    if ((cmdSeq = createCmdSeq()) == NULL) {
        return NULL;
    }

    // Prepare first command
    Command *cmd;
    if ((cmd = createCmd()) == NULL) {
        return NULL;
    }

    // Parse string to commands
    unsigned i, cmdI, paramI;
    for (i = cmdI = paramI = 0; i < strlen(string); i++) {
        if (string[i] == ';') {
            // Close the command
            addNewCmdToSeq(cmdSeq, cmd);

            // Prepare the next command
            cmdI = 0;
            paramI = 0;
            if ((cmd = createCmd()) == NULL) {
                return NULL;
            }
        } else if(string[i] == ' ' && string[i - 1] != '\\') {
            // Move to the next parameter
            paramI++;
            // Reset parameter string iteration var
            cmdI = 0;
        } else {
            // Selection commands: [R, C], [R1,C1,R2,C2], [_] and [_,_]
            if (cmdI == 0 && string[i] == '[' && (isdigit(string[i + 1]) || string[i + 1] == '_')) {
                // Skip the '[' char
                i++;

                // Set the name for selection commands
                // Classic commands has already had name, so only process selection contains in its parameter
                if (paramI == 0) {
                    // Set a type and a name (this type of commands doesn't have a name in input string)
                    cmd->type = SELECTION_COMMAND;
                    memcpy(cmd->name, "select", 7);
                    paramI = 1;
                }

                // Load parameters
                while (string[i] != ']' && string[i] != ';') {
                    if (string[i] == ',') {
                        // Move to the next parameter
                        paramI++;
                        // Reset parameter string iteration var
                        cmdI = 0;
                    } else {
                        // Resize string parameters to cmd + 2 for the saving of the next char
                        // cmd + 2: indexing from 0 and space for '\0'
                        // [0] => name, [1] => firstParameter --> -1 (array with parameters start at index 0)
                        char **param = &(cmd->strParams[paramI - 1]);
                        char *tmp;
                        if ((tmp = realloc(*param, sizeof(char) * (cmdI + 2))) == NULL) {
                            return NULL;
                        } else {
                            *param = tmp;
                        }

                        // Save the char
                        (*param)[cmdI] = string[i];
                        (*param)[cmdI + 1] = '\0';
                    }

                    i++;
                }

                if (string[i] == ';') {
                    *flag = INVALID_INPUT_FORMAT;

                    return NULL;
                }

                continue;
            }

            // Skip ']' char at the end of selection commands
            if (string[i] == ']' && (string[i + 1] == ' ' || string[i + 1] == ';' || string[i + 1] == '\0')) {
                continue;
            }

            // Data processing commands and named selection commands ([min], [max], [find STR], [set])
            // Loading command name
            if (paramI == 0) {
                // Skip '[' char at the start of selection commands and set command type
                if (cmdI == 0 && string[i] == '[') {
                    cmd->type = SELECTION_COMMAND;
                    continue;
                }

                cmd->name[cmdI] = string[i];
            } else {
                // Skip escape char
                if (string[i] == '\\' && string[i - 1] != '\\') {
                    continue;
                }

                // Resize string parameters to cmd + 2 for the saving of the next char
                // cmd + 2: indexing from 0 and space for '\0'
                // [0] => name, [1] => firstParameter --> -1 (array with parameters start at index 0)
                char **param = &(cmd->strParams[paramI - 1]);
                char *tmp;
                if ((tmp = realloc(*param, sizeof(char) * (cmdI + 2))) == NULL) {
                    return NULL;
                } else {
                    *param = tmp;
                }

                // Save the char
                (*param)[cmdI] = string[i];
                (*param)[cmdI + 1] = '\0';
            }

            // Increment command name/parameter string iteration var
            cmdI++;
        }
    }

    // Close the last command
    addNewCmdToSeq(cmdSeq, cmd);

    // Convert string to int types of parameters
    convertTypesInCommandParams(cmdSeq);

    return cmdSeq;
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
        for (unsigned j = 0; j < table->rows[i]->size; j++) {
            Cell *cell = table->rows[i]->cells[j];

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
            if (j + 1 < table->rows[i]->size) {
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
    fprintf(stderr, "sps: %s\n", message);
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

    if ((table->rows = malloc(TABLE_START_CAPACITY * sizeof(Row *))) == NULL) {
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

    if ((row->cells = malloc(ROW_START_CAPACITY * sizeof(Cell *))) == NULL) {
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
 * Adds a row to a table
 * @param table Table to edit
 * @param row Row to add to the table
 * @param position Position in the table (1 = first)
 * @return Error information
 */
ErrorInfo addRowToTable(Table *table, Row *row, unsigned int position) {
    ErrorInfo err = {.error = false};

    // There are coordinates from the real world in row and column (indexed from 1) --> - 1
    position--;

    // Resizing table if needed
    if (table->capacity < (table->size + 1)) {
        if ((table->rows = realloc(table->rows, table->capacity * 2 * sizeof(Row *))) == NULL) {
            err.error = true;
            err.message = "Nepodarilo se rozsirit pametovy prostor pro tabulku.";

            return err;
        }

        table->capacity *= 2;
    }

    // Free up space on specified position
    for (unsigned i = table->size; i > position; i--) {
        table->rows[i] = table->rows[i - 1];
    }

    // Insert the row to the specified position
    table->rows[position] = row;
    table->size++;

    return err;
}

/**
 * Adds a column to the table (inserts empty cell to all of the rows)
 * @param table Table to edit
 * @param position Position in the table (1 = first)
 * @return Error information
 */
ErrorInfo addColumnToTable(Table *table, unsigned int position) {
    ErrorInfo err = {.error = false};

    // There are coordinates from the real world in row and column (indexed from 1) --> - 1
    position--;

    // Add cell to every row at specified position
    for (unsigned i = 0; i < table->size; i++) {
        Cell *cell;
        if ((cell = createCell()) == NULL) {
            err.error = true;
            err.message = "Nepodarilo se alokovat pamet pro novou bunku.";

            return err;
        }

        if ((err = addCellToRow(table->rows[i], cell, position + 1)).error) {
            return err;
        }
    }

    return err;
}

/**
 * Adds a cell to a row
 * @param row Row to edit
 * @param cell Cell to add to the row
 * @param position Position in the row (1 = first)
 * @return Error information
 */
ErrorInfo addCellToRow(Row *row, Cell *cell, unsigned int position) {
    ErrorInfo err = {.error = false};

    // There are coordinates from the real world in row and column (indexed from 1) --> - 1
    position--;

    // Resizing the row if needed
    if (row->capacity < (row->size + 1)) {
        if ((row->cells = realloc(row->cells, row->capacity * 2 * sizeof(Cell *))) == NULL) {
            err.error = true;
            err.message = "Nepodarilo se rozsirit pametovy prostor pro radek.";

            return err;
        }

        row->capacity *= 2;
    }

    // Free up the space on specified position
    for (unsigned i = row->size; i > position; i--) {
        row->cells[i] = row->cells[i - 1];
    }

    // Insert the cell to the specified position
    row->cells[position] = cell;
    row->size++;

    return err;
}

/**
 * Adds a char to the cell
 * @param cell Cell to edit
 * @param c Char to insert
 * @param position Position in the cell (1 = first)
 * @return Error information
 */
ErrorInfo addCharToCell(Cell *cell, char c, unsigned int position) {
    ErrorInfo err = {.error = false};

    // There are coordinates from the real world in row and column (indexed from 1) --> - 1
    position--;

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
        cell->data[i] = cell->data[i - 1];
    }

    // Append char to the cell data (cell.size == last index + 1)
    cell->data[position] = c;
    cell->size++;

    return err;
}

/**
 * Deletes the row from the table
 * @param table Table to edit
 * @param position Position with the row to delete (1 = first)
 */
void deleteRowFromTable(Table *table, unsigned int position) {
    // There are coordinates from the real world in row and column (indexed from 1) --> - 1
    position--;

    // Destruct the row
    destructRow(table->rows[position]);

    // Move rows to replace and fill the deleted position
    for (unsigned i = position; i < table->size - 1; i++) {
        table->rows[i] = table->rows[i + 1];
    }

    // The size has been changed
    table->size--;
}

/**
 * Deletes the column from the table
 * @param table Table to edit
 * @param columnNumber Number of column to delete (1 = first)
 */
void deleteColumnFromTable(Table *table, unsigned int columnNumber) {
    // There are coordinates from the real world in row and column (indexed from 1) --> - 1
    columnNumber--;

    // Delete the cell on position columnNumber from every row of the table
    for (unsigned i = 0; i < table->size; i++) {
        // Destruct the cell
        destructCell(table->rows[i]->cells[columnNumber]);

        // Move cells to replace and fill the deleted position
        for (unsigned j = (int)columnNumber; j < table->rows[i]->size - 1; j++) {
            table->rows[i]->cells[j] = table->rows[i]->cells[j + 1];
        }

        // The size has been changed
        table->rows[i]->size--;
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
        if (table->rows[i]->size > table->rows[biggestRow]->size) {
            biggestRow = i;
        }
    }

    // Set number of cells in each row by the row with the most cells
    for (unsigned i = 0; i < table->size; i++) {
        for (unsigned j = table->rows[i]->size; j < table->rows[biggestRow]->size; j++) {
            // Prepare empty cell
            Cell *cell;
            if ((cell = createCell()) == NULL) {
                err.error = true;
                err.message = "Nepodarilo se alokovat pamet pro novou bunku.";

                return err;
            }

            if ((err = addCellToRow(table->rows[i], cell, j + 1)).error) {
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
        for (unsigned j = 0; j < table->rows[i]->size; j++) {
            if (table->rows[i]->cells[j]->size != 0) {
                validColumns = j + 1;
            }
        }

        if (validColumns > mostColumns) {
            mostColumns = validColumns;
        }
    }

    // Delete all unnecessary columns
    for (unsigned j = table->rows[0]->size; j > mostColumns; j--) {
        deleteColumnFromTable(table, j + 1);
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
    for (unsigned i = table->rows[0]->size; i < columns; i++) {
        // Prepare the new cell
        Cell *cell;
        if ((cell = createCell()) == NULL) {
            err.error = true;
            err.message = "Nepodarilo se alokovat pamet pro novou bunku.";

            return err;
        }

        // Add the cell to the row
        if ((err = addCellToRow(table->rows[0], cell, i + 1)).error) {
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
        if ((err = addRowToTable(table, row, i + 1)).error) {
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
        destructRow(table->rows[i]);
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
        destructCell(row->cells[i]);
    }

    free(row->cells);
    row->capacity = 0;
    row->size = 0;

    free(row);
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

    free(cell);
}

/**
 * Sets a new value to the selected cell of the table
 * @param table Table to edit
 * @param row Row selection (1 = first)
 * @param column Column selection (1 = first)
 * @param newValue New value
 * @return Error information
 */
ErrorInfo setCellValue(Table *table, unsigned int row, unsigned int column, const char *newValue) {
    ErrorInfo err = {.error = false};

    // Get cell and new value's size for easier manipulation
    // There are coordinates from the real world in row and column (indexed from 1) --> - 1
    Cell *cell = table->rows[row - 1]->cells[column - 1];
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
 * @param row Selected row (1 = first)
 * @param column Selected column (1 = first)
 * @return Value of the cell
 */
char *getCellValue(Table *table, unsigned int row, unsigned int column) {
    // There are coordinates from the real world in row and column (indexed from 1) --> - 1
    row--;
    column--;

    if (((table->size - 1) < row) || ((table->rows[0]->size - 1) < column)) {
        return NULL;
    }

    return table->rows[row]->cells[column]->data;
}

/**
 * Creates command sequence
 * @return Pointer to the newly created command sequence
 */
CommandSequence *createCmdSeq() {
    CommandSequence *cmdSeq;
    if ((cmdSeq = malloc(sizeof(CommandSequence))) == NULL) {
        return NULL;
    }

    // Set default values
    cmdSeq->firstCmd = NULL;
    cmdSeq->lastCmd = NULL;

    return cmdSeq;
}

/**
 * Create a new command
 * @return Pointer to the new command
 */
Command *createCmd() {
    Command *cmd;
    if ((cmd = malloc(sizeof(Command))) == NULL) {
        return NULL;
    }

    // Set default values
    cmd->type = CLASSIC_COMMAND;
    memset(cmd->name, '\0', COMMAND_NAME_SIZE + 1);
    memset(cmd->intParams, BAD_ROW_COL_NUMBER, sizeof(int) * COMMAND_PARAMS_SIZE);
    cmd->next = NULL;

    // Allocate space for string parameters
    for (unsigned i = 0; i < COMMAND_PARAMS_SIZE; i++) {
        if ((cmd->strParams[i] = malloc(sizeof(char))) == NULL) {
            return NULL;
        }

        // Set value to allocated space (string will be detected well)
        cmd->strParams[i][0] = '\0';
    }

    return cmd;
}

/**
 * Adds a new command to the command sequence
 * @param cmdSeq Command sequence to edit
 * @param cmd New command to add
 */
void addNewCmdToSeq(CommandSequence *cmdSeq, Command *cmd) {
    // There are two set commands, so we need to differ them
    if (streq(cmd->name, "set") && cmd->type == SELECTION_COMMAND) {
        memcpy(cmd->name, "set-v", 5 * sizeof(char));
    }

    // Behaviour is different for the first command
    if (cmdSeq->firstCmd == NULL) {
        cmdSeq->firstCmd = cmd;
        cmdSeq->lastCmd = cmd;

        return;
    }

    // Add command to the linked-list (link it to the last command)
    Command *lastCmd = cmdSeq->lastCmd;
    lastCmd->next = cmd;

    // Set the new command as the last of the sequence
    cmdSeq->lastCmd = cmd;
}

/**
 * Converts types of commands' parameters (from string to int, if possible)
 * @param cmdSeq Command sequence with commands to edit
 */
void convertTypesInCommandParams(CommandSequence *cmdSeq) {
    Command *cmd = cmdSeq->firstCmd;
    while (cmd != NULL) {
        for (unsigned i = 0; i < COMMAND_PARAMS_SIZE; i++) {
            int value;
            if (streq(cmd->strParams[i], "_") || streq(cmd->strParams[i], "-")) {
                cmd->intParams[i] = LAST_ROW_COL_NUMBER;
            } else if ((value = (int)strtol(cmd->strParams[i], NULL, 10)) != 0) {
                cmd->intParams[i] = value;
            }
        }

        // Move to the next command
        cmd = cmd->next;
    }
}

/**
 * Destructs command sequence
 * @param cmdSeq Command sequence to be destructed
 */
void destructCommandSequence(CommandSequence *cmdSeq) {
    // Command sequence has been already freed
    if (cmdSeq == NULL) {
        return;
    }

    Command *cmd = cmdSeq->firstCmd;
    while (cmd != NULL) {
        // Backup next
        Command *next = cmd->next;

        // Destruct actual command
        destructCommand(cmd);

        // Load next from backup
        cmd = next;
    }

    // Deallocate the sequence
    cmdSeq->firstCmd = NULL;
    cmdSeq->lastCmd = NULL;
    free(cmdSeq);
}

/**
 * Destructs command
 * @param cmd Command to be destructed
 */
void destructCommand(Command *cmd) {
    // Command sequence has been already freed
    if (cmd == NULL) {
        return;
    }

    // Set static memory fields to empty states
    memset(cmd->name, '\0', COMMAND_NAME_SIZE);
    memset(cmd->intParams, 0, sizeof(int) * COMMAND_PARAMS_SIZE);
    cmd->next = NULL;

    // Free dynamic memory field
    for (unsigned i = 0; i < COMMAND_PARAMS_SIZE; i++) {
        free(cmd->strParams[i]);
    }

    // Deallocate the command
    free(cmd);
}

/**
 * Processes commands on the table
 * @param cmdSeq Sequence of commands to process
 * @param table Table with data to work with
 * @return Error information
 */
ErrorInfo processCommands(CommandSequence *cmdSeq, Table *table) {
    ErrorInfo err = {.error = false};

    // Functions known by the system
    char *names[] = {
            "select", "min", "max", "find", "irow", "arow", "drow", "icol", "acol", "dcol", "set",
            "clear", "swap", "sum", "avg", "count", "len", "def", "use", "inc", "set-v"
    };
    ErrorInfo (*functions[])() = {
            standardSelect, minMaxSelect, minMaxSelect, findSelect, irow, arow, drow, icol, acol, dcol, setEdit,
            clearEdit, swapEdit, sumAvgEdit, sumAvgEdit, countEdit, lenEdit, defVars, useVars, incVars, setVars
    };

    // Preparation of selection and variables
    Selection *sel;
    if ((sel = createSelection()) == NULL) {
        err.error = true;
        err.message = "Nepodarilo se alokovat pamet pro selekci.";

        return err;
    }

    Variables *vars;
    if ((vars = createVars()) == NULL) {
        err.error = true;
        err.message = "Nepodarilo se alokovat pamet pro docasne promenne.";

        return err;
    }

    // Apply each command from the sequence
    Command *cmd = cmdSeq->firstCmd;
    while (cmd != NULL) {
        // Find related function
        int found = -1;
        for (unsigned i = 0; i < sizeof(names) / sizeof(char *); i++) {
            if (streq(names[i], cmd->name)) {
                found = (int)i;
                break;
            }
        }

        // Commands isn't implemented
        if (found < 0) {
            err.error = true;
            err.message = "Byl zadan prikaz, ktery neni definovan.";

            return err;
        }

        // Apply command by its type
        if (cmd->type == SELECTION_COMMAND) {
            // Selection commands are applied everytime once
            if ((err = functions[found](cmd, table, sel, vars)).error) {
                return err;
            }
        } else {
            // Other command are applied for every selected cell
            for (unsigned i = sel->rowFrom; i <= sel->rowTo; i++) {
                for (unsigned j = sel->colFrom; j <= sel->colTo; j++) {
                    // Set current coords
                    sel->curRow = i;
                    sel->curCol = j;

                    if ((err = functions[found](cmd, table, sel, vars)).error) {
                        return err;
                    }
                }
            }
        }

        // Move to the next command in sequence
        cmd = cmd->next;
    }

    // Selection and temporary variables deallocation
    destructSelection(sel);
    destructVars(vars);

    return err;
}

/**
 * Creates a new selection
 * @return Pointer to the new selection or NULL if memory problems occurred
 */
Selection *createSelection() {
    Selection *sel;
    if ((sel = malloc(sizeof(Selection))) == NULL) {
        return NULL;
    }

    // Set data to default values
    sel->rowFrom = 1;
    sel->rowTo = 1;
    sel->colFrom = 1;
    sel->colTo = 1;
    sel->curRow = 0;
    sel->curCol = 0;

    return sel;
}

/**
 * Updates selection data by data from another selection
 * @param sel Selection to change
 * @param pattern Selection to get data from
 */
void updateSelectionBySelection(Selection *sel, Selection *pattern) {
    sel->rowFrom = pattern->rowFrom;
    sel->rowTo = pattern->rowTo;
    sel->colFrom = pattern->colFrom;
    sel->colTo = pattern->colTo;
}

/**
 * Destructs selection
 * @param sel Selection to be destructed
 */
void destructSelection(Selection *sel) {
    // Selection has been already destructed
    if (sel == NULL) {
        return;
    }

    free(sel);
}

/**
 * Creates a new variables
 * @return Pointer to the new variables or NULL if memory problems occurred
 */
Variables *createVars() {
    Variables *vars;
    if ((vars = malloc(sizeof(Variables))) == NULL) {
        return NULL;
    }

    // Data default values
    if ((vars->sel = malloc(sizeof(Selection))) == NULL) {
        return NULL;
    }

    for (unsigned i = 0; i < NUMBER_OF_VARIABLES; i++) {
        if ((vars->data[i] = malloc(sizeof(char))) == NULL) {
            return NULL;
        }

        // Set value to allocated space (string will be detected well)
        vars->data[i][0] = '\0';
    }

    return vars;
}

/**
 * Destructs variables
 * @param vars Variables to be destructed
 */
void destructVars(Variables *vars) {
    // Variables has been already destructed
    if (vars == NULL) {
        return;
    }

    // Deallocate selection variable
    free(vars->sel);

    // Deallocate each data variable
    for (unsigned i = 0; i < NUMBER_OF_VARIABLES; i++) {
        free(vars->data[i]);
    }

    free(vars);
}

/**
 * Applies standard select ([R,C] and its subtypes)
 * @param cmd Command that is applying
 * @param table Table with data
 * @param sel Selection
 * @param vars Temporary vars
 * @return Error information
 */
ErrorInfo standardSelect(Command *cmd, Table *table, Selection *sel, Variables *vars) {
    ErrorInfo err = {.error = false};

    // Aliases for better code readability
    int row = cmd->intParams[0];
    int col = cmd->intParams[1];
    int rowSecond = cmd->intParams[2];
    int colSecond = cmd->intParams[3];

    // [_]
    if (row == LAST_ROW_COL_NUMBER && col == BAD_ROW_COL_NUMBER) {
        // Selection hasn't been saved yet
        if (vars->sel->rowFrom == 0) {
            err.error = true;
            err.message = "Vyber z docastne promenne neni mozne nacist, protoze promenna zadny vyber neobsahuje.";

            return err;
        }

        // Revert selection from backup
        updateSelectionBySelection(sel, vars->sel);

        return err;
    }

    // Bad parameters for [R,C]
    if (row == BAD_ROW_COL_NUMBER || col == BAD_ROW_COL_NUMBER) {
        err.error = true;
        err.message = "Funkce [R,C] vyzaduje, aby bylo R i C prirozene cislo nebo znak '_'.";

        return err;
    }

    if (rowSecond != BAD_ROW_COL_NUMBER && colSecond != BAD_ROW_COL_NUMBER) {
        // [R1,C1,R2,R2] (it solves another function)
        if((err = windowSelect(cmd, table, sel, vars)).error) {
            return err;
        }
    } else {
        // [R,C]
        if (row != LAST_ROW_COL_NUMBER) {
            // R != '_'
            sel->rowFrom = sel->rowTo = row;
        } else {
            // R == '_'
            sel->rowFrom = 1;
            sel->rowTo = table->size;
        }
        if (col != LAST_ROW_COL_NUMBER) {
            // C != '_'
            sel->colFrom = sel->colTo = col;
        } else {
            // R = '_'
            sel->colFrom = 1;
            sel->colTo = table->rows[0]->size;
        }
    }

    // Resize table if select is bigger than table size
    if (sel->rowTo > table->size) {
        resizeTable(table, sel->rowTo, table->rows[0]->size);
    }
    if (sel->colTo > table->rows[0]->size) {
        resizeTable(table, table->size, sel->colTo);
    }

    return err;
}

/**
 * Applies window select ([R1,C1,R2,C2])
 * @param cmd Command that is applying
 * @param table Table with data
 * @param sel Selection
 * @param vars Temporary vars (not used)
 * @return Error information
 */
ErrorInfo windowSelect(Command *cmd, Table *table, Selection *sel, Variables *vars) {
    ErrorInfo err = {.error = false};

    // Not used parameters
    (void)vars;

    // Aliases for better code readability
    int row = cmd->intParams[0];
    int col = cmd->intParams[1];
    int rowSecond = cmd->intParams[2];
    int colSecond = cmd->intParams[3];

    // Bad input parameters
    if (row > rowSecond || col > colSecond) {
        err.error = true;
        err.message = "Funkce [R1,C1,R2,R2] vyzaduje, aby bylo R1 <= R2 a C1 <= C2.";

        return err;
    }
    // Bad parameters for [R1,C1,R2,R2]
    if ((rowSecond != BAD_ROW_COL_NUMBER && colSecond == BAD_ROW_COL_NUMBER) || (rowSecond == BAD_ROW_COL_NUMBER && colSecond != BAD_ROW_COL_NUMBER)) {
        err.error = true;
        err.message = "Funkce [R1,C1,R2,R2] vyzaduje, aby bylo R1, C1, R2 i C2 prirozene cislo.";

        return err;
    }

    // Update selection
    sel->rowFrom = row;
    sel->rowTo = (rowSecond != LAST_ROW_COL_NUMBER ? (unsigned)rowSecond : table->size);
    sel->colFrom = col;
    sel->colTo = (colSecond != LAST_ROW_COL_NUMBER ? (unsigned)colSecond : table->rows[0]->size);

    return err;
}

/**
 * Applies minimum/maximum select - it selects cell with minimum/maximum value
 * @param cmd Command that is applying
 * @param table Table with data
 * @param sel Selection
 * @param vars Temporary vars (not used)
 * @return Error information
 */
ErrorInfo minMaxSelect(Command *cmd, Table *table, Selection *sel, Variables *vars) {
    ErrorInfo err = {.error = false};

    // Not used parameters
    (void)vars;

    // This commands must be called after selection
    if (sel->rowFrom == 0) {
        err.error = true;
        err.message = "Pred volanim prikazu [min] a [max] je nutne nejprve provest vyber.";

        return err;
    }

    // Prepare min/max cell's coords and value
    struct {
        int row;
        int col;
    } coords;
    coords.row = -1;
    coords.col = -1;
    double actualMinMax;

    // Find minimum/maximum
    for (unsigned i = sel->rowFrom; i <= sel->rowTo; i++) {
        for (unsigned j = sel->colFrom; j <= sel->colTo; j++) {
            char *value = getCellValue(table, i, j);
            if (isValidNumber(value)) {
                double number = strtod(value, NULL);
                if (coords.row == -1 || (streq(cmd->name, "min") && number < actualMinMax) || (streq(cmd->name, "max") && number > actualMinMax)) {
                    // Save the new minimum/maximum
                    coords.row = (int)i;
                    coords.col = (int)j;
                    actualMinMax = number;
                }
            }
        }
    }

    // No numeric values found
    if (coords.row == -1) {
        err.error = true;
        err.message = "Vyber neobsahuje zadne numericke bunky, selekci [min] nebo [max] neni mozne provest.";

        return err;
    }

    // Update selection
    sel->rowFrom = (unsigned)coords.row;
    sel->rowTo = (unsigned)coords.row;
    sel->colFrom = (unsigned)coords.col;
    sel->colTo = (unsigned)coords.col;

    return err;
}

/**
 * Applies find select - it selects first cell contains some value
 * @param cmd Command that is applying
 * @param table Table with data
 * @param sel Selection
 * @param vars Temporary vars (not used)
 * @return Error information
 */
ErrorInfo findSelect(Command *cmd, Table *table, Selection *sel, Variables *vars) {
    ErrorInfo err = {.error = false};

    // Not used parameters
    (void)vars;

    // First parameter can't be empty
    if (streq(cmd->strParams[0], "")) {
        err.error = true;
        err.message = "Funkce [find STR] vyzaduje jako STR neprazdny retezec";

        return err;
    }

    // Find the cell with STR
    for (unsigned i = sel->rowFrom; i <= sel->rowTo; i++) {
        for (unsigned j = sel->colFrom; j <= sel->colTo; j++) {
            if (strstr(getCellValue(table, i, j), cmd->strParams[0]) != NULL) {
                sel->rowFrom = i;
                sel->rowTo = i;
                sel->colFrom = j;
                sel->colTo = j;

                return err;
            }
        }
    }

    return err;
}

/**
 * Inserts row before selected row
 * @param cmd Command that is applying
 * @param table Table with data
 * @param sel Selection (not used)
 * @param vars Temporary vars (not used)
 * @return Error information
 */
ErrorInfo irow(Command *cmd, Table *table, Selection *sel, Variables *vars) {
    ErrorInfo err = {.error = false};

    // Not used parameters
    (void)cmd;
    (void)vars;

    // Create empty row
    Row *row;
    if ((row = createRow()) == NULL) {
        err.error = true;
        err.message = "Pri alokaci pameti pro novy radek doslo k chybe.";

        return err;
    }

    // Add the row into table
    if ((err = addRowToTable(table, row, sel->curRow)).error) {
        return err;
    }
    if ((err = alignRowSizes(table)).error) {
        return err;
    }

    return err;
}

/**
 * Inserts a row after selected row
 * @param cmd Command that is applying (not used)
 * @param table Table with data
 * @param sel Selection
 * @param vars Temporary vars (not used)
 * @return Error information
 */
ErrorInfo arow(Command *cmd, Table *table, Selection *sel, Variables *vars) {
    ErrorInfo err = {.error = false};

    // Not used parameters
    (void)cmd;
    (void)vars;

    // Create empty row
    Row *row;
    if ((row = createRow()) == NULL) {
        err.error = true;
        err.message = "Pri alokaci pameti pro novy radek doslo k chybe.";

        return err;
    }

    // Add the row into table
    if ((err = addRowToTable(table, row, sel->curRow + 1)).error) {
        return err;
    }
    if ((err = alignRowSizes(table)).error) {
        return err;
    }

    return err;
}

/**
 * Deletes selected row
 * @param cmd Command that is applying (not used)
 * @param table Table with data
 * @param sel Selection
 * @param vars Temporary vars (not used)
 * @return Error information
 */
ErrorInfo drow(Command *cmd, Table *table, Selection *sel, Variables *vars) {
    ErrorInfo err = {.error = false};

    // Not used parameters
    (void)cmd;
    (void)vars;

    // Delete row
    deleteRowFromTable(table, sel->curRow);

    return err;
}

/**
 * Insert column before selection
 * @param cmd Command that is applying (not used)
 * @param table Table with data
 * @param sel Selection
 * @param vars Temporary vars (not used)
 * @return Error information
 */
ErrorInfo icol(Command *cmd, Table *table, Selection *sel, Variables *vars) {
    ErrorInfo err = {.error = false};

    // Not used parameters
    (void)cmd;
    (void)vars;

    // Add column to the table
    if ((err = addColumnToTable(table, sel->curCol)).error) {
        return err;
    }

    return err;
}

/**
 * Insert column after selection
 * @param cmd Command that is applying (not used)
 * @param table Table with data
 * @param sel Selection
 * @param vars Temporary vars (not used)
 * @return Error information
 */
ErrorInfo acol(Command *cmd, Table *table, Selection *sel, Variables *vars) {
    ErrorInfo err = {.error = false};

    // Not used parameters
    (void)cmd;
    (void)vars;

    // Add column to the table
    if ((err = addColumnToTable(table, sel->curCol + 1)).error) {
        return err;
    }

    return err;
}

/**
 * Deletes selected columns
 * @param cmd Command that is applying (not used)
 * @param table Table with data
 * @param sel Selection
 * @param vars Temporary vars (not used)
 * @return Error information
 */
ErrorInfo dcol(Command *cmd, Table *table, Selection *sel, Variables *vars) {
    ErrorInfo err = {.error = false};

    // Not used parameters
    (void)cmd;
    (void)vars;

    // Delete column
    deleteColumnFromTable(table, sel->curCol);

    return err;
}

/**
 * Table editing function for setting a value to the selected cell
 * @param cmd Command that is applying
 * @param table Table with data
 * @param sel Selection
 * @param vars Temporary vars (not used)
 * @return Error information
 */
ErrorInfo setEdit(Command *cmd, Table *table, Selection *sel, Variables *vars) {
    ErrorInfo err = {.error = false};

    // Not used parameters
    (void)vars;

    // Set the new value to the selected cell
    if ((err = setCellValue(table, sel->curRow, sel->curCol, cmd->strParams[0])).error) {
        return err;
    }

    return err;
}

/**
 * Table editing function for clearing (deleting value) from selected cell
 * @param cmd Command that is applying (not used)
 * @param table Table with data
 * @param sel Selection
 * @param vars Temporary vars (not used)
 * @return Error information
 */
ErrorInfo clearEdit(Command *cmd, Table *table, Selection *sel, Variables *vars) {
    ErrorInfo err = {.error = false};

    // Not used parameters
    (void)cmd;
    (void)vars;

    // Set the new value to the selected cell
    if ((err = setCellValue(table, sel->curRow, sel->curCol, "")).error) {
        return err;
    }

    return err;
}

/**
 * Table editing function for swapping a value of selected cell with cell selected by input arguments
 * @param cmd Command that is applying
 * @param table Table with data
 * @param sel Selection
 * @param vars Temporary vars (not used)
 * @return Error information
 */
ErrorInfo swapEdit(Command *cmd, Table *table, Selection *sel, Variables *vars) {
    ErrorInfo err = {.error = false};

    // Not used parameters
    (void)vars;

    // Create aliases for better code readability
    int argRow = cmd->intParams[0];
    int argCol = cmd->intParams[1];

    // Bad parameters
    if (argRow < 1 || argCol < 1) {
        err.error = true;
        err.message = "Souradnice bunky musi byt vzdy ve tvaru [R,C], kde R i C jsou prirozena cisla.";

        return err;
    }

    // Get values of both cells
    char *selCell = getCellValue(table, sel->curRow, sel->curCol);
    char *argCell;
    if ((argCell = getCellValue(table, argRow, argCol)) == NULL) {
        err.error = true;
        err.message = "Funkce swap vyzaduje vyber takove bunky, ktera je v tabulce obsazena.";

        return err;
    }

    char *tmp;
    if ((tmp = malloc(sizeof(char) * (strlen(selCell) + 1))) == NULL) {
        err.error = true;
        err.message = "Pri alokaci pameti pro docasnou promennou doslo k chybe.";

        return err;
    }
    memcpy(tmp, selCell, strlen(selCell) + 1);

    // Swap cells' values
    if ((err = setCellValue(table, sel->curRow, sel->curCol, argCell)).error) {
        return err;
    }
    if ((err = setCellValue(table, (unsigned)argRow, (unsigned)argCol, tmp)).error) {
        return err;
    }

    free(tmp);
    return err;
}

/**
 * Table editing function for counting a sum/average of selection and saving it to cell selected in input arguments
 * @param cmd Command that is applying
 * @param table Table with data
 * @param sel Selection
 * @param vars Temporary vars
 * @return Error information
 */
ErrorInfo sumAvgEdit(Command *cmd, Table *table, Selection *sel, Variables *vars) {
    ErrorInfo err = {.error = false};

    // Create aliases for better code readability
    int argRow = cmd->intParams[0];
    int argCol = cmd->intParams[1];

    // Bad parameters
    if (argRow < 1 || argCol < 1) {
        err.error = true;
        err.message = "Souradnice bunky musi byt vzdy ve tvaru [R,C], kde R i C jsou prirozena cisla.";

        return err;
    }

    // First iteration --> prepare temp variable
    if (sel->curRow == sel->rowFrom && sel->curCol == sel->colFrom) {
        vars->number = 0.0;
    }

    // Actual selection cell value
    char *selCell = getCellValue(table, sel->curRow, sel->curCol);

    // This selection cell is not numeric --> cannot be added to the sum
    if (!isValidNumber(selCell)) {
        return err;
    }

    // Add value of selection cell to the temp variable
    vars->number += strtod(selCell, NULL);

    // The last iteration
    if (sel->curRow == sel->rowTo && sel->curCol == sel->colTo) {
        // Count average (= summary divided by number of iterations (number of items))
        if (streq(cmd->name, "avg")) {
            vars->number = vars->number / ((sel->rowTo - sel->rowFrom + 1) * (sel->colTo - sel->colFrom + 1));
        }

        // Save the result
        char textResult[50];
        sprintf(textResult, "%g", vars->number);
        if ((err = setCellValue(table, argRow, argCol, textResult)).error) {
            return err;
        }
    }

    return err;
}

/**
 * Table editing function for counting number of non-empty cells in selection and saving it to cell from arguments
 * @param cmd Command that is applying
 * @param table Table with data
 * @param sel Selection
 * @param vars Temporary vars (not used)
 * @return Error information
 */
ErrorInfo countEdit(Command *cmd, Table *table, Selection *sel, Variables *vars) {
    ErrorInfo err = {.error = false};

    // Not used parameters
    (void)vars;

    // Create aliases for better code readability
    int argRow = cmd->intParams[0];
    int argCol = cmd->intParams[1];

    // Bad parameters
    if (argRow < 1 || argCol < 1) {
        err.error = true;
        err.message = "Souradnice bunky musi byt vzdy ve tvaru [R,C], kde R i C jsou prirozena cisla.";

        return err;
    }

    // First iteration --> set value of the cell to store the result to 0
    if (sel->curRow == sel->rowFrom && sel->curCol == sel->colFrom) {
        if ((err = setCellValue(table, argRow, argCol, "0")).error) {
            return err;
        }
    }

    // If selection cell has non-empty value, increment value of cell with result
    if (!streq(getCellValue(table, sel->curRow, sel->curCol), "")) {
        // Actual arguments cell value
        char *argCell;
        if ((argCell = getCellValue(table, argRow, argCol)) == NULL) {
            err.error = true;
            err.message = "Funkce swap vyzaduje vyber takove bunky, ktera je v tabulce obsazena.";

            return err;
        }

        // Increment the value
        int result = (int)strtol(argCell, NULL, 10) + 1;

        // Save the result
        char textResult[20];
        sprintf(textResult, "%d", result);
        if ((err = setCellValue(table, argRow, argCol, textResult)).error) {
            return err;
        }
    }

    return err;
}

/**
 * Table editing function for counting length of selected cell and saving it to cell from input arguments
 * @param cmd Command that is applying
 * @param table Table with data
 * @param sel Selection
 * @param vars Temporary vars (not used)
 * @return Error information
 */
ErrorInfo lenEdit(Command *cmd, Table *table, Selection *sel, Variables *vars) {
    ErrorInfo err = {.error = false};

    // Not used parameters
    (void)vars;

    // Create aliases for better code readability
    int argRow = cmd->intParams[0];
    int argCol = cmd->intParams[1];

    // Bad parameters
    if (argRow < 1 || argCol < 1) {
        err.error = true;
        err.message = "Souradnice bunky musi byt vzdy ve tvaru [R,C], kde R i C jsou prirozena cisla.";

        return err;
    }

    int result = (int)strlen(getCellValue(table, sel->curRow, sel->curCol));

    // Save the result
    char textResult[20];
    sprintf(textResult, "%d", result);
    if ((err = setCellValue(table, argRow, argCol, textResult)).error) {
        return err;
    }

    return err;
}

/**
 * Variable using function for saving a value to the variable
 * @param cmd Command that is applying
 * @param table Table with data
 * @param sel Selection
 * @param vars Temporary vars
 * @return Error information
 */
ErrorInfo defVars(Command *cmd, Table *table, Selection *sel, Variables *vars) {
    ErrorInfo err = {.error = false};

    // Bad parameters
    if (cmd->strParams[0][0] != '_' || !isdigit(cmd->strParams[0][1]) || cmd->strParams[0][2] != '\0') {
        err.error = true;
        err.message = "Je mozne vyuzit pouze promennych s oznacenim _0 az _9.";

        return err;
    }

    // Get var number from params
    int varNumber = (int)cmd->strParams[0][1] - '0';

    // Get value from the cell
    char *value =getCellValue(table, sel->curRow, sel->curCol);

    // Save selected value to the var
    if ((vars->data[varNumber] = realloc(vars->data[varNumber], strlen(value) + 1)) == NULL) {
        err.error = true;
        err.message = "Pri alokaci pameti pro data promenne doslo k chybe.";

        return err;
    }
    memset(vars->data[varNumber], '\0', strlen(value) + 1);
    memcpy(vars->data[varNumber], value, strlen(value));

    return err;
}

/**
 * Variable using function for setting selected cell to value from variable
 * @param cmd Command that is applying
 * @param table Table with data
 * @param sel Selection
 * @param vars Temporary vars
 * @return Error information
 */
ErrorInfo useVars(Command *cmd, Table *table, Selection *sel, Variables *vars) {
    ErrorInfo err = {.error = false};

    // Bad parameters
    if (cmd->strParams[0][0] != '_' || !isdigit(cmd->strParams[0][1]) || cmd->strParams[0][2] != '\0') {
        err.error = true;
        err.message = "Je mozne vyuzit pouze promennych s oznacenim _0 az _9.";

        return err;
    }

    // Get var number from params
    int varNumber = (int)cmd->strParams[0][1] - '0';

    // Load value from variable
    char *value = vars->data[varNumber];

    // Set the value to the cell
    if ((err = setCellValue(table, sel->curRow, sel->curCol, value)).error) {
        return err;
    }

    return err;
}

/**
 * Variable using function for incrementing value of variable
 * @param cmd Command that is applying
 * @param table Table with data (not used)
 * @param sel Selection (not used)
 * @param vars Temporary vars
 * @return Error information
 */
ErrorInfo incVars(Command *cmd, Table *table, Selection *sel, Variables *vars) {
    ErrorInfo err = {.error = false};

    // Not used parameters
    (void)table;
    (void)sel;

    // Bad parameters
    if (cmd->strParams[0][0] != '_' || !isdigit(cmd->strParams[0][1]) || cmd->strParams[0][2] != '\0') {
        err.error = true;
        err.message = "Je mozne vyuzit pouze promennych s oznacenim _0 az _9.";

        return err;
    }

    // Get var number from params
    int varNumber = (int)cmd->strParams[0][1] - '0';

    // Get incremented value
    double value = strtod(vars->data[varNumber], NULL) + 1.0;

    // Convert value to text form
    char textValue[50];
    sprintf(textValue, "%g", value);

    // Save changed value to the var
    if ((vars->data[varNumber] = realloc(vars->data[varNumber], strlen(textValue) + 1)) == NULL) {
        err.error = true;
        err.message = "Pri alokaci pameti pro data promenne doslo k chybe.";

        return err;
    }
    memset(vars->data[varNumber], '\0', strlen(textValue) + 1);
    memcpy(vars->data[varNumber], textValue, strlen(textValue));

    return err;
}

/**
 * Variable using function for saving a backup of actual selection
 * @param cmd Command that is applying (not used)
 * @param table Table with data (not used)
 * @param sel Selection
 * @param vars Temporary vars
 * @return Error information
 */
ErrorInfo setVars(Command *cmd, Table *table, Selection *sel, Variables *vars) {
    ErrorInfo err = {.error = false};

    // Not used parameters
    (void)cmd;
    (void)table;

    // Backup actual selection
    updateSelectionBySelection(vars->sel, sel);

    return err;
}

/**
 * Checks if the string contains valid number
 * @param number String for testing
 * @return Is valid number in the string?
 */
bool isValidNumber(char *number) {
    bool decimalPoint = false; // Was decimal point found?
    for (int i = 0; i < (int) strlen(number); i++) {
        if (((number[i] < '0') || (number[i] > '9')) && (i == 0 && (number[i] != '-'))) {
            if (number[i] == '.' && decimalPoint == false) {
                decimalPoint = true;
            } else {
                return false;
            }
        }
    }

    return true;
}