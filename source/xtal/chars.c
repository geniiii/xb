//~ Character check functions

#define CharIsSpace(c) ((c) <= 32)
#define CharIsDigit(c) ((c) >= '0' && (c) <= '9')
#define CharIsNumeric  CharIsDigit

internal inline b32 CharIsAlpha(char c) {
    return (c >= 'a' && c <= 'z') ||
           (c >= 'A' && c <= 'Z');
}

internal inline b32 CharIsSymbol(char c) {
    return c == '~' ||
           c == '!' ||
           c == '%' ||
           c == '^' ||
           c == '&' ||
           c == '*' ||
           c == '(' ||
           c == ')' ||
           c == '[' ||
           c == ']' ||
           c == '{' ||
           c == '}' ||
           c == '-' ||
           c == '+' ||
           c == '=' ||
           c == ';' ||
           c == ':' ||
           c == '<' ||
           c == '>' ||
           c == '/' ||
           c == '?' ||
           c == '.' ||
           c == ',';
}

//~ Character conversion functions

internal inline char CharToLower(char c) {
    if (c >= 'A' && c <= 'Z') {
        return c + 32;
    }
    return c;
}

internal inline char CharToUpper(char c) {
    if (c >= 'a' && c <= 'z') {
        return c - 32;
    }
    return c;
}
