//~ String matching functions

#define CalculateCStringLength(s)  ((u32) strlen(s))
#define CalculateWCStringLength(s) ((u32) wcslen(s))

//~ Copy functions

internal b32  CStringMatchCaseInsensitiveN(const char* a, const char* b, u64 n);
internal void CopyCStringToFixedSizeBuffer(char* destination, u64 destination_max, char* source);
internal void CopyCStringToFixedSizeBufferN(char* destination, u64 destination_max, char* source, u64 source_max);

//~ Conversion functions

internal inline void ConvertCStringToLowercase(char* str);
internal inline void ConvertCStringToUppercase(char* str);
internal inline void ConvertCStringToLowercaseWithUnderscores(char* str);

//~ Append functions

internal void AppendCStringToFixedSizeCString(char* destination, u64 destination_max, char* str);

//~ Conversion functions

#define CStringToI32(s) ((i32) atoi(s))
#define CStringToU32(s) ((u32) atoi(s))
#define CStringToF32(s) ((f32) atof(s))
#define CStringToU64(s) ((u64) strtoull(s, NULL, 10))
#define CStringToI64(s) ((u64) strtoll(s, NULL, 10))

//~ Find occurrence functions

internal f32 GetFirstF32FromCString(const char* str);
