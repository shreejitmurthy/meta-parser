# Meta Parser
**meta_parser** is a single-header, STB-style C library for data-driven code generation. It parses simple metadata files describing objects and fields, automatically generating corresponding C structs in an output header file. Lightweight, dependency-free, and easy to integrate into any project.

This was made primarily to support serialisation and deserialisation for low level game development. Version 1.0 was developed in an afternoon, and was very enjoyable.

> [!WARNING]
> The software is unfinished, and has not yet achieved it's original aim. It is largely useless.

## Basic Language Features
The language serves as a lightweight schema definition format that describes objects and their fields. It is a a minimal, declarative language for defining data objects. Designed for simplicity and direct translation to C structs.

### Object Definition
An object starts with the keyword `obj ::`, followed by the object name and an opening brace `{`.
```
obj :: Player {
```

### Ending an Object
An object block ends with a closing brace `}`.
```
}
```

### Field Definition
Fields are defined inside an object block using the syntax:
```
field_name :: field_type
```
* `field_name`: The name of the field (who would have thought)
* `field_type`: The data type of the field, which can be any valid C type (e.g. `int`, `float`, `char`) as well as previously defined objects.
<!-- EOL -->
```
{
    health :: int
    name :: char
}
```
v1.0.0 has no support for arrays such as `char[32]` or pointers such as `const char*` (*yet*, of course)

### Comments
Lines beginning with `#` can be used for comments. Comments are NOT generated in the C header file (subject to change).
```
# This is a comment
obj :: Weapon {
    damage :: int
    # rounds :: int
}
```

## Other Features
**v1.2.0** added better error handling. The parser now detects when there is syntax error with field names or types. These error messages can be optionally logged to console, but will always be displayed as a comment in the generated file so as not to cause compile errors. 

> NOTE: Console error and warnings can be enabled by writing `#define META_LOG_CONSOLE` and must be done *before* including `"meta_parser.h"`. 

Syntax errors will occur if a variable or type would cause an error in the C language, i.e. for special characters in names, or using an undefined type.\
For example:
```
# data.meta

# Player object
obj :: Player {
    health :: int
    level :: int
    position :: float
}

obj :: World {
    player :: Player
    enemy :: Enemy
}

# Enemy object
obj :: Enemy {
    !health :: int
    friendship :: int
    position :: long long
}

# Ally object
obj :: Ally {
    health :: int
    friendship :: int
}
```
Here, the `Enemy` object type is used in the `World` object before it has been created, and the Enemy health variable contains special character `!` at the beginning of its name.
The generated output would be as such:
```c
/* Auto-generated code - do not edit! */

typedef struct PlayerData {
   int health;
   int level;
   float position;
} PlayerData;

typedef struct WorldData {
   PlayerData player;
   // Enemy enemy;  // Error: Unresolved or invalid type 'Enemy'
} WorldData;

typedef struct EnemyData {
   // int !health;  // Error: Cannot use special characters or numbers in field names
   int friendship;
   long position;
} EnemyData;

typedef struct AllyData {
   int health;
   int friendship;
} AllyData;

```
If an error is occured with an object, such as naming an object a C keyword or type, the struct is commented out, and no fields are written:
```
# data.meta

obj :: int {
    var :: int
    var1 :: float
}
```
Outputs:
```c
// Invalid object name 'int'
// typedef struct intData {
// } intData;
```
While *technically* it is possible to get away with using C types and/or keywords as object names, it means that the type (in this example, `int`) cannot be used later, as the parser would believe the C type to be a meta object.

## Rough Roadmap (Things TODO)
- [x] Minor: Mostly complete compile-time safety.
- [ ] Patch: Disallow duplicate objects.
- [ ] Patch: Allow comments without space following it, e.g. `#This is a comment` instead of only suppporting `# This is a comment`.
- [ ] Collections: Support for arrays.
- [ ] Minor: Type Enhancements
    - [ ] Custom type definitions (type aliases)
    - [ ] Constraints or validation rules for fields, generating runtime validation functions.
    - [ ] Enum support
- [ ] Minor: Attributes (maybe)
    - [ ] Field-level or object-level attributes that translate to C annotations.
- [ ] Minor: Code blocks
    - [ ] Suppport for embedding raw C code in the meta lang
- [ ] Minor: Support for default fields, generating initialisation code.

