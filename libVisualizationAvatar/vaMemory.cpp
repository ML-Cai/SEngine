#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <sys/time.h>
#include <sutHash.h>
#include <sutQuaternion.h>
#include <sutCharacterSetConvert.h>
#include <opencv2/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include "vaMemory.h"
// ---------------------------------------------------------------------------------
using namespace MMDCppDev;
using namespace VisualizationSystem;
// ---------------------------------------------------------------------------------
#define pNONE "\033[m"
#define pRED "\033[0;32;31m"
#define pLIGHT_RED "\033[1;31m"
#define pGREEN "\033[0;32;32m"
#define pLIGHT_GREEN "\033[1;32m"
#define pBLUE "\033[0;32;34m"
#define pLIGHT_BLUE "\033[1;34m"
// ---------------------------------------------------------------------------------
enum InputFileMode {
    InputFileMode_PMD =0x01,
    InputFileMode_PMX =0x02
};
/** ---------------------------------------------------------------------------------
 * Generate the part of Bone(Skeleton) Memory.
 */
int vaMemory::
generate_BoneMemory_Soruce_PMD(struct PolygonModelDataContext *srcPMD)
{
    int i, IKi;
    struct IKChainContext *IKChain_ptr;
    struct BoneUnitContext *BoneUnit_ptr;
    struct PMD_BoneInformation *pFetch = srcPMD->BoneInfo;
    struct PMD_InverseKinematicsInformation *IKpFetch = srcPMD->InverseKinematicsInfo;

    printf("srcPMD->InverseKinematicsInfoCount %d\n", srcPMD->InverseKinematicsInfoCount);
    vMemoryCtx->BoneMemory.NullParentID = -1;
    vMemoryCtx->BoneMemory.IKChainCount = srcPMD->InverseKinematicsInfoCount;
    vMemoryCtx->BoneMemory.IKChain = (struct IKChainContext *)malloc(sizeof(IKChainContext) * srcPMD->InverseKinematicsInfoCount);
    vMemoryCtx->BoneMemory.BoneUnitCount = srcPMD->BoneInfoCount;
    vMemoryCtx->BoneMemory.BoneUnit = (struct BoneUnitContext *)malloc(sizeof(struct BoneUnitContext) *srcPMD->BoneInfoCount);
    if(vMemoryCtx->BoneMemory.BoneUnit == NULL)
        return 0;

    BoneUnit_ptr = vMemoryCtx->BoneMemory.BoneUnit;
    for(i = 0 ; i < srcPMD->BoneInfoCount ; i++) {
        BoneUnit_ptr->nameKey_DKDR = pFetch->NameKey_DKDR;
        BoneUnit_ptr->nameKey_djb2 = pFetch->NameKey_djb2;
        BoneUnit_ptr->BoneVertex[0] = pFetch->BoneAxis[0];
        BoneUnit_ptr->BoneVertex[1] = pFetch->BoneAxis[1];
        BoneUnit_ptr->BoneVertex[2] = pFetch->BoneAxis[2];
        BoneUnit_ptr->bID_Parent = pFetch->bID_Parent;
        BoneUnit_ptr->DeformableLevel = 0;
        BoneUnit_ptr->BoneType = pFetch->Type;
        BoneUnit_ptr++;
        pFetch++;
    }

    IKChain_ptr = vMemoryCtx->BoneMemory.IKChain;
    for(i = 0 ; i < srcPMD->InverseKinematicsInfoCount ; i++) {
        IKChain_ptr->MaxAngle        = IKpFetch->MaxAngle;
        IKChain_ptr->MaxIterations   = IKpFetch->MaxIterations;
        IKChain_ptr->ReachBoneID     = IKpFetch->bID_Reach;
        IKChain_ptr->TargetBoneID    = IKpFetch->bID_End;
        IKChain_ptr->IKChainCount    = IKpFetch->ChainCount ;
        IKChain_ptr->IKChain         = (struct IKChainUnit *)malloc(sizeof(struct IKChainUnit) * IKChain_ptr->IKChainCount);

        for(IKi = 0 ; IKi < IKChain_ptr->IKChainCount ; IKi++) {
            struct IKChainUnit *IKChainUnit_ptr = IKChain_ptr->IKChain + IKi;
            IKChainUnit_ptr->isLimit        = 0;
            IKChainUnit_ptr->LinkBone       = *(IKpFetch->Chain + IKi);
            IKChainUnit_ptr->LowerBound[0]  = 0;
            IKChainUnit_ptr->LowerBound[1]  = 0;
            IKChainUnit_ptr->LowerBound[2]  = 0;
            IKChainUnit_ptr->UpBound[0]     = 0;
            IKChainUnit_ptr->UpBound[1]     = 0;
            IKChainUnit_ptr->UpBound[2]     = 0;
        }
        IKpFetch++;
        IKChain_ptr++;
    }
    return 1;
}
// ---------------------------------------------------------------------------------
int vaMemory::
generate_BoneMemory_Soruce_PMX(struct PolygonModeleXtendedContext *srcPMX)
{
    int i, IKi;
    struct IKChainContext *IKChain_ptr;
    struct BoneUnitContext *BoneUnit_ptr;
    struct PMX_BoneInformation *pFetch = srcPMX->BoneInfo  ;

    printf("srcPMX->InverseKinematicsInfoCount %d\n", srcPMX->InverseKinematicsInfoCount);
    vMemoryCtx->BoneMemory.IKChainCount = srcPMX->InverseKinematicsInfoCount;
    vMemoryCtx->BoneMemory.IKChain = (struct IKChainContext *)malloc(sizeof(IKChainContext) * srcPMX->InverseKinematicsInfoCount);
    vMemoryCtx->BoneMemory.BoneUnitCount = srcPMX->BoneInfoCount;
    vMemoryCtx->BoneMemory.BoneUnit = (struct BoneUnitContext *)malloc(sizeof(struct BoneUnitContext) *srcPMX->BoneInfoCount);
    if(vMemoryCtx->BoneMemory.BoneUnit == NULL)
        return 0;

    if(srcPMX->GlobalParamSet.BoneIndexSize == 1) vMemoryCtx->BoneMemory.NullParentID = 0xFF;
    if(srcPMX->GlobalParamSet.BoneIndexSize == 2) vMemoryCtx->BoneMemory.NullParentID = 0xFFFF;
    if(srcPMX->GlobalParamSet.BoneIndexSize == 4) vMemoryCtx->BoneMemory.NullParentID = -1;

    IKChain_ptr = vMemoryCtx->BoneMemory.IKChain;
    BoneUnit_ptr = vMemoryCtx->BoneMemory.BoneUnit;

    const size_t defaultNameLen = 512;
    char *nameUTF8 = (char *)malloc(defaultNameLen);
    for(i = 0 ; i < srcPMX->BoneInfoCount ; i++) {
        memset(nameUTF8, 0, defaultNameLen);

        BoneUnit_ptr->nameKey_DKDR = sutHash_DKDR(pFetch->name_JP.text_UTF8, strlen(pFetch->name_JP.text_UTF8));
        BoneUnit_ptr->nameKey_djb2 = sutHash_djb2(pFetch->name_JP.text_UTF8, strlen(pFetch->name_JP.text_UTF8));
        /*if(srcPMX->GlobalParamSet.TextEncoding ==0)
            nameLen = sutUTF16_to_UTF8(pFetch->name_JP.text_UTF8, pFetch->name_JP.Length, nameUTF8, defaultNameLen);
        else
            memcpy(nameUTF8, pFetch->name_JP.text_UTF8, pFetch->name_JP.Length);
        BoneUnit_ptr->nameKey_DKDR = sutHash_DKDR(nameUTF8, strlen(nameUTF8));
        BoneUnit_ptr->nameKey_djb2 = sutHash_djb2(nameUTF8, strlen(nameUTF8));*/

        //  printf("[%d] [DKDR %d, djb2 %d] %s\n",i, BoneUnit_ptr->nameKey_DKDR, BoneUnit_ptr->nameKey_djb2, nameUTF8);

        BoneUnit_ptr->bID = pFetch->bID;
        BoneUnit_ptr->BoneVertex[0] = pFetch->BoneVertex[0];
        BoneUnit_ptr->BoneVertex[1] = pFetch->BoneVertex[1];
        BoneUnit_ptr->BoneVertex[2] = pFetch->BoneVertex[2];
        BoneUnit_ptr->bID_Parent = pFetch->bID_Parent;
        BoneUnit_ptr->DeformableLevel = pFetch->DeformableLevel;
        BoneUnit_ptr->BoneType = pFetch->BoneType;
        BoneUnit_ptr->BoneType_0x0001_BoneOffset[0]        = pFetch->TypeParam_0x0001_BoneOffset[0];
        BoneUnit_ptr->BoneType_0x0001_BoneOffset[1]        = pFetch->TypeParam_0x0001_BoneOffset[1];
        BoneUnit_ptr->BoneType_0x0001_BoneOffset[2]        = pFetch->TypeParam_0x0001_BoneOffset[2];
        BoneUnit_ptr->BoneType_0x0001_BoneIndex            = pFetch->TypeParam_0x0001_BoneIndex;
        BoneUnit_ptr->BoneType_0x0100_BoneIndex            = pFetch->TypeParam_0x0100_BoneIndex;
        BoneUnit_ptr->BoneType_0x0100_Percent              = pFetch->TypeParam_0x0100_Percent;
        BoneUnit_ptr->BoneType_0x0400_DirectonVertex[0]    = pFetch->TypeParam_0x0400_DirectonVertex[0];
        BoneUnit_ptr->BoneType_0x0400_DirectonVertex[1]    = pFetch->TypeParam_0x0400_DirectonVertex[1];
        BoneUnit_ptr->BoneType_0x0400_DirectonVertex[2]    = pFetch->TypeParam_0x0400_DirectonVertex[2];
        BoneUnit_ptr->BoneType_0x0800_Vertex_X[0]          = pFetch->TypeParam_0x0800_Vertex_X[0];
        BoneUnit_ptr->BoneType_0x0800_Vertex_X[1]          = pFetch->TypeParam_0x0800_Vertex_X[1];
        BoneUnit_ptr->BoneType_0x0800_Vertex_X[2]          = pFetch->TypeParam_0x0800_Vertex_X[2];
        BoneUnit_ptr->BoneType_0x0800_Vertex_Z[0]          = pFetch->TypeParam_0x0800_Vertex_Z[0];
        BoneUnit_ptr->BoneType_0x0800_Vertex_Z[1]          = pFetch->TypeParam_0x0800_Vertex_Z[1];
        BoneUnit_ptr->BoneType_0x0800_Vertex_Z[2]          = pFetch->TypeParam_0x0800_Vertex_Z[2];
        BoneUnit_ptr->BoneType_0x2000_Key                  = pFetch->TypeParam_0x2000_Key;
        if(pFetch->BoneType & 0x0020) {
            IKChain_ptr->MaxAngle        = pFetch->TypeParam_0x0020_IK.MaxAngle;
            IKChain_ptr->MaxIterations   = pFetch->TypeParam_0x0020_IK.MaxIterations;
            IKChain_ptr->ReachBoneID     = i;
            IKChain_ptr->TargetBoneID    = pFetch->TypeParam_0x0020_IK.TargetBoneID;
            IKChain_ptr->IKChainCount    = pFetch->TypeParam_0x0020_IK.IKChainCount;
            IKChain_ptr->IKChain         = (struct IKChainUnit *)malloc(sizeof(struct IKChainUnit) * IKChain_ptr->IKChainCount);

            for(IKi = 0 ; IKi < IKChain_ptr->IKChainCount ; IKi++) {
                struct IKChainUnit *IKChainUnit_ptr = IKChain_ptr->IKChain + IKi;
                struct PMX_InverseKinematicsChainContext *PMX_IKChainUnit_ptr = pFetch->TypeParam_0x0020_IK.IKChain + IKi;
                IKChainUnit_ptr->isLimit        = PMX_IKChainUnit_ptr->isLimit;
                IKChainUnit_ptr->LinkBone       = PMX_IKChainUnit_ptr->LinkBone;
                IKChainUnit_ptr->LowerBound[0]  = PMX_IKChainUnit_ptr->LowerBound[0];
                IKChainUnit_ptr->LowerBound[1]  = PMX_IKChainUnit_ptr->LowerBound[1];
                IKChainUnit_ptr->LowerBound[2]  = PMX_IKChainUnit_ptr->LowerBound[2];
                IKChainUnit_ptr->UpBound[0]     = PMX_IKChainUnit_ptr->UpBound[0];
                IKChainUnit_ptr->UpBound[1]     = PMX_IKChainUnit_ptr->UpBound[1];
                IKChainUnit_ptr->UpBound[2]     = PMX_IKChainUnit_ptr->UpBound[2];
                IKChainUnit_ptr->IKQuaternion.init();
                /*IKChainUnit_ptr->LowerBound[0]  = PMX_IKChainUnit_ptr->LowerBound[0] / 3.1415926535f;
                IKChainUnit_ptr->LowerBound[1]  = PMX_IKChainUnit_ptr->LowerBound[1] / 3.1415926535f;
                IKChainUnit_ptr->LowerBound[2]  = PMX_IKChainUnit_ptr->LowerBound[2] / 3.1415926535f;
                IKChainUnit_ptr->UpBound[0]     = PMX_IKChainUnit_ptr->UpBound[0] / 3.1415926535f;
                IKChainUnit_ptr->UpBound[1]     = PMX_IKChainUnit_ptr->UpBound[1] / 3.1415926535f;
                IKChainUnit_ptr->UpBound[2]     = PMX_IKChainUnit_ptr->UpBound[2] / 3.1415926535f;*/
            }
            IKChain_ptr++;
        }
        BoneUnit_ptr++;
        pFetch++;
    }
    free(nameUTF8);
    return 1;
}
// ---------------------------------------------------------------------------------
int vaMemory::
generate_BoneMemory(void *srcData, int srcMode)
{
    int ret = 0;
    switch(srcMode) {
    case InputFileMode_PMD :
        ret = generate_BoneMemory_Soruce_PMD((struct PolygonModelDataContext *)srcData);
        break;
    case InputFileMode_PMX :
        ret = generate_BoneMemory_Soruce_PMX((struct PolygonModeleXtendedContext *)srcData);
        break;
    }
    printf("generate_BoneMemory ... Return value %d\n", ret);
    return ret;
}

/** ---------------------------------------------------------------------------------
 * Generate the part of Texture Memory.
 */
static IplImage *load_Texture(char *fPath, char *fName)
{
    char FullPath[1024];

    printf("load_Texture , path %s, name %s\n", fPath, fName);
    strcpy(FullPath, fPath);
    strcat(FullPath, fName);

    /* check the input file (only support bmp) */
    char *tgaPath = FullPath + strlen(FullPath) -3;
    printf("%s\n", tgaPath);
    if(strcmp("tga", tgaPath) ==0) {
        *tgaPath++ = 'b';
        *tgaPath++ = 'm';
        *tgaPath++ = 'p';
    }

    printf("Load Texture in path <%s>\n", FullPath);
    IplImage *TextureImg = cvLoadImage(FullPath, CV_LOAD_IMAGE_UNCHANGED);
    if(!TextureImg) {
        printf(pLIGHT_RED "Fatal" pNONE " : Read texture file: %s ... Failed\n" pNONE, FullPath);
        return NULL;
    }
    printf(pLIGHT_GREEN "Info " pNONE ": Read texture file: %s ... Success\n", FullPath);
    printf("\tWidth : %d,  Height : %d, Channels : %d\n", TextureImg->width, TextureImg->height, TextureImg->nChannels);
    printf("\tImageSize : %d, WidthStep : %d\n", TextureImg->imageSize, TextureImg->widthStep );
    return TextureImg ;
}
// ---------------------------------------------------------------------------------
int vaMemory::
generate_TextureMemory_Soruce_PMD(struct PolygonModelDataContext *srcPMD)
{
    int i;
    struct PMD_MaterialInformation *src_ptr;
    struct TextureContext **Texture_ptr;
    vMemoryCtx->TextureMemory.TextureCount = srcPMD->MaterialTextureCount;
    vMemoryCtx->TextureMemory.TextureImg  = (TextureContext **)malloc(sizeof(TextureContext *) *vMemoryCtx->TextureMemory.TextureCount);
    printf("vMemoryCtx->TextureMemory.TextureCount %d\n", vMemoryCtx->TextureMemory.TextureCount);

    if(vMemoryCtx->TextureMemory.TextureImg == NULL)
        return 0;

    Texture_ptr = vMemoryCtx->TextureMemory.TextureImg;
    src_ptr = srcPMD->MaterialInfo;
    for(i = 0 ; i < srcPMD->MaterialInfoCount ; i++) {  //calculate the cout if texture
        if(src_ptr->TextureImg !=NULL) {
            (*Texture_ptr) = (TextureContext *)malloc(sizeof(TextureContext));
            (*Texture_ptr)->Width = src_ptr->TextureImg->Width;
            (*Texture_ptr)->Height = src_ptr->TextureImg->Height;
            (*Texture_ptr)->ByteLen = src_ptr->TextureImg->ByteLen;
            (*Texture_ptr)->RGBA8888 = (unsigned char *)malloc(src_ptr->TextureImg->Width * src_ptr->TextureImg->Height *4);

            unsigned char *store_ptr = (*Texture_ptr)->RGBA8888;
            unsigned char *pFetch = src_ptr->TextureImg->RGB888;
            for(int y = 0 ; y < src_ptr->TextureImg->Height ; y++) {
                for(int x = 0 ; x < src_ptr->TextureImg->Width ; x++) {
                    *store_ptr++ = *(pFetch+2);
                    *store_ptr++ = *(pFetch+1);
                    *store_ptr++ = *(pFetch);
                    *store_ptr++ = 255;
                    pFetch +=3;
                }
            }
            Texture_ptr++;
        }
        src_ptr++;
    }
    return 1;
}
// ---------------------------------------------------------------------------------
int vaMemory::
generate_TextureMemory_Soruce_PMX(struct PolygonModeleXtendedContext *srcPMX)
{
    int i;
    struct PMX_TextureInformation *TextureInfo = srcPMX->TextureInfo;

    vMemoryCtx->TextureMemory.TextureCount = srcPMX->TextureInfoCount ;
    vMemoryCtx->TextureMemory.TextureImg  = (TextureContext **)malloc(sizeof(TextureContext *) *srcPMX->TextureInfoCount);
    if(vMemoryCtx->TextureMemory.TextureImg == NULL) return 0;

    TextureContext **Texture_ptr = vMemoryCtx->TextureMemory.TextureImg;
    printf("srcPMX->TextureInfoCount %d\n", srcPMX->TextureInfoCount);
    for(i = 0 ; i < srcPMX->TextureInfoCount ; i++) {
        *Texture_ptr = (TextureContext *)malloc(sizeof(TextureContext));
        IplImage *rImg = load_Texture(srcPMX->mPath, TextureInfo->name_US.text_UTF8);
        (*Texture_ptr)->Width = rImg->width;
        (*Texture_ptr)->Height = rImg->height;
        (*Texture_ptr)->ByteLen = rImg->width * rImg->height * 4;
        (*Texture_ptr)->RGBA8888 = (unsigned char *)malloc((*Texture_ptr)->ByteLen);

        unsigned char *store_ptr = (*Texture_ptr)->RGBA8888;
        unsigned char *pFetch = (unsigned char *)rImg->imageData;
        if(rImg->nChannels == 3) {
            int iSize = rImg->height * rImg->height;
            while(iSize >0) {
                *store_ptr++ = *(pFetch +2);
                *store_ptr++ = *(pFetch +1);
                *store_ptr++ = *(pFetch);
                *store_ptr++ = 255;
                pFetch +=3;
                iSize--;
            }
        }
        else if(rImg->nChannels == 4) {
            int iSize = rImg->height * rImg->height;
            while(iSize >0) {
                *store_ptr++ = *(pFetch +2);
                *store_ptr++ = *(pFetch +1);
                *store_ptr++ = *(pFetch);
                *store_ptr++ = *(pFetch +3);
                pFetch +=4;
                iSize--;
            }
        }
        Texture_ptr++;
        TextureInfo++;
    }
    return 1;
}
// ---------------------------------------------------------------------------------
int vaMemory::
generate_TextureMemory(void *srcData, int srcMode)
{
    int ret = 0;
    switch(srcMode) {
    case InputFileMode_PMD :
        ret = generate_TextureMemory_Soruce_PMD((struct PolygonModelDataContext *)srcData);
        break;
    case InputFileMode_PMX :
        ret = generate_TextureMemory_Soruce_PMX((struct PolygonModeleXtendedContext *)srcData);
        break;
    }
    printf("generate_TextureMemory ... Return value %d\n", ret);
    return ret;
}

/** ---------------------------------------------------------------------------------
 * Generate the part of Material Memory.
 */
int vaMemory::
generate_MaterialMemory_Soruce_PMD(struct PolygonModelDataContext *srcPMD)
{
    int i;
    struct MaterialUnitContext *MaterialUnit_ptr;
    struct PMD_MaterialInformation *pFetch = srcPMD->MaterialInfo ;

    printf("srcPMD->MaterialInfoCount %d\n", srcPMD->MaterialInfoCount);
    vMemoryCtx->MaterialMemory.MaterialUnitCount = srcPMD->MaterialInfoCount;
    vMemoryCtx->MaterialMemory.MaterialUnit = (struct MaterialUnitContext *)malloc(sizeof(struct MaterialUnitContext) *srcPMD->MaterialInfoCount);
    if(vMemoryCtx->MaterialMemory.MaterialUnit == NULL)
        return 0;

    MaterialUnit_ptr= vMemoryCtx->MaterialMemory.MaterialUnit;
    for(i = 0 ; i < srcPMD->MaterialInfoCount ; i++) {
        MaterialUnit_ptr->JPnameKey_DKDR   = sutHash_DKDR(pFetch->fname, strlen(pFetch->fname));
        MaterialUnit_ptr->JPnameKey_djb2   = sutHash_djb2(pFetch->fname, strlen(pFetch->fname));
        MaterialUnit_ptr->Diffuse_RGBA[0]  = pFetch->Diffuse_RGBA[0];
        MaterialUnit_ptr->Diffuse_RGBA[1]  = pFetch->Diffuse_RGBA[1];
        MaterialUnit_ptr->Diffuse_RGBA[2]  = pFetch->Diffuse_RGBA[2];
        MaterialUnit_ptr->Diffuse_RGBA[3]  = pFetch->Diffuse_RGBA[3];
        MaterialUnit_ptr->Specular_RGBA[0] = pFetch->Specular_RGB[0];
        MaterialUnit_ptr->Specular_RGBA[1] = pFetch->Specular_RGB[1];
        MaterialUnit_ptr->Specular_RGBA[2] = pFetch->Specular_RGB[2];
        MaterialUnit_ptr->Specular_RGBA[3] = 0.0f;
        MaterialUnit_ptr->Ambient_RGBA[0]  = pFetch->Ambient_RGB[0];
        MaterialUnit_ptr->Ambient_RGBA[1]  = pFetch->Ambient_RGB[1];
        MaterialUnit_ptr->Ambient_RGBA[2]  = pFetch->Ambient_RGB[2];
        MaterialUnit_ptr->Ambient_RGBA[3]  = 0.0f;
        MaterialUnit_ptr->Specularity      = pFetch->Specularity;
        MaterialUnit_ptr->SurfaceCount     = pFetch->EffectRange ;
        MaterialUnit_ptr->TextureIndex     = pFetch->TextureIndex ;
        MaterialUnit_ptr++;
        pFetch++;
    }
    return 1;
}
// ---------------------------------------------------------------------------------
int vaMemory::
generate_MaterialMemory_Soruce_PMX(struct PolygonModeleXtendedContext *srcPMX)
{
    int i;
    struct MaterialUnitContext *MaterialUnit_ptr;
    struct PMX_MaterialInformation *pFetch = srcPMX->MaterialInfo ;

    vMemoryCtx->MaterialMemory.MaterialUnitCount = srcPMX->MaterialInfoCount;
    vMemoryCtx->MaterialMemory.MaterialUnit = (struct MaterialUnitContext *)malloc(sizeof(struct MaterialUnitContext) *srcPMX->MaterialInfoCount);
    if(vMemoryCtx->MaterialMemory.MaterialUnit == NULL)
        return 0;

    MaterialUnit_ptr= vMemoryCtx->MaterialMemory.MaterialUnit;
    for(i = 0 ; i < srcPMX->MaterialInfoCount ; i++) {
        MaterialUnit_ptr->JPnameKey_DKDR   = sutHash_DKDR(pFetch->name_JP.text_UTF8, pFetch->name_JP.Length);
        MaterialUnit_ptr->JPnameKey_djb2   = sutHash_djb2(pFetch->name_JP.text_UTF8, pFetch->name_JP.Length);
        MaterialUnit_ptr->Diffuse_RGBA[0]  = pFetch->Diffuse_RGBA[0];
        MaterialUnit_ptr->Diffuse_RGBA[1]  = pFetch->Diffuse_RGBA[1];
        MaterialUnit_ptr->Diffuse_RGBA[2]  = pFetch->Diffuse_RGBA[2];
        MaterialUnit_ptr->Diffuse_RGBA[3]  = pFetch->Diffuse_RGBA[3];
        MaterialUnit_ptr->Specular_RGBA[0] = pFetch->Specular_RGB[0];
        MaterialUnit_ptr->Specular_RGBA[1] = pFetch->Specular_RGB[1];
        MaterialUnit_ptr->Specular_RGBA[2] = pFetch->Specular_RGB[2];
        MaterialUnit_ptr->Specular_RGBA[2] = 0.0f;
        MaterialUnit_ptr->Ambient_RGBA[0]  = pFetch->Ambient_RGB[0];
        MaterialUnit_ptr->Ambient_RGBA[1]  = pFetch->Ambient_RGB[1];
        MaterialUnit_ptr->Ambient_RGBA[2]  = pFetch->Ambient_RGB[2];
        MaterialUnit_ptr->Ambient_RGBA[3]  = 0.0f;
        MaterialUnit_ptr->Specularity      = pFetch->Specularity;
        MaterialUnit_ptr->bitFlag          = pFetch->bitFlag;
        MaterialUnit_ptr->SurfaceCount     = pFetch->SurfaceCount;
        MaterialUnit_ptr->Edge_RGBA[0]     = pFetch->Edge_RGBA[0];
        MaterialUnit_ptr->Edge_RGBA[1]     = pFetch->Edge_RGBA[1];
        MaterialUnit_ptr->Edge_RGBA[2]     = pFetch->Edge_RGBA[2];
        MaterialUnit_ptr->Edge_RGBA[3]     = pFetch->Edge_RGBA[3];
        MaterialUnit_ptr->bitFlag          = pFetch->bitFlag;
        MaterialUnit_ptr->Edge_Scale       = pFetch->Edge_Scale;
        MaterialUnit_ptr->TextureIndex     = pFetch->TextureIndex;
        MaterialUnit_ptr->sphereTextureIndex = pFetch->sphereTextureIndex;
        MaterialUnit_ptr->sphereTextureBlendMode = pFetch->sphereTextureBlendMode;
        MaterialUnit_ptr->ToonReference    = pFetch->ToonReference;
        MaterialUnit_ptr->ToonValue        = pFetch->ToonValue;
        MaterialUnit_ptr->bitFlag          = pFetch->bitFlag;
        MaterialUnit_ptr++;
        pFetch++;
    }
    return 1;
}
// ---------------------------------------------------------------------------------
int vaMemory::
generate_MaterialMemory(void *srcData, int srcMode)
{
    int ret = 0;
    switch(srcMode) {
    case InputFileMode_PMD :
        ret = generate_MaterialMemory_Soruce_PMD((struct PolygonModelDataContext *)srcData);
        break;
    case InputFileMode_PMX :
        ret = generate_MaterialMemory_Soruce_PMX((struct PolygonModeleXtendedContext *)srcData);
        break;
    }
    printf("generate_MaterialMemory ... Return value %d\n", ret);
    return ret;
}

/** ---------------------------------------------------------------------------------
 * Generate the part of Vertex memory.
 */
int vaMemory::
generate_VertexMemory_Soruce_PMD(struct PolygonModelDataContext *srcPMD)
{
    int i;
    struct VertexUnitContext *VertexUnit_ptr;
    float *StoreVertex_ptr;
    float *StoreNoraml_ptr;
    float *StoreTexture_ptr;
    struct DeformType_BDEF2 *StoreBDEF2_ptr;
    struct PMD_VertexInformation *pFetch = srcPMD->VertexInfo;

    //parse the count of each deform type
    memcpy(vMemoryCtx->GeometryCtx.Center, srcPMD->ModelGeometryCtx.ModelCenter, sizeof(float) *3);
    memcpy(vMemoryCtx->GeometryCtx.Size, srcPMD->ModelGeometryCtx.ModelSize, sizeof(float) *3);
    memcpy(vMemoryCtx->GeometryCtx.Boundary, srcPMD->ModelGeometryCtx.ModelBoundary, sizeof(float) *3 *2);
    memcpy(vMemoryCtx->GeometryCtx.RawCenter, srcPMD->ModelGeometryCtx.RawModelCenter, sizeof(float) *3);
    memcpy(vMemoryCtx->GeometryCtx.RawSize, srcPMD->ModelGeometryCtx.RawModelSize, sizeof(float) *3);
    memcpy(vMemoryCtx->GeometryCtx.RawBoundary, srcPMD->ModelGeometryCtx.RawModelBoundary, sizeof(float) *3 *2);
    vMemoryCtx->GeometryCtx.ScaleRate = srcPMD->ModelGeometryCtx.ModelBoundScaleRate;
    vMemoryCtx->VertexMemory.BDEF1Count = 0;
    vMemoryCtx->VertexMemory.BDEF2Count = srcPMD->VertexInfoCount;
    vMemoryCtx->VertexMemory.BDEF4Count = 0;
    vMemoryCtx->VertexMemory.SDEFCount  = 0;
    printf("BDEF1Count : %d\n", vMemoryCtx->VertexMemory.BDEF1Count);
    printf("BDEF2Count : %d\n", vMemoryCtx->VertexMemory.BDEF2Count);
    printf("BDEF4Count : %d\n", vMemoryCtx->VertexMemory.BDEF4Count);
    printf("SDEFCount : %d\n", vMemoryCtx->VertexMemory.SDEFCount);

    vMemoryCtx->VertexMemory.VertexUnitCount = srcPMD->VertexInfoCount;
    vMemoryCtx->VertexMemory.VertexUnit = (struct VertexUnitContext *)malloc(sizeof(struct VertexUnitContext) *srcPMD->VertexInfoCount);
    vMemoryCtx->VertexMemory.Vertex      = (float *)malloc(sizeof(float) *srcPMD->VertexInfoCount *3);
    vMemoryCtx->VertexMemory.Normal      = (float *)malloc(sizeof(float) *srcPMD->VertexInfoCount *3);
    vMemoryCtx->VertexMemory.TexCoord    = (float *)malloc(sizeof(float) *srcPMD->VertexInfoCount *2);
    vMemoryCtx->VertexMemory.BDEF1       = (struct DeformType_BDEF1 *)malloc(sizeof(struct DeformType_BDEF1) *vMemoryCtx->VertexMemory.BDEF1Count);
    vMemoryCtx->VertexMemory.BDEF2       = (struct DeformType_BDEF2 *)malloc(sizeof(struct DeformType_BDEF2) *vMemoryCtx->VertexMemory.BDEF2Count);
    vMemoryCtx->VertexMemory.BDEF4       = (struct DeformType_BDEF4 *)malloc(sizeof(struct DeformType_BDEF4) *vMemoryCtx->VertexMemory.BDEF4Count);
    vMemoryCtx->VertexMemory.SDEF        = (struct DeformType_SDEF *)malloc(sizeof(struct DeformType_SDEF) *vMemoryCtx->VertexMemory.SDEFCount);

    if(vMemoryCtx->VertexMemory.VertexUnit == NULL || vMemoryCtx->VertexMemory.Vertex   == NULL ||
       vMemoryCtx->VertexMemory.Normal     == NULL || vMemoryCtx->VertexMemory.TexCoord == NULL ||
       (vMemoryCtx->VertexMemory.BDEF1Count >0 && vMemoryCtx->VertexMemory.BDEF1 == NULL) ||
       (vMemoryCtx->VertexMemory.BDEF2Count >0 && vMemoryCtx->VertexMemory.BDEF2 == NULL) ||
       (vMemoryCtx->VertexMemory.BDEF4Count >0 && vMemoryCtx->VertexMemory.BDEF4 == NULL) ||
       (vMemoryCtx->VertexMemory.SDEFCount >0  && vMemoryCtx->VertexMemory.SDEF  == NULL))
        return 0;

    VertexUnit_ptr  = vMemoryCtx->VertexMemory.VertexUnit;
    StoreVertex_ptr  = vMemoryCtx->VertexMemory.Vertex;
    StoreNoraml_ptr  = vMemoryCtx->VertexMemory.Normal;
    StoreTexture_ptr = vMemoryCtx->VertexMemory.TexCoord;
    StoreBDEF2_ptr   = vMemoryCtx->VertexMemory.BDEF2;

    //store data into memory
    pFetch = srcPMD->VertexInfo;
    for(i = 0 ; i < srcPMD->VertexInfoCount ; i++) {
        VertexUnit_ptr->vID = i;
        VertexUnit_ptr->EdgeScale = 1 ;
        VertexUnit_ptr->DeformType = 1; // 1:BDEF2
        StoreBDEF2_ptr->BoneIndex_1 = pFetch->BoneID_0;
        StoreBDEF2_ptr->BoneIndex_2 = pFetch->BoneID_1;
        StoreBDEF2_ptr->Weight_1    = (float)pFetch->BoneWeight / 100.0f;
        VertexUnit_ptr->DeformID    = StoreBDEF2_ptr - vMemoryCtx->VertexMemory.BDEF2;
        StoreBDEF2_ptr++;
        *StoreVertex_ptr++ = pFetch->Vertex[0];
        *StoreVertex_ptr++ = pFetch->Vertex[1];
        *StoreVertex_ptr++ = pFetch->Vertex[2];
        *StoreNoraml_ptr++ = pFetch->Normal[0];
        *StoreNoraml_ptr++ = pFetch->Normal[1];
        *StoreNoraml_ptr++ = pFetch->Normal[2];
        *StoreTexture_ptr++ = fabs(pFetch->Texture[0]);
        *StoreTexture_ptr++ = fabs(pFetch->Texture[1]) ;
        pFetch++;
        VertexUnit_ptr++;
    }
    return 1;
}
// ---------------------------------------------------------------------------------
int vaMemory::
generate_VertexMemory_Soruce_PMX(struct PolygonModeleXtendedContext *srcPMX)
{
    int i;
    struct VertexUnitContext *VertexUnit_ptr;
    float *StoreVertex_ptr;
    float *StoreNoraml_ptr;
    float *StoreTexture_ptr;
    struct DeformType_BDEF1 *StoreBDEF1_ptr;
    struct DeformType_BDEF2 *StoreBDEF2_ptr;
    struct DeformType_BDEF4 *StoreBDEF4_ptr;
    struct DeformType_SDEF  *StoreSDEF_ptr;
    struct PMX_VertexInformation *pFetch = srcPMX->VertexInfo;

    //parse the count of each deform type
    memcpy(vMemoryCtx->GeometryCtx.Center, srcPMX->ModelGeometryCtx.ModelCenter, sizeof(float) *3);
    memcpy(vMemoryCtx->GeometryCtx.Size, srcPMX->ModelGeometryCtx.ModelSize, sizeof(float) *3);
    memcpy(vMemoryCtx->GeometryCtx.Boundary, srcPMX->ModelGeometryCtx.ModelBoundary, sizeof(float) *3 *2);
    memcpy(vMemoryCtx->GeometryCtx.RawCenter, srcPMX->ModelGeometryCtx.RawModelCenter, sizeof(float) *3);
    memcpy(vMemoryCtx->GeometryCtx.RawSize, srcPMX->ModelGeometryCtx.RawModelSize, sizeof(float) *3);
    memcpy(vMemoryCtx->GeometryCtx.RawBoundary, srcPMX->ModelGeometryCtx.RawModelBoundary, sizeof(float) *3 *2);
    vMemoryCtx->GeometryCtx.ScaleRate = srcPMX->ModelGeometryCtx.ModelBoundScaleRate;
    vMemoryCtx->VertexMemory.BDEF1Count =0;
    vMemoryCtx->VertexMemory.BDEF2Count =0;
    vMemoryCtx->VertexMemory.BDEF4Count =0;
    vMemoryCtx->VertexMemory.SDEFCount =0;
    for(i = 0 ; i < srcPMX->VertexInfoCount ; i++) {
        switch(pFetch->DeformType) {
        case 0: // 0:BDEF1
            vMemoryCtx->VertexMemory.BDEF1Count++;
            break;
        case 1: // 1:BDEF2
            vMemoryCtx->VertexMemory.BDEF2Count++;
            break;
        case 2: // 2:BDEF4
            vMemoryCtx->VertexMemory.BDEF4Count++;
            break;
        case 3: // 3:SDEF
            vMemoryCtx->VertexMemory.SDEFCount++;
            break;
        }
        pFetch++;
    }
    printf("BDEF1Count : %d\n", vMemoryCtx->VertexMemory.BDEF1Count);
    printf("BDEF2Count : %d\n", vMemoryCtx->VertexMemory.BDEF2Count);
    printf("BDEF4Count : %d\n", vMemoryCtx->VertexMemory.BDEF4Count);
    printf("SDEFCount : %d\n", vMemoryCtx->VertexMemory.SDEFCount);

    vMemoryCtx->VertexMemory.VertexUnitCount = srcPMX->VertexInfoCount;
    vMemoryCtx->VertexMemory.VertexUnit = (struct VertexUnitContext *)malloc(sizeof(struct VertexUnitContext) *srcPMX->VertexInfoCount);
    vMemoryCtx->VertexMemory.Vertex      = (float *)malloc(sizeof(float) *srcPMX->VertexInfoCount *3);
    vMemoryCtx->VertexMemory.Normal      = (float *)malloc(sizeof(float) *srcPMX->VertexInfoCount *3);
    vMemoryCtx->VertexMemory.TexCoord    = (float *)malloc(sizeof(float) *srcPMX->VertexInfoCount *2);
    vMemoryCtx->VertexMemory.BDEF1       = (struct DeformType_BDEF1 *)malloc(sizeof(struct DeformType_BDEF1) *vMemoryCtx->VertexMemory.BDEF1Count);
    vMemoryCtx->VertexMemory.BDEF2       = (struct DeformType_BDEF2 *)malloc(sizeof(struct DeformType_BDEF2) *vMemoryCtx->VertexMemory.BDEF2Count);
    vMemoryCtx->VertexMemory.BDEF4       = (struct DeformType_BDEF4 *)malloc(sizeof(struct DeformType_BDEF4) *vMemoryCtx->VertexMemory.BDEF4Count);
    vMemoryCtx->VertexMemory.SDEF        = (struct DeformType_SDEF *)malloc(sizeof(struct DeformType_SDEF) *vMemoryCtx->VertexMemory.SDEFCount);

    if(vMemoryCtx->VertexMemory.VertexUnit == NULL || vMemoryCtx->VertexMemory.Vertex   == NULL ||
       vMemoryCtx->VertexMemory.Normal     == NULL || vMemoryCtx->VertexMemory.TexCoord == NULL ||
       (vMemoryCtx->VertexMemory.BDEF1Count >0 && vMemoryCtx->VertexMemory.BDEF1 == NULL) ||
       (vMemoryCtx->VertexMemory.BDEF2Count >0 && vMemoryCtx->VertexMemory.BDEF2 == NULL) ||
       (vMemoryCtx->VertexMemory.BDEF4Count >0 && vMemoryCtx->VertexMemory.BDEF4 == NULL) ||
       (vMemoryCtx->VertexMemory.SDEFCount >0  && vMemoryCtx->VertexMemory.SDEF  == NULL))
        return 0;

    VertexUnit_ptr  = vMemoryCtx->VertexMemory.VertexUnit;
    StoreVertex_ptr  = vMemoryCtx->VertexMemory.Vertex;
    StoreNoraml_ptr  = vMemoryCtx->VertexMemory.Normal;
    StoreTexture_ptr = vMemoryCtx->VertexMemory.TexCoord;
    StoreBDEF1_ptr   = vMemoryCtx->VertexMemory.BDEF1;
    StoreBDEF2_ptr   = vMemoryCtx->VertexMemory.BDEF2;
    StoreBDEF4_ptr   = vMemoryCtx->VertexMemory.BDEF4;
    StoreSDEF_ptr    = vMemoryCtx->VertexMemory.SDEF;

    //store data into memory
    pFetch = srcPMX->VertexInfo;
    for(i = 0 ; i < srcPMX->VertexInfoCount ; i++) {
        VertexUnit_ptr->vID = i;
        VertexUnit_ptr->EdgeScale = pFetch->EdgeScale;
        VertexUnit_ptr->DeformType = pFetch->DeformType;

        switch(pFetch->DeformType) {
        case 0: // 0:BDEF1
            StoreBDEF1_ptr->vID = i *3;
            StoreBDEF1_ptr->BoneIndex = pFetch->BDEF1.BoneIndex;
            VertexUnit_ptr->DeformID = StoreBDEF1_ptr - vMemoryCtx->VertexMemory.BDEF1;
            StoreBDEF1_ptr++;
            break;
        case 1: // 1:BDEF2
            StoreBDEF2_ptr->vID = i *3;
            StoreBDEF2_ptr->BoneIndex_1 = pFetch->BDEF2.BoneIndex_1;
            StoreBDEF2_ptr->BoneIndex_2 = pFetch->BDEF2.BoneIndex_2;
            StoreBDEF2_ptr->Weight_1    = pFetch->BDEF2.Weight_1;
            VertexUnit_ptr->DeformID    = StoreBDEF2_ptr - vMemoryCtx->VertexMemory.BDEF2;
            StoreBDEF2_ptr++;
            break;
        case 2: // 2:BDEF4
            StoreBDEF4_ptr->vID = i *3;
            StoreBDEF4_ptr->BoneIndex_1 = pFetch->BDEF4.BoneIndex_1;
            StoreBDEF4_ptr->BoneIndex_2 = pFetch->BDEF4.BoneIndex_2;
            StoreBDEF4_ptr->BoneIndex_3 = pFetch->BDEF4.BoneIndex_3;
            StoreBDEF4_ptr->BoneIndex_4 = pFetch->BDEF4.BoneIndex_4;
            StoreBDEF4_ptr->Weight_1    = pFetch->BDEF4.Weight_1;
            StoreBDEF4_ptr->Weight_2    = pFetch->BDEF4.Weight_2;
            StoreBDEF4_ptr->Weight_3    = pFetch->BDEF4.Weight_3;
            StoreBDEF4_ptr->Weight_4    = pFetch->BDEF4.Weight_4;
            VertexUnit_ptr->DeformID = StoreBDEF4_ptr - vMemoryCtx->VertexMemory.BDEF4;
            StoreBDEF4_ptr++;
            break;
        case 3: // 3:SDEF
            StoreSDEF_ptr->vID = i *3;
            StoreSDEF_ptr->BoneIndex_1 = pFetch->SDEF.BoneIndex_1;
            StoreSDEF_ptr->BoneIndex_2 = pFetch->SDEF.BoneIndex_2;
            StoreSDEF_ptr->C[0] = pFetch->SDEF.C.x;
            StoreSDEF_ptr->C[1] = pFetch->SDEF.C.y;
            StoreSDEF_ptr->C[2] = pFetch->SDEF.C.z;
            StoreSDEF_ptr->R0[0] = pFetch->SDEF.R0.x;
            StoreSDEF_ptr->R0[1] = pFetch->SDEF.R0.y;
            StoreSDEF_ptr->R0[2] = pFetch->SDEF.R0.z;
            StoreSDEF_ptr->R1[0] = pFetch->SDEF.R1.x;
            StoreSDEF_ptr->R1[1] = pFetch->SDEF.R1.y;
            StoreSDEF_ptr->R1[2] = pFetch->SDEF.R1.z;
            VertexUnit_ptr->DeformID = StoreSDEF_ptr - vMemoryCtx->VertexMemory.SDEF;
            StoreSDEF_ptr++;
            break;
        }
        *StoreVertex_ptr++ = pFetch->Vertex[0];
        *StoreVertex_ptr++ = pFetch->Vertex[1];
        *StoreVertex_ptr++ = pFetch->Vertex[2];
        *StoreNoraml_ptr++ = pFetch->Normal[0];
        *StoreNoraml_ptr++ = pFetch->Normal[1];
        *StoreNoraml_ptr++ = pFetch->Normal[2];
        *StoreTexture_ptr++ = pFetch->Texture[0];
        *StoreTexture_ptr++ = pFetch->Texture[1];
        pFetch++;
        VertexUnit_ptr++;
    }


    return 1;
}

// ---------------------------------------------------------------------------------
int vaMemory::
generate_VertexMemory(void *srcData, int srcMode)
{
    int ret = 0;
    switch(srcMode) {
    case InputFileMode_PMD :
        ret = generate_VertexMemory_Soruce_PMD((struct PolygonModelDataContext *)srcData);
        break;
    case InputFileMode_PMX :
        ret = generate_VertexMemory_Soruce_PMX((struct PolygonModeleXtendedContext *)srcData);
        break;
    }
    printf("generate_VertexMemory ... Return value %d\n", ret);
    return ret;
}

/** ---------------------------------------------------------------------------------
 * Generate the part of Mesh Index memory.
 */
int vaMemory::
generate_IndexMemory_Soruce_PMD(struct PolygonModelDataContext *srcPMD)
{
    vMemoryCtx->IndexMemory.IndexUnitCount = srcPMD->IndexInfoCount ;
    vMemoryCtx->IndexMemory.IndexUnitTypeSize = 2;
    vMemoryCtx->IndexMemory.IndexUnit = (unsigned char *)malloc(srcPMD->IndexInfoCount * vMemoryCtx->IndexMemory.IndexUnitTypeSize);
    if(vMemoryCtx->IndexMemory.IndexUnit == NULL) return 0;
    memcpy(vMemoryCtx->IndexMemory.IndexUnit, srcPMD->IndexInfo, srcPMD->IndexInfoCount * vMemoryCtx->IndexMemory.IndexUnitTypeSize);
    return 1;
}
// ---------------------------------------------------------------------------------
int vaMemory::
generate_IndexMemory_Soruce_PMX(struct PolygonModeleXtendedContext *srcPMX)
{

    printf("VertexIndexSize %d, IndexCount : %d\n", srcPMX->GlobalParamSet.VertexIndexSize, srcPMX->IndexInfoCount);
    vMemoryCtx->IndexMemory.IndexUnitCount = srcPMX->IndexInfoCount;
    vMemoryCtx->IndexMemory.IndexUnitTypeSize = srcPMX->GlobalParamSet.VertexIndexSize;
    vMemoryCtx->IndexMemory.IndexUnit = (unsigned char *)malloc(srcPMX->IndexInfoCount * srcPMX->GlobalParamSet.VertexIndexSize);
    if(vMemoryCtx->IndexMemory.IndexUnit == NULL) return 0;
    memcpy(vMemoryCtx->IndexMemory.IndexUnit,
           srcPMX->IndexInfo,
           srcPMX->IndexInfoCount * srcPMX->GlobalParamSet.VertexIndexSize);
    return 1;
}
// ---------------------------------------------------------------------------------
int vaMemory::
generate_IndexMemory(void *srcData, int srcMode)
{
    int ret = 0;
    switch(srcMode) {
    case InputFileMode_PMD :
        ret = generate_IndexMemory_Soruce_PMD((struct PolygonModelDataContext *)srcData);
        break;
    case InputFileMode_PMX :
        ret = generate_IndexMemory_Soruce_PMX((struct PolygonModeleXtendedContext *)srcData);
        break;
    }
    printf("generate_IndexMemory ... Return value %d\n", ret);
    return ret;
}

/** ---------------------------------------------------------------------------------
 * Create the memory from PMD/PMX file.
 */
struct vaMemoryContext *vaMemory::
create(struct PolygonModelDataContext *srcPMD, unsigned int mElements)
{
    struct timeval pStart, pEnd;
    float uTime;

    printf("vaMemory Create\n");
    if(srcPMD == NULL) return 0;

    gettimeofday(&pStart, NULL);
    vMemoryCtx = (struct vaMemoryContext *)malloc(sizeof(struct vaMemoryContext));
    memset(vMemoryCtx, 0, sizeof(struct vaMemoryContext));

    vMemoryCtx->MaterializeElementBitFiled = mElements;
    if(mElements & MemoryElement_Body) {
        generate_VertexMemory((void *)srcPMD, InputFileMode_PMD);
        generate_IndexMemory((void *)srcPMD, InputFileMode_PMD);
    }
    if(mElements & MemoryElement_Material)  {
        generate_MaterialMemory((void *)srcPMD, InputFileMode_PMD);
        generate_TextureMemory((void *)srcPMD, InputFileMode_PMD);
    }
    if(mElements & MemoryElement_Bone) generate_BoneMemory((void *)srcPMD, InputFileMode_PMD);

    gettimeofday(&pEnd, NULL);
    uTime = (pEnd.tv_sec -pStart.tv_sec)*1000000.0 +(pEnd.tv_usec -pStart.tv_usec);
    printf("Memory time : %lf ms\n", uTime /1000.0f);
    printf("--------------------------------------------------------------------\n");
    return vMemoryCtx;
}
// ---------------------------------------------------------------------------------
struct vaMemoryContext *vaMemory::
create(PolygonModeleXtendedContext *srcPMX, unsigned int mElements)
{
    struct timeval pStart, pEnd;
    float uTime;

    printf("vaMemory Create\n");
    if(srcPMX == NULL) return 0;

    gettimeofday(&pStart, NULL);
    vMemoryCtx = (struct vaMemoryContext *)malloc(sizeof(struct vaMemoryContext));
    memset(vMemoryCtx, 0, sizeof(struct vaMemoryContext));

    vMemoryCtx->MaterializeElementBitFiled = mElements;
    if(mElements & MemoryElement_Body) {
        generate_VertexMemory((void *)srcPMX, InputFileMode_PMX);
        generate_IndexMemory((void *)srcPMX, InputFileMode_PMX);
    }
    if(mElements & MemoryElement_Material) {
        generate_MaterialMemory((void *)srcPMX, InputFileMode_PMX);
        generate_TextureMemory((void *)srcPMX, InputFileMode_PMX);
    }
    if(mElements & MemoryElement_Bone) generate_BoneMemory((void *)srcPMX, InputFileMode_PMX);

    gettimeofday(&pEnd, NULL);
    uTime = (pEnd.tv_sec -pStart.tv_sec)*1000000.0 +(pEnd.tv_usec -pStart.tv_usec);
    printf("Memory time : %lf ms\n", uTime /1000.0f);
    printf("--------------------------------------------------------------------\n");
    return vMemoryCtx;
}
// ---------------------------------------------------------------------------------
void vaMemory::
release(struct vaMemoryContext *srcVM)
{
    int i;
    if(srcVM->IndexMemory.IndexUnit != NULL) free(srcVM->IndexMemory.IndexUnit);
    if(srcVM->MaterialMemory.MaterialUnit != NULL) free(srcVM->MaterialMemory.MaterialUnit);

    if(srcVM->TextureMemory.TextureImg != NULL) {
        for(i = 0 ; i < srcVM->TextureMemory.TextureCount ; i++) {
            if(*(srcVM->TextureMemory.TextureImg +i) != NULL) free(*(srcVM->TextureMemory.TextureImg +i));
        }
        free(srcVM->TextureMemory.TextureImg);
    }
    if(srcVM->TextureMemory.ToonTextureImg != NULL) {
        for(i = 0 ; i < srcVM->TextureMemory.TextureCount ; i++) {
            if(*(srcVM->TextureMemory.ToonTextureImg +i) != NULL) free(*(srcVM->TextureMemory.ToonTextureImg +i));
        }
        free(srcVM->TextureMemory.ToonTextureImg);
    }
    if(srcVM->VertexMemory.BDEF1 != NULL) free (srcVM->VertexMemory.BDEF1);
    if(srcVM->VertexMemory.BDEF2 != NULL) free (srcVM->VertexMemory.BDEF2);
    if(srcVM->VertexMemory.BDEF4 != NULL) free (srcVM->VertexMemory.BDEF4);
    if(srcVM->VertexMemory.SDEF != NULL) free (srcVM->VertexMemory.SDEF);
    if(srcVM->VertexMemory.Normal  != NULL) free (srcVM->VertexMemory.Normal);
    if(srcVM->VertexMemory.Vertex  != NULL) free (srcVM->VertexMemory.Vertex);
    if(srcVM->VertexMemory.TexCoord  != NULL) free (srcVM->VertexMemory.TexCoord);
    if(srcVM->VertexMemory.VertexUnit  != NULL) free (srcVM->VertexMemory.VertexUnit);
}
