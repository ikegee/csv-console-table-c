/* (c) 2024 G.E. Eidsness */

#include <stdbool.h>
#include <string.h>
#include "../csv_processor.h"

bool check_matrix(CSVData *csv) {
    if (csv->rows == 0) {
        printf("Error: Matrix is empty.\n");
        return false;
    }

    // Count fields in the first row
    int first_row_fields = 0;
    while (first_row_fields < MAX_FIELDS && csv->data[0][first_row_fields] != NULL) {
        first_row_fields++;
    }

    if (first_row_fields == 0) {
        printf("Error: First row is empty.\n");
        return false;
    }

    FieldType first_row_types[MAX_FIELDS];
    for (int i = 0; i < first_row_fields; i++) {
        first_row_types[i] = detect_field_type(csv->data[0][i]);
    }

    for (int i = 1; i < csv->rows; i++) {
        int current_fields = 0;
        while (current_fields < MAX_FIELDS && csv->data[i][current_fields] != NULL) {
            current_fields++;
        }

        if (current_fields != first_row_fields) {
            printf("Error: Inconsistent number of fields in row %d. Expected %d, found %d.\n", 
                   i + 1, first_row_fields, current_fields);
            return false;
        }

        for (int j = 0; j < current_fields; j++) {
            if (strlen(csv->data[i][j]) == 0) {
                printf("Error: Empty field detected at row %d, column %d.\n", i + 1, j + 1);
                return false;
            }
            if (detect_field_type(csv->data[i][j]) != first_row_types[j]) {
                printf("Error: Inconsistent data type at row %d, column %d.\n", i + 1, j + 1);
                return false;
            }
        }
    }

    return true;
}

/* 
FieldType detect_field_type(const char *field) {
    if (strchr(field, '.') != NULL) {
        return FLOAT;
    } else if (strchr(field, '-') != NULL) {
        return INTEGER;
    } else {
        return STRING;
    }
}
*/