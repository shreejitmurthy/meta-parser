/* meta_parser.h - v1.0 - public domain 
   An STB-Style single-file C header library for generating C structs from metadata files.
   This was done for fun!

   USAGE:
        Define `META_PARSER_IMPLEMENTATION` in *one* source file before including this header:
            #define META_PARSER_IMPLEMENTATION
            #include "meta_parser.h"
        
        Example input file:
            obj :: Player {
                health :: int
                level :: int
            }
        
        Generates:
            typedef struct PlayerData {
                int health;
                int level;
            } PlayerData;
        
        See the README.md for all features.

   LICENSE:
       See end of file for license information.
       
*/

#ifndef META_PARSER_H
#define META_PARSER_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#define MAX_LINE 256
#define MAX_NAME 64
#define MAX_FIELDS 50

#ifndef META_PARSER_MAX_LINE
#define META_PARSER_MAX_LINE 256
#endif

#ifndef META_PARSER_MAX_NAME
#define META_PARSER_MAX_NAME 64
#endif

#ifndef META_PARSER_MAX_FIELDS
#define META_PARSER_MAX_FIELDS 32
#endif

typedef struct meta_field {
    char name[MAX_NAME];
    char type[MAX_NAME];
} meta_field;

typedef struct meta_object {
    char name[MAX_NAME];
    meta_field fields[MAX_FIELDS];
    int field_count;
} meta_object;

/* FUNCTION DECLARATIONS */

/**
 * Parses a metadata file and generates a corresponding C header file with structs.
 *
 * @param input_file  Path to the input metadata file.
 * @param output_file Path to the output C header file.
 * @return 0 on success, -1 if an error occurs (e.g., file cannot be opened).
 */
int meta_parse(const char *input_file, const char *output_file);

#endif /* META_PARSER_H */

/* --------------------------- IMPLEMENTATION ---------------------------- */
#ifdef META_PARSER_IMPLEMENTATION

/**
 * Trims leading whitespace (spaces or tabs) from a line of text.
 *
 * @param line [in, out] Pointer to the line to trim. It will be updated to skip over whitespace.
 * @return 1 if the line becomes empty after trimming, 0 otherwise.
 */
static int meta_trim_line(char *line) {
    while (*line == ' ' || *line == '\t') line++;
    return *line == 0; /* Return true if line is empty */
}

/**
 * Parses the start of an object definition from a line.
 *
 * Expected format: "obj :: ObjectName {"
 *
 * @param obj  Pointer to the `meta_object` to store the parsed object name.
 * @param line Input line containing the object definition.
 * @return 1 if parsing is successful, 0 otherwise.
 */
static int meta_parse_object_start(meta_object *obj, const char *line) {
    return sscanf(line, "obj :: %s {", obj->name) == 1;
}

/**
 * Parses a single field definition within an object block.
 *
 * Expected format: "field_name :: field_type"
 *
 * @param obj  Pointer to the `meta_object` where the field will be stored.
 * @param line Input line containing the field definition.
 * @return 1 if parsing is successful, 0 otherwise.
 */
static int meta_parse_field(meta_object *obj, const char *line) {
    meta_field *field = &obj->fields[obj->field_count];
    if (line != NULL && *line == '#') return true;
    if (sscanf(line, "%s :: %s", field->name, field->type) == 2) {
        obj->field_count++;
        return true;
    }
    return false;
}

/**
 * Writes a typedef struct for the parsed object to the output file.
 *
 * Example output:
 *     typedef struct ObjectNameData {
 *         type field1;
 *         type field2;
 *     } ObjectNameData;
 *
 * @param out Pointer to the output file stream.
 * @param obj Pointer to the `meta_object` containing the object and field definitions.
 */
static void meta_write_object(FILE *out, const meta_object *obj) {
    fprintf(out, "typedef struct %sData {\n", obj->name);
    for (int i = 0; i < obj->field_count; i++) {
        fprintf(out, "   %s %s;\n", obj->fields[i].type, obj->fields[i].name);
    }
    fprintf(out, "} %sData;\n\n", obj->name);
}

/**
 * Parses an input metadata file and generates the corresponding C structs in an output file.
 *
 * @param input_file  Path to the input metadata file.
 * @param output_file Path to the output C header file.
 * @return 0 on success, -1 if an error occurs (e.g., file cannot be opened).
 */
int meta_parse(const char *input_file, const char *output_file) {
    FILE *in = fopen(input_file, "r");
    if (!in) return -1;

    FILE *out = fopen(output_file, "w");
    if (!out) { fclose(in); return -1; }

    char line[META_PARSER_MAX_LINE];
    meta_object obj = {0};
    memset(&obj, 0, sizeof(obj));
    int in_object = 0;

    fprintf(out, "/* Auto-generated code - do not edit! */\n\n");

    while (fgets(line, sizeof(line), in)) {
        if (meta_trim_line(line)) continue;

        // Start of new object
        if (strncmp(line, "obj ::", 6) == 0) {
            // Writeout previous object details
            if (in_object) {
                meta_write_object(out, &obj);
            }
            memset(&obj, 0, sizeof(obj));
            in_object = meta_parse_object_start(&obj, line);
        
        // End of current object
        } else if (in_object && strstr(line, "}")) {
            meta_write_object(out, &obj);
            in_object = 0;

        // Parse fields inside object
        } else if (in_object) {
            meta_parse_field(&obj, line);
        }
    }

    fclose(in);
    fclose(out);
    return 0; /* Success */
}

#endif /* META_PARSER_IMPLEMENTATION */

/*

MIT License

Copyright (c) 2024 Shreejit Murthy

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

*/