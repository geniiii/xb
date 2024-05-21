Xtal_HashMapDeclareNamed(String8, String8, Xtal_INIVarsHashMap)
    Xtal_HashMapDeclareNamed(String8, Xtal_INIVarsHashMap, Xtal_INISectionsHashMap)

        typedef struct {
    String8 message;
    u32     line;
    u32     col;
} Xtal_INIError;

//~ Scanner

typedef u32 Xtal_INIScanner_TokenType;
enum {
#define TokenType(name) Xtal_INIScanner_TokenType_##name,
#include "ini_tokens.inc"
#undef TokenType
    Xtal_INIScanner_TokenType_Count,
};

typedef struct {
    String8 str;
} Xtal_INIScanner_TokenLiteral;

typedef struct {
    Xtal_INIScanner_TokenType    type;
    u32                          line;
    u32                          col;
    String8                      lexeme;
    Xtal_INIScanner_TokenLiteral literal;
} Xtal_INIScanner_Token;

#define XTAL_INISCANNER_MAX_ERRORS 256
typedef struct {
    String8                file;
    String8                source;
    u32                    start;
    u32                    current;
    u32                    line;
    u32                    col;
    Xtal_INIScanner_Token* tokens;
    Xtal_INIError          errors[XTAL_INISCANNER_MAX_ERRORS];
    b32                    unterminated_assignment;
    u32                    error_count;
} Xtal_INIScanner;

//~ Parser

#define XTAL_INIPARSER_MAX_ERRORS XTAL_INISCANNER_MAX_ERRORS
typedef struct {
    Xtal_INISectionsHashMap sections;
    u32                     error_count;
    Xtal_INIError*          errors[XTAL_INIPARSER_MAX_ERRORS + XTAL_INISCANNER_MAX_ERRORS];
} Xtal_INI;

typedef struct {
    Xtal_INIScanner*        scanner;
    u32                     current;
    String8                 current_section;
    u32                     error_count;
    Xtal_INIError           errors[XTAL_INIPARSER_MAX_ERRORS];
    b32                     panic_mode;
    Xtal_MArena             arena;
    Xtal_INISectionsHashMap sections;
} Xtal_INIParser;

internal Xtal_INI Xtal_INIParse(String8 filename, String8 contents);
internal void     Xtal_INIDestroy(Xtal_INI* ini);

internal String8*             Xtal_INIGetVar(Xtal_INIVarsHashMap* hm, String8 key);
internal Xtal_INIVarsHashMap* Xtal_INIGetSection(Xtal_INI* ini, String8 key);
internal Xtal_INIVarsHashMap* Xtal_INICreateSection(Xtal_INI* ini, String8 key);
internal void                 Xtal_INISetVar(Xtal_INIVarsHashMap* hm, String8 key, String8 val);

internal String8 Xtal_INISerialize(Xtal_INI* ini, Xtal_MArena* arena);

internal b32 Xtal_INILogErrors(Xtal_INI* ini);

internal void Xtal_INIAST_Expression(Xtal_INIParser* parser);
