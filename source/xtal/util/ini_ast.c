enum {
    Xtal_INIAST_BindingExpr,
    Xtal_INIAST_SectionExpr,
    Xtal_INIAST_LiteralExpr,
};
typedef u32 Xtal_INIAST_ExprType;

typedef struct Expr Expr;

typedef struct {
    Expr* left;
    Expr* right;
} BindingExpr;
typedef struct {
    Expr* expr;
} SectionExpr;
typedef struct {
    Xtal_INIScanner_TokenLiteral expr;
} LiteralExpr;

struct Expr {
    Xtal_INIAST_ExprType tag;
    union {
        BindingExpr binding;
        SectionExpr section;
        LiteralExpr literal;
    };
};

internal Expr* Xtal_INIAST_Primary(Xtal_INIParser* parser) {
}

internal Expr* Xtal_INIAST_Section(Xtal_INIParser* parser) {
    if (_Xtal_INIParser_Consume(parser, Xtal_INIScanner_TokenType_LeftBracket)) {
        Xtal_INIScanner_Token* start   = _Xtal_INIParser_PeekPrevious(parser);
        Expr*                  section = Xtal_INIAST_Primary(parser);
        if (!_Xtal_INIParser_Consume(parser, Xtal_INIScanner_TokenType_RightBracket)) {
            _Xtal_INIParser_ErrorAt(parser, S8Lit("Unterminated section (expected ']')"), start->line, start->col);
            // TODO(geni): Proper error handling (idk what to do here lmao)
        }
        parser->current_section = section->literal.expr.str;
    }
}

internal Expr* Xtal_INIAST_Expression(Xtal_INIParser* parser) {
    return Xtal_INIAST_Section(parser);
}
