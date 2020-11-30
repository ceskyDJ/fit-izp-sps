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

// Input/output functions
void writeErrorMessage(const char *message);

int main(int argc, char **argv) {
    /* ARGUMENTS PARSING */
    if (argc < 3) {
        writeErrorMessage("Nedostatecny pocet vstupnich argumentu.");

        return EXIT_FAILURE;
    } else if (argc > 5) {
        writeErrorMessage("Prekrocen maximalni pocet vstupnich argumentu.");

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