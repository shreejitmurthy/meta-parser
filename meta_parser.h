/* meta_parser.h - v1.1.1
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

struct {
    meta_object objects[MAX_FIELDS];
    size_t objects_length;
} meta_parser_state;

/* FUNCTION DECLARATIONS */

void meta_parse_init();
int meta_parse(const char *input_file, const char *output_file);

#endif /* META_PARSER_H */

/* --------------------------- IMPLEMENTATION ---------------------------- */
#ifdef META_PARSER_IMPLEMENTATION

/**
 * Appends an object to the global parser state.
 *
 * @param obj The object to append.
 */
static void meta_state_append(meta_object obj) {
    if (meta_parser_state.objects_length < MAX_FIELDS) {
        meta_parser_state.objects[meta_parser_state.objects_length++] = obj;
    } else {
        fprintf(stderr, "Error: Object list is full!\n");
    }
}

/**
 * Initializes the global parser state.
 */
void meta_parse_init() {
    meta_parser_state.objects_length = 0;
}

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
   if (sscanf(line, "obj :: %63s {", obj->name) == 1) {
        obj->field_count = 0;
        meta_state_append(*obj);
        return 1;
    }
    return 0;
}

/**
 * Checks if a given type matches an existing object name.
 *
 * @param type The field type to check.
 * @return 1 if the type matches an object name, 0 otherwise.
 */
static int meta_is_object_type(const char *type) {
    for (size_t i = 0; i < meta_parser_state.objects_length; i++) {
        if (strcmp(type, meta_parser_state.objects[i].name) == 0) {
            return 1;
        }
    }
    return 0;
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
    char type[64];
    if (sscanf(line, "%63s :: %63s", field->name, type) == 2) {
        // Check if the type matches an existing object
        if (meta_is_object_type(type)) {
            snprintf(field->type, sizeof(field->type), "%sData", type);
        } else {
            strncpy(field->type, type, sizeof(field->type) - 1);
        }
        obj->field_count++;
        return 1;
    }
    return 0;
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
 * Parses a metadata file and generates a corresponding C header file with structs.
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
    revision history:
        1.1.1  (2024-12-17)  Ensure scanning of 63 symbols when reading
                             strings
        1.1.0  (2024-12-17)  Added global state tracking; type-aware 
                             field parsing; improved metadata handling 
                             and struct generation.
        1.0.0  (2024-12-17)  First push;
*/


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