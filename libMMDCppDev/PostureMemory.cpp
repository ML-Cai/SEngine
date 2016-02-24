#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>
#include "PostureMemory.h"
#include "sutHash.h"
//---------------------------------------------------------------------
using namespace MMDCppDev;
//---------------------------------------------------------------------
#define pNONE "\033[m"
#define pRED "\033[0;32;31m"
#define pLIGHT_RED "\033[1;31m"
#define pGREEN "\033[0;32;32m"
#define pLIGHT_GREEN "\033[1;32m"
#define pBLUE "\033[0;32;34m"
#define pLIGHT_BLUE "\033[1;34m"
// ---------------------------------------------------------------------
int read_UTF16_string(FILE *in_f, char *dstString)
{
    int ret = 0;
    unsigned short *str = (unsigned short *)dstString;
    unsigned short char_UTF16;
    str--;
    do {
        str ++;
        fread(&char_UTF16, sizeof(char) *2, 1, in_f);
        if((char_UTF16 != 0x000d) &&
           (char_UTF16 != 0x000a) && //next line
           (char_UTF16 != 0x0020)) {
            *str = char_UTF16;
            ret++;
        }
    } while((char_UTF16 != 0x000a) && //next line
            (char_UTF16 != 0x0020));  //space
    return ret;
}
// ---------------------------------------------------------------------
float UTF16read_float(FILE *in_f, char StopFlag)
{
    float ret =0;
    float mul =0.1f;
    int isNative =0;
    unsigned short char_UTF16;

    fread(&char_UTF16, sizeof(char_UTF16), 1, in_f);
    if(char_UTF16 == (unsigned short)'-') {
        isNative = 1;
        char_UTF16 = 48;
    }
    do {
        ret *=10;
        ret += (float)(char_UTF16 -48);
        fread(&char_UTF16, sizeof(char_UTF16), 1, in_f);
    } while(char_UTF16 != (unsigned short)'.');

    do {
        fread(&char_UTF16, sizeof(char_UTF16), 1, in_f);
        if(char_UTF16 >= (unsigned short)'0' && char_UTF16 <= (unsigned short)'9') {
            ret += (float)(char_UTF16 -48) * mul;
            mul *=0.1f;
        }
    } while((char_UTF16 != 0x0020) &&
            (char_UTF16 != StopFlag));

    if(isNative) ret *= -1;
    return ret;
}
/* ---------------------------------------------------------------------
 *  Read the UTF16 data to int output from specift file.
 *      @param in_f : The file handle readed from.
 *      @param StopFlag : The char to stop read, example : >1234567, 15975 < , the ',' will be the stopflag.
 */
int read_UTF16_int(FILE *in_f, char StopFlag)
{
    int ret =0;
    int isNative =0;
    unsigned char char_UTF16[2];

    fread(char_UTF16, sizeof(char_UTF16), 1, in_f);
    if(char_UTF16[0] == '-') {
        isNative = 1;
        char_UTF16[0] ='0';
    }
    do {
        ret *=10;
        ret += (char_UTF16[0]-48);
        fread(char_UTF16, sizeof(char_UTF16), 1, in_f);
    } while(char_UTF16[0] != StopFlag);

    if(isNative) ret *= -1;
    return ret;
}
/* ---------------------------------------------------------------------
 * Parse the VPD file
*/
int PostureMemoryManager::
parse_PostureMemoryUnit_ShiftJIS()
{
    int i =0;
    int BoneID;
    char char_UTF16[2];
    char name_ShiftJIS[256];
    char CommentChar[512];
    char *CommentChar_ptr = CommentChar;
    size_t len = 512;

    /** --------------------------------------------------
     * Important : android 4.0 don't support getline ....
     * */

    getline(&CommentChar_ptr, &len, ProfileFile_ptr);
   // printf("<%s>\n", CommentChar);
    getline(&CommentChar_ptr, &len, ProfileFile_ptr);
   // printf("<%s>\n", CommentChar);
    getline(&CommentChar_ptr, &len, ProfileFile_ptr);
   // printf("<%s>\n", CommentChar);

    fscanf(ProfileFile_ptr, "%d;\n", &ProfileCtx_ptr->PoseUnitCount);
   // printf("ProfileCtx_ptr->PoseUnitCount %d\n", ProfileCtx_ptr->PoseUnitCount);
    ProfileCtx_ptr->PoseUnit = (struct PostureMemoryUnit *)malloc(sizeof(struct PostureMemoryUnit) * ProfileCtx_ptr->PoseUnitCount);
    if(ProfileCtx_ptr->PoseUnit == NULL) return 0;

    getline(&CommentChar_ptr, &len, ProfileFile_ptr);
    //printf("[%s]\n", CommentChar);

    for(i = 0 ; i < ProfileCtx_ptr->PoseUnitCount ; i++) {
        struct PostureMemoryUnit *PoseUnit_ptr = ProfileCtx_ptr->PoseUnit + i;

        fscanf(ProfileFile_ptr, "%4s%d{%s\n", CommentChar, &BoneID, name_ShiftJIS);
        PoseUnit_ptr->Key_ShiftJIS_djb2 = sutHash_djb2(name_ShiftJIS , strlen(name_ShiftJIS));
        PoseUnit_ptr->Key_ShiftJIS_DKDR = sutHash_DKDR(name_ShiftJIS, strlen(name_ShiftJIS));

        //printf("key %d %d\n", PoseUnit_ptr->Key_ShiftJIS_djb2,PoseUnit_ptr->Key_ShiftJIS_DKDR);

        fscanf(ProfileFile_ptr, "%f,%f,%f;\n", &PoseUnit_ptr->Translate[0],
                                               &PoseUnit_ptr->Translate[1],
                                               &PoseUnit_ptr->Translate[2]);
        //printf("%f %f %f\n", PoseUnit_ptr->Translate[0], PoseUnit_ptr->Translate[1], PoseUnit_ptr->Translate[2]);
        getline(&CommentChar_ptr, &len, ProfileFile_ptr);

        fscanf(ProfileFile_ptr, "%f,%f,%f,%f;\n", &PoseUnit_ptr->Quaternion[0],
                                                  &PoseUnit_ptr->Quaternion[1],
                                                  &PoseUnit_ptr->Quaternion[2],
                                                  &PoseUnit_ptr->Quaternion[3]);

        //printf("%f %f %f %f\n", PoseUnit_ptr->Quaternion[0], PoseUnit_ptr->Quaternion[1], PoseUnit_ptr->Quaternion[2], PoseUnit_ptr->Quaternion[3]);
        getline(&CommentChar_ptr, &len, ProfileFile_ptr);
        getline(&CommentChar_ptr, &len, ProfileFile_ptr);
    }

    return 1;
}
/* ---------------------------------------------------------------------
 * Parse the VPD file
*/
int PostureMemoryManager::
parse_PostureMemoryUnit()
{
    int i =0;
    int BoneID;
    char char_UTF16[2];
    char name_ShiftJIS[256];
    char CommentChar[512];
    char *CommentChar_ptr = CommentChar;
    size_t len = 512;

    // bypass file comment
    do {
        fread(char_UTF16, sizeof(char_UTF16), 1, ProfileFile_ptr);
    } while(char_UTF16[0] !=';');
    do {
        fread(char_UTF16, sizeof(char_UTF16), 1, ProfileFile_ptr);
    } while(char_UTF16[0] !='\n');

    //read Bone Count
    ProfileCtx_ptr->PoseUnitCount = read_UTF16_int(ProfileFile_ptr, ';');
    printf("ProfileCtx_ptr->PoseUnitCount %d\n", ProfileCtx_ptr->PoseUnitCount);

    //bypass comment
    do {
        fread(char_UTF16, sizeof(char_UTF16), 1, ProfileFile_ptr);
    } while(char_UTF16[0] !='\n');

    ProfileCtx_ptr->PoseUnit = (struct PostureMemoryUnit *)malloc(sizeof(struct PostureMemoryUnit) * ProfileCtx_ptr->PoseUnitCount);
    if(ProfileCtx_ptr->PoseUnit == NULL) return 0;

    int NameLen;
    for(i = 0 ; i < ProfileCtx_ptr->PoseUnitCount ; i++) {
        struct PostureMemoryUnit *PoseUnit_ptr = ProfileCtx_ptr->PoseUnit + i;

        // bypass the text before >>> Bone?{ <<<<
        do {
            fread(char_UTF16, sizeof(char_UTF16), 1, ProfileFile_ptr);
        } while(char_UTF16[0] !='{');

        // read bone name
        NameLen =read_UTF16_string(ProfileFile_ptr, name_ShiftJIS);
        PoseUnit_ptr->Key_ShiftJIS_djb2 = sutHash_djb2(name_ShiftJIS, NameLen *2);
        PoseUnit_ptr->Key_ShiftJIS_DKDR = sutHash_DKDR(name_ShiftJIS, NameLen *2);

        //read 2 space
        fread(char_UTF16, sizeof(char_UTF16), 1, ProfileFile_ptr);
        fread(char_UTF16, sizeof(char_UTF16), 1, ProfileFile_ptr);

        //read float
        PoseUnit_ptr->Translate[0] = UTF16read_float(ProfileFile_ptr, ',');
        PoseUnit_ptr->Translate[1] = UTF16read_float(ProfileFile_ptr, ',');
        PoseUnit_ptr->Translate[2] = UTF16read_float(ProfileFile_ptr, ',');

        //bypass comment
        do {
            fread(char_UTF16, sizeof(char_UTF16), 1, ProfileFile_ptr);
        } while(char_UTF16[0] !='\n');

        //read space
        fread(char_UTF16, sizeof(char_UTF16), 1, ProfileFile_ptr);
        fread(char_UTF16, sizeof(char_UTF16), 1, ProfileFile_ptr);

        //read Quaternion
        PoseUnit_ptr->Quaternion[0] = UTF16read_float(ProfileFile_ptr, ',');
        PoseUnit_ptr->Quaternion[1] = UTF16read_float(ProfileFile_ptr, ',');
        PoseUnit_ptr->Quaternion[2] = UTF16read_float(ProfileFile_ptr, ',');
        PoseUnit_ptr->Quaternion[3] = UTF16read_float(ProfileFile_ptr, ';');

//#define POSTURE_DBG
#ifdef POSTURE_DBG
        printf("-------------------------------------\n");
        printf("[%d/%d]\n", i, ProfileCtx_ptr->PoseUnitCount);
        printf("Str len %d ,djb2 %d, DKDR %d\n" ,NameLen *2, PoseUnit_ptr->Key_ShiftJIS_djb2, PoseUnit_ptr->Key_ShiftJIS_DKDR);
        printf("%f %f %f\n", PoseUnit_ptr->Translate[0], PoseUnit_ptr->Translate[1], PoseUnit_ptr->Translate[2]);
        printf("%f %f %f %f\n", PoseUnit_ptr->Quaternion[0], PoseUnit_ptr->Quaternion[1],
                                PoseUnit_ptr->Quaternion[2], PoseUnit_ptr->Quaternion[3]);
        getchar();
#endif
    }
    return 1;

#ifdef ASD
    int i =0;
    int BoneID;
    char name_ShiftJIS[256];
    char CommentChar[512];
    char *CommentChar_ptr = CommentChar;
    size_t len = 512;

    getline(&CommentChar_ptr, &len, ProfileFile_ptr);
    //printf("%s\n", CommentChar);
    getline(&CommentChar_ptr, &len, ProfileFile_ptr);
    //printf("%s\n", CommentChar);
    getline(&CommentChar_ptr, &len, ProfileFile_ptr);
    //printf("%s\n", CommentChar);

    fscanf(ProfileFile_ptr, "%d;\n", &ProfileCtx_ptr->PoseUnitCount);
    //printf("ProfileCtx_ptr->PoseUnitCount %d\n", ProfileCtx_ptr->PoseUnitCount);
    ProfileCtx_ptr->PoseUnit = (struct PostureMemoryUnit *)malloc(sizeof(struct PostureMemoryUnit) * ProfileCtx_ptr->PoseUnitCount);
    if(ProfileCtx_ptr->PoseUnit == NULL) return 0;

    getline(&CommentChar_ptr, &len, ProfileFile_ptr);
    //printf("[%s]\n", CommentChar);

    for(i = 0 ; i < ProfileCtx_ptr->PoseUnitCount ; i++) {
        struct PostureMemoryUnit *PoseUnit_ptr = ProfileCtx_ptr->PoseUnit + i;

        fscanf(ProfileFile_ptr, "%4s%d{%s\n", CommentChar, &BoneID, name_ShiftJIS);
        PoseUnit_ptr->Key_ShiftJIS_djb2 = sutHash_djb2(name_ShiftJIS , strlen(name_ShiftJIS));
        PoseUnit_ptr->Key_ShiftJIS_DKDR = sutHash_DKDR(name_ShiftJIS, strlen(name_ShiftJIS));

        printf("key %d %d\n", PoseUnit_ptr->Key_ShiftJIS_djb2,PoseUnit_ptr->Key_ShiftJIS_DKDR);

        fscanf(ProfileFile_ptr, "%f,%f,%f;\n", &PoseUnit_ptr->Translate[0],
                                               &PoseUnit_ptr->Translate[1],
                                               &PoseUnit_ptr->Translate[2]);
        printf("%f %f %f\n", PoseUnit_ptr->Translate[0], PoseUnit_ptr->Translate[1], PoseUnit_ptr->Translate[2]);
        getline(&CommentChar_ptr, &len, ProfileFile_ptr);

        fscanf(ProfileFile_ptr, "%f,%f,%f,%f;\n", &PoseUnit_ptr->Quaternion[0],
                                                  &PoseUnit_ptr->Quaternion[1],
                                                  &PoseUnit_ptr->Quaternion[2],
                                                  &PoseUnit_ptr->Quaternion[3]);

        printf("%f %f %f %f\n", PoseUnit_ptr->Quaternion[0], PoseUnit_ptr->Quaternion[1], PoseUnit_ptr->Quaternion[2], PoseUnit_ptr->Quaternion[3]);
        getline(&CommentChar_ptr, &len, ProfileFile_ptr);
        getline(&CommentChar_ptr, &len, ProfileFile_ptr);
    }
    return 1;
#endif
}
/* ---------------------------------------------------------------------
 * Generate the Context of VPD file
 *  @return : On success, this call return the empty context of VPD file.
 *            On error, NULL is returnned.
*/
struct PostureMemoryContext *PostureMemoryManager::generate_Context()
{
    struct PostureMemoryContext *tmp = (struct PostureMemoryContext*)malloc(sizeof(struct PostureMemoryContext));
    if(tmp != NULL) {
        tmp->PoseUnitCount = 0;
        tmp->PoseUnit = NULL;
    }
    return tmp;
}
/* ---------------------------------------------------------------------
 * Load the VPD file and parse data into input context.
 *  @param inCtx    : The target VPD context to carry data, this param is gerentated by generate_Context().
 *  @param Path     : The VPD file path.
 *
 *  @return : On success, this call return the empty context of VPD file.
 *            On error, NULL is returnned.
*/
int PostureMemoryManager::load(struct PostureMemoryContext *inCtx, const char *Path)
{
    int ret;
    unsigned short TypeUTF;

    printf("Load %s\n", Path);
    ProfileFile_ptr = fopen(Path, "rb");
    if(ProfileFile_ptr) {
        fread(&TypeUTF, sizeof(unsigned short), 1, ProfileFile_ptr);
        switch(TypeUTF) {
        case 0xFFFE:
            ret =0;
            printf("The UTF16BE is under constructed\n");
            break;
        case 0xFEFF:
            ProfileCtx_ptr = inCtx;
            ret = parse_PostureMemoryUnit();
            printf("UTF16LE\n");
            break;
        default:
            printf("The Posture Memory will try to profile file as ShiftJIS\n");
            fclose(ProfileFile_ptr);
            ProfileFile_ptr = fopen(Path, "rb");
            ProfileCtx_ptr = inCtx;
            parse_PostureMemoryUnit_ShiftJIS();
            //ret =0;
            //printf("The Posture Memory only support the UTF16LE encode file\n");
            break;
        }
        fclose(ProfileFile_ptr);
    }
    return ret;
}

/* ---------------------------------------------------------------------
 * Release the VPD context.
 *  @param inCtx : Pointer to a memory block previously allocated with generate_Context.
*/
void PostureMemoryManager::release(struct PostureMemoryContext *relCtx)
{
    if(relCtx == NULL) return ;
    if(relCtx->PoseUnit != NULL) {
        free(relCtx->PoseUnit);
        relCtx->PoseUnit = NULL;
    }
    relCtx->PoseUnitCount = 0;
}
//---------------------------------------------------------------------

