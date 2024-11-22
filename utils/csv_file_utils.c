#include <stdio.h>
#include <stdbool.h>
#include "../csv_processor.h"

#define MAX_FILE_SIZE (10 * 1024)  // 10 KB

long get_file_size(FILE *file) {
    long current_pos = ftell(file);
    fseek(file, 0L, SEEK_END);
    long size = ftell(file);
    fseek(file, current_pos, SEEK_SET);
    return size;
}

bool validate_file_size(FILE *file) {
    long file_size = get_file_size(file);
    if (file_size > MAX_FILE_SIZE) {
        printf("File too large. Maximum size allowed: %d bytes\n", MAX_FILE_SIZE);
        return false;
    }
    return true;
}

FILE* open_file(const char* filename) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        printf("Error opening file: %s\n", filename);
        return NULL;
    }
    return file;
}

int read_csv_data(FILE *file, CSVData *csv) {
    char line[MAX_LINE_LENGTH];
    
    while (fgets(line, sizeof(line), file) && csv->rows < MAX_ROWS) {
        char *token = strtok(line, ",");
        csv->fields = 0;
        while (token != NULL && csv->fields < MAX_FIELDS) {
            token[strcspn(token, "\n")] = 0;
            csv->data[csv->rows][csv->fields] = strdup(token);
            if (csv->data[csv->rows][csv->fields] == NULL) {
                printf("Memory allocation error.\n");
                return 1;
            }
            token = strtok(NULL, ",");
            csv->fields++;
        }
        csv->rows++;
    }
    return 0;
}
