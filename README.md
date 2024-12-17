# Meta Parser
**meta_parser** is a single-header, STB-style C library for data-driven code generation. It parses simple metadata files describing objects and fields, automatically generating corresponding C structs in an output header file. Lightweight, dependency-free, and easy to integrate into any project.

This was made primarily to support serialisation and deserialisation for low level game development. It was developed in an afternoon, and was very enjoyable.

## The Language
The language serves as a lightweight schema definition format that describes objects and their fields. It is a a minimal, declarative language for defining data objects. Designed for simplicity and direct translation to C structs.

### Object Definition
An object starts with the keyword obj `::`, followed by the object name and an opening brace `{`.\
Example:
```
obj :: Player {
```

### Field Definition
Fields are defined inside an object block using the syntax:
```
field_name :: field_type
```
* `field_name`: The name of the field (who would have thought)
* `field_type`: The data type of the field, which can be any valid C type (e.g. `int`, `float`, `char[32]`)
<!-- EOL -->
Example:
```
{
    health :: int
    name :: char[64]
}
```

### Ending an Object
An object block ends with a closing brace `}`.\
An exciting example:
```
}
```

### Comments
Lines beginning with `#` can be used for comments. Comments are NOT generated in the C header file (subject to change).\
Example:
```
# This is a comment
obj :: Weapon {
    damage :: int
    # rounds :: int
}
```
