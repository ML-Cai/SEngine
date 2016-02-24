#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <sys/time.h>
#include "sutHash.h"
#include "VisualizationMemory.h"
// ---------------------------------------------------------------------------------
using namespace MMDCppDev;
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
// ---------------------------------------------------------------------------------
int VisualizationMemory::
generate_BoneMemory_Soruce_PMD(struct PolygonModelDataContext *srcPMD)
{
    int i, IKi;
    struct IKUnitContext *IKUnit_ptr;
    struct BoneUnitContext *BoneUnit_ptr;
    struct PMD_BoneInformation *fetch_ptr = srcPMD->BoneInfo;
    struct PMD_InverseKinematicsInformation *IKfetch_ptr = srcPMD->InverseKinematicsInfo;

    printf("srcPMD->InverseKinematicsInfoCount %d\n", srcPMD->InverseKinematicsInfoCount);
    vMemoryCtx->BoneMemory.IKUnitCount = srcPMD->InverseKinematicsInfoCount;
    vMemoryCtx->BoneMemory.IKUnit = (struct IKUnitContext *)malloc(sizeof(IKUnitContext) * srcPMD->InverseKinematicsInfoCount);
    vMemoryCtx->BoneMemory.BoneUnitCount = srcPMD->BoneInfoCount;
    vMemoryCtx->BoneMemory.BoneUnit = (struct BoneUnitContext *)malloc(sizeof(struct BoneUnitContext) *srcPMD->BoneInfoCount);
    if(vMemoryCtx->BoneMemory.BoneUnit == NULL)
        return 0;

    vMemoryCtx->BoneMemory.NullParentID = -1;


    BoneUnit_ptr = vMemoryCtx->BoneMemory.BoneUnit;
    for(i = 0 ; i < srcPMD->BoneInfoCount ; i++) {
        BoneUnit_ptr->nameKey_DKDR = fetch_ptr->NameKey_DKDR;
        BoneUnit_ptr->nameKey_djb2 = fetch_ptr->NameKey_djb2;
        BoneUnit_ptr->BoneVertex[0] = fetch_ptr->BoneAxis[0];
        BoneUnit_ptr->BoneVertex[1] = fetch_ptr->BoneAxis[1];
        BoneUnit_ptr->BoneVertex[2] = fetch_ptr->BoneAxis[2];
        BoneUnit_ptr->bID_Parent = fetch_ptr->bID_Parent;
        BoneUnit_ptr->DeformableLevel = 0;
        BoneUnit_ptr->BoneType = fetch_ptr->Type;
        BoneUnit_ptr++;
        fetch_ptr++;
    }

    IKUnit_ptr = vMemoryCtx->BoneMemory.IKUnit;
    for(i = 0 ; i < srcPMD->InverseKinematicsInfoCount ; i++) {
        IKUnit_ptr->MaxAngle        = IKfetch_ptr->MaxAngle;
        IKUnit_ptr->MaxIterations   = IKfetch_ptr->MaxIterations;
        IKUnit_ptr->ReachBoneID     = IKfetch_ptr->bID_Reach;
        IKUnit_ptr->TargetBoneID    = IKfetch_ptr->bID_End;
        IKUnit_ptr->IKChainCount    = IKfetch_ptr->ChainCount ;
        IKUnit_ptr->IKChain         = (struct IKChainUnit *)malloc(sizeof(struct IKChainUnit) * IKUnit_ptr->IKChainCount);

        for(IKi = 0 ; IKi < IKUnit_ptr->IKChainCount ; IKi++) {
            struct IKChainUnit *IKChainUnit_ptr = IKUnit_ptr->IKChain + IKi;
            IKChainUnit_ptr->isLimit        = 0;
            IKChainUnit_ptr->LinkBone       = *(IKfetch_ptr->Chain + IKi);
            IKChainUnit_ptr->LowerBound[0]  = 0;
            IKChainUnit_ptr->LowerBound[1]  = 0;
            IKChainUnit_ptr->LowerBound[2]  = 0;
            IKChainUnit_ptr->UpBound[0]     = 0;
            IKChainUnit_ptr->UpBound[1]     = 0;
            IKChainUnit_ptr->UpBound[2]     = 0;
        }
        IKfetch_ptr++;
        IKUnit_ptr++;
    }
}
// ---------------------------------------------------------------------------------
int VisualizationMemory::
generate_BoneMemory_Soruce_PMX(struct PolygonModeleXtendedContext *srcPMX)
{
    int i, IKi;
    struct IKUnitContext *IKUnit_ptr;
    struct BoneUnitContext *BoneUnit_ptr;
    struct PMX_BoneInformation *fetch_ptr = srcPMX->BoneInfo  ;

    printf("srcPMX->InverseKinematicsInfoCount %d\n", srcPMX->InverseKinematicsInfoCount);
    vMemoryCtx->BoneMemory.IKUnitCount = srcPMX->InverseKinematicsInfoCount;
    vMemoryCtx->BoneMemory.IKUnit = (struct IKUnitContext *)malloc(sizeof(IKUnitContext) * srcPMX->InverseKinematicsInfoCount);
    vMemoryCtx->BoneMemory.BoneUnitCount = srcPMX->BoneInfoCount;
    vMemoryCtx->BoneMemory.BoneUnit = (struct BoneUnitContext *)malloc(sizeof(struct BoneUnitContext) *srcPMX->BoneInfoCount);
    if(vMemoryCtx->BoneMemory.BoneUnit == NULL)
        return 0;

    if(srcPMX->GlobalParamSet.BoneIndexSize == 1) vMemoryCtx->BoneMemory.NullParentID = 0xFF;
    if(srcPMX->GlobalParamSet.BoneIndexSize == 2) vMemoryCtx->BoneMemory.NullParentID = 0xFFFF;
    if(srcPMX->GlobalParamSet.BoneIndexSize == 4) vMemoryCtx->BoneMemory.NullParentID = -1;

    IKUnit_ptr = vMemoryCtx->BoneMemory.IKUnit;
    BoneUnit_ptr = vMemoryCtx->BoneMemory.BoneUnit;
    for(i = 0 ; i < srcPMX->BoneInfoCount ; i++) {
        BoneUnit_ptr->nameKey_DKDR = fetch_ptr->nameKey_DKDR;
        BoneUnit_ptr->nameKey_djb2 = fetch_ptr->nameKey_djb2;
        BoneUnit_ptr->BoneVertex[0] = fetch_ptr->BoneVertex[0];
        BoneUnit_ptr->BoneVertex[1] = fetch_ptr->BoneVertex[1];
        BoneUnit_ptr->BoneVertex[2] = fetch_ptr->BoneVertex[2];
        BoneUnit_ptr->bID_Parent = fetch_ptr->bID_Parent;
        BoneUnit_ptr->DeformableLevel = fetch_ptr->DeformableLevel;
        BoneUnit_ptr->BoneType = fetch_ptr->BoneType;
        BoneUnit_ptr->BoneType_0x0001_BoneOffset[0]        = fetch_ptr->TypeParam_0x0001_BoneOffset[0];
        BoneUnit_ptr->BoneType_0x0001_BoneOffset[1]        = fetch_ptr->TypeParam_0x0001_BoneOffset[1];
        BoneUnit_ptr->BoneType_0x0001_BoneOffset[2]        = fetch_ptr->TypeParam_0x0001_BoneOffset[2];
        BoneUnit_ptr->BoneType_0x0001_BoneIndex            = fetch_ptr->TypeParam_0x0001_BoneIndex;
        BoneUnit_ptr->BoneType_0x0100_BoneIndex            = fetch_ptr->TypeParam_0x0100_BoneIndex;
        BoneUnit_ptr->BoneType_0x0100_Percent              = fetch_ptr->TypeParam_0x0100_Percent;
        BoneUnit_ptr->BoneType_0x0400_DirectonVertex[0]    = fetch_ptr->TypeParam_0x0400_DirectonVertex[0];
        BoneUnit_ptr->BoneType_0x0400_DirectonVertex[1]    = fetch_ptr->TypeParam_0x0400_DirectonVertex[1];
        BoneUnit_ptr->BoneType_0x0400_DirectonVertex[2]    = fetch_ptr->TypeParam_0x0400_DirectonVertex[2];
        BoneUnit_ptr->BoneType_0x0800_Vertex_X[0]          = fetch_ptr->TypeParam_0x0800_Vertex_X[0];
        BoneUnit_ptr->BoneType_0x0800_Vertex_X[1]          = fetch_ptr->TypeParam_0x0800_Vertex_X[1];
        BoneUnit_ptr->BoneType_0x0800_Vertex_X[2]          = fetch_ptr->TypeParam_0x0800_Vertex_X[2];
        BoneUnit_ptr->BoneType_0x0800_Vertex_Z[0]          = fetch_ptr->TypeParam_0x0800_Vertex_Z[0];
        BoneUnit_ptr->BoneType_0x0800_Vertex_Z[1]          = fetch_ptr->TypeParam_0x0800_Vertex_Z[1];
        BoneUnit_ptr->BoneType_0x0800_Vertex_Z[2]          = fetch_ptr->TypeParam_0x0800_Vertex_Z[2];
        BoneUnit_ptr->BoneType_0x2000_Key                  = fetch_ptr->TypeParam_0x2000_Key;
        if(fetch_ptr->BoneType & 0x0020) {
            IKUnit_ptr->MaxAngle        = fetch_ptr->TypeParam_0x0020_IK.MaxAngle;
            IKUnit_ptr->MaxIterations   = fetch_ptr->TypeParam_0x0020_IK.MaxIterations;
            IKUnit_ptr->ReachBoneID     = i;
            IKUnit_ptr->TargetBoneID    = fetch_ptr->TypeParam_0x0020_IK.TargetBoneID;
            IKUnit_ptr->IKChainCount    = fetch_ptr->TypeParam_0x0020_IK.IKChainCount;
            IKUnit_ptr->IKChain         = (struct IKChainUnit *)malloc(sizeof(struct IKChainUnit) * IKUnit_ptr->IKChainCount);

            for(IKi = 0 ; IKi < IKUnit_ptr->IKChainCount ; IKi++) {
                struct IKChainUnit *IKChainUnit_ptr = IKUnit_ptr->IKChain + IKi;
                struct PMX_InverseKinematicsChainContext *PMX_IKChainUnit_ptr = fetch_ptr->TypeParam_0x0020_IK.IKChain + IKi;
                IKChainUnit_ptr->isLimit        = PMX_IKChainUnit_ptr->isLimit;
                IKChainUnit_ptr->LinkBone       = PMX_IKChainUnit_ptr->LinkBone;
                IKChainUnit_ptr->LowerBound[0]  = PMX_IKChainUnit_ptr->LowerBound[0];
                IKChainUnit_ptr->LowerBound[1]  = PMX_IKChainUnit_ptr->LowerBound[1];
                IKChainUnit_ptr->LowerBound[2]  = PMX_IKChainUnit_ptr->LowerBound[2];
                IKChainUnit_ptr->UpBound[0]     = PMX_IKChainUnit_ptr->UpBound[0];
                IKChainUnit_ptr->UpBound[1]     = PMX_IKChainUnit_ptr->UpBound[1];
                IKChainUnit_ptr->UpBound[2]     = PMX_IKChainUnit_ptr->UpBound[2];
            }
            IKUnit_ptr++;
        }
        BoneUnit_ptr++;
        fetch_ptr++;
    }
}
// ---------------------------------------------------------------------------------
int VisualizationMemory::
generate_BoneMemory(void *srcData, int srcMode)
{
    int ret;
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
// ---------------------------------------------------------------------------------
/*static IplImage *load_Texture(char *fPath, int fPathLen, int fPathCode,
                              char *fName, int fNameLen, int fNameCode)
{
    int i;
    char FullPath[1024];
    char *FullPath_ptr = FullPath;

    switch(fPathCode) {
    case 0 :    //UTF16
        for(i =0; i < fPathLen; i++) *FullPath_ptr++ = *(fPath +i);
        break;
    case 1 :    //UTF8
        break;
    case 8:     //Ascii
        for(i =0; i < fPathLen; i++) *FullPath_ptr++ = *(fPath +i);
        break;
    default :
        break;
    }
    *FullPath_ptr++='/';

    switch(fNameCode) {
    case 0 :    //UTF16
        for(i =0; i < fNameLen; i+=2) {
            *FullPath_ptr++ = *(fName +i);
            printf("<%c>", *(fName +i));
        }
        printf("\n");
        break;
    case 1 :    //UTF8
        break;
    case 8:     //Ascii
        for(i =0; i < fNameLen; i++) *FullPath_ptr++ = *(fName +i);
        break;
    default :
        break;
    }
    *FullPath_ptr++='\0';

    printf("Load Texture in path <%s>\n", FullPath);
    IplImage *TextureImg = cvLoadImage(FullPath, CV_LOAD_IMAGE_COLOR);
    if(!TextureImg) {
        printf(pLIGHT_RED "Fatal" pNONE " : Read texture file: %s in path %s ... Failed\n" pNONE, fPath, fName);
        return NULL ;
    }
    printf(pLIGHT_GREEN "Info " pNONE ": Read texture file: %s ... Success\n", FullPath);
    printf("\tWidth : %d,  Height : %d, Channels : %d\n", TextureImg->width, TextureImg->height, TextureImg->nChannels);
    printf("\tImageSize : %d, WidthStep : %d\n", TextureImg->imageSize, TextureImg->widthStep );
    return TextureImg ;
}*/

int VisualizationMemory::
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
            unsigned char *fetch_ptr = src_ptr->TextureImg->RGB888;
            for(int y = 0 ; y < src_ptr->TextureImg->Height ; y++) {
                for(int x = 0 ; x < src_ptr->TextureImg->Width ; x++) {
                    *store_ptr++ = *(fetch_ptr+2);
                    *store_ptr++ = *(fetch_ptr+1);
                    *store_ptr++ = *(fetch_ptr);
                    *store_ptr++ = 255;
                    fetch_ptr +=3;
                }
            }
            Texture_ptr++;
        }
        src_ptr++;
    }
    return 1;
}

int VisualizationMemory::
generate_TextureMemory_Soruce_PMX(struct PolygonModeleXtendedContext *srcPMX)
{
    int i;
    //IplImage **Texture_ptr;
    struct PMX_TextureInformation *TextureInfo;
/*
    vMemoryCtx->TextureMemory.TextureCount = srcPMX->TextureInfoCount ;
    vMemoryCtx->TextureMemory.TextureImg  = (IplImage **)malloc(sizeof(IplImage *) *srcPMX->TextureInfoCount);
    if(vMemoryCtx->TextureMemory.TextureImg == NULL)
        return 0;

    Texture_ptr = vMemoryCtx->TextureMemory.TextureImg;
    for(i = 0 ; i < srcPMX->TextureInfoCount ; i++) {
        TextureInfo = (srcPMX->TextureInfo +i);
        *Texture_ptr = load_Texture("./PMX/", 6, 8,
                                    TextureInfo->name_US.Text, TextureInfo->name_US.Length, srcPMX->GlobalParamSet.TextEncoding);
        Texture_ptr++;
    }*/
    return 1;
}

int VisualizationMemory::
generate_TextureMemory(void *srcData, int srcMode)
{
    int ret;
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
// ---------------------------------------------------------------------------------
int VisualizationMemory::
generate_MaterialMemory_Soruce_PMD(struct PolygonModelDataContext *srcPMD)
{
    int i;
    struct MaterialUnitContext *MaterialUnit_ptr;
    struct PMD_MaterialInformation *fetch_ptr = srcPMD->MaterialInfo ;

    printf("srcPMD->MaterialInfoCount %d\n", srcPMD->MaterialInfoCount);
    vMemoryCtx->MaterialMemory.MaterialUnitCount = srcPMD->MaterialInfoCount;
    vMemoryCtx->MaterialMemory.MaterialUnit = (struct MaterialUnitContext *)malloc(sizeof(struct MaterialUnitContext) *srcPMD->MaterialInfoCount);
    if(vMemoryCtx->MaterialMemory.MaterialUnit == NULL)
        return 0;

    MaterialUnit_ptr= vMemoryCtx->MaterialMemory.MaterialUnit;
    for(i = 0 ; i < srcPMD->MaterialInfoCount ; i++) {
        MaterialUnit_ptr->JPnameKey_DKDR   = sutHash_DKDR(fetch_ptr->fname, strlen(fetch_ptr->fname));
        MaterialUnit_ptr->JPnameKey_djb2   = sutHash_djb2(fetch_ptr->fname, strlen(fetch_ptr->fname));
        MaterialUnit_ptr->Diffuse_RGBA[0]  = fetch_ptr->Diffuse_RGBA[0];
        MaterialUnit_ptr->Diffuse_RGBA[1]  = fetch_ptr->Diffuse_RGBA[1];
        MaterialUnit_ptr->Diffuse_RGBA[2]  = fetch_ptr->Diffuse_RGBA[2];
        MaterialUnit_ptr->Diffuse_RGBA[3]  = fetch_ptr->Diffuse_RGBA[3];
        MaterialUnit_ptr->Specular_RGB[0]  = fetch_ptr->Specular_RGB[0];
        MaterialUnit_ptr->Specular_RGB[1]  = fetch_ptr->Specular_RGB[1];
        MaterialUnit_ptr->Specular_RGB[2]  = fetch_ptr->Specular_RGB[2];
        MaterialUnit_ptr->Ambient_RGB[0]   = fetch_ptr->Ambient_RGB[0];
        MaterialUnit_ptr->Ambient_RGB[1]   = fetch_ptr->Ambient_RGB[1];
        MaterialUnit_ptr->Ambient_RGB[2]   = fetch_ptr->Ambient_RGB[2];
        MaterialUnit_ptr->Specularity      = fetch_ptr->Specularity;
        MaterialUnit_ptr->SurfaceCount     = fetch_ptr->EffectRange ;
        MaterialUnit_ptr->TextureIndex     = fetch_ptr->TextureIndex ;
        MaterialUnit_ptr++;
        fetch_ptr++;
    }
    return 1;
}
// ---------------------------------------------------------------------------------
int VisualizationMemory::
generate_MaterialMemory_Soruce_PMX(struct PolygonModeleXtendedContext *srcPMX)
{
    int i;
    struct MaterialUnitContext *MaterialUnit_ptr;
    struct PMX_MaterialInformation *fetch_ptr = srcPMX->MaterialInfo ;

    vMemoryCtx->MaterialMemory.MaterialUnitCount = srcPMX->MaterialInfoCount;
    vMemoryCtx->MaterialMemory.MaterialUnit = (struct MaterialUnitContext *)malloc(sizeof(struct MaterialUnitContext) *srcPMX->MaterialInfoCount);
    if(vMemoryCtx->MaterialMemory.MaterialUnit == NULL)
        return 0;

    MaterialUnit_ptr= vMemoryCtx->MaterialMemory.MaterialUnit;
    for(i = 0 ; i < srcPMX->MaterialInfoCount ; i++) {
        MaterialUnit_ptr->JPnameKey_DKDR   = sutHash_DKDR(fetch_ptr->name_JP.Text, fetch_ptr->name_JP.Length);
        MaterialUnit_ptr->JPnameKey_djb2   = sutHash_djb2(fetch_ptr->name_JP.Text, fetch_ptr->name_JP.Length);
        MaterialUnit_ptr->Diffuse_RGBA[0]  = fetch_ptr->Diffuse_RGBA[0];
        MaterialUnit_ptr->Diffuse_RGBA[1]  = fetch_ptr->Diffuse_RGBA[1];
        MaterialUnit_ptr->Diffuse_RGBA[2]  = fetch_ptr->Diffuse_RGBA[2];
        MaterialUnit_ptr->Diffuse_RGBA[3]  = fetch_ptr->Diffuse_RGBA[3];
        MaterialUnit_ptr->Specular_RGB[0]  = fetch_ptr->Specular_RGB[0];
        MaterialUnit_ptr->Specular_RGB[1]  = fetch_ptr->Specular_RGB[1];
        MaterialUnit_ptr->Specular_RGB[2]  = fetch_ptr->Specular_RGB[2];
        MaterialUnit_ptr->Ambient_RGB[0]   = fetch_ptr->Ambient_RGB[0];
        MaterialUnit_ptr->Ambient_RGB[1]   = fetch_ptr->Ambient_RGB[1];
        MaterialUnit_ptr->Ambient_RGB[2]   = fetch_ptr->Ambient_RGB[2];
        MaterialUnit_ptr->Specularity      = fetch_ptr->Specularity;
        MaterialUnit_ptr->bitFlag          = fetch_ptr->bitFlag;
        MaterialUnit_ptr->SurfaceCount     = fetch_ptr->SurfaceCount;
        MaterialUnit_ptr->Edge_RGBA[0]     = fetch_ptr->Edge_RGBA[0];
        MaterialUnit_ptr->Edge_RGBA[1]     = fetch_ptr->Edge_RGBA[1];
        MaterialUnit_ptr->Edge_RGBA[2]     = fetch_ptr->Edge_RGBA[2];
        MaterialUnit_ptr->Edge_RGBA[3]     = fetch_ptr->Edge_RGBA[3];
        MaterialUnit_ptr->bitFlag          = fetch_ptr->bitFlag;
        MaterialUnit_ptr->Edge_Scale       = fetch_ptr->Edge_Scale;
        MaterialUnit_ptr->TextureIndex     = fetch_ptr->TextureIndex;
        MaterialUnit_ptr->EnvironmentIndex = fetch_ptr->EnvironmentIndex;
        MaterialUnit_ptr->EnvironmentBlendMode = fetch_ptr->EnvironmentBlendMode;
        MaterialUnit_ptr->ToonReference    = fetch_ptr->ToonReference;
        MaterialUnit_ptr->ToonValue        = fetch_ptr->ToonValue;
        MaterialUnit_ptr->bitFlag          = fetch_ptr->bitFlag;
        MaterialUnit_ptr++;
        fetch_ptr++;
    }
    return 1;
}
// ---------------------------------------------------------------------------------
int VisualizationMemory::
generate_MaterialMemory(void *srcData, int srcMode)
{
    int ret;
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
// ---------------------------------------------------------------------------------
int VisualizationMemory::
generate_VertexMemory_Soruce_PMD(struct PolygonModelDataContext *srcPMD)
{
    int i;
    struct VertexUnitContext *VertexUnit_ptr;
    float *StoreVertex_ptr;
    float *StoreNoraml_ptr;
    float *StoreTexture_ptr;
    struct DeformType_BDEF2 *StoreBDEF2_ptr;
    struct PMD_VertexInformation *fetch_ptr = srcPMD->VertexInfo;

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
    fetch_ptr = srcPMD->VertexInfo;
    for(i = 0 ; i < srcPMD->VertexInfoCount ; i++) {
        VertexUnit_ptr->vID = i;
        VertexUnit_ptr->EdgeScale = 1 ;
        VertexUnit_ptr->DeformType = 1; // 1:BDEF2
        StoreBDEF2_ptr->BoneIndex_1 = fetch_ptr->BoneID_0;
        StoreBDEF2_ptr->BoneIndex_2 = fetch_ptr->BoneID_1;
        StoreBDEF2_ptr->Weight_1    = (float)fetch_ptr->BoneWeight / 100.0f;
        VertexUnit_ptr->DeformID    = StoreBDEF2_ptr - vMemoryCtx->VertexMemory.BDEF2;
        StoreBDEF2_ptr++;
        *StoreVertex_ptr++ = fetch_ptr->Vertex[0];
        *StoreVertex_ptr++ = fetch_ptr->Vertex[1];
        *StoreVertex_ptr++ = fetch_ptr->Vertex[2];
        *StoreNoraml_ptr++ = fetch_ptr->Normal[0];
        *StoreNoraml_ptr++ = fetch_ptr->Normal[1];
        *StoreNoraml_ptr++ = fetch_ptr->Normal[2];
        *StoreTexture_ptr++ = fabs(fetch_ptr->Texture[0]);
        *StoreTexture_ptr++ = fabs(fetch_ptr->Texture[1]) ;
        fetch_ptr++;
        VertexUnit_ptr++;
    }
    return 1;
}
// ---------------------------------------------------------------------------------
int VisualizationMemory::
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
    struct PMX_VertexInformation *fetch_ptr = srcPMX->VertexInfo;

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
        switch(fetch_ptr->DeformType) {
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
        fetch_ptr++;
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
    fetch_ptr = srcPMX->VertexInfo;
    for(i = 0 ; i < srcPMX->VertexInfoCount ; i++) {
        VertexUnit_ptr->vID = i;
        VertexUnit_ptr->EdgeScale = fetch_ptr->EdgeScale;
        VertexUnit_ptr->DeformType = fetch_ptr->DeformType;

        switch(fetch_ptr->DeformType) {
        case 0: // 0:BDEF1
            StoreBDEF1_ptr->BoneIndex = fetch_ptr->BDEF1.BoneIndex;
            VertexUnit_ptr->DeformID = StoreBDEF1_ptr - vMemoryCtx->VertexMemory.BDEF1;
            StoreBDEF1_ptr++;
            break;
        case 1: // 1:BDEF2
            StoreBDEF2_ptr->BoneIndex_1 = fetch_ptr->BDEF2.BoneIndex_1;
            StoreBDEF2_ptr->BoneIndex_2 = fetch_ptr->BDEF2.BoneIndex_2;
            StoreBDEF2_ptr->Weight_1    = fetch_ptr->BDEF2.Weight_1;
            VertexUnit_ptr->DeformID    = StoreBDEF2_ptr - vMemoryCtx->VertexMemory.BDEF2;
            StoreBDEF2_ptr++;
            break;
        case 2: // 2:BDEF4
            StoreBDEF4_ptr->BoneIndex_1 = fetch_ptr->BDEF4.BoneIndex_1;
            StoreBDEF4_ptr->BoneIndex_2 = fetch_ptr->BDEF4.BoneIndex_2;
            StoreBDEF4_ptr->BoneIndex_3 = fetch_ptr->BDEF4.BoneIndex_3;
            StoreBDEF4_ptr->BoneIndex_4 = fetch_ptr->BDEF4.BoneIndex_4;
            StoreBDEF4_ptr->Weight_1    = fetch_ptr->BDEF4.Weight_1;
            StoreBDEF4_ptr->Weight_2    = fetch_ptr->BDEF4.Weight_2;
            StoreBDEF4_ptr->Weight_3    = fetch_ptr->BDEF4.Weight_3;
            StoreBDEF4_ptr->Weight_4    = fetch_ptr->BDEF4.Weight_4;
            VertexUnit_ptr->DeformID = StoreBDEF4_ptr - vMemoryCtx->VertexMemory.BDEF4;
            StoreBDEF4_ptr++;
            break;
        case 3: // 3:SDEF
            StoreSDEF_ptr->BoneIndex_1 = fetch_ptr->SDEF.BoneIndex_1;
            StoreSDEF_ptr->BoneIndex_2 = fetch_ptr->SDEF.BoneIndex_2;
            StoreSDEF_ptr->C[0] = fetch_ptr->SDEF.C.x;
            StoreSDEF_ptr->C[1] = fetch_ptr->SDEF.C.y;
            StoreSDEF_ptr->C[2] = fetch_ptr->SDEF.C.z;
            StoreSDEF_ptr->R0[0] = fetch_ptr->SDEF.R0.x;
            StoreSDEF_ptr->R0[1] = fetch_ptr->SDEF.R0.y;
            StoreSDEF_ptr->R0[2] = fetch_ptr->SDEF.R0.z;
            StoreSDEF_ptr->R1[0] = fetch_ptr->SDEF.R1.x;
            StoreSDEF_ptr->R1[1] = fetch_ptr->SDEF.R1.y;
            StoreSDEF_ptr->R1[2] = fetch_ptr->SDEF.R1.z;
            VertexUnit_ptr->DeformID = StoreSDEF_ptr - vMemoryCtx->VertexMemory.SDEF;
            StoreSDEF_ptr++;
            break;
        }
        *StoreVertex_ptr++ = fetch_ptr->Vertex[0];
        *StoreVertex_ptr++ = fetch_ptr->Vertex[1];
        *StoreVertex_ptr++ = fetch_ptr->Vertex[2];
        *StoreNoraml_ptr++ = fetch_ptr->Normal[0];
        *StoreNoraml_ptr++ = fetch_ptr->Normal[1];
        *StoreNoraml_ptr++ = fetch_ptr->Normal[2];
        *StoreTexture_ptr++ = fabs(fetch_ptr->Texture[0]);
        *StoreTexture_ptr++ = fabs(fetch_ptr->Texture[1]) ;
        fetch_ptr++;
        VertexUnit_ptr++;
    }
    return 1;
}

// ---------------------------------------------------------------------------------
int VisualizationMemory::
generate_VertexMemory(void *srcData, int srcMode)
{
    int ret;
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
// ---------------------------------------------------------------------------------
int VisualizationMemory::
generate_IndexMemory_Soruce_PMD(struct PolygonModelDataContext *srcPMD)
{
    vMemoryCtx->IndexMemory.IndexUnitCount = srcPMD->IndexInfoCount ;
    vMemoryCtx->IndexMemory.IndexUnitTypeSize = 2;
    vMemoryCtx->IndexMemory.IndexUnit = (unsigned char *)malloc(srcPMD->IndexInfoCount * vMemoryCtx->IndexMemory.IndexUnitTypeSize);
    if(vMemoryCtx->IndexMemory.IndexUnit == NULL) return 0;
    memcpy(vMemoryCtx->IndexMemory.IndexUnit, srcPMD->IndexInfo, srcPMD->IndexInfoCount * vMemoryCtx->IndexMemory.IndexUnitTypeSize);
    return 1;
}

int VisualizationMemory::
generate_IndexMemory_Soruce_PMX(struct PolygonModeleXtendedContext *srcPMX)
{
    vMemoryCtx->IndexMemory.IndexUnitCount = srcPMX->IndexInfoCount;
    vMemoryCtx->IndexMemory.IndexUnitTypeSize = srcPMX->GlobalParamSet.VertexIndexSize;
    vMemoryCtx->IndexMemory.IndexUnit = (unsigned char *)malloc(srcPMX->IndexInfoCount * srcPMX->GlobalParamSet.VertexIndexSize);
    if(vMemoryCtx->IndexMemory.IndexUnit == NULL) return 0;
    memcpy(vMemoryCtx->IndexMemory.IndexUnit, srcPMX->IndexInfo, srcPMX->IndexInfoCount * srcPMX->GlobalParamSet.VertexIndexSize);
    return 1;
}
// ---------------------------------------------------------------------------------
int VisualizationMemory::
generate_IndexMemory(void *srcData, int srcMode)
{
    int ret;
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
// ---------------------------------------------------------------------------------
struct VisualizationMemoryContext *VisualizationMemory::
create(struct PolygonModelDataContext *srcPMD, unsigned int mElements)
{
    struct timeval pStart, pEnd;
    float uTime;

    printf("VisualizationMemory Create\n");
    if(srcPMD == NULL) return 0;

    gettimeofday(&pStart, NULL);
    vMemoryCtx = (struct VisualizationMemoryContext *)malloc(sizeof(struct VisualizationMemoryContext));
    memset(vMemoryCtx, 0, sizeof(struct VisualizationMemoryContext));

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
struct VisualizationMemoryContext *VisualizationMemory::
create(PolygonModeleXtendedContext *srcPMX, unsigned int mElements)
{
    struct timeval pStart, pEnd;
    float uTime;

    printf("VisualizationMemory Create\n");
    if(srcPMX == NULL) return 0;

    gettimeofday(&pStart, NULL);
    vMemoryCtx = (struct VisualizationMemoryContext *)malloc(sizeof(struct VisualizationMemoryContext));
    memset(vMemoryCtx, 0, sizeof(struct VisualizationMemoryContext));

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
void VisualizationMemory::
release(struct VisualizationMemoryContext *srcVM)
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
