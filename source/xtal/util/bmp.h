typedef u32 Xtal_BMP_Compression;
enum {
    // TODO(geni): Support BI_BITFIELDS and BI_ALPHABITFIELDS
    Xtal_BMP_CompressionNone = 0,
};

#pragma pack(push, 1)
typedef struct {
    u8  signature[2];  // NOTE(geni): Should always be "BM"
    u32 size;          // NOTE(geni): Size of file
    u8  reserved[4];   // NOTE(geni): Reserved
    u32 data_offset;   // NOTE(geni): Offset where actual bitmap data starts
} Xtal_BMP_FileHeader;

typedef struct {
    u32 header_size;              // NOTE(geni): Should always be equal to sizeof(Xtal_BMP_InfoHeader) = 40

    ivec2s               size;    // NOTE(geni): Size in pixels
    u16                  planes;  // NOTE(geni): Must always be 1
    u16                  depth;   // NOTE(geni): Color depth (bits per pixel), depth ∈ {1, 4, 8, 16, 24, 32}
    Xtal_BMP_Compression compression;

    u32 data_size;         // NOTE(geni): Size of actual bitmap data

    i32 h_ppm;             // NOTE(geni): Horizontal resolution in pixels per meter
    i32 v_ppm;             // NOTE(geni): Vertical resolution in pixels per meter

    u32 num_colors;        // NOTE(geni): A value of 0 means 2^depth
    u32 important_colors;  // NOTE(geni): Ignored

    // TODO(geni): Bitmasks for BI_BITFIELDS/BI_ALPHABITFIELDS
    // NOTE(geni): Optional

} Xtal_BMP_InfoHeader;

typedef struct {
    Xtal_BMP_FileHeader header;
    Xtal_BMP_InfoHeader info;
} Xtal_BMP_File;
#pragma pack(pop)

typedef struct {
    ivec2s size;
    u8*    data;  // NOTE(geni): Always R8B8G8A8
} Xtal_BMP;
typedef struct {
    Xtal_BMP bmp;
    b32      has_error;
    String8  error;
} Xtal_BMP_LoadResult;

internal Xtal_BMP_LoadResult Xtal_BMP_Load(Xtal_MArena* arena, const void* file_data, u64 file_data_size);
internal Xtal_BMP_LoadResult Xtal_BMP_LoadFromPath(Xtal_MArena* arena, String8 path);
