#include "sutHash.h"
/* ------------------------------------------------------------ */
unsigned int sutHash_DKDR(char *src_str, int len)
{
    char * fetch_ptr = src_str;
    unsigned int basis =131 ;
    unsigned int hash_key = 0;
    unsigned int c ;

    while ((c = *fetch_ptr) && (len > 0)) {
        hash_key = hash_key * basis + c;
        fetch_ptr++;
        len--;
    }
    return (hash_key & 0x7FFFFFFF);
}
/* ------------------------------------------------------------ */
unsigned int sutHash_djb2(char *src_str, int len)
{
    char * fetch_ptr = src_str;
    unsigned int hash_key = 5381;
    unsigned int c ;

    while ((c = *fetch_ptr) && (len > 0)) {
        hash_key = ((hash_key << 5) + hash_key) + c; /* hash * 33 + c */
        fetch_ptr++;
        len--;
    }

    return hash_key;
}
