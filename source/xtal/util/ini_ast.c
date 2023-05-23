typedef struct {
    String8                      identifier;
    Xtal_INIScanner_TokenLiteral right;
} BindingExpr;

internal void Xtal_INIAST_Primary(Xtal_INIParser* parser) {
}

internal void Xtal_INIAST_Binding(Xtal_INIParser* parser) {
    if (_Xtal_INIParser_Consume(parser, Xtal_INIScanner_TokenType_Identifier)) {
        BindingExpr result = {
            .identifier = _Xtal_INIParser_PeekPrevious(parser)->lexeme,
        };
        if (_Xtal_INIParser_Consume(parser, Xtal_INIScanner_TokenType_Number) || _Xtal_INIParser_Consume(parser, Xtal_INIScanner_TokenType_String)) {
            result.right = _Xtal_INIParser_PeekPrevious(parser)->literal;
        }
    }
}

internal void Xtal_INIAST_Section(Xtal_INIParser* parser) {
    if (_Xtal_INIParser_Consume(parser, Xtal_INIScanner_TokenType_LeftBracket)) {
        Xtal_INIScanner_Token* start = _Xtal_INIParser_PeekPrevious(parser);
        if (!_Xtal_INIParser_Consume(parser, Xtal_INIScanner_TokenType_Identifier)) {
            // TODO(geni): Actually format this
            _Xtal_INIParser_ErrorAt(parser, S8Lit("Expected literal, got %S instead"), start->line, start->col);
            // TODO(geni): Proper error handling (idk what to do here lmao)
        }
        Xtal_INIScanner_Token* section = _Xtal_INIParser_PeekPrevious(parser);
        if (!_Xtal_INIParser_Consume(parser, Xtal_INIScanner_TokenType_RightBracket)) {
            _Xtal_INIParser_ErrorAt(parser, S8Lit("Unterminated section (expected ']')"), start->line, start->col);
            // TODO(geni): Proper error handling (idk what to do here lmao)
        }
        parser->current_section         = section->lexeme;
        Xtal_INIVarsHashMap section_map = Xtal_INIVarsHashMap_New(8);
        Xtal_INISectionsHashMap_Insert(&parser->sections, parser->current_section, XXH64(parser->current_section.data, parser->current_section.size, 0), section_map);
    } else {
        Xtal_INIAST_Binding(parser);
    }
}

internal void Xtal_INIAST_Expression(Xtal_INIParser* parser) {
    return Xtal_INIAST_Section(parser);
}
