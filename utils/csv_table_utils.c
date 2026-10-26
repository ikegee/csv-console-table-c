#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../csv_processor.h"

static bool parse_full_double(const char *value) {
    char *endptr;
    strtod(value, &endptr);
    return endptr != value && *endptr == '\0';
}

/* More than FLOAT_DECIMAL_PLACES fractional digits, an exponent, or a trailing
   d/D suffix selects double. Shorter decimals stay float. */
static bool is_double_literal(const char *value) {
    size_t len = strlen(value);
    char buf[MAX_LINE_LENGTH];
    const char *numeric = value;

    if (len > 1 && (value[len - 1] == 'd' || value[len - 1] == 'D')) {
        if (len >= sizeof(buf)) {
            return false;
        }
        memcpy(buf, value, len - 1);
        buf[len - 1] = '\0';
        numeric = buf;
        return parse_full_double(numeric);
    }

    if (!parse_full_double(numeric)) {
        return false;
    }
    if (strpbrk(numeric, "eE") != NULL) {
        return true;
    }

    const char *dot = strchr(numeric, '.');
    if (dot == NULL) {
        return false;
    }

    int fractional_digits = 0;
    for (const char *p = dot + 1; *p >= '0' && *p <= '9'; p++) {
        fractional_digits++;
    }
    return fractional_digits > FLOAT_DECIMAL_PLACES;
}

static bool literal_uses_exponent(const char *value) {
    size_t len = strlen(value);
    if (len > 1 && (value[len - 1] == 'd' || value[len - 1] == 'D')) {
        len--;
    }
    for (size_t i = 0; i < len; i++) {
        if (value[i] == 'e' || value[i] == 'E') {
            return true;
        }
    }
    return false;
}

static void format_double_field(const char *raw, char *out, size_t out_len) {
    double dval = strtod(raw, NULL);
    if (literal_uses_exponent(raw)) {
        snprintf(out, out_len, "%.*e", DOUBLE_DECIMAL_PLACES, dval);
    } else {
        snprintf(out, out_len, "%.*f", DOUBLE_DECIMAL_PLACES, dval);
    }
}

FieldType detect_field_type(const char *value) {    
    if (strchr(value, '"') || (strlen(value) >= 3 && strcmp(value + strlen(value) - 3, "...") == 0)) {
        return TYPE_STRING;
    }

    if (is_double_literal(value)) {
        return TYPE_DOUBLE;
    }

    if (strchr(value, '.')) {
        char *endptr;
        strtof(value, &endptr);
        if (*endptr == '\0') {
            return TYPE_FLOAT;
        }
        return TYPE_STRING;
    }

    char *endptr;
    long int_val = strtol(value, &endptr, 10);
    if (*endptr == '\0')
        return TYPE_INT;

    if (strcasecmp(value, "true") == 0 || strcasecmp(value, "false") == 0)
        return TYPE_BOOL;

    return TYPE_STRING;
}

void print_data_table_with_types(CSVData *csv) {
    FieldType types[MAX_FIELDS];
    int widths[MAX_FIELDS];
    printf("\n");    
    
    // Detect types and calculate widths
    for (int i = 0; i < csv->fields; i++) {
        types[i] = detect_field_type(csv->data[0][i]);
        widths[i] = types[i] == TYPE_BOOL ? 5 : 
                    types[i] == TYPE_STRING ? MIN_FIELD_WIDTH : 
                    TYPE_WIDTH(types[i]);
        // For string types, calculate optimal width
        if (types[i] == TYPE_STRING) {
            int max_content_width = MIN_FIELD_WIDTH;
            for (int j = 0; j < csv->rows; j++) {
                int content_len = strlen(csv->data[j][i]);
                // Account for truncation with "..."
                if (content_len > MAX_FIELD_LENGTH - 3) {
                    content_len = MAX_FIELD_LENGTH - 3;
                }
                if (content_len > max_content_width) {
                    max_content_width = content_len;
                }
            }
            widths[i] = max_content_width < MIN_FIELD_WIDTH ? 
                        MIN_FIELD_WIDTH : 
                        max_content_width + 3; // Add space for "..."
        }
        // Doubles are reprinted with DOUBLE_DECIMAL_PLACES, which can be wider than the raw text
        else if (types[i] == TYPE_DOUBLE) {
            for (int j = 0; j < csv->rows; j++) {
                char formatted[MAX_LINE_LENGTH];
                format_double_field(csv->data[j][i], formatted, sizeof(formatted));
                int len = (int)strlen(formatted);
                if (len > widths[i]) {
                    widths[i] = len;
                }
            }
        }
        // For non-string types, keep existing width calculation
        else if (types[i] != TYPE_BOOL) {
            for (int j = 0; j < csv->rows; j++) {
                int len = strlen(csv->data[j][i]);
                if (len > widths[i]) {
                    widths[i] = len;
                }
            }
        }
    }
    
    // Print type headers
    for (int i = 0; i < csv->fields; i++) {
        const char *type_name = types[i] == TYPE_INT ? "int" :
                               types[i] == TYPE_FLOAT ? "float" :
                               types[i] == TYPE_DOUBLE ? "double" :
                               types[i] == TYPE_BOOL ? "bool" : "char*";
        printf("%-*s ", widths[i], type_name);
    }
    printf("\n");

    // Print data with proper formatting
    for (int i = 0; i < csv->rows; i++) {
        for (int j = 0; j < csv->fields; j++) {
            switch (types[j]) {
            case TYPE_STRING: {
                char formatted[MAX_FIELD_LENGTH + 1];
                strncpy(formatted, csv->data[i][j], MAX_FIELD_LENGTH - 3);
                formatted[MAX_FIELD_LENGTH - 3] = '\0';
                if (strlen(csv->data[i][j]) > MAX_FIELD_LENGTH - 3) {
                    strcat(formatted, "...");
                }
                printf("%-*s ", widths[j], formatted);
                break;
            }
            case TYPE_INT:
                printf("%-*d ", widths[j], atoi(csv->data[i][j]));
                break;
            case TYPE_FLOAT:
                printf("%-*.*f ", widths[j], FLOAT_DECIMAL_PLACES, atof(csv->data[i][j]));
                break;
            case TYPE_DOUBLE: {
                char formatted[MAX_LINE_LENGTH];
                format_double_field(csv->data[i][j], formatted, sizeof(formatted));
                printf("%-*s ", widths[j], formatted);
                break;
            }
            case TYPE_BOOL:
                printf("%-*s ", widths[j], strcasecmp(csv->data[i][j], "true") == 0 ? "TRUE" : "FALSE");
                break;
            }
        }
        printf("\n");
    }
    printf("\n");
}

void print_data_table(CSVData *csv) {
    printf("\n");
    for (int i = 0; i < csv->rows; i++) {
        for (int j = 0; j < csv->fields; j++) {
            printf("%s ", csv->data[i][j]);
        }
        printf("\n");
    }
    printf("\n");
}
