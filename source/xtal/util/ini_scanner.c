#define XTAL_INISCANNER_MAX_ERRORS 256

typedef u32 Xtal_INIScanner_TokenType;
enum {
    Xtal_INIScanner_TokenType_Invalid,

    Xtal_INIScanner_TokenType_RightBracket,
    Xtal_INIScanner_TokenType_LeftBracket,
    Xtal_INIScanner_TokenType_Semicolon,

    Xtal_INIScanner_TokenType_Equal,

    Xtal_INIScanner_TokenType_Identifier,
    Xtal_INIScanner_TokenType_String,
    Xtal_INIScanner_TokenType_Number,
    Xtal_INIScanner_TokenType_Nil,
    Xtal_INIScanner_TokenType_False,
    Xtal_INIScanner_TokenType_True,

    Xtal_INIScanner_TokenType_EOL,
    Xtal_INIScanner_TokenType_EOF,
};

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

internal u8 _Xtal_INIScanner_Peek(Xtal_INIScanner* scanner) {
}

internal b32 _Xtal_INIScanner_Match(Xtal_INIScanner* scanner, u8 expected) {
}

internal void _Xtal_INIScanner_Advance(Xtal_INIScanner* scanner) {
    ++scanner->current;
    ++scanner->col;
}

internal void _Xtal_INIScannerHandleNumLiteral(Xtal_INIScanner* scanner, Xtal_INIScanner_Token* token) {
    u32 start = scanner->current;
    // TODO(geni): Basically bad Odin string slices lmao, idk if this even works
    i64 num = S8_GetFirstI64((String8){scanner->source.data + start, scanner->source.size - start});
    *token  = (Xtal_INIScanner_Token){
         .type    = Xtal_INIScanner_TokenType_Number,
         .literal = {
                     .num = num,
                     .tag = Xtal_INIScanner_TokenLiteral_Number,
                     },
    };
}

internal void _Xtal_INIScannerHandleIdentifier(Xtal_INIScanner* scanner, Xtal_INIScanner_Token* token) {
    u32 start = scanner->current;

    while (CharIsAlpha(_Xtal_INIScanner_Peek(scanner))) {
        _Xtal_INIScanner_Advance(scanner);
    }
    String8 lexeme = {.data = scanner->source.data + start, .size = scanner->source.size - scanner->current};
    *token         = (Xtal_INIScanner_Token){
                .type   = Xtal_INIScanner_TokenType_Identifier,
                .lexeme = lexeme,
    };
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
            token.type =.EQUAL
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
            token.type    =.STRING;
            token.literal = scanner.source [start_of_lit:scanner.current];
            token.line    = start_line;
            token.col     = start_col;
            advance(scanner);
        } break;
        case '\n': {
            scanner->line += 1;
            scanner->col = 0;
            token.type   =.EOL;
        } break;
        case ' ', '\r', '\t':
        break case '[':
        token.type =.LEFT_BRACKET case ']':
        token.type =.RIGHT_BRACKET case:
            if unicode
                .is_digit(char) {
                start_line:
                    = scanner.line
                                                                                                                                       start_col :                                                                                     = scanner.col if !handle_num_literal (scanner, &token){
                                          error_at(scanner, "Invalid numeric literal", start_line, start_col)} token.line = start_line token.col = start_col
                }
            else if unicode
                .is_alpha(char) {
                start_line:
                    = scanner.line
                                                                                                                                  start_col :                                                                                = scanner.col if !handle_identifier (scanner, &token){
                                          error_at(scanner, "Invalid identifier", start_line, start_col)} token.line = start_line token.col = start_col
                }
            else {
                error(scanner, "Received unexpected character")
                    ok = false
            }
    }

    return
}

scan_tokens ::proc(scanner
                   : ^TokenScanner) {
        for
            scanner.current < u32(len(scanner.source)) {
                token, ok : = scan_token(scanner) if !ok {
                    continue
                }
                if token
                    .type ==.INVALID{
                                continue} append(&scanner.tokens, token)
            }
        append(
            &scanner.tokens,
            Token{
                type =.EOF,
                line = scanner.line,
                col  = scanner.tokens[len(scanner.tokens) - 1].col + 1,
            }, )
}
