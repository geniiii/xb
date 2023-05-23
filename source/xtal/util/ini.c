internal b32 Xtal_HashMapCompare_String8(String8* a, String8 b, u64 b_hash) {
    return a->size == b.size && XXH64(a->data, a->size, 0) == b_hash && memcmp(a->data, b.data, b.size) == 0;
}

Xtal_HashMapDefineNamed(String8, String8, Xtal_INIVarsHashMap, Xtal_HashMapCompare_String8)
    Xtal_HashMapDefineNamed(String8, Xtal_INIVarsHashMap, Xtal_INISectionsHashMap, Xtal_HashMapCompare_String8)

        typedef Xtal_INIScanner_Error Xtal_INIParser_Error;
#define XTAL_INIPARSER_MAX_ERRORS XTAL_INISCANNER_MAX_ERRORS

typedef struct {
    Xtal_INISectionsHashMap sections;
} Xtal_INI;

typedef struct {
    Xtal_INIScanner*        scanner;
    u32                     current;
    String8                 current_section;
    u32                     error_count;
    Xtal_INIParser_Error    errors[XTAL_INIPARSER_MAX_ERRORS];
    b32                     panic_mode;
    Xtal_MArena             arena;
    Xtal_INISectionsHashMap sections;
} Xtal_INIParser;

internal Xtal_INIScanner_Token* _Xtal_INIParser_Peek(Xtal_INIParser* parser) {
    return &parser->scanner->tokens[parser->current];
}
internal Xtal_INIScanner_Token* _Xtal_INIParser_PeekPrevious(Xtal_INIParser* parser) {
    return &parser->scanner->tokens[parser->current - 1];
}
internal void _Xtal_INIParser_Advance(Xtal_INIParser* parser) {
    ++parser->current;
}
internal b32 _Xtal_INIParser_IsAtEnd(Xtal_INIParser* parser) {
    return _Xtal_INIParser_Peek(parser)->type == Xtal_INIScanner_TokenType_EOF;
}
internal b32 _Xtal_INIParser_Consume(Xtal_INIParser* parser, Xtal_INIScanner_TokenType expected) {
    if (_Xtal_INIParser_Peek(parser)->type == expected) {
        _Xtal_INIParser_Advance(parser);
        return 1;
    }
    return 0;
}
internal void _Xtal_INIParser_ErrorAt(Xtal_INIParser* parser, String8 message, u32 line, u32 col) {
    if (parser->error_count == sizeof parser->errors - 2) {
        parser->errors[parser->error_count] = (Xtal_INIParser_Error){
            .message = S8Lit("Reached error limit"),
        };
    }
    parser->errors[parser->error_count++] = (Xtal_INIParser_Error){
        .message = message,
        .line    = line,
        .col     = col,
    };
}
internal void _Xtal_INIParser_ErrorAtCurrent(Xtal_INIParser* parser, String8 message) {
    _Xtal_INIParser_ErrorAt(parser, message, _Xtal_INIParser_Peek(parser)->line, _Xtal_INIParser_Peek(parser)->col);
}

internal Xtal_INI Xtal_INIParse(String8 file) {
    Xtal_INIScanner scanner = Xtal_INIScannerNew(S8Lit("aaa"), file);
    Xtal_INIScanner_ScanTokens(&scanner);

    Xtal_INIParser parser = {
        .scanner = &scanner,
    };

    for (u32 i = 0; i < Xtal_ArenaArrayCount(scanner.tokens); ++i) {
        String8 name = Xtal_INIScanner_TokenTypeName(scanner.tokens[i].type);
        if (scanner.tokens[i].type == Xtal_INIScanner_TokenType_String) {
            Log("%S = %S", name, scanner.tokens[i].literal.str);
        }
        switch (scanner.tokens[i].literal.tag) {
            case Xtal_INIScanner_TokenLiteral_String8: Log("%S = %S", name, scanner.tokens[i].literal.str); break;
            case Xtal_INIScanner_TokenLiteral_Real: Log("%S = %f", name, scanner.tokens[i].literal.real); break;
            case Xtal_INIScanner_TokenLiteral_Number: Log("%S = %lli", name, scanner.tokens[i].literal.num); break;
            default: scanner.tokens[i].lexeme.size == 0 ? Log("%S", name) : Log("%S = %S", name, scanner.tokens[i].lexeme);
        }
    }

    while (!_Xtal_INIParser_IsAtEnd(&parser)) {
        Xtal_INIAST_Expression(&parser);
    }

    return (Xtal_INI){0};
}
internal Xtal_INIVarsHashMap* Xtal_INIGetSection(Xtal_INI* ini, String8 key);
internal String8*             Xtal_INIGetVar(Xtal_INIVarsHashMap* section, String8 key);
