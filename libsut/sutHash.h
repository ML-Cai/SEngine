#ifndef SUTHASH_H
#define SUTHASH_H
//--------------------------------------------------------------------------
#ifdef  __cplusplus
extern "C" {
#endif

unsigned int sutHash_DKDR(char *src_str, int len);
unsigned int sutHash_djb2(char *src_str, int len);

#ifdef  __cplusplus
}
#endif
//--------------------------------------------------------------------------
#endif // SUTHASH

