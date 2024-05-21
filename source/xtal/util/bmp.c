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
