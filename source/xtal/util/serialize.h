enum {
    Xtal_SerializerOperation_Write,
    Xtal_SerializerOperation_Read,
};
typedef u32 Xtal_SerializerOperation;

typedef struct Xtal_Serializer Xtal_Serializer;
struct Xtal_Serializer {
    u32                      version;
    Xtal_SerializerOperation operation;

    void (*Write)(Xtal_Serializer* serializer, u64 offset, u8* data, u64 data_size);
    void (*Read)(Xtal_Serializer* serializer, u64 offset, u8* data, u64 data_size);
    void (*Destroy)(Xtal_Serializer* serializer);

    u64 offset;
    u64 header_size;

    Xtal_OS_File file;
    Xtal_MArena* arena;
    u8*          mem;
};

#define Xtal_Serialize(serializer, data) Xtal_SerializeData(serializer, &data, sizeof(data))
#define Xtal_SerializeV(serializer, data, _version) \
    do {                                            \
        if ((serializer)->version >= (_version)) {  \
            Xtal_Serialize(serializer, data);       \
        }                                           \
    } while (0)
internal void Xtal_SerializeData(Xtal_Serializer* serializer, void* data, u64 data_size);
internal void Xtal_SerializeString8(Xtal_Serializer* serializer, String8* str, Xtal_MArena* arena);
internal void Xtal_SerializerDestroy(Xtal_Serializer* serializer);

//~ File

internal Xtal_Serializer Xtal_SerializerNewWrite(u32 version, String8 path);
internal Xtal_Serializer Xtal_SerializerNewRead(String8 path);
internal void            Xtal_SerializerWriteToFile(Xtal_Serializer* serializer, u64 offset, u8* data, u64 data_size);
internal void            Xtal_SerializerReadFromFile(Xtal_Serializer* serializer, u64 offset, u8* data, u64 data_size);
internal void            Xtal_SerializerDestroyFile(Xtal_Serializer* serializer);

//~ Arena

internal Xtal_Serializer Xtal_ArenaSerializerNewWrite(u32 version, Xtal_MArena* arena);
internal Xtal_Serializer Xtal_ArenaSerializerNewRead(Xtal_MArena* arena);
internal void            Xtal_SerializerWriteToArena(Xtal_Serializer* serializer, u64 offset, u8* data, u64 data_size);
internal void            Xtal_SerializerReadFromArena(Xtal_Serializer* serializer, u64 offset, u8* data, u64 data_size);
internal void            Xtal_SerializerDestroyArena(Xtal_Serializer* serializer);

//~ Memory

internal Xtal_Serializer Xtal_MemorySerializerNewWrite(u32 version, u8* mem);
internal Xtal_Serializer Xtal_MemorySerializerNewRead(u8* mem);
internal void            Xtal_SerializerWriteToMemory(Xtal_Serializer* serializer, u64 offset, u8* data, u64 data_size);
internal void            Xtal_SerializerReadFromMemory(Xtal_Serializer* serializer, u64 offset, u8* data, u64 data_size);
