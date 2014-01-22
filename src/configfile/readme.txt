Description:
    This class reads simple generic configuration files in a format like this:
        someVariable = someValue
    The first part is the name of the setting, the second is its value.
    There must be an equals symbol in between!
    These settings can be different types too, and named almost anything.

Example:
    someNumber = 500
    someDecimal = 3.1415926535 // Decimals are stored as double precision floats
    someBoolean = True
    anotherBool = false
    bool3 = FALSE // Booleans are not case sensitive
    bool4 = 0 // Zero is false
    bool5 = 100 // A non-zero value is true

Strings:
    How strings are handled:
        someString =    this is a test
    This string is interpreted as "this is a test". It ignores all whitespace "around" the data.
    It is recommended to use quotes for strings, so that it is interpreted as you expect:
        someString = "    this is a test"
    Symbols in strings work fine too, including quotes:
        str = "!@#$%^&*()"""""""_+-="
    The first and last quote are used for determining what is contained in the string.

Sections:
    You can also have "sections", which are optional to use with code that uses this class.
    If you use sections in your code, then they must be specified correctly in order for options
        to be distinguished from other options in other sections.
    If no sections are used, a default section is used which is just an empty string.
        This also applies to options in the beginning of the file before any section headers.
        Specifying an empty string section with "[]" would have the same behavior as using no section.
        You can even use "" as a section in your code which refers to the default section.
            Notice that this is used as the default value for the section string parameters.

Sections Example:
    [SomeSection]
    option = value
    anotherOption = 123

    [AnotherSection]
    option = 5000

    // So "option" will be read as "value" in SomeSection, but 5000 in AnotherSection.

Comments:
    Comments can be made with most symbols (//, #, ::, and ;)
    Comments MUST be on their own lines
        The symbols are checked only in the beginning of each line
        The whole line is ignored if it is a comment
    Multi-line comments are also supported with /star and star/
        Note that "star" is really a "*"

Other Notes:
    Whitespace is ignored around setting names.
    Semicolons CANNOT be used to separate lines, only new lines can.
