enum {
    OAMBit_Palette = 4,
    OAMBit_XFlip,
    OAMBit_YFlip,
    OAMBIT_BGWindowOverOBJ,
};

#pragma pack(push, 1)
typedef struct {
    u8 y;
    u8 x;
    u8 index;
    u8 flags;
} SpriteEntry;
#pragma pack(pop)
