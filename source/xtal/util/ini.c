internal b32 Xtal_HashMapCompare_String8(String8* a, String8 b, u64 b_hash) {
    return a->size == b.size && XXH64(a->data, a->size, 0) == b_hash && memcmp(a->data, b.data, b.size) == 0;
}
Xtal_HashMapDefineNamed(String8, String8, Xtal_INIVarsHashMap, Xtal_HashMapCompare_String8)
    Xtal_HashMapDefineNamed(String8, Xtal_INIVarsHashMap, Xtal_INISectionsHashMap, Xtal_HashMapCompare_String8)

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
        parser->errors[parser->error_count] = (Xtal_INIError){
            .message = S8Lit("Reached error limit"),
        };
    }
    parser->errors[parser->error_count++] = (Xtal_INIError){
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
    Xtal_INI result = {
        .sections = parser.sections,
    };
    // NOTE(geni): It's not possible for error_count to go over the maximum size of result.errors
    for (u32 i = 0; i < parser.error_count; ++i) {
        result.errors[result.error_count++] = &parser.errors[i];
    }
    for (u32 i = 0; i < scanner.error_count; ++i) {
        result.errors[result.error_count++] = &scanner.errors[i];
    }
    return result;
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
internal Xtal_INIVarsHashMap* Xtal_INICreateSection(Xtal_INI* ini, String8 key) {
    if (ini == NULL) {
        return NULL;
    }
    Xtal_INIVarsHashMap* section_hm;
    if (!Xtal_INISectionsHashMap_Get(&ini->sections, key, XXH64(key.data, key.size, 0), &section_hm)) {
        u64 index = Xtal_INISectionsHashMap_Insert(&ini->sections, key, XXH64(key.data, key.size, 0), Xtal_INIVarsHashMap_New(8));
        return &ini->sections.buckets[index].value;
    }
    return NULL;
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
internal void Xtal_INISetVar(Xtal_INIVarsHashMap* hm, String8 key, String8 val) {
    if (hm == NULL) {
        return;
    }
    String8* value;
    if (!Xtal_INIVarsHashMap_Get(hm, key, XXH64(key.data, key.size, 0), &value)) {
        Xtal_INIVarsHashMap_Insert(hm, key, XXH64(key.data, key.size, 0), val);
    } else {
        *value = val;
    }
}
internal void Xtal_INIDestroy(Xtal_INI* ini) {
    u32 destroyed_sections = 0;
    for (u32 i = 0; i < ini->sections.capacity && destroyed_sections < ini->sections.count; ++i) {
        Xtal_INISectionsHashMap_Bucket* current_bucket = &ini->sections.buckets[i];
        if (!_Xtal_HashMapOccupied(current_bucket->hash)) {
            continue;
        }
        Xtal_INIVarsHashMap_Destroy(&current_bucket->value);
        ++destroyed_sections;
    }
    Xtal_INISectionsHashMap_Destroy(&ini->sections);
}
internal String8 Xtal_INISerialize(Xtal_INI* ini, Xtal_MArena* arena) {
    Xtal_MArenaTemp scratch = Xtal_GetScratch(NULL, 0);
    S8List          list    = {0};

    u32 serialized_sections = 0;
    for (u32 i = 0; i < ini->sections.capacity && serialized_sections < ini->sections.count; ++i) {
        Xtal_INISectionsHashMap_Bucket* current_bucket = &ini->sections.buckets[i];
        if (!_Xtal_HashMapOccupied(current_bucket->hash)) {
            continue;
        }
        Xtal_INIVarsHashMap* current = &current_bucket->value;

        S8ListPushF(&list, scratch.arena, "[%S]", current_bucket->key);
        u32 serialized_vars = 0;
        for (u32 j = 0; j < current->capacity && serialized_vars < current->count; ++j) {
            Xtal_INIVarsHashMap_Bucket* var_bucket = &current->buckets[j];
            if (!_Xtal_HashMapOccupied(var_bucket->hash)) {
                continue;
            }
            String8* var = &var_bucket->value;
            S8ListPushF(&list, scratch.arena, "%S = %S", var_bucket->key, *var);
            ++serialized_vars;
        }

        ++serialized_sections;
    }

    String8 result = S8ListJoin(&list, arena, S8Lit("\n"));
    Xtal_MArenaTempEnd(scratch);
    return result;
}

internal b32 Xtal_INILogErrors(Xtal_INI* ini) {
    if (ini->error_count > 0) {
        for (u32 i = 0; i < ini->error_count; ++i) {
        }
        return 1;
    }
    return 0;
}
