# CSV Console Table in C

Reads a small CSV file, checks that each column keeps the type of its first cell, and prints an aligned table.

## Example input

[`data/generic.csv`](data/generic.csv):

```csv
1,"Welcome to csv-console-table-c",9.98,UNKOWN,1,true,3.141592653590
2,"This is a sample data csv file",19.97,321654AE,2,false,2.718281828460
3,"1st row: set header types",1.99,"null",3,true,1.414213562370
4,"Detect headers: 'int *char float *char int bool double'",11.99,NULL,4,true,6.022140760000
5,"Unknown types become `*char/string`",19.97,OU812,5,true,9.806650000000
6,"'gcc -o xyz .\csv_console_table.c'",19.17,"string",6,true,1.602176634000
7,"'.\xyz .\yourfile.csv'",19.84,"*char",7,false,6.674300000000
```

The first cell in each column sets the type. Later rows must have the same number of fields, and each value must match that column's type.

- `int`: a whole number.
- `float`: a decimal with at most 2 digits after the point. Printed with 2 decimal places (`9.98`).
- `double`: more than 2 digits after the point, an exponent (`1.5e-3`), or a trailing `d`/`D` suffix (`1.25d`). Printed with 6 decimal places (`3.141593`). Exponent literals print in scientific notation (`1.500000e-03`).
- `bool`: `true` or `false`. Printed as `TRUE` or `FALSE`.
- `char*`: quoted text, text ending in `...`, or any other value. Long text is cut and shown with `...`.

The reader keeps at most 20 rows and 10 fields, and rejects files larger than 10 KB.

## Example output

```text
Open file: data/generic.csv
Matrix validation successful. Data structure consistent.

int  char*                                    float    char*                   int  bool  double
1    "Welcome to csv-console-table-c"         9.98     UNKOWN                  1    TRUE  3.141593
2    "This is a sample data csv file"         19.97    321654AE                2    FALSE 2.718282
3    "1st row: set header types"              1.99     "null"                  3    TRUE  1.414214
4    "Detect headers: 'int *char float *ch... 11.99    NULL                    4    TRUE  6.022141
5    "Unknown types become `*char/string`"    19.97    OU812                   5    TRUE  9.806650
6    "'gcc -o xyz .\csv_console_table.c'"     19.17    "string"                6    TRUE  1.602177
7    "'.\xyz .\yourfile.csv'"                 19.84    "*char"                 7    FALSE 6.674300
```

A row with a different field count, an empty field, or a mismatched type stops the program. [screenshots/csv_console_example.txt](screenshots/csv_console_example.txt) shows a short row.

## Usage

From the project directory in PowerShell:

```powershell
gcc -o xyz .\csv_console_table.c
.\xyz.exe
.\xyz.exe .\path\to\file.csv
```

With no path, the program reads `data/generic.csv`.

## Requirements

- C compiler (gcc)
- C standard library

## Other ways to print a CSV

PowerShell, using the header row in [`data/data.csv`](data/data.csv):

```powershell
Import-Csv .\data\data.csv | Format-Table
```

csvlook, treating the first row as data:

```bash
csvlook -H ./data/generic.csv
```

## License

MIT. See [LICENSE](LICENSE).
