#ifndef sutCharacterSetConvert_h
#define sutCharacterSetConvert_h
//--------------------------------------------------------------------------
#include <iconv.h>
//--------------------------------------------------------------------------
#ifdef  __cplusplus
extern "C" {
#endif

size_t sutShiftJIS_to_UTF8(char *inSJIS, size_t inSJISLen, char *outUTF8, size_t outUTF8Len);
size_t sutUTF8_to_ShiftJIS(char *inUTF8, size_t inUTF8Len, char *outSJIS, size_t outSJISLen);
size_t sutUTF16_to_UTF8(char *inUTF16, size_t inUTF16Len, char *outUTF8, size_t outUTF8Len);

#ifdef  __cplusplus
}
#endif
//--------------------------------------------------------------------------
#endif
