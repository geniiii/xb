typedef u32 Xtal_BMP_Compression;
enum {
    // TODO(geni): Support BI_BITFIELDS and BI_ALPHABITFIELDS
    Xtal_BMP_CompressionNone = 0,
};

// NOTE(geni): Disable padding
#pragma pack(push, 1)
typedef struct {
    u8  signature[2];  // NOTE(geni): Should always be "BM"
    u32 size;          // NOTE(geni): Size of file
    u8  reserved[4];   // NOTE(geni): Reserved
    u32 data_offset;   // NOTE(geni): Offset of the byte where actual bitmap data starts
} Xtal_BMP_FileHeader;

typedef struct {
    u32 header_size;  // NOTE(geni): Should always be equal to sizeof(Xtal_BMP_InfoHeader) = 40

    ivec2s               size;    // NOTE(geni): Size in pixels
    u16                  planes;  // NOTE(geni): Must always be 1
    u16                  depth;   // NOTE(geni): Color depth (bits per pixel), depth ∈ {1, 4, 8, 16, 24, 32}
    Xtal_BMP_Compression compression;

    u32 data_size;  // NOTE(geni): Size of actual bitmap data

    i32 h_ppm;  // NOTE(geni): Horizontal resolution in pixels per meter
    i32 v_ppm;  // NOTE(geni): Vertical resolution in pixels per meter

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

internal Xtal_BMP_LoadResult Xtal_BMP_Load(Xtal_MArena* arena, const void* file_data, u64 file_data_size) {
#define BMPAssert(b, msg)                  \
    do {                                   \
        if (!(b)) {                        \
            result.error     = S8Lit(msg); \
            result.has_error = 1;          \
            return result;                 \
        }                                  \
    } while (0)

    Xtal_BMP_LoadResult result = {0};

    // NOTE(geni): Ensure correct size
    BMPAssert(file_data_size > sizeof(Xtal_BMP_File), "File too small to be valid");

    Xtal_BMP_File* file = (Xtal_BMP_File*) file_data;

    BMPAssert(file->header.signature[0] == 'B' && file->header.signature[1] == 'M', "File signature incorrect");
    BMPAssert(file->header.size == file_data_size, "File size in header incorrect");
    BMPAssert(file->info.header_size == sizeof file->info, "Info header size incorrect");

    BMPAssert(file->info.num_colors == 0, "Paletted BMPs are unsupported");
    BMPAssert(file->info.compression == Xtal_BMP_CompressionNone, "Compression type unsupported");

    u8* data        = (u8*) file_data + file->header.data_offset;
    u8* result_data = NULL;
    switch (file->info.depth) {
        // TODO(geni): Support paletted BMPs
        case 24: {
            result_data = Xtal_MArenaPush(arena, file->info.data_size + (file->info.size.width * file->info.size.height));
            u8* write   = result_data;
            // NOTE(geni): Layout is BB GG RR, bottom-up
            // NOTE(geni): We write it as RR GG BB AA, where AA is always 0xFF
            for (i32 y = 0; y < file->info.size.height; ++y) {
                for (i32 x = 0; x < file->info.size.width; ++x) {
                    *(write++) = data[2];  // NOTE(geni): RR
                    *(write++) = data[1];  // NOTE(geni): GG
                    *(write++) = data[0];  // NOTE(geni): BB
                    *(write++) = 0xFF;     // NOTE(geni): Alpha
                    data += 3;
                }
            }
        } break;
        case 32: {
            // TODO(geni): Test this
            result_data = Xtal_MArenaPush(arena, file->info.data_size);
            u8* write   = result_data;
            // NOTE(geni): Layout is BB GG RR AA, bottom-up
            // NOTE(geni): We write it as RR GG BB AA
            for (i32 y = 0; y < file->info.size.height; ++y) {
                for (i32 x = 0; x < file->info.size.width; ++x) {
                    *(write++) = data[2];  // NOTE(geni): RR
                    *(write++) = data[1];  // NOTE(geni): GG
                    *(write++) = data[0];  // NOTE(geni): BB
                    *(write++) = data[3];  // NOTE(geni): Alpha
                    data += 4;
                }
            }
            break;
        }
        default: {
            // NOTE(geni): This can never actually break
            BMPAssert(0, "Depth unsupported");
        }
    }

    result.bmp = (Xtal_BMP){
        .size = file->info.size,
        .data = result_data,
    };

    return result;

#undef BMPAssert
}

internal Xtal_BMP_LoadResult Xtal_BMP_LoadFromPath(Xtal_MArena* arena, String8 path) {
    void*        data;
    u64          data_size;
    Xtal_OS_File file = xtal_os->OpenFile(path, Xtal_OS_FilePermission_Read, Xtal_OS_FileOpen_OpenExisting);
    xtal_os->LoadEntireFile(file, arena, &data, &data_size);
    xtal_os->CloseFile(file);
    return Xtal_BMP_Load(arena, data, data_size);
}
