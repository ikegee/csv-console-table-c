#ifndef CSV_PROCESSOR_H
#define CSV_PROCESSOR_H

#include <stdio.h>
#include <stdbool.h>

#define MAX_FIELDS 10
#define MAX_ROWS 20
#define MAX_FIELD_LENGTH 40
#define MAX_LINE_LENGTH 256
#define MIN_FIELD_WIDTH 20
#define DEFAULT_CSV_FILE "data/generic.csv"
// future parameter validation macros/functions
#define VALIDATE_ROW_COUNT(rows) ((rows) > 0 && (rows) <= MAX_ROWS)
#define VALIDATE_FIELD_COUNT(fields) ((fields) > 0 && (fields) <= MAX_FIELDS)
#define VALIDATE_FIELD_LENGTH(length) ((length) > 0 && (length) <= MAX_FIELD_LENGTH)
#define VALIDATE_STRING_LENGTH(length) ((length) > 0 && (length) <= MAX_LINE_LENGTH)
#define VALIDATE_FIELD_TYPE(type) ((type) == TYPE_INT || (type) == TYPE_FLOAT || (type) == TYPE_BOOL || (type) == TYPE_STRING)

#define TYPE_WIDTH(type) ((type) == TYPE_INT ? 4 : \
                          (type) == TYPE_FLOAT ? 8 : \
                          (type) == TYPE_BOOL ? 5 : \
                          (type) == TYPE_NULL ? 4 : \
                          (type) == TYPE_STRING ? MIN_FIELD_WIDTH : 0)

typedef struct {
    char *data[MAX_ROWS][MAX_FIELDS];
    int rows;
    int fields;
    int result;
} CSVData;

typedef enum {
    TYPE_INT,
    TYPE_FLOAT,
    TYPE_BOOL,
    TYPE_STRING,
    TYPE_NULL
} FieldType;

long get_file_size(FILE *file);
bool validate_file_size(FILE *file);
int read_csv_data(FILE *file, CSVData *csv);
FieldType detect_field_type(const char *value);
bool parse_bool(const char *value);
void trim_string(char *str, int max_length);
void trim_strings(char *data[MAX_ROWS][MAX_FIELDS], int rows, int fields, int max_string_length);
bool check_matrix(CSVData *csv); 
void print_data_table(CSVData *csv);
void print_data_table_with_types(CSVData *csv);

#endif /* CSV_PROCESSOR_H */
