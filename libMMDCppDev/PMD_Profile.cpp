#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <sutDataStruct.h>
#include <sutHash.h>
#include <sutBmp.h>
#include "PMD_Profile.h"
// ---------------------------------------------------------------------------------
#define pNONE "\033[m"
#define pRED "\033[0;32;31m"
#define pLIGHT_RED "\033[1;31m"
#define pGREEN "\033[0;32;32m"
#define pLIGHT_GREEN "\033[1;32m"
#define pBLUE "\033[0;32;34m"
#define pLIGHT_BLUE "\033[1;34m"
// ---------------------------------------------------------------------------------
using namespace MMDCppDev;
// ---------------------------------------------------------------------------------
void PMD_profile::parse_Boundary()
{
    int i;
    struct PMD_VertexInformation *VertexInfo_ptr = ProfileCtx_ptr->VertexInfo;
    struct ModelGeometryContext *ModelGeometryCtx_ptr = &ProfileCtx_ptr->ModelGeometryCtx;
    ModelGeometryCtx_ptr->RawModelBoundary[0][0] = VertexInfo_ptr->Vertex[0];
    ModelGeometryCtx_ptr->RawModelBoundary[0][1] = VertexInfo_ptr->Vertex[0];
    ModelGeometryCtx_ptr->RawModelBoundary[1][0] = VertexInfo_ptr->Vertex[1];
    ModelGeometryCtx_ptr->RawModelBoundary[1][1] = VertexInfo_ptr->Vertex[1];
    ModelGeometryCtx_ptr->RawModelBoundary[2][0] = VertexInfo_ptr->Vertex[2];
    ModelGeometryCtx_ptr->RawModelBoundary[2][1] = VertexInfo_ptr->Vertex[2];

    /* search boundary and calculate cneter */
    for(i = 0 ; i < ProfileCtx_ptr->VertexInfoCount ; i++) {
        float v0 = VertexInfo_ptr->Vertex[0];
        float v1 = VertexInfo_ptr->Vertex[1];
        float v2 = VertexInfo_ptr->Vertex[2];
        ModelGeometryCtx_ptr->RawModelCenter[0] += v0;
        ModelGeometryCtx_ptr->RawModelCenter[1] += v1;
        ModelGeometryCtx_ptr->RawModelCenter[2] += v2;
        if(ModelGeometryCtx_ptr->RawModelBoundary[0][0] < v0) ModelGeometryCtx_ptr->RawModelBoundary[0][0] =v0;
        if(ModelGeometryCtx_ptr->RawModelBoundary[0][1] > v0) ModelGeometryCtx_ptr->RawModelBoundary[0][1] =v0;
        if(ModelGeometryCtx_ptr->RawModelBoundary[1][0] < v1) ModelGeometryCtx_ptr->RawModelBoundary[1][0] =v1;
        if(ModelGeometryCtx_ptr->RawModelBoundary[1][1] > v1) ModelGeometryCtx_ptr->RawModelBoundary[1][1] =v1;
        if(ModelGeometryCtx_ptr->RawModelBoundary[2][0] < v2) ModelGeometryCtx_ptr->RawModelBoundary[2][0] =v2;
        if(ModelGeometryCtx_ptr->RawModelBoundary[2][1] > v2) ModelGeometryCtx_ptr->RawModelBoundary[2][1] =v2;
        VertexInfo_ptr++;
    }
    ModelGeometryCtx_ptr->RawModelSize[0] = ModelGeometryCtx_ptr->RawModelBoundary[0][0] - ModelGeometryCtx_ptr->RawModelBoundary[0][1];
    ModelGeometryCtx_ptr->RawModelSize[1] = ModelGeometryCtx_ptr->RawModelBoundary[1][0] - ModelGeometryCtx_ptr->RawModelBoundary[1][1];
    ModelGeometryCtx_ptr->RawModelSize[2] = ModelGeometryCtx_ptr->RawModelBoundary[2][0] - ModelGeometryCtx_ptr->RawModelBoundary[2][1];
    ModelGeometryCtx_ptr->RawModelCenter[0] /= (float)ProfileCtx_ptr->VertexInfoCount;
    ModelGeometryCtx_ptr->RawModelCenter[1] /= (float)ProfileCtx_ptr->VertexInfoCount;
    ModelGeometryCtx_ptr->RawModelCenter[2] /= (float)ProfileCtx_ptr->VertexInfoCount;

    printf("Model Size : Width : %5.5f, Height : %5.5f, Length : %5.5f\n", ModelGeometryCtx_ptr->RawModelSize[0], ModelGeometryCtx_ptr->RawModelSize[1] , ModelGeometryCtx_ptr->RawModelSize[2]);
}
// ---------------------------------------------------------------------------------
void PMD_profile::resize_Model()
{
    int i;
    struct ModelGeometryContext *ModelGeometryCtx_ptr = &ProfileCtx_ptr->ModelGeometryCtx;
    float ModelRatio_W = MaxModelSize[0] / ModelGeometryCtx_ptr->RawModelSize[0];  // Calculate the model size ratio of weight, height, length
    float ModelRatio_H = MaxModelSize[1] / ModelGeometryCtx_ptr->RawModelSize[1];
    float ModelRatio_L = MaxModelSize[2] / ModelGeometryCtx_ptr->RawModelSize[2];

    ModelGeometryCtx_ptr->ModelBoundScaleRate  = ModelRatio_W;
    if(ModelGeometryCtx_ptr->ModelBoundScaleRate > ModelRatio_H) ModelGeometryCtx_ptr->ModelBoundScaleRate = ModelRatio_H;
    if(ModelGeometryCtx_ptr->ModelBoundScaleRate > ModelRatio_L) ModelGeometryCtx_ptr->ModelBoundScaleRate = ModelRatio_L;

    for(i =0 ; i < ProfileCtx_ptr->VertexInfoCount ;i++) {
        struct PMD_VertexInformation *VertexInfo_ptr = ProfileCtx_ptr->VertexInfo +i;
        VertexInfo_ptr->Vertex[0] -= ModelGeometryCtx_ptr->RawModelCenter[0];
        VertexInfo_ptr->Vertex[1] -= ModelGeometryCtx_ptr->RawModelCenter[1] ;
        VertexInfo_ptr->Vertex[2] -= ModelGeometryCtx_ptr->RawModelCenter[2];
        VertexInfo_ptr->Vertex[0] *= ModelGeometryCtx_ptr->ModelBoundScaleRate;
        VertexInfo_ptr->Vertex[1] *= ModelGeometryCtx_ptr->ModelBoundScaleRate;
        VertexInfo_ptr->Vertex[2] *= ModelGeometryCtx_ptr->ModelBoundScaleRate;
    }
    for(i =0 ; i < ProfileCtx_ptr->BoneInfoCount ;i++) {
        struct PMD_BoneInformation *BoneInfo_ptr = ProfileCtx_ptr->BoneInfo +i;
        BoneInfo_ptr->BoneAxis[0] -= ModelGeometryCtx_ptr->RawModelCenter[0];
        BoneInfo_ptr->BoneAxis[1] -= ModelGeometryCtx_ptr->RawModelCenter[1];
        BoneInfo_ptr->BoneAxis[2] -= ModelGeometryCtx_ptr->RawModelCenter[2];
        BoneInfo_ptr->BoneAxis[0] *= ModelGeometryCtx_ptr->ModelBoundScaleRate;
        BoneInfo_ptr->BoneAxis[1] *= ModelGeometryCtx_ptr->ModelBoundScaleRate;
        BoneInfo_ptr->BoneAxis[2] *= ModelGeometryCtx_ptr->ModelBoundScaleRate;
    }
    ModelGeometryCtx_ptr->ModelCenter[0] = 0.0f;
    ModelGeometryCtx_ptr->ModelCenter[1] = 0.0f;
    ModelGeometryCtx_ptr->ModelCenter[2] = 0.0f;
}

/* ---------------------------------------------------------------------------------
 *  Pase PMD file inforamtion (Below)
 *      @include :  1. FileHeader
 *                  2. ModelHeader
 *                  3. VertexInformation
 *                  4. IndexInformation
 *                  5. MaterialInformation
 *                  6. BoneInformation
 *                  7. InverseKinematicsInformation
 *                  8. FaceMorphInformation
 *                  9. DisplayNameInformation
 */
// ---------------------------------------------------------------------------------
int PMD_profile::parse_FileHeader()
{
    printf(pLIGHT_GREEN "Parse File Header \n" pNONE);
    if(0 == fread(&ProfileCtx_ptr->FileHeaderInfo, sizeof(PMD_FileHeader), 1, fModel)) {
        return 0;
    }
    printf("%c%c%c , Version %f\n", ProfileCtx_ptr->FileHeaderInfo.PMD[0],
                                    ProfileCtx_ptr->FileHeaderInfo.PMD[1],
                                    ProfileCtx_ptr->FileHeaderInfo.PMD[2],
                                    ProfileCtx_ptr->FileHeaderInfo.version);
    return 1;
}
// ---------------------------------------------------------------------------------
int PMD_profile::parse_ModelHeader()
{
    printf(pLIGHT_GREEN "Parse Model Header \n" pNONE);
    if(0 == fread(&ProfileCtx_ptr->ModelHeaderInfo, sizeof(PMD_ModelHeader), 1, fModel)) {
        return 0;
    }
    printf("Model Name    : %s\n", ProfileCtx_ptr->ModelHeaderInfo.name);
    printf("Model Comment : %s\n", ProfileCtx_ptr->ModelHeaderInfo.comment);
    return 1;
}
// ---------------------------------------------------------------------------------
int PMD_profile::parse_VertexInformation()
{
    int ret ;

    printf(pLIGHT_GREEN "Parse Vertex information\n" pNONE);
    fread(&ProfileCtx_ptr->VertexInfoCount, sizeof(int), 1, fModel);
    ProfileCtx_ptr->VertexInfo = (PMD_VertexInformation *)malloc(sizeof(PMD_VertexInformation) * ProfileCtx_ptr->VertexInfoCount);
    if(ProfileCtx_ptr->VertexInfo ==NULL) {
        printf(pLIGHT_RED "Error : malloc failed\n" pNONE);
        return 0;
    }
    else {
        printf("VertexCount : %d\n", ProfileCtx_ptr->VertexInfoCount);
        ret = fread(ProfileCtx_ptr->VertexInfo, sizeof(PMD_VertexInformation), ProfileCtx_ptr->VertexInfoCount, fModel);
        if(ret != ProfileCtx_ptr->VertexInfoCount) {
            printf(pLIGHT_RED "Error : File size incomplete\n" pNONE);
            return 0;
        }
        /*for(int i =0 ; i <ProfileCtx_ptr->VertexInfoCount ;i++) {
            struct PMD_VertexInformation *VertexInfo_ptr = ProfileCtx_ptr->VertexInfo +i;
            VertexInfo_ptr->Vertex[2] = -VertexInfo_ptr->Vertex[2];
        }*/
//#define PROFILE_DBG
#ifdef PROFILE_DBG
        for(int i =0 ; i <VertexInfoCount ;i++) {
            struct PMD_VertexInformation *VertexInfo_ptr = VertexInfo +i;
            printf("V %5.5f %5.5f %5.5f ,", VertexInfo_ptr->Vertex[0], VertexInfo_ptr->Vertex[1], VertexInfo_ptr->Vertex[2]);
            printf("N %5.5f %5.5f %5.5f ,", VertexInfo_ptr->Normal[0], VertexInfo_ptr->Normal[1], VertexInfo_ptr->Normal[2]);
            printf("T %5.5f %5.5f ,\t", VertexInfo_ptr->Texture[0], VertexInfo_ptr->Texture[1]);
            printf("B %5d %5d,", VertexInfo_ptr->BoneID_0 , VertexInfo_ptr->BoneID_1);
            printf("BoneWeight %5d, EdgeFlag %5d\n", VertexInfo_ptr->BoneWeight , VertexInfo_ptr->EdgeFlag);
        }
#endif
#undef PROFILE_DBG
        return 1;
    }
}
// ---------------------------------------------------------------------------------
int PMD_profile::parse_IndexInformation()
{
    int ret ;

    printf(pLIGHT_GREEN "Parse Index information\n" pNONE);
    fread(&ProfileCtx_ptr->IndexInfoCount, sizeof(int), 1, fModel);
    ProfileCtx_ptr->IndexInfo = (unsigned short *)malloc(sizeof(unsigned short) * ProfileCtx_ptr->IndexInfoCount);
    if(ProfileCtx_ptr->IndexInfo ==NULL) {
        printf(pLIGHT_RED "Error : malloc failed\n" pNONE);
        return 0;
    }
    else {
        printf("IndexCount : %d\n", ProfileCtx_ptr->IndexInfoCount);
        ret = fread(ProfileCtx_ptr->IndexInfo, sizeof(unsigned short), ProfileCtx_ptr->IndexInfoCount, fModel);
        if(ret != ProfileCtx_ptr->IndexInfoCount) {
            printf(pLIGHT_RED "Error : File size incomplete\n" pNONE);
            return 0;
        }
#ifdef PROFILE_DBG
        for(int i =0 ; i < ProfileCtx_ptr->IndexInfoCount ;i+=3) {
            printf("%5d / %5d / %5d\n", ProfileCtx_ptr->IndexInfo[i], ProfileCtx_ptr->IndexInfo[i +1], ProfileCtx_ptr->IndexInfo[i +2]);
        }
#endif
        return 1;
    }
}
// ---------------------------------------------------------------------------------
int PMD_profile::parse_MaterialInformation(const char *FilePath)
{
    int mi ;
    int TextureIndex = 0;
    printf(pLIGHT_GREEN "Parse Material information\n" pNONE);
    fread(&ProfileCtx_ptr->MaterialInfoCount, sizeof(int), 1, fModel);
    ProfileCtx_ptr->MaterialInfo = (struct PMD_MaterialInformation *)malloc(sizeof(struct PMD_MaterialInformation) * ProfileCtx_ptr->MaterialInfoCount);

    if(ProfileCtx_ptr->MaterialInfo ==NULL) {
        printf(pLIGHT_RED "Error : malloc failed\n" pNONE);
        return 0;
    }
    else {
        printf("MaterialInfoCount %d\n", ProfileCtx_ptr->MaterialInfoCount);
        for(mi = 0 ; mi < ProfileCtx_ptr->MaterialInfoCount ; mi++) {
            struct PMD_MaterialInformation *MaterialInfo_ptr = ProfileCtx_ptr->MaterialInfo +mi;
            if(0 == fread(MaterialInfo_ptr, sizeof(struct PMD_MaterialInformation) - sizeof(PMD_TextureContext *) -sizeof(int), 1, fModel)) {
                printf(pLIGHT_RED "Error : File size incomplete\n" pNONE);
                return 0;
            }

            if(strlen(MaterialInfo_ptr->fname) >0) {
                MaterialInfo_ptr->TextureIndex = TextureIndex++;
                MaterialInfo_ptr->TextureImg = load_Texture(FilePath, MaterialInfo_ptr->fname);
                printf("MaterialInfo_ptr->TextureImg : %X\n", MaterialInfo_ptr->TextureImg);
                if(MaterialInfo_ptr->TextureImg != NULL) ProfileCtx_ptr->MaterialTextureCount++;
            }
            else {
                MaterialInfo_ptr->TextureImg = NULL;
                MaterialInfo_ptr->TextureIndex = 255;
            }

//#define PROFILE_DBG
#ifdef PROFILE_DBG
            printf("-------------------------------------------------------\n");
            printf("\tID          : %d\n", mi);
            printf("\tDiffuse     : R %5.5f, G %5.5f, B %5.5f ,A %5.5f\n", MaterialInfo_ptr->Diffuse_RGBA[0], MaterialInfo_ptr->Diffuse_RGBA[1], MaterialInfo_ptr->Diffuse_RGBA[0], MaterialInfo_ptr->Diffuse_RGBA[2]);
            printf("\tSpecularity : %5.5f\n", MaterialInfo_ptr->Specularity);
            printf("\tSpecular    : R %5.5f, G %5.5f, B %5.5f ,A %5.5f\n", MaterialInfo_ptr->Specular_RGB[0], MaterialInfo_ptr->Specular_RGB[1], MaterialInfo_ptr->Specular_RGB[0], MaterialInfo_ptr->Specular_RGB[2]);
            printf("\tAmbient     : R %5.5f, G %5.5f, B %5.5f ,A %5.5f\n", MaterialInfo_ptr->Diffuse_RGBA[0], MaterialInfo_ptr->Diffuse_RGBA[1], MaterialInfo_ptr->Diffuse_RGBA[0], MaterialInfo_ptr->Diffuse_RGBA[2]);
            printf("\tToonID      : %d\n", MaterialInfo_ptr->ToonID);
            printf("\tEdgeFlag    : %d\n", MaterialInfo_ptr->EdgeFlag);
            printf("\tEffectRange : %d\n", MaterialInfo_ptr->EffectRange);
            printf("\tFile name   : %s\n", MaterialInfo_ptr->fname);
#endif
#undef PROFILE_DBG
        }
    }

    return 1;
}
// ---------------------------------------------------------------------------------
int PMD_profile::parse_BoneInformation()
{
    int ret ;

    printf(pLIGHT_GREEN "Parse Bone information\n" pNONE);
    if(0 == fread(&ProfileCtx_ptr->BoneInfoCount, sizeof(unsigned short), 1, fModel)) {
        printf(pLIGHT_RED "Error : File size incomplete\n" pNONE);
        return 0;
    }

    ProfileCtx_ptr->BoneInfo = (struct PMD_BoneInformation *)malloc(sizeof(struct PMD_BoneInformation) * ProfileCtx_ptr->BoneInfoCount);
    if(ProfileCtx_ptr->BoneInfo ==NULL) {
        printf(pLIGHT_RED "Error : malloc failed\n" pNONE);
        return 0;
    }
    else {
        printf("BoneInfoCount : %d\n", ProfileCtx_ptr->BoneInfoCount);
        for(int i =0 ; i <ProfileCtx_ptr->BoneInfoCount ; i++) {
            struct PMD_BoneInformation * BondInfo_ptr = ProfileCtx_ptr->BoneInfo +i;
            ret = fread(BondInfo_ptr, sizeof(struct PMD_BoneInformation) - 2*sizeof(unsigned int), 1, fModel);
            if(ret ==0) {
                printf(pLIGHT_RED "Error : File size incomplete\n" pNONE);
                return 0;
            }
            //BondInfo_ptr->BoneAxis[2] = -BondInfo_ptr->BoneAxis[2] ;
            BondInfo_ptr->NameKey_djb2 = sutHash_djb2(BondInfo_ptr->Name_shift_jis, strlen(BondInfo_ptr->Name_shift_jis));
            BondInfo_ptr->NameKey_DKDR = sutHash_DKDR(BondInfo_ptr->Name_shift_jis, strlen(BondInfo_ptr->Name_shift_jis));
        }
        /*ret = fread(ProfileCtx_ptr->BoneInfo, sizeof(struct PMD_BoneInformation), ProfileCtx_ptr->BoneInfoCount, fModel);
        if(ret != ProfileCtx_ptr->BoneInfoCount) {
            printf(pLIGHT_RED "Error : File size incomplete\n" pNONE);
            return 0;
        }*/
#define PROFILE_DBG
#ifdef PROFILE_DBG
        for(int i =0 ; i <ProfileCtx_ptr->BoneInfoCount ; i++) {
            struct PMD_BoneInformation *BoneInfo_ptr = ProfileCtx_ptr->BoneInfo +i;
            printf("-------------------------------------------------------\n");
            printf(pLIGHT_GREEN "\tID                : %d\n" pNONE, i);
            printf("\tName              : %s\n", BoneInfo_ptr->Name_shift_jis );
            printf("\tKey               : %d, %d\n", BoneInfo_ptr->NameKey_djb2, BoneInfo_ptr->NameKey_DKDR);
            printf("\tBoneID for Parent : %hd\n", BoneInfo_ptr->bID_Parent);
            printf("\tBoneID for Child  : %hd\n", BoneInfo_ptr->bID_Child);
            printf("\tType              : %d\n", BoneInfo_ptr->Type);
            printf("\tTargetBone        : %d\n", BoneInfo_ptr->TargetBone);
            printf("\tAxis              : %5.5f, %5.5f, %5.5f\n", BoneInfo_ptr->BoneAxis[0], BoneInfo_ptr->BoneAxis[1], BoneInfo_ptr->BoneAxis[2]);
        }
#endif
#undef PROFILE_DBG
        return 1;
    }
}
/* -------------------------------------------------------------------------------- */
int PMD_profile::parse_InverseKinematicsInformation()
{
    int ret ;

    printf(pLIGHT_GREEN "Parse Inverse Kinematics information\n" pNONE);
    fread(&ProfileCtx_ptr->InverseKinematicsInfoCount, sizeof(unsigned short), 1, fModel);
    ProfileCtx_ptr->InverseKinematicsInfo = (struct PMD_InverseKinematicsInformation *)malloc(sizeof(struct PMD_InverseKinematicsInformation) * ProfileCtx_ptr->InverseKinematicsInfoCount);

    if(ProfileCtx_ptr->InverseKinematicsInfo ==NULL) {
        printf("Error : malloc failed\n");
        return 0;
    }
    else {
        printf("InverseKinematicsInfoCount : %d\n", ProfileCtx_ptr->InverseKinematicsInfoCount);
        for(int i =0 ; i < ProfileCtx_ptr->InverseKinematicsInfoCount ; i++) {
            struct PMD_InverseKinematicsInformation *InverseKinematicsInfo_ptr = ProfileCtx_ptr->InverseKinematicsInfo +i;

            ret = fread(InverseKinematicsInfo_ptr, sizeof(struct PMD_InverseKinematicsInformation) - sizeof(short *), 1, fModel);
            if(ret == 0) {
                printf("Error : fread failed\n");
                return 0;
            }
            InverseKinematicsInfo_ptr->Chain = (short *)malloc(sizeof(short) * InverseKinematicsInfo_ptr->ChainCount );
            ret = fread(InverseKinematicsInfo_ptr->Chain, sizeof(short), InverseKinematicsInfo_ptr->ChainCount, fModel);
            if(ret == 0 || ret != InverseKinematicsInfo_ptr->ChainCount) {
                printf("Error : fread failed\n");
                return 0;
            }
//#define PROFILE_DBG
#ifdef PROFILE_DBG
            printf("-------------------------------------------------------\n");
            printf("\tID            : %d\n", i);
            printf("\tbID_Reach     : %d\n", InverseKinematicsInfo_ptr->bID_Reach );
            printf("\tbID_End       : %d\n", InverseKinematicsInfo_ptr->bID_End );
            printf("\tChainCount    : %d\n", InverseKinematicsInfo_ptr->ChainCount );
            printf("\tMaxIterations : %d\n", InverseKinematicsInfo_ptr->MaxIterations );
            printf("\tMaxAngle      : %f\n", InverseKinematicsInfo_ptr->MaxAngle );
            printf("\tChain         : ");

            for(int j =0; j < InverseKinematicsInfo_ptr->ChainCount; j++) {
                printf("%d ,", *(InverseKinematicsInfo_ptr->Chain +j));
            }
            printf("\n");
#endif
#undef PROFILE_DBG
        }

        return 1;
    }
}
// ---------------------------------------------------------------------------------
int PMD_profile::parse_FaceMorphInformation()
{
    int ret ;

    printf(pLIGHT_GREEN "Parse Face Morph information\n" pNONE);
    fread(&ProfileCtx_ptr->FaceMorphInfoCount, sizeof(unsigned short), 1, fModel);
    ProfileCtx_ptr->FaceMorphInfo = (struct PMD_FaceMorphInformation *)malloc(sizeof(struct PMD_FaceMorphInformation) * ProfileCtx_ptr->FaceMorphInfoCount);
    if(ProfileCtx_ptr->FaceMorphInfo ==NULL) {
        printf("Error : malloc failed\n");
        return 0;
    }
    else {
        printf("FaceMorphInfoCount : %d\n", ProfileCtx_ptr->FaceMorphInfoCount);
        for(int i =0 ; i <ProfileCtx_ptr->FaceMorphInfoCount ; i++) {
            struct PMD_FaceMorphInformation *FaceMorphInfo_ptr = ProfileCtx_ptr->FaceMorphInfo +i;

            ret = fread(FaceMorphInfo_ptr, sizeof(struct PMD_FaceMorphInformation) - sizeof(struct PMD_FaceVertexList *), 1, fModel);
            if(ret == 0) {
                printf("Error : fread failed\n");
                return 0;
            }

            FaceMorphInfo_ptr->EffectList  = (struct PMD_FaceVertexList *)malloc(sizeof(struct PMD_FaceVertexList) * FaceMorphInfo_ptr->EffectCount );
            ret = fread(FaceMorphInfo_ptr->EffectList, sizeof(struct PMD_FaceVertexList), FaceMorphInfo_ptr->EffectCount, fModel);
            if(ret == 0 || ret != (int)FaceMorphInfo_ptr->EffectCount) {
                printf("Error : fread failed\n");
                return 0;
            }
#ifdef PROFILE_DBG
            printf("-------------------------------------------------------\n");
            printf("\tID            : %d\n", i);
            printf("\tName          : %s\n", FaceMorphInfo_ptr->Name_shift_jis);
            printf("\tEffectCount   : %d\n", FaceMorphInfo_ptr->EffectCount );
            printf("\tType          : %d\n", FaceMorphInfo_ptr->Type);
            printf("\tVertexList    : \n");

            for(int j =0 ; j < FaceMorphInfo_ptr->EffectCount ; j++) {
                printf("\t\t%5d ,%5.5f , %5.5f, %5.5f\n", FaceMorphInfo_ptr->EffectList[i].Index,
                                                          FaceMorphInfo_ptr->EffectList[i].Vertex[0],
                                                          FaceMorphInfo_ptr->EffectList[i].Vertex[1],
                                                          FaceMorphInfo_ptr->EffectList[i].Vertex[2]);
            }
            printf("\n");
#endif
        }

        return 1;
    }
}
// ---------------------------------------------------------------------------------
int PMD_profile::parse_DisplayNameInformation()
{
    return 1;
}
/* ---------------------------------------------------------------------------------
 *  Alloc and reset a new PMDContext for return
 *      @param ID : Specify id.
 *      @param MaxWidth, MaxHeight, MaxLength : The boundary size of width, height, length.
 */
struct PolygonModelDataContext *PMD_profile::genContext(int ID, float MaxWidth, float MaxHeight, float MaxLength)
{
    struct PolygonModelDataContext *Ctx = (struct PolygonModelDataContext *)malloc(sizeof(struct PolygonModelDataContext));

    if(Ctx == NULL) return NULL;
    this->fModel                    = NULL;
    Ctx->VertexInfo                 = NULL;
    Ctx->MaterialInfo               = NULL;
    Ctx->IndexInfo                  = NULL;
    Ctx->BoneInfo                   = NULL;
    Ctx->InverseKinematicsInfo      = NULL;
    Ctx->FaceMorphInfo              = NULL;
    Ctx->CtxID                      = ID;
    Ctx->VertexInfoCount            = 0;
    Ctx->IndexInfoCount             = 0;
    Ctx->MaterialInfoCount          = 0;
    Ctx->MaterialTextureCount       = 0;
    Ctx->BoneInfoCount              = 0;
    Ctx->InverseKinematicsInfoCount = 0;
    Ctx->FaceMorphInfoCount         = 0;
    memset(&Ctx->ModelGeometryCtx, 0, sizeof(ModelGeometryContext));
    MaxModelSize[0] = MaxWidth;
    MaxModelSize[1] = MaxHeight;
    MaxModelSize[2] = MaxLength;
    return Ctx;
}
// ---------------------------------------------------------------------------------
void PMD_profile::release(PolygonModelDataContext *relCtx)
{
    int i;
    if(NULL == relCtx) return;
    if(NULL != relCtx->VertexInfo )     free(relCtx->VertexInfo);
    if(NULL != relCtx->MaterialInfo) {
        free(relCtx->MaterialInfo);
    }
    if(NULL != relCtx->IndexInfo)       free(relCtx->IndexInfo);
    if(NULL != relCtx->BoneInfo)        free(relCtx->BoneInfo);
    if(NULL != relCtx->InverseKinematicsInfo) {
        for(i = 0 ; i < relCtx->InverseKinematicsInfoCount ; i++) {
            if(NULL != (relCtx->InverseKinematicsInfo +i)->Chain) free((relCtx->InverseKinematicsInfo +i)->Chain);
        }
        free(relCtx->InverseKinematicsInfo);
    }
    if(NULL != relCtx->FaceMorphInfo) {
        for(i = 0 ; i < relCtx->FaceMorphInfoCount ; i++) {
            if(NULL != (relCtx->FaceMorphInfo +i)->EffectList) free((relCtx->FaceMorphInfo +i)->EffectList);
        }
        free(relCtx->FaceMorphInfo);
    }
}
/* ---------------------------------------------------------------------------------
 * Opoen the .obj file
 *      @param FilePath : The path of target .obj file.
 *      @param TexturePath : The path of target material file.
*/
int PMD_profile::load(struct PolygonModelDataContext *PMDCtx ,const char *FilePath, const char *FileName, const char *ToonTexturePath)
{
    int i;
    char *FullPath = (char *)malloc(strlen(FilePath) + strlen(FilePath) +1);
    if(FullPath == NULL) goto ANY_ERROR;
    strcpy(FullPath, FilePath);
    strcat(FullPath, "/");
    strcat(FullPath, FileName);

    fModel= fopen(FullPath, "r");
    if((fModel != NULL )&& (PMDCtx != NULL)) {
        free(FullPath);
        ProfileCtx_ptr = PMDCtx;
        /*for(i = 0 ; i < DefaultToonTextureSize ; i++) {
            ToonTextureCtx[i].TextureImg = cvLoadImage(ToonTexturePath, CV_LOAD_IMAGE_COLOR);
            if(!ToonTextureCtx[i].TextureImg) {
                printf("Info : Read texture file: %s ... Failed\n", ToonTexturePath);
                return 0 ;
            }
            printf("Info : Read texture file: %s ... Success\n", TexturePath);
            printf("[%d %d %d] [imageSize %d], [widthStep %d]\n", ObjTextureImg->width, ObjTextureImg->height,\
                           ObjTextureImg->nChannels, ObjTextureImg->imageSize, ObjTextureImg->widthStep );
        }*/

        /* parse PMD model data */
        if(!parse_FileHeader()) goto ANY_ERROR;
        if(!parse_ModelHeader()) goto ANY_ERROR;
        if(!parse_VertexInformation()) goto ANY_ERROR;
        if(!parse_IndexInformation()) goto ANY_ERROR;
        if(!parse_MaterialInformation(FilePath)) goto ANY_ERROR;
        if(!parse_BoneInformation()) goto ANY_ERROR;
        if(!parse_InverseKinematicsInformation()) goto ANY_ERROR;
        if(!parse_FaceMorphInformation()) goto ANY_ERROR;
        if(!parse_DisplayNameInformation()) goto ANY_ERROR;
        fclose(fModel);

        /* paser extend data */
        parse_Boundary();
        resize_Model();

        printf ("Info : Read object file : %s ... Success\n", FullPath) ;
        return 1 ;
    }

ANY_ERROR:
    printf ("Info : Read object file : %s ... Failed\n", FullPath) ;
    release(PMDCtx);
    return 0 ;
}
// ---------------------------------------------------------------------------------
PMD_TextureContext *PMD_profile::load_Texture(const char *fPath, const char *fName)
{
    BMP_file Texture;
    Texture.read(fPath, fName);

    PMD_TextureContext *TextureImg = (PMD_TextureContext *)malloc(sizeof(PMD_TextureContext));

    TextureImg->Width = Texture.get_info_header()->width;
    TextureImg->Height = Texture.get_info_header()->height;
    TextureImg->ByteLen = TextureImg->Width * TextureImg->Height * 3;
    TextureImg->RGB888 = (unsigned char *)malloc(TextureImg->ByteLen);

    memcpy(TextureImg->RGB888, Texture.get_color_ptr(), TextureImg->ByteLen);

   /* char *FullPath = (char *)malloc(strlen(fPath) + strlen(fName) +1);

    if(FullPath ==NULL) return NULL;
    strcpy(FullPath, fPath);
    strcat(FullPath, "/");
    strcat(FullPath, fName);

    PMD_TextureContext *TextureImg = cvLoadImage(FullPath, CV_LOAD_IMAGE_COLOR);
    free(FullPath);
    if(!TextureImg) {
        printf(pLIGHT_RED "\t\tFatal" pNONE " : Read texture file: %s ... Failed\n" pNONE, FullPath);
        return NULL ;
    }
    printf(pLIGHT_GREEN "\t\tInfo " pNONE ": Read texture file: %s ... Success\n", FullPath);
    printf("\t\t\tWidth : %d,  Height : %d, Channels : %d\n", TextureImg->width, TextureImg->height, TextureImg->nChannels);
    printf("\t\t\tImageSize : %d, WidthStep : %d\n", TextureImg->imageSize, TextureImg->widthStep );
*/
    return TextureImg ;
}
