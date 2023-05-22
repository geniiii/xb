#define XTAL_INISCANNER_MAX_ERRORS 256

typedef u32 Xtal_INIScanner_TokenType;
enum {
#define TokenType(name) Xtal_INIScanner_TokenType_##name,
#include "ini_tokens.inc"
#undef TokenType
    Xtal_INIScanner_TokenType_Count,
};
internal String8 Xtal_INIScanner_TokenTypeName(Xtal_INIScanner_TokenType index) {
    local_persist String8 strings[Xtal_INIScanner_TokenType_Count] = {
#define TokenType(name) S8LitComp(#name),
#include "ini_tokens.inc"
#undef TokenType
    };

    return index < ArrayCount(strings) ? strings[index] : S8Lit("Invalid");
}

typedef struct {
    union {
        String8 str;
        f64     real;
        i64     num;
        b32     boolean;
    };
    enum {
        Xtal_INIScanner_TokenLiteral_None,
        Xtal_INIScanner_TokenLiteral_String8,
        Xtal_INIScanner_TokenLiteral_Real,
        Xtal_INIScanner_TokenLiteral_Number,
        Xtal_INIScanner_TokenLiteral_Boolean,
    } tag;
} Xtal_INIScanner_TokenLiteral;

typedef struct {
    Xtal_INIScanner_TokenType    type;
    u32                          line;
    u32                          col;
    String8                      lexeme;
    Xtal_INIScanner_TokenLiteral literal;
} Xtal_INIScanner_Token;

typedef struct {
    String8 message;
    u32     line;
    u32     col;
} Xtal_INIScanner_Error;

typedef struct {
    String8                file;
    String8                source;
    u32                    start;
    u32                    current;
    u32                    line;
    u32                    col;
    Xtal_INIScanner_Token* tokens;
    Xtal_INIScanner_Error  errors[XTAL_INISCANNER_MAX_ERRORS];
    b32                    unterminated_assignment;
    u32                    error_count;
} Xtal_INIScanner;

internal Xtal_INIScanner Xtal_INIScannerNew(String8 file, String8 source) {
    return (Xtal_INIScanner){
        .file   = file,
        .source = source,
        .tokens = Xtal_ArenaArrayNewN(Xtal_INIScanner_Token, 8192),
    };
}

internal void _Xtal_INIScanner_ErrorAt(Xtal_INIScanner* scanner, String8 message, u32 line, u32 col) {
    if (scanner->error_count == sizeof scanner->errors - 2) {
        scanner->errors[scanner->error_count] = (Xtal_INIScanner_Error){
            .message = S8Lit("Reached error limit"),
        };
    }
    scanner->errors[scanner->error_count++] = (Xtal_INIScanner_Error){
        .message = message,
        .line    = line,
        .col     = col,
    };
}
internal void _Xtal_INIScanner_ErrorAtCurrent(Xtal_INIScanner* scanner, String8 message) {
    _Xtal_INIScanner_ErrorAt(scanner, message, scanner->line, scanner->col);
}

internal void _Xtal_INIScanner_Advance(Xtal_INIScanner* scanner) {
    ++scanner->current;
    ++scanner->col;
}

internal char _Xtal_INIScanner_Peek(Xtal_INIScanner* scanner) {
    return scanner->source.cstr[scanner->current];
}
internal b32 _Xtal_INIScanner_Match(Xtal_INIScanner* scanner, u8 expected) {
    if (scanner->current >= scanner->source.size) {
        return false;
    }
    // TODO(geni): Not actually using runes.. yet
    u8 read_rune = _Xtal_INIScanner_Peek(scanner);
    if (read_rune != expected) {
        return false;
    }
    _Xtal_INIScanner_Advance(scanner);
    return true;
}

internal b32 _Xtal_INIScannerHandleNumLiteral(Xtal_INIScanner* scanner, Xtal_INIScanner_Token* token) {
    u32 start    = scanner->current - 1;
    b32 is_float = 0;
    while (CharIsNumeric(_Xtal_INIScanner_Peek(scanner))) {
        _Xtal_INIScanner_Advance(scanner);
        if (_Xtal_INIScanner_Peek(scanner) == '.') {
            is_float = 1;
            _Xtal_INIScanner_Advance(scanner);
        }
    }
    // TODO(geni): Basically bad Odin string slices lmao
    String8 num_str = {scanner->source.str + start, scanner->source.size - scanner->current};
    if (is_float) {
        // NOTE(geni): Might want to add TokenType_Real?
        f64 num = strtod(num_str.cstr, NULL);
        *token  = (Xtal_INIScanner_Token){
             .type    = Xtal_INIScanner_TokenType_Number,
             .literal = {
                         .real = num,
                         .tag  = Xtal_INIScanner_TokenLiteral_Real,
                         },
        };
    } else {
        i64 num = S8_GetFirstI64(num_str);
        *token  = (Xtal_INIScanner_Token){
             .type    = Xtal_INIScanner_TokenType_Number,
             .literal = {
                         .num = num,
                         .tag = Xtal_INIScanner_TokenLiteral_Number,
                         }
        };
    }
    return 1;
}

// TODO(geni): Bad name, this also handles strings
internal b32 _Xtal_INIScannerHandleIdentifier(Xtal_INIScanner* scanner, Xtal_INIScanner_Token* token) {
    u32 start = scanner->current - 1;

    while (scanner->unterminated_assignment ? (_Xtal_INIScanner_Peek(scanner) != '\r' && _Xtal_INIScanner_Peek(scanner) != '\n') : CharIsAlpha(_Xtal_INIScanner_Peek(scanner))) {
        _Xtal_INIScanner_Advance(scanner);
    }

    String8 lexeme = {.str = scanner->source.str + start, .size = scanner->current - start};
    if (lexeme.size == 0) {
        lexeme = S8Lit("");
    }
    *token = (Xtal_INIScanner_Token){
        .type   = scanner->unterminated_assignment ? Xtal_INIScanner_TokenType_String : Xtal_INIScanner_TokenType_Identifier,
        .lexeme = lexeme,
    };
    return 1;
}

internal b32 Xtal_INIScanner_ScanToken(Xtal_INIScanner* scanner, Xtal_INIScanner_Token* out) {
    b32  ok = true;
    char c  = _Xtal_INIScanner_Peek(scanner);
    _Xtal_INIScanner_Advance(scanner);

    Xtal_INIScanner_Token token = {
        .line = scanner->line,
        .col  = scanner->col,
    };

    switch (c) {
        case ';':
        case '#': {
            while (scanner->current < sizeof scanner->source && _Xtal_INIScanner_Peek(scanner) != '\n') {
                _Xtal_INIScanner_Advance(scanner);
            }
        } break;
        case '=': {
            token.type                       = Xtal_INIScanner_TokenType_Equal;
            scanner->unterminated_assignment = 1;
        } break;
        case '"': {
            u32 start_line   = scanner->line;
            u32 start_col    = scanner->col;
            u32 start_of_lit = scanner->current;
            while (_Xtal_INIScanner_Peek(scanner) != '"') {
                if (scanner->current >= sizeof scanner->source) {
                    _Xtal_INIScanner_ErrorAt(scanner, S8Lit("Unterminated string"), start_line, start_col);
                    ok = false;
                    goto end;
                }
                _Xtal_INIScanner_Advance(scanner);
            }
            token.type = Xtal_INIScanner_TokenType_String;
            // TODO(geni): Slices..
            token.literal = (Xtal_INIScanner_TokenLiteral){
                .tag = Xtal_INIScanner_TokenLiteral_String8,
                .str = {.str = scanner->source.str + start_of_lit, .size = scanner->source.size - start_of_lit - scanner->current},
            };
            token.line = start_line;
            token.col  = start_col;
            _Xtal_INIScanner_Advance(scanner);
            scanner->unterminated_assignment = 1;
        } break;
        case '\n': {
            scanner->line += 1;
            scanner->col                     = 0;
            scanner->unterminated_assignment = 0;
            token.type                       = Xtal_INIScanner_TokenType_EOL;
        } break;
        case ' ':
        case '\r':
        case '\t': break;
        case '[': {
            token.type = Xtal_INIScanner_TokenType_LeftBracket;
        } break;
        case ']': {
            token.type = Xtal_INIScanner_TokenType_RightBracket;
        } break;
        default:
            if (CharIsDigit(c)) {
                u32 start_line = scanner->line;
                u32 start_col  = scanner->col;
                if (!_Xtal_INIScannerHandleNumLiteral(scanner, &token)) {
                    _Xtal_INIScanner_ErrorAt(scanner, S8Lit("Invalid numeric literal"), start_line, start_col);
                    ok = false;
                    goto end;
                }
                token.line = start_line;
                token.col  = start_col;
            } else if (CharIsAlpha(c)) {
                u32 start_line = scanner->line;
                u32 start_col  = scanner->col;
                if (!_Xtal_INIScannerHandleIdentifier(scanner, &token)) {
                    ok = false;
                    goto end;
                }
                token.line = start_line;
                token.col  = start_col;
            } else {
                _Xtal_INIScanner_ErrorAtCurrent(scanner, S8Lit("Received unexpected character"));
                ok = false;
                goto end;
            }
    }

    *out = token;
end:
    return ok;
}

internal void Xtal_INIScanner_ScanTokens(Xtal_INIScanner* scanner) {
    Xtal_INIScanner_Token token;
    while (scanner->current < scanner->source.size) {
        if (!Xtal_INIScanner_ScanToken(scanner, &token)) {
            continue;
        }
        if (token.type == Xtal_INIScanner_TokenType_Invalid) {
            continue;
        }
        Xtal_ArenaArrayPush(scanner->tokens, token);
    }
    token = (Xtal_INIScanner_Token){
        .type = Xtal_INIScanner_TokenType_EOF,
        .line = scanner->line,
        .col  = scanner->tokens[Xtal_ArenaArrayCount(scanner->tokens) - 1].col + 1,
    };
}
