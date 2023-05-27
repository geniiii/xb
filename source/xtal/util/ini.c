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

internal void     Xtal_INIAST_Expression(Xtal_INIParser* parser);
internal Xtal_INI Xtal_INIParse(String8 filename, String8 contents) {
    Xtal_INIScanner scanner = Xtal_INIScannerNew(filename, contents);
    Xtal_INIScanner_ScanTokens(&scanner);

    Xtal_INIParser parser = {
        .scanner         = &scanner,
        .sections        = Xtal_INISectionsHashMap_New(8),
        .current_section = S8Lit(""),
    };
    while (!_Xtal_INIParser_IsAtEnd(&parser)) {
        Xtal_INIAST_Expression(&parser);
    }
    return (Xtal_INI){
        .sections = parser.sections,
    };
}

internal Xtal_INIVarsHashMap* Xtal_INIGetSection(Xtal_INI* ini, String8 key) {
    if (ini == NULL) {
        return NULL;
    }
    Xtal_INIVarsHashMap* section_hm;
    if (!Xtal_INISectionsHashMap_Get(&ini->sections, key, XXH64(key.data, key.size, 0), &section_hm)) {
        section_hm = NULL;
    }
    return section_hm;
}
internal String8* Xtal_INIGetVar(Xtal_INIVarsHashMap* hm, String8 key) {
    if (hm == NULL) {
        return NULL;
    }
    String8* value;
    if (!Xtal_INIVarsHashMap_Get(hm, key, XXH64(key.data, key.size, 0), &value)) {
        value = NULL;
    }
    return value;
}
internal void Xtal_INIDestroy(Xtal_INI* ini) {
    // TODO(geni): Unimplemented
}

internal String8 Xtal_INISerialize(Xtal_INI* ini, Xtal_MArena arena) {
}
