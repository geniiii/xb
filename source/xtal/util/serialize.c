// TODO(geni): Linked list of operations, we go through each and get the required file size from there, then serialize... maybe?

//~ Serialization

internal void Xtal_SerializeData(Xtal_Serializer* serializer, void* data, u64 data_size) {
    switch (serializer->operation) {
        case Xtal_SerializerOperation_Write: {
            serializer->Write(serializer, serializer->offset + serializer->header_size, data, data_size);
        } break;
        case Xtal_SerializerOperation_Read: {
            serializer->Read(serializer, serializer->offset + serializer->header_size, data, data_size);
        } break;
    }
    serializer->offset += data_size;
}

internal void Xtal_SerializeString8(Xtal_Serializer* serializer, String8* str, Xtal_MArena* arena) {
    Xtal_Serialize(serializer, str->size);
    switch (serializer->operation) {
        case Xtal_SerializerOperation_Write: {
            serializer->Write(serializer, serializer->offset + serializer->header_size, str->s, str->size);
        } break;
        case Xtal_SerializerOperation_Read: {
            str->s = Xtal_MArenaPush(arena, str->size);
            serializer->Read(serializer, serializer->offset + serializer->header_size, str->s, str->size);
        } break;
    }
    serializer->offset += str->size;
}

//~ Creation

internal void Xtal_SerializerWriteToFile(Xtal_Serializer* serializer, u64 offset, u8* data, u64 data_size) {
    xtal_os->WriteToFile(serializer->file, data, data_size, offset, Xtal_OS_FileSeek_FromStart);
}
internal void Xtal_SerializerReadFromFile(Xtal_Serializer* serializer, u64 offset, u8* data, u64 data_size) {
    xtal_os->ReadFromFileTo(serializer->file, data, data_size, offset, Xtal_OS_FileSeek_FromStart);
}
internal void Xtal_SerializerDestroyFile(Xtal_Serializer* serializer) {
    xtal_os->CloseFile(serializer->file);
}

// TODO(geni): This is a very finicky API, as messing with the arena inbetween calls to write/read functions will break stuff
internal void Xtal_SerializerWriteToArena(Xtal_Serializer* serializer, u64 offset, u8* data, u64 data_size) {
    u8* alloc = Xtal_MArenaPush(serializer->arena, data_size);
    if (!serializer->mem) {
        serializer->mem = alloc;
    }
    Xtal_MemoryCopy(serializer->mem + offset, data, data_size);
}
internal void Xtal_SerializerReadFromArena(Xtal_Serializer* serializer, u64 offset, u8* data, u64 data_size) {
    u8* alloc = Xtal_MArenaPush(serializer->arena, data_size);
    if (!serializer->mem) {
        serializer->mem = alloc;
    }
    Xtal_MemoryCopy(data, serializer->mem + offset, data_size);
}
internal void Xtal_SerializerDestroyArena(Xtal_Serializer* serializer) {
    Xtal_MArenaPop(serializer->arena, serializer->offset + serializer->header_size);
}

internal void Xtal_SerializerWriteToMemory(Xtal_Serializer* serializer, u64 offset, u8* data, u64 data_size) {
    Xtal_MemoryCopy(serializer->mem + offset, data, data_size);
}
internal void Xtal_SerializerReadFromMemory(Xtal_Serializer* serializer, u64 offset, u8* data, u64 data_size) {
    Xtal_MemoryCopy(data, serializer->mem + offset, data_size);
}

internal void _Xtal_SerializeVersion(Xtal_Serializer* serializer) {
    serializer->header_size += sizeof serializer->version;
    switch (serializer->operation) {
        case Xtal_SerializerOperation_Write: {
            serializer->Write(serializer, 0, (void*) &serializer->version, sizeof serializer->version);
        } break;
        case Xtal_SerializerOperation_Read: {
            serializer->Read(serializer, 0, (void*) &serializer->version, sizeof serializer->version);
        } break;
    }
}

internal Xtal_Serializer _Xtal_SerializerNew(Xtal_SerializerOperation operation, u32 version, String8 path, Xtal_OS_FileOpen open_method) {
    Xtal_Serializer result = {
        .version   = version,
        .file      = xtal_os->OpenFile(path, Xtal_OS_FilePermission_Read | Xtal_OS_FilePermission_Write, open_method),
        .Write     = Xtal_SerializerWriteToFile,
        .Read      = Xtal_SerializerReadFromFile,
        .Destroy   = Xtal_SerializerDestroyFile,
        .operation = operation,
    };
    _Xtal_SerializeVersion(&result);
    return result;
}

internal Xtal_Serializer _Xtal_ArenaSerializerNew(Xtal_SerializerOperation operation, u32 version, Xtal_MArena* arena) {
    Xtal_Serializer result = {
        .version   = version,
        .arena     = arena,
        .Write     = Xtal_SerializerWriteToArena,
        .Read      = Xtal_SerializerReadFromArena,
        .Destroy   = Xtal_SerializerDestroyArena,
        .operation = operation,
    };
    _Xtal_SerializeVersion(&result);
    return result;
}

internal Xtal_Serializer _Xtal_MemorySerializerNew(Xtal_SerializerOperation operation, u32 version, u8* mem) {
    Xtal_Serializer result = {
        .version   = version,
        .mem       = mem,
        .Write     = Xtal_SerializerWriteToMemory,
        .Read      = Xtal_SerializerReadFromMemory,
        .operation = operation,
    };
    _Xtal_SerializeVersion(&result);
    return result;
}

internal void Xtal_SerializerDestroy(Xtal_Serializer* serializer) {
    if (serializer->Destroy) {
        serializer->Destroy(serializer);
    }
}

internal Xtal_Serializer Xtal_SerializerNewWrite(u32 version, String8 path) {
    return _Xtal_SerializerNew(Xtal_SerializerOperation_Write, version, path, Xtal_OS_FileOpen_CreateNew);
}
internal Xtal_Serializer Xtal_SerializerNewRead(String8 path) {
    return _Xtal_SerializerNew(Xtal_SerializerOperation_Read, 0, path, Xtal_OS_FileOpen_OpenExisting);
}

internal Xtal_Serializer Xtal_ArenaSerializerNewWrite(u32 version, Xtal_MArena* arena) {
    return _Xtal_ArenaSerializerNew(Xtal_SerializerOperation_Write, version, arena);
}
internal Xtal_Serializer Xtal_ArenaSerializerNewRead(Xtal_MArena* arena) {
    return _Xtal_ArenaSerializerNew(Xtal_SerializerOperation_Read, 0, arena);
}

internal Xtal_Serializer Xtal_MemorySerializerNewWrite(u32 version, u8* mem) {
    return _Xtal_MemorySerializerNew(Xtal_SerializerOperation_Write, version, mem);
}
internal Xtal_Serializer Xtal_MemorySerializerNewRead(u8* mem) {
    return _Xtal_MemorySerializerNew(Xtal_SerializerOperation_Read, 0, mem);
}
