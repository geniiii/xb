typedef struct {
    String8                      identifier;
    Xtal_INIScanner_TokenLiteral right;
} BindingExpr;

internal void Xtal_INIAST_Binding(Xtal_INIParser* parser) {
    if (_Xtal_INIParser_Consume(parser, Xtal_INIScanner_TokenType_Identifier)) {
        BindingExpr result = {
            .identifier = _Xtal_INIParser_PeekPrevious(parser)->lexeme,
        };
        if (_Xtal_INIParser_Consume(parser, Xtal_INIScanner_TokenType_Number) || _Xtal_INIParser_Consume(parser, Xtal_INIScanner_TokenType_String)) {
            result.right = _Xtal_INIParser_PeekPrevious(parser)->literal;
        }
        u64                  hash = XXH64(parser->current_section.data, parser->current_section.size, 0);
        Xtal_INIVarsHashMap* hm;
        Xtal_INISectionsHashMap_Get(&parser->sections, parser->current_section, hash, &hm);
        // TODO(geni): This hashmap probably shouldn't have String8 as the value
        // TODO(geni): I'm not sure if inserting result.right.str works with blank expressions like "varName ="
        Xtal_INIVarsHashMap_Insert(hm, result.identifier, XXH64(result.identifier.data, result.identifier.size, 0), result.right.str);

        if (!_Xtal_INIParser_Consume(parser, Xtal_INIScanner_TokenType_EOL)) {
            _Xtal_INIParser_ErrorAt(parser, S8Lit("Expected end of line"), start->line, start->col);
            // TODO(geni): Proper error handling (idk what to do here lmao)
        }
    }
    // NOTE(geni): Ignore all remaining whitespace
    while (_Xtal_INIParser_Consume(parser, Xtal_INIScanner_TokenType_EOL)) {}
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
        u64                 hash        = XXH64(parser->current_section.data, parser->current_section.size, 0);
        if (Xtal_INISectionsHashMap_Get(&parser->sections, parser->current_section, hash, NULL)) {
            _Xtal_INIParser_ErrorAt(parser, S8Lit("Duplicate section definition"), start->line, start->col);
            // TODO(geni): Proper error handling (idk what to do here lmao)
        } else {
            Xtal_INISectionsHashMap_Insert(&parser->sections, parser->current_section, hash, section_map);
        }

        if (!_Xtal_INIParser_Consume(parser, Xtal_INIScanner_TokenType_EOL)) {
            _Xtal_INIParser_ErrorAt(parser, S8Lit("Expected end of line"), start->line, start->col);
            // TODO(geni): Proper error handling (idk what to do here lmao)
        }
    } else {
        Xtal_INIAST_Binding(parser);
    }
}

internal void Xtal_INIAST_Expression(Xtal_INIParser* parser) {
    return Xtal_INIAST_Section(parser);
}
