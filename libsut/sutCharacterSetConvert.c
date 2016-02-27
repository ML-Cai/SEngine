#include <string.h>
#include <stdio.h>
#include <iconv.h>
#include <errno.h>
#include <stdlib.h>
/* --------------------------------------------------------------------- 
 * Conver the character set into UTF-8 from Shift-JIS
 *
 * @note this function is implemented from libiconv.
 * @param inUTF8 argument is the address of a variable that points to the first character of the input sequence.
 * @param inUTF8Len indicates the number of bytes in ${inUTF8} buffer.
 * @param outSJIS  argument is the address of a variable that points to the first character of the output sequence.
 * @param outSJISLen  indicates the number of bytes in ${outSJIS} buffer.
 * 
 * @return 0 when all character in ${inUTF8} converted successfull, or returns the number of characters converted in a \
 *	nonreversible way during this call (> 0), otherwise, it sets the errno and return -1.
**/
size_t sutUTF8_to_ShiftJIS(char *inUTF8, size_t inUTF8Len, char *outSJIS, size_t outSJISLen) 
{
	iconv_t icCtx = iconv_open("SJIS", "UTF-8"); // iconv
	size_t len = iconv(icCtx, &inUTF8, &inUTF8Len, &outSJIS, &outSJISLen);
    //if(len == (size_t)-1) printf("Fatal : Character set convert failed : %s\n", strerror(errno));
	iconv_close(icCtx);
	return len;
}
/* --------------------------------------------------------------------- 
 * Conver the character set into Shift-JIS from UTF-8
 *
 * @note this function is implemented from libiconv.
 * @param inSJIS  argument is the address of a variable that points to the first character of the input sequence.
 * @param inSJISLen  indicates the number of bytes in ${inSJIS} buffer.
 * @param outUTF8 argument is the address of a variable that points to the first character of the output sequence.
 * @param outUTF8Len indicates the number of bytes in ${outUTF8} buffer.
 * 
 * @return 0 when all character in ${inSJIS} converted successfull, or returns the number of characters converted in a \
 *	nonreversible way during this call (> 0), otherwise, it sets the errno and return -1.
**/
size_t sutShiftJIS_to_UTF8(char *inSJIS, size_t inSJISLen, char *outUTF8, size_t outUTF8Len) 
{
    iconv_t icCtx = iconv_open("UTF-8", "SJIS"); // iconv
    size_t len = iconv(icCtx, &inSJIS, &inSJISLen, &outUTF8, &outUTF8Len);
    //if(len == (size_t)-1) printf("Fatal : Character set convert failed : %s\n", strerror(errno));
	iconv_close(icCtx);
	return len;
}

/* ---------------------------------------------------------------------
 * Conver the character set into Shift-JIS from UTF-8
 *
 * @note this function is implemented from libiconv.
 * @param inSJIS  argument is the address of a variable that points to the first character of the input sequence.
 * @param inSJISLen  indicates the number of bytes in ${inSJIS} buffer.
 * @param outUTF8 argument is the address of a variable that points to the first character of the output sequence.
 * @param outUTF8Len indicates the number of bytes in ${outUTF8} buffer.
 *
 * @return 0 when all character in ${inSJIS} converted successfull, or returns the number of characters converted in a \
 *	nonreversible way during this call (> 0), otherwise, it sets the errno and return -1.
**/
size_t sutUTF16_to_UTF8(char *inUTF16, size_t inUTF16Len, char *outUTF8, size_t outUTF8Len)
{
    iconv_t icCtx = iconv_open("UTF-8", "UTF-16"); // iconv
    size_t len = iconv(icCtx, &inUTF16, &inUTF16Len, &outUTF8, &outUTF8Len);
    //if(len == (size_t)-1) printf("Fatal : Character set convert failed : %s\n", strerror(errno));
    iconv_close(icCtx);
    return len;
}
