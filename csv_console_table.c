/* (c) 2024 G.E. Eidsness */

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "csv_processor.h"
#include "utils/csv_file_utils.c"
#include "utils/csv_table_utils.c"
#include "utils/csv_validator.c"

// gcc -o xyz  .\csv_console_table.c ; replace '-o xyz' with '-fsyntax-only' to check syntax
// ./xyz {yourfile.csv}
int main(int argc, char *argv[]) {
    const char *filename = (argc > 1) ? argv[1] : DEFAULT_CSV_FILE;
    printf("Open file: %s\n", filename);
    FILE *file = open_file(filename);
    if (file == NULL) {
        return 1;
    }
    if (!validate_file_size(file)) {
        fclose(file);
        return 1;
    }
    //char *data[MAX_ROWS][MAX_FIELDS]; old method
    CSVData csv = {0}; // Initialize struct to zero
    if (read_csv_data(file, &csv) != 0) {
        goto cleanup;
    }

    if (!check_matrix(&csv)) {
        printf("Matrix validation failed. Data type doesn't match column type.\n");
        csv.result = 1;
        goto cleanup;
    }
    printf("Matrix validation successful. Data structure consistent.\n");
    
    print_data_table_with_types(&csv);

cleanup:
    if (file) fclose(file);
    for (int i = 0; i < csv.rows; i++) {
        for (int j = 0; j < MAX_FIELDS; j++) {
            free(csv.data[i][j]);
        }
    }
    return csv.result;
}
