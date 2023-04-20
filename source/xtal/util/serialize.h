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
