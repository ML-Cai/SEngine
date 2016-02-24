#include <stdlib.h>
#include <string.h>
#include "VMD_Profile.h"
#include "sutHash.h"
#include "sutDataStruct.h"
#include <sutCharacterSetConvert.h>
// --------------------------------------------------------------------
struct BoneMotionUnit_ProfileType {
    unsigned int    nameKey_UTF8_DKDR;   //The key of bone name
    unsigned int    nameKey_UTF8_djb2;
    sutLinkingList KeyFrameList;
};
// --------------------------------------------------------------------
using namespace MMDCppDev;
VMD_Profile::VMD_Profile()
{

}
// --------------------------------------------------------------------
void QuickSort(int *inData, int size)
{
    int i;
    int counter = size -1;
    if(size <=1) return ;
    int *basis = inData;
    int *fetch_ptr = inData +1;

    while(counter > 0) {
        if(*fetch_ptr < *basis) {
            int tmp = *basis;
            *basis =*fetch_ptr;
            *fetch_ptr = tmp;
        }
        fetch_ptr++;
        counter--;
    } //indata basis indata+size
    QuickSort(basis -1, basis -inData);
    QuickSort(basis +1, size- (int)(basis -inData) -1);
}

// --------------------------------------------------------------------
int VMD_Profile::
parse_VocaloidMotionKeyFrameUnit_ShiftJIS()
{
    int i , kfi;
    int KeyFrameCount;
    char MagicChar[31];
    sutLinkingList BoneMotionList_djb2;
    sutLinkingList BoneMotionList_DKDR;
    struct KeyFrameUnit *KeyFrame;

    ProfiledCtx_ptr->effectiveBoneCount = 0;
    BoneMotionList_djb2.init(0);
    BoneMotionList_DKDR.init(0);

    // bypass file comment
    fread(MagicChar, sizeof(char) *30, 1, fMotion);
    printf("%s\n", MagicChar);
    fread(MagicChar, sizeof(char) *20, 1, fMotion);
    printf("%s\n", MagicChar);

    //read motion Count
    fread(&KeyFrameCount , sizeof(int), 1, fMotion);
    printf("ProfiledCtx_ptr->KeyFrameCount %d\n", KeyFrameCount);

    KeyFrame = (struct KeyFrameUnit *)malloc(sizeof(struct KeyFrameUnit) *KeyFrameCount);
    if(KeyFrame == NULL) return 0;
    memset(KeyFrame, 0, sizeof(struct KeyFrameUnit) *KeyFrameCount);

    const int vmdStringLen = 15;
    char bName[vmdStringLen];
    const size_t defaultNameLen = 512;
    char *nameUTF8 = (char *)malloc(defaultNameLen);
    for(i = 0 ; i < KeyFrameCount ; i++) {
        unsigned int Key_ShiftJIS_djb2;
        unsigned int Key_ShiftJIS_DKDR;
        struct KeyFrameUnit *PoseUnit_ptr = KeyFrame + i;
        struct BoneMotionUnit_ProfileType *MotionUnit_djb2_ptr;
        struct BoneMotionUnit_ProfileType *MotionUnit_DKDR_ptr;
        struct BoneMotionUnit_ProfileType *MotionUnit_ptr;

        fread(bName, vmdStringLen, 1, fMotion);
        memset(nameUTF8, 0, defaultNameLen);
        sutShiftJIS_to_UTF8(bName, vmdStringLen, nameUTF8, defaultNameLen);

        Key_ShiftJIS_djb2 = sutHash_djb2(nameUTF8, strlen(nameUTF8));
        Key_ShiftJIS_DKDR = sutHash_DKDR(nameUTF8, strlen(nameUTF8));
        //printf("<[%d> [DKDR %d, djb2 %d] %s\n",i, Key_ShiftJIS_djb2, Key_ShiftJIS_DKDR, nameUTF8);

        fread(&PoseUnit_ptr->FrameID, sizeof(int), 1, fMotion);
        fread(PoseUnit_ptr->Translate, sizeof(float) *3, 1, fMotion);
        fread(PoseUnit_ptr->Quaternion, sizeof(float) *4, 1, fMotion);
        fread(PoseUnit_ptr->Interpolation, 64, 1, fMotion);
        if(PoseUnit_ptr->FrameID > ProfiledCtx_ptr->MotionStamp) ProfiledCtx_ptr->MotionStamp = PoseUnit_ptr->FrameID;

        MotionUnit_djb2_ptr = (struct BoneMotionUnit_ProfileType *)BoneMotionList_djb2.search_by_key(Key_ShiftJIS_djb2);
        MotionUnit_DKDR_ptr = (struct BoneMotionUnit_ProfileType *)BoneMotionList_DKDR.search_by_key(Key_ShiftJIS_DKDR);

        if(MotionUnit_djb2_ptr == NULL || MotionUnit_DKDR_ptr ==NULL) {
            ProfiledCtx_ptr->effectiveBoneCount++;
            MotionUnit_ptr = (struct BoneMotionUnit_ProfileType *)malloc(sizeof(BoneMotionUnit_ProfileType));
            MotionUnit_ptr->nameKey_UTF8_djb2 = Key_ShiftJIS_djb2;
            MotionUnit_ptr->nameKey_UTF8_DKDR = Key_ShiftJIS_DKDR;
            MotionUnit_ptr->KeyFrameList.init(0);
            BoneMotionList_djb2.add(MotionUnit_ptr, Key_ShiftJIS_djb2);
            BoneMotionList_DKDR.add(MotionUnit_ptr, Key_ShiftJIS_DKDR);
        }
        else {
            // Choose theunit to fit 2 keys. the key maybe collission if without this check.
            if(MotionUnit_djb2_ptr->nameKey_UTF8_djb2 == Key_ShiftJIS_djb2 &&
               MotionUnit_djb2_ptr->nameKey_UTF8_DKDR == Key_ShiftJIS_DKDR)
                MotionUnit_ptr = MotionUnit_djb2_ptr;
            else if(MotionUnit_DKDR_ptr->nameKey_UTF8_djb2 == Key_ShiftJIS_djb2 &&
                    MotionUnit_DKDR_ptr->nameKey_UTF8_DKDR == Key_ShiftJIS_DKDR)
                MotionUnit_ptr = MotionUnit_DKDR_ptr;
        }
        MotionUnit_ptr->KeyFrameList.add(PoseUnit_ptr, PoseUnit_ptr->FrameID);
    }
    printf("The TimeStamp of this motion %d\n", ProfiledCtx_ptr->MotionStamp);
    printf("The count of effected Bone in motion file : %d\n", ProfiledCtx_ptr->effectiveBoneCount);

    //conver the linklist key frame to array.
    ProfiledCtx_ptr->effectiveBoneSet = (struct BoneMotionUnit *)malloc(sizeof(struct BoneMotionUnit) * ProfiledCtx_ptr->effectiveBoneCount);
    printf("Conver linklist to frame array ...\n");
    printf("ProfiledCtx_ptr->effectiveBoneCount %d \n", ProfiledCtx_ptr->effectiveBoneCount);

    for(i = 0 ; i < ProfiledCtx_ptr->effectiveBoneCount ; i++) {
        struct BoneMotionUnit *dstMotionUnit_ptr = ProfiledCtx_ptr->effectiveBoneSet +i;
        struct BoneMotionUnit_ProfileType *srcMotionUnit_ptr = (struct BoneMotionUnit_ProfileType *)BoneMotionList_djb2.search_by_sequence(i);

        dstMotionUnit_ptr->nameKey_UTF8_djb2 = srcMotionUnit_ptr->nameKey_UTF8_djb2;
        dstMotionUnit_ptr->nameKey_UTF8_DKDR = srcMotionUnit_ptr->nameKey_UTF8_DKDR;
        dstMotionUnit_ptr->KeyFrameCount = srcMotionUnit_ptr->KeyFrameList.count() ;
        dstMotionUnit_ptr->KeyFrame = (struct KeyFrameUnit *)malloc(sizeof(struct KeyFrameUnit) *dstMotionUnit_ptr->KeyFrameCount);

        //Store the key frame by frame ID
        int *SortID = (int *)malloc(sizeof(int) * dstMotionUnit_ptr->KeyFrameCount);
        for(kfi = 0 ; kfi < dstMotionUnit_ptr->KeyFrameCount ; kfi++) {
            struct KeyFrameUnit *Frame_ptr = (struct KeyFrameUnit *)srcMotionUnit_ptr->KeyFrameList.search_by_sequence(kfi) ;
            *(SortID +kfi) =Frame_ptr->FrameID;
        }
        QuickSort(SortID, dstMotionUnit_ptr->KeyFrameCount);

        for(kfi = 0 ; kfi < dstMotionUnit_ptr->KeyFrameCount ; kfi++) {
            struct KeyFrameUnit *Frame_ptr = (struct KeyFrameUnit *)srcMotionUnit_ptr->KeyFrameList.fetch_by_key(*(SortID +kfi));
            memcpy(dstMotionUnit_ptr->KeyFrame +kfi, Frame_ptr, sizeof(struct KeyFrameUnit));
        }
        free(SortID);
    }

//#define DBG
#ifdef DBG
    for(i = 0 ; i < ProfiledCtx_ptr->effectiveBoneCount ; i++) {
       printf("------------------------------------------------------\n");
       printf("[%d] , Frame ID %d, FrameCount %d\n", i, WorldFrame->FrameID, WorldFrame->KeyFrameCount);
       for(kfi = 0 ; kfi < WorldFrame->KeyFrameCount ; kfi++) {
           struct VocaloidMotionKeyFrameUnit *PoseUnit_ptr = (struct VocaloidMotionKeyFrameUnit *)WorldFrame->KeyFrameList.search_by_sequence(kfi) ;
           printf("Key        : %10d %10d\n", PoseUnit_ptr->Key_ShiftJIS_djb2, PoseUnit_ptr->Key_ShiftJIS_DKDR);
           printf("\tTranslate  : %f %f %f\n", PoseUnit_ptr->Translate[0], PoseUnit_ptr->Translate[1], PoseUnit_ptr->Translate[2]);
           printf("\tQuaternion : %f %f %f %f\n", PoseUnit_ptr->Quaternion[0], PoseUnit_ptr->Quaternion[1], PoseUnit_ptr->Quaternion[2], PoseUnit_ptr->Quaternion[3]);
       }
       getchar();
    }
#endif
    BoneMotionList_djb2.release();
    BoneMotionList_DKDR.release();
    return 1;
}

// --------------------------------------------------------------------
struct VocaloidMotionDataContext *VMD_Profile::
genContext(int ID)
{
    struct VocaloidMotionDataContext *tmp = (struct VocaloidMotionDataContext *)malloc(sizeof(struct VocaloidMotionDataContext));
    if(tmp != NULL) {
        tmp->ObjID = ID;
        tmp->effectiveBoneSet = NULL;
        tmp->effectiveBoneCount = 0;
        tmp->MotionStamp = 0;
    }
    return tmp;
}
// --------------------------------------------------------------------
int VMD_Profile::
load(struct VocaloidMotionDataContext *dstCtx , const char *fPath, const char *fName)
{
    int ret =0;
    char *FullPath = (char *)malloc(strlen(fPath) + strlen(fName));
    unsigned short TypeUTF;

    if(FullPath ==NULL) return ret;
    strcpy(FullPath, fPath);
    strcat(FullPath, fName);

    printf("Load Vocaloid Motion Data from %s\n", FullPath);
    fMotion = fopen(FullPath, "rb");
    if(fMotion) {
        fread(&TypeUTF, sizeof(unsigned short), 1, fMotion);
        switch(TypeUTF) {
        case 0xFFFE:
            ret =0;
            printf("The UTF16BE is under constructed\n");
            break;
        case 0xFEFF:
            ret =0;
            printf("The UTF16LE is under constructed\n");
            break;
        default:
            printf("The Posture Memory will try to profile file as ShiftJIS\n");
            fclose(fMotion);
            fMotion = fopen(FullPath, "rb");
            ProfiledCtx_ptr = dstCtx;
            ret = parse_VocaloidMotionKeyFrameUnit_ShiftJIS();
            break;
        }
        fclose(fMotion);
    }
    return ret;
}

// --------------------------------------------------------------------
