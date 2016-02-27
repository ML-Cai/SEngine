#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <sys/time.h>
#include "PMX_Profile.h"
#include "sutDataStruct.h"
#include "sutHash.h"
#include "sutBmp.h"
#include <sutCharacterSetConvert.h>
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
void PMX_profile::parse_Boundary()
{
    int i;
    struct PMX_VertexInformation *VertexInfo_ptr = ProfileCtx_ptr->VertexInfo;
    struct PMX_ModelGeometryContext *ModelGeometryCtx_ptr = &ProfileCtx_ptr->ModelGeometryCtx;

    printf(pLIGHT_GREEN "Parse Model Boundary\n" pNONE);
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

    if(ModelGeometryCtx_ptr->RawModelSize[0] < 0.0001f) ModelGeometryCtx_ptr->RawModelSize[0] =1.0f;
    if(ModelGeometryCtx_ptr->RawModelSize[1] < 0.0001f) ModelGeometryCtx_ptr->RawModelSize[1] =1.0f;
    if(ModelGeometryCtx_ptr->RawModelSize[2] < 0.0001f) ModelGeometryCtx_ptr->RawModelSize[2] =1.0f;

    printf("Model Size : Width %5.5f, Height %5.5f, Length %5.5f\n"
           "Model Center : %5.5f, %5.5f, %5.5f\n",
           ModelGeometryCtx_ptr->RawModelSize[0],
           ModelGeometryCtx_ptr->RawModelSize[1],
           ModelGeometryCtx_ptr->RawModelSize[2],
           ModelGeometryCtx_ptr->RawModelCenter[0],
           ModelGeometryCtx_ptr->RawModelCenter[1],
           ModelGeometryCtx_ptr->RawModelCenter[2]);
}
// ---------------------------------------------------------------------------------
void PMX_profile::resize_Model()
{
    int i;
    struct PMX_ModelGeometryContext *ModelGeometryCtx_ptr = &ProfileCtx_ptr->ModelGeometryCtx;
    float ModelRatio_W = MaxModelSize[0] / ModelGeometryCtx_ptr->RawModelSize[0];  // Calculate the model size ratio of weight, height, length
    float ModelRatio_H = MaxModelSize[1] / ModelGeometryCtx_ptr->RawModelSize[1];
    float ModelRatio_L = MaxModelSize[2] / ModelGeometryCtx_ptr->RawModelSize[2];

    printf(pLIGHT_GREEN "Resize Model\n" pNONE);
    ModelGeometryCtx_ptr->ModelBoundScaleRate  = ModelRatio_W;
    if(ModelGeometryCtx_ptr->ModelBoundScaleRate > ModelRatio_H) ModelGeometryCtx_ptr->ModelBoundScaleRate = ModelRatio_H;
    if(ModelGeometryCtx_ptr->ModelBoundScaleRate > ModelRatio_L) ModelGeometryCtx_ptr->ModelBoundScaleRate = ModelRatio_L;
    printf("Model Scale Rate : %lf\n", ModelGeometryCtx_ptr->ModelBoundScaleRate);

    for(i =0 ; i < ProfileCtx_ptr->VertexInfoCount ;i++) {
        struct PMX_VertexInformation *VertexInfo_ptr = ProfileCtx_ptr->VertexInfo +i;
        VertexInfo_ptr->Vertex[0] -= ModelGeometryCtx_ptr->RawModelCenter[0];
        VertexInfo_ptr->Vertex[1] -= ModelGeometryCtx_ptr->RawModelCenter[1] ;
        VertexInfo_ptr->Vertex[2] -= ModelGeometryCtx_ptr->RawModelCenter[2];
        VertexInfo_ptr->Vertex[0] *= ModelGeometryCtx_ptr->ModelBoundScaleRate;
        VertexInfo_ptr->Vertex[1] *= ModelGeometryCtx_ptr->ModelBoundScaleRate;
        VertexInfo_ptr->Vertex[2] *= ModelGeometryCtx_ptr->ModelBoundScaleRate;
    }
    for(i =0 ; i < ProfileCtx_ptr->BoneInfoCount ;i++) {
        struct PMX_BoneInformation *BoneInfo_ptr = ProfileCtx_ptr->BoneInfo +i;
        BoneInfo_ptr->BoneVertex[0] -= ModelGeometryCtx_ptr->RawModelCenter[0];
        BoneInfo_ptr->BoneVertex[1] -= ModelGeometryCtx_ptr->RawModelCenter[1];
        BoneInfo_ptr->BoneVertex[2] -= ModelGeometryCtx_ptr->RawModelCenter[2];
        BoneInfo_ptr->BoneVertex[0] *= ModelGeometryCtx_ptr->ModelBoundScaleRate;
        BoneInfo_ptr->BoneVertex[1] *= ModelGeometryCtx_ptr->ModelBoundScaleRate;
        BoneInfo_ptr->BoneVertex[2] *= ModelGeometryCtx_ptr->ModelBoundScaleRate;
    }
    ModelGeometryCtx_ptr->ModelCenter[0] = 0.0f;
    ModelGeometryCtx_ptr->ModelCenter[1] = 0.0f;
    ModelGeometryCtx_ptr->ModelCenter[2] = 0.0f;
}
// ---------------------------------------------------------------------------------
int PMX_profile::Read_PMX_Text(FILE *fptr, struct PMX_Text *dstText)
{
    if(0 == fread(&(dstText->Length), sizeof(int), 1, fptr)) return 0;
    if(dstText->Length ==0) {   // maybe no text
        dstText->text_UTF8 =NULL;
        return 1;
    }

    if(ProfileCtx_ptr->GlobalParamSet.TextEncoding != 1) {  // != UTF8
        char *UTF16 = (char *)malloc(dstText->Length +1);
        UTF16[dstText->Length] = '\0';
        dstText->text_UTF8 = (char *)malloc(dstText->Length *2);
        memset(dstText->text_UTF8, 0, dstText->Length *2);
        if(0 == fread(UTF16 , dstText->Length, 1, fptr)) return 0;
        sutUTF16_to_UTF8(UTF16, dstText->Length, dstText->text_UTF8, dstText->Length *2);
        dstText->Length *=2;
        free(UTF16);
    }
    else {
        dstText->text_UTF8 = (char *)malloc(dstText->Length +1);
        dstText->text_UTF8[dstText->Length] = '\0';
        if(0 == fread(dstText->text_UTF8 , dstText->Length, 1, fptr)) return 0;
    }
    return 1;
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
 *                  8. MorphInformation
 *                  9. DisplayNameInformation
 */
// ---------------------------------------------------------------------------------
int PMX_profile::parse_FileHeader()
{
    int i;
    struct PMX_Text *TextSet[]= {&ProfileCtx_ptr->FileHeaderInfo.name_JP,
                                 &ProfileCtx_ptr->FileHeaderInfo.name_UTF,
                                 &ProfileCtx_ptr->FileHeaderInfo.comment_JP,
                                 &ProfileCtx_ptr->FileHeaderInfo.comment_UTF};

    printf(pLIGHT_GREEN "Parse File Header \n" pNONE);
    if(0 == fread(&ProfileCtx_ptr->FileHeaderInfo, sizeof(char) *4 + sizeof(float), 1, fModel))
        goto ANY_ERROR;
    if(0 == fread(&ProfileCtx_ptr->GlobalParamSet.GlobalParamCount , sizeof(unsigned char), 1, fModel))
        goto ANY_ERROR;
    if(0 == fread(&ProfileCtx_ptr->GlobalParamSet.TextEncoding, sizeof(unsigned char) *ProfileCtx_ptr->GlobalParamSet.GlobalParamCount, 1, fModel))
        goto ANY_ERROR;

    for(i = 0 ; i < 4; i++) {
        if(0 == Read_PMX_Text(fModel, TextSet[i])) goto ANY_ERROR;
    }
#define PROFILE_DBG
#ifdef PROFILE_DBG
    int si;
    printf("%c%c%c%c , Version %f\n", ProfileCtx_ptr->FileHeaderInfo.PMX[0],
                                      ProfileCtx_ptr->FileHeaderInfo.PMX[1],
                                      ProfileCtx_ptr->FileHeaderInfo.PMX[2],
                                      ProfileCtx_ptr->FileHeaderInfo.PMX[3],
                                      ProfileCtx_ptr->FileHeaderInfo.version);
    printf("Global Param Count : %d\n", ProfileCtx_ptr->GlobalParamSet.GlobalParamCount);
    for(i = 0; i< ProfileCtx_ptr->GlobalParamSet.GlobalParamCount ; i++) {
        printf("GlobalParamSet [%2d] %d\n", i, *((unsigned char *)&ProfileCtx_ptr->GlobalParamSet.TextEncoding + i));
    }
    for(i = 0 ; i < 4; i++) {
        struct PMX_Text *TextSet_ptr = TextSet[i];
        printf("----------------------------------------------------\n");
        printf("Text Length [%d] :\n",TextSet_ptr->Length);
        for(si = 0 ; si <TextSet_ptr->Length ; si++)
            printf("%c", TextSet_ptr->text_UTF8[si]);
        printf("\n");
    }
#endif
#undef PROFILE_DBG
    return 1;

ANY_ERROR:
    return 0;
}
// ---------------------------------------------------------------------------------
int PMX_profile::parse_VertexInformation()
{
    const int sizeof_DeformType[] ={
        ProfileCtx_ptr->GlobalParamSet.BoneIndexSize,                       // BDEF1
        ProfileCtx_ptr->GlobalParamSet.BoneIndexSize *2 +sizeof(float),     // BDEF2
        ProfileCtx_ptr->GlobalParamSet.BoneIndexSize *4 +sizeof(float) *4,  // BDEF4
        ProfileCtx_ptr->GlobalParamSet.BoneIndexSize *2 +sizeof(float) +sizeof(pmxVec3) *3};   // SDEF

    printf(pLIGHT_GREEN "Parse Vertex information\n" pNONE);

    fread(&ProfileCtx_ptr->VertexInfoCount, sizeof(int), 1, fModel);
    ProfileCtx_ptr->VertexInfo = (PMX_VertexInformation *)malloc(sizeof(PMX_VertexInformation) * ProfileCtx_ptr->VertexInfoCount);
    if(ProfileCtx_ptr->VertexInfo ==NULL) {
        printf(pLIGHT_RED "Error : malloc failed\n" pNONE);
        return 0;
    }
    else {
        memset(ProfileCtx_ptr->VertexInfo, 0, sizeof(PMX_VertexInformation) * ProfileCtx_ptr->VertexInfoCount);
        printf("VertexCount : %d\n", ProfileCtx_ptr->VertexInfoCount);
        for(int i =0 ; i <ProfileCtx_ptr->VertexInfoCount ;i++) {
            struct PMX_VertexInformation *VertexInfo_ptr = ProfileCtx_ptr->VertexInfo +i;

            fread(VertexInfo_ptr, sizeof(float) * (3 + 3 + 2), 1, fModel);  //3 vertex, 3 normal, 2 texcoord.
            if(ProfileCtx_ptr->GlobalParamSet.AdditionalVec4Count >0)
                fread(VertexInfo_ptr->Additional_UV, sizeof(float) *4 *ProfileCtx_ptr->GlobalParamSet.AdditionalVec4Count, 1, fModel);

            fread(&VertexInfo_ptr->DeformType , 1, 1, fModel);
            switch(VertexInfo_ptr->DeformType) {
            case 0: //BDEF1
                fread(&VertexInfo_ptr->BDEF1.BoneIndex , sizeof_DeformType[VertexInfo_ptr->DeformType], 1, fModel);
                break;
            case 1: //BDEF2
                fread(&VertexInfo_ptr->BDEF2.BoneIndex_1, ProfileCtx_ptr->GlobalParamSet.BoneIndexSize, 1, fModel);
                fread(&VertexInfo_ptr->BDEF2.BoneIndex_2, ProfileCtx_ptr->GlobalParamSet.BoneIndexSize, 1, fModel);
                fread(&VertexInfo_ptr->BDEF2.Weight_1, sizeof(float), 1, fModel);
                break;
            case 2: // BDEF4
                fread(&VertexInfo_ptr->BDEF4.BoneIndex_1, ProfileCtx_ptr->GlobalParamSet.BoneIndexSize, 1, fModel);
                fread(&VertexInfo_ptr->BDEF4.BoneIndex_2, ProfileCtx_ptr->GlobalParamSet.BoneIndexSize, 1, fModel);
                fread(&VertexInfo_ptr->BDEF4.BoneIndex_3, ProfileCtx_ptr->GlobalParamSet.BoneIndexSize, 1, fModel);
                fread(&VertexInfo_ptr->BDEF4.BoneIndex_4, ProfileCtx_ptr->GlobalParamSet.BoneIndexSize, 1, fModel);
                fread(&VertexInfo_ptr->BDEF4.Weight_1 , sizeof(float), 4, fModel);
                break;
            case 3: //SDEF
                fread(&VertexInfo_ptr->SDEF.BoneIndex_1, ProfileCtx_ptr->GlobalParamSet.BoneIndexSize, 1, fModel);
                fread(&VertexInfo_ptr->SDEF.BoneIndex_2, ProfileCtx_ptr->GlobalParamSet.BoneIndexSize, 1, fModel);
                fread(&VertexInfo_ptr->SDEF.C , sizeof(float), 1 +3* 3, fModel);
                //printf("C %f %f %f\t ", VertexInfo_ptr->SDEF.C.x, VertexInfo_ptr->SDEF.C.y, VertexInfo_ptr->SDEF.C.z);
                //printf("R0 %f %f %ft ", VertexInfo_ptr->SDEF.R0 .x, VertexInfo_ptr->SDEF.R0.y, VertexInfo_ptr->SDEF.R0.z);
                //printf("R1 %f %f %f\n ", VertexInfo_ptr->SDEF.R1 .x, VertexInfo_ptr->SDEF.R1.y, VertexInfo_ptr->SDEF.R1.z);
                break;
            }
            fread(&VertexInfo_ptr->EdgeScale, sizeof(float), 1, fModel);

//#define PROFILE_DBG
#ifdef PROFILE_DBG
            printf("V : %5.5f %5.5f %5.5f , ", VertexInfo_ptr->Vertex[0], VertexInfo_ptr->Vertex[1], VertexInfo_ptr->Vertex[2]);
            printf("N : %5.5f %5.5f %5.5f , ", VertexInfo_ptr->Normal[0], VertexInfo_ptr->Normal[1], VertexInfo_ptr->Normal[2]);
            printf("UV : %5.5f %5.5f ,", VertexInfo_ptr->Texture[0], VertexInfo_ptr->Texture[1]);
            printf("DeformType %d,", VertexInfo_ptr->DeformType);
            printf("EdgeScale %lf\n", VertexInfo_ptr->EdgeScale);
#endif
#undef PROFILE_DBG
        }
        return 1;
    }
    return 0;
}
// ---------------------------------------------------------------------------------
int PMX_profile::parse_IndexInformation()
{
    int ret ;
    printf(pLIGHT_GREEN "Parse Index information\n" pNONE);
    fread(&ProfileCtx_ptr->IndexInfoCount, sizeof(int), 1, fModel);
    ProfileCtx_ptr->IndexInfo = (unsigned char *)malloc(ProfileCtx_ptr->GlobalParamSet.VertexIndexSize *
                                                        ProfileCtx_ptr->IndexInfoCount);
    if(ProfileCtx_ptr->IndexInfo ==NULL) {
        printf(pLIGHT_RED "Error : malloc failed ... VertexIndexSize %d, IndexInfoCount %d\n" pNONE, ProfileCtx_ptr->GlobalParamSet.VertexIndexSize, ProfileCtx_ptr->IndexInfoCount);
        return 0;
    }
    else {
        printf("VertexIndexSize %d, IndexCount : %d\n", ProfileCtx_ptr->GlobalParamSet.VertexIndexSize, ProfileCtx_ptr->IndexInfoCount);
        ret = fread(ProfileCtx_ptr->IndexInfo, ProfileCtx_ptr->GlobalParamSet.VertexIndexSize, ProfileCtx_ptr->IndexInfoCount, fModel);
        if(ret != ProfileCtx_ptr->IndexInfoCount) {
            printf(pLIGHT_RED "Error : File size incomplete\n" pNONE);
            return 0;
        }
//#define PROFILE_DBG
#ifdef PROFILE_DBG
        unsigned char *uchar_ptr    = ProfileCtx_ptr->IndexInfo;
        unsigned short *ushort_ptr  = (unsigned short *)ProfileCtx_ptr->IndexInfo;
        unsigned int *uint_ptr      = (unsigned int *)ProfileCtx_ptr->IndexInfo;

        switch(ProfileCtx_ptr->GlobalParamSet.VertexIndexSize) {
        case 1 :
            for(int i =0 ; i < ProfileCtx_ptr->IndexInfoCount ;i+=3) {
                printf("%5d / %5d / %5d\n", uchar_ptr[i], uchar_ptr[i +1], uchar_ptr[i +2]);
            }
            break;
        case 2:
            for(int i =0 ; i < ProfileCtx_ptr->IndexInfoCount ;i+=3) {
                printf("%5d / %5d / %5d\n", ushort_ptr[i], ushort_ptr[i +1], ushort_ptr[i +2]);
            }
            break;
        case 4:
            for(int i =0 ; i < ProfileCtx_ptr->IndexInfoCount ;i+=3) {
                printf("%5d / %5d / %5d\n", uint_ptr[i], uint_ptr[i +1], uint_ptr[i +2]);
            }
            break;
        }
        exit(0);
#endif
#undef PROFILE_DBG
        return 1;
    }
}
// ---------------------------------------------------------------------------------
int PMX_profile::parse_TextureInformation()
{
    int ti ;

    printf(pLIGHT_GREEN "Parse Texture information\n" pNONE);
    fread(&ProfileCtx_ptr->TextureInfoCount , sizeof(int), 1, fModel);
    ProfileCtx_ptr->TextureInfo = (struct PMX_TextureInformation *)malloc(sizeof(struct PMX_TextureInformation) * ProfileCtx_ptr->TextureInfoCount);

    if(ProfileCtx_ptr->TextureInfo ==NULL) {
        printf(pLIGHT_RED "Error : malloc failed\n" pNONE);
        return 0;
    }
    else {
        printf("TextureInfoCount %d\n", ProfileCtx_ptr->TextureInfoCount);
        for(ti = 0 ; ti < ProfileCtx_ptr->TextureInfoCount ; ti++) {
            struct PMX_TextureInformation *TextureInfo_ptr = ProfileCtx_ptr->TextureInfo +ti;
            Read_PMX_Text(fModel, &TextureInfo_ptr->name_US);

#define PROFILE_DBG
#ifdef PROFILE_DBG
            printf("Textxture [%d] , Length [%d] : [", ti, TextureInfo_ptr->name_US.Length);
            for(int si = 0 ; si < TextureInfo_ptr->name_US.Length ; si++)
                printf("%c", TextureInfo_ptr->name_US.text_UTF8[si]);
            printf("]\n");
#endif
#undef PROFILE_DBG

        }
    }
    return 1;
}
// ---------------------------------------------------------------------------------
int PMX_profile::parse_MaterialInformation()
{
    int mi ;

    printf(pLIGHT_GREEN "Parse Material information\n" pNONE);
    fread(&ProfileCtx_ptr->MaterialInfoCount, sizeof(int), 1, fModel);
    ProfileCtx_ptr->MaterialInfo = (struct PMX_MaterialInformation *)malloc(sizeof(struct PMX_MaterialInformation) * ProfileCtx_ptr->MaterialInfoCount);

    if(ProfileCtx_ptr->MaterialInfo ==NULL) {
        printf(pLIGHT_RED "Error : malloc failed\n" pNONE);
        return 0;
    }
    else {
        memset(ProfileCtx_ptr->MaterialInfo, 0, sizeof(struct PMX_MaterialInformation) * ProfileCtx_ptr->MaterialInfoCount);
        printf("MaterialInfoCount %d\n", ProfileCtx_ptr->MaterialInfoCount);
        for(mi = 0 ; mi < ProfileCtx_ptr->MaterialInfoCount ; mi++) {
            struct PMX_MaterialInformation *MaterialInfo_ptr = ProfileCtx_ptr->MaterialInfo +mi;

            if(0 == Read_PMX_Text(fModel, &MaterialInfo_ptr->name_JP)) return 0;
            if(0 == Read_PMX_Text(fModel, &MaterialInfo_ptr->name_US)) return 0;

            fread(MaterialInfo_ptr->Diffuse_RGBA, sizeof(float) *4, 1, fModel);
            fread(MaterialInfo_ptr->Specular_RGB, sizeof(float) *3, 1, fModel);
            fread(&MaterialInfo_ptr->Specularity, sizeof(float), 1, fModel);
            fread(MaterialInfo_ptr->Ambient_RGB, sizeof(float) *3, 1, fModel);
            fread(&MaterialInfo_ptr->bitFlag, sizeof(char), 1, fModel);
            fread(MaterialInfo_ptr->Edge_RGBA, sizeof(float) *4, 1, fModel);
            fread(&MaterialInfo_ptr->Edge_Scale, sizeof(float), 1, fModel);

            fread(&MaterialInfo_ptr->TextureIndex, ProfileCtx_ptr->GlobalParamSet.TextureIndexSize , 1, fModel);
            fread(&MaterialInfo_ptr->sphereTextureIndex, ProfileCtx_ptr->GlobalParamSet.TextureIndexSize, 1, fModel);
            fread(&MaterialInfo_ptr->sphereTextureBlendMode, 1, 1, fModel);
            fread(&MaterialInfo_ptr->ToonReference, 1, 1, fModel);
            if(MaterialInfo_ptr->ToonReference == 0)
                fread(&MaterialInfo_ptr->ToonValue, 1, 1, fModel);
            else
                fread(&MaterialInfo_ptr->ToonValue, ProfileCtx_ptr->GlobalParamSet.TextureIndexSize, 1, fModel);

            if(0 == Read_PMX_Text(fModel, &MaterialInfo_ptr->MetaData)) return 0;
            fread(&MaterialInfo_ptr->SurfaceCount, sizeof(int), 1, fModel);

#define PROFILE_DBG
#ifdef PROFILE_DBG
            printf("-------------------------------------------------------\n");
            printf("\tMaterial ID            : %d\n", mi);
            printf("\tname JP                : ");
            for(int si = 0 ; si < MaterialInfo_ptr->name_JP.Length ; si++) printf("%c", MaterialInfo_ptr->name_JP.text_UTF8[si]);
            printf("\n");
            printf("\tname US                : ");
            for(int si = 0 ; si < MaterialInfo_ptr->name_US.Length ; si++) printf("%c", MaterialInfo_ptr->name_US.text_UTF8[si]);
            printf("\n");
            printf("\tDiffuse                : R %5.4f, G %5.4f, B %5.4f, A %5.4f\n", MaterialInfo_ptr->Diffuse_RGBA[0], MaterialInfo_ptr->Diffuse_RGBA[1], MaterialInfo_ptr->Diffuse_RGBA[2], MaterialInfo_ptr->Diffuse_RGBA[3]);
            printf("\tSpecularity            : %5.4f\n", MaterialInfo_ptr->Specularity);
            printf("\tSpecular               : R %5.4f, G %5.4f, B %5.4f, A %5.4f\n", MaterialInfo_ptr->Specular_RGB[0], MaterialInfo_ptr->Specular_RGB[1], MaterialInfo_ptr->Specular_RGB[2], MaterialInfo_ptr->Specular_RGB[3]);
            printf("\tAmbient                : R %5.4f, G %5.4f, B %5.4f\n", MaterialInfo_ptr->Ambient_RGB[0], MaterialInfo_ptr->Ambient_RGB[1], MaterialInfo_ptr->Ambient_RGB[2]);
            printf("\tBit flag               : %X\n", MaterialInfo_ptr->bitFlag);
            printf("\tEdge Color             : R %5.4f, G %5.4f, B %5.4f, A %5.4f\n", MaterialInfo_ptr->Edge_RGBA[0], MaterialInfo_ptr->Edge_RGBA[1], MaterialInfo_ptr->Edge_RGBA[2], MaterialInfo_ptr->Edge_RGBA[3]);
            printf("\tEdge Scale             : %5.4f\n", MaterialInfo_ptr->Edge_Scale);
            printf("\tTexture Index          : %5d\n", MaterialInfo_ptr->TextureIndex);
            printf("\tEnvironment Index      : %5d\n", MaterialInfo_ptr->sphereTextureIndex);
            printf("\tEnvironment Blend Mode : %5d\n", MaterialInfo_ptr->sphereTextureBlendMode);
            printf("\tToon Reference         : %5d\n", MaterialInfo_ptr->ToonReference);
            printf("\tSurface Count          : %5d\n", MaterialInfo_ptr->SurfaceCount);
#endif
#undef PROFILE_DBG
        }
    }

    return 1;
}
// ---------------------------------------------------------------------------------
int PMX_profile::parse_BoneInformation()
{
    int ti, ret ;

    printf(pLIGHT_GREEN "Parse Bone information\n" pNONE);
    if(0 == fread(&ProfileCtx_ptr->BoneInfoCount, sizeof(int), 1, fModel)) {
        printf(pLIGHT_RED "Error : File size incomplete\n" pNONE);
        return 0;
    }

    ProfileCtx_ptr->BoneInfo = (struct PMX_BoneInformation *)malloc(sizeof(struct PMX_BoneInformation) * ProfileCtx_ptr->BoneInfoCount);
    if(ProfileCtx_ptr->BoneInfo ==NULL) {
        printf(pLIGHT_RED "Error : malloc failed\n" pNONE);
        return 0;
    }
    else {
        printf("BoneInfoCount : %d\n", ProfileCtx_ptr->BoneInfoCount);
        memset(ProfileCtx_ptr->BoneInfo, 0, sizeof(struct PMX_BoneInformation) * ProfileCtx_ptr->BoneInfoCount);
        for(int i =0 ; i <ProfileCtx_ptr->BoneInfoCount ; i++) {
            struct PMX_BoneInformation * BoneInfo_ptr = ProfileCtx_ptr->BoneInfo +i;

            if(0 == Read_PMX_Text(fModel, &BoneInfo_ptr->name_JP)) return 0;
            if(0 == Read_PMX_Text(fModel, &BoneInfo_ptr->name_US)) return 0;

            /*printf("------------------------------------\n");
            printf("%s\n", BoneInfo_ptr->name_JP.Text);
            for(int ti = 0 ; ti < BoneInfo_ptr->name_JP.Length ;ti++) {
                printf("[%d] %c \%2X \n", ti, BoneInfo_ptr->name_JP.Text[ti], BoneInfo_ptr->name_JP.Text[ti] );
            }*/
           // printf("PMX %s, len %d,  %d %d\n", BoneInfo_ptr->name_JP.Text, BoneInfo_ptr->name_JP.Length, BoneInfo_ptr->nameKey_djb2, BoneInfo_ptr->nameKey_DKDR);

            BoneInfo_ptr->bID = i;
            ret = fread(BoneInfo_ptr->BoneVertex, sizeof(float) *3, 1, fModel);
            ret = fread(&BoneInfo_ptr->bID_Parent, ProfileCtx_ptr->GlobalParamSet.BoneIndexSize, 1, fModel);
            ret = fread(&BoneInfo_ptr->DeformableLevel, sizeof(int), 1, fModel);
            ret = fread(&BoneInfo_ptr->BoneType , sizeof(unsigned short) , 1, fModel);

            // 0x0001  : 接続先(PMD子ボーン指定)表示方法 -> 0:座標オフセットで指定 1:ボーンで指定
            if(BoneInfo_ptr->BoneType & 0x0001) {
                fread(&BoneInfo_ptr->TypeParam_0x0001_BoneIndex, ProfileCtx_ptr->GlobalParamSet.BoneIndexSize, 1, fModel);
            }
            else {
                fread(BoneInfo_ptr->TypeParam_0x0001_BoneOffset, sizeof(float) *3, 1, fModel);
            }

            // 0x0100  : 回転付与
            if(BoneInfo_ptr->BoneType & 0x0100) {
                fread(&BoneInfo_ptr->TypeParam_0x0100_BoneIndex, ProfileCtx_ptr->GlobalParamSet.BoneIndexSize, 1, fModel);
                fread(&BoneInfo_ptr->TypeParam_0x0100_Percent, sizeof(float), 1, fModel);
            }

            // 0x0400  : 軸固定
            if(BoneInfo_ptr->BoneType & 0x0400) {
                fread(BoneInfo_ptr->TypeParam_0x0400_DirectonVertex, sizeof(float) *3, 1, fModel);
            }

            // 0x0800  : ローカル軸
            if(BoneInfo_ptr->BoneType & 0x0800) {
                fread(BoneInfo_ptr->TypeParam_0x0800_Vertex_X, sizeof(float) *3, 1, fModel);
                fread(BoneInfo_ptr->TypeParam_0x0800_Vertex_Z, sizeof(float) *3, 1, fModel);
            }

            // 0x2000  : 外部親変形
            if(BoneInfo_ptr->BoneType & 0x2000) {
                fread(&BoneInfo_ptr->TypeParam_0x2000_Key , sizeof(float) *3, 1, fModel);
            }

            // 0x0020  : IK
            if(BoneInfo_ptr->BoneType & 0x0020) {
                ProfileCtx_ptr->InverseKinematicsInfoCount++;
                fread(&BoneInfo_ptr->TypeParam_0x0020_IK.TargetBoneID, ProfileCtx_ptr->GlobalParamSet.BoneIndexSize, 1, fModel);
                fread(&BoneInfo_ptr->TypeParam_0x0020_IK.MaxIterations, sizeof(int), 1, fModel);
                fread(&BoneInfo_ptr->TypeParam_0x0020_IK.MaxAngle, sizeof(float), 1, fModel);

                fread(&BoneInfo_ptr->TypeParam_0x0020_IK.IKChainCount, sizeof(int), 1, fModel);
                BoneInfo_ptr->TypeParam_0x0020_IK.IKChain = (struct PMX_InverseKinematicsChainContext *)malloc(sizeof(struct PMX_InverseKinematicsChainContext) * BoneInfo_ptr->TypeParam_0x0020_IK.IKChainCount);
                memset(BoneInfo_ptr->TypeParam_0x0020_IK.IKChain, 0, sizeof(struct PMX_InverseKinematicsChainContext) * BoneInfo_ptr->TypeParam_0x0020_IK.IKChainCount);

                for(int iki =0 ; iki < BoneInfo_ptr->TypeParam_0x0020_IK.IKChainCount ; iki++) {
                    struct PMX_InverseKinematicsChainContext *IK_ptr = BoneInfo_ptr->TypeParam_0x0020_IK.IKChain +iki;
                    fread(&IK_ptr->LinkBone, ProfileCtx_ptr->GlobalParamSet.BoneIndexSize, 1, fModel);
                    fread(&IK_ptr->isLimit, 1, 1, fModel);
                    if(IK_ptr->isLimit) {
                        fread(IK_ptr->LowerBound, sizeof(float) *3, 1, fModel);
                        fread(IK_ptr->UpBound, sizeof(float) *3, 1, fModel);
                    }
                }
            }

#define PROFILE_DBG
#ifdef PROFILE_DBG
            printf("-------------------------------------------------------\n");
            printf(pLIGHT_GREEN "\tID    : %d\n" pNONE, i);
            printf("\tName (JP)          : ");
            for(int ti = 0 ; ti < BoneInfo_ptr->name_JP.Length ; ti++) printf("%c", BoneInfo_ptr->name_JP.text_UTF8[ti]);
            printf("\n");
            printf("\tName (US)          : ");
            for(int ti = 0 ; ti < BoneInfo_ptr->name_US.Length ; ti++) printf("%c", BoneInfo_ptr->name_US.text_UTF8[ti]);
            printf("\n");
            printf("\tAxis               : %5.5f, %5.5f, %5.5f\n", BoneInfo_ptr->BoneVertex[0], BoneInfo_ptr->BoneVertex[1], BoneInfo_ptr->BoneVertex[2]);
            printf("\tBoneID for Parent  : %hd\n", BoneInfo_ptr->bID_Parent);
            printf("\tType               : %X\n", BoneInfo_ptr->BoneType);

            if(BoneInfo_ptr->BoneType & 0x0001) {
                printf("\tTypeParam_0x0001_BoneIndex : %d\n", BoneInfo_ptr->TypeParam_0x0001_BoneIndex);
            }
            else {
                printf("\tTypeParam_0x0001_BoneOffset : %f %f %f\n", BoneInfo_ptr->TypeParam_0x0001_BoneOffset[0],
                                                                   BoneInfo_ptr->TypeParam_0x0001_BoneOffset[0],
                                                                   BoneInfo_ptr->TypeParam_0x0001_BoneOffset[0]);
            }
            if(BoneInfo_ptr->TypeParam_0x0020_IK.IKChainCount >0) {
                printf("\tIK Count : %d, TargetBoneID %d, MaxIterations %d, MaxAngle %f\n",
                       BoneInfo_ptr->TypeParam_0x0020_IK.IKChainCount,
                       BoneInfo_ptr->TypeParam_0x0020_IK.TargetBoneID,
                       BoneInfo_ptr->TypeParam_0x0020_IK.MaxIterations,
                       BoneInfo_ptr->TypeParam_0x0020_IK.MaxAngle);

                for(int iki =0 ; iki < BoneInfo_ptr->TypeParam_0x0020_IK.IKChainCount ; iki++) {
                    struct PMX_InverseKinematicsChainContext *IK_ptr = BoneInfo_ptr->TypeParam_0x0020_IK.IKChain +iki;
                    printf("\t\tChain [%d] Bone %d , isLimit %d, Lower Bound : %.6f %.6f %.6f , Up Bound : %f %f %f\n", iki,
                           IK_ptr->LinkBone, IK_ptr->isLimit,
                           IK_ptr->LowerBound[0], IK_ptr->LowerBound[1], IK_ptr->LowerBound[2],
                           IK_ptr->UpBound[0], IK_ptr->UpBound[1], IK_ptr->UpBound[2]);
                }
            }
#endif
#undef PROFILE_DBG
        }
        return 1;
    }
}
// ---------------------------------------------------------------------------------
int PMX_profile::parse_MorphInformation()
{
    int ci, ri, ret, typeSize, indexSize;
    const int typeSizeSet[] = { sizeof(MorphType_Group),    //0:グループ
                                sizeof(MorphType_Vertex),   //1:頂点
                                sizeof(MorphType_Bone),     //2:ボーン
                                sizeof(MorphType_UV),       //3:UV,
                                sizeof(MorphType_UV),       //4:追加UV1
                                sizeof(MorphType_UV),       //5:追加UV2
                                sizeof(MorphType_UV),       //6:追加UV3
                                sizeof(MorphType_UV),       //7:追加UV4
                                sizeof(MorphType_Material)};//8:材質
    const int indexSizeSet[] = {ProfileCtx_ptr->GlobalParamSet.MaterialIndexSize,   //0:グループ
                                ProfileCtx_ptr->GlobalParamSet.VertexIndexSize ,    //1:頂点
                                ProfileCtx_ptr->GlobalParamSet.BoneIndexSize,       //2:ボーン
                                ProfileCtx_ptr->GlobalParamSet.VertexIndexSize,     //3:UV,
                                ProfileCtx_ptr->GlobalParamSet.VertexIndexSize,     //4:追加UV1
                                ProfileCtx_ptr->GlobalParamSet.VertexIndexSize,     //5:追加UV2
                                ProfileCtx_ptr->GlobalParamSet.VertexIndexSize,     //6:追加UV3
                                ProfileCtx_ptr->GlobalParamSet.VertexIndexSize,     //7:追加UV4
                                ProfileCtx_ptr->GlobalParamSet.MaterialIndexSize};  //8:材質

    printf(pLIGHT_GREEN "Parse Morph information\n" pNONE);
    fread(&ProfileCtx_ptr->MorphInfoCount, sizeof(int), 1, fModel);
    printf("Morph count %d\n", ProfileCtx_ptr->MorphInfoCount);
    ProfileCtx_ptr->MorphInfo = (struct PMX_MorphInformation *)malloc(sizeof(struct PMX_MorphInformation) * ProfileCtx_ptr->MorphInfoCount);
    if(ProfileCtx_ptr->MorphInfo ==NULL) {
        printf("Error : malloc failed\n");
        return 0;
    }
    else {
        for(ci =0 ; ci <ProfileCtx_ptr->MorphInfoCount ; ci++) {
            struct PMX_MorphInformation *pMorphInfo = ProfileCtx_ptr->MorphInfo +ci;
            void *pStore;
            if(0 == Read_PMX_Text(fModel, &pMorphInfo->name_JP)) return 0;
            if(0 == Read_PMX_Text(fModel, &pMorphInfo->name_US)) return 0;
            ret = fread(&pMorphInfo->operatorType, 1, 1, fModel);
            ret = fread(&pMorphInfo->morphType, 1, 1, fModel);
            ret = fread(&pMorphInfo->morphCount, sizeof(int), 1, fModel);

            indexSize = indexSizeSet[pMorphInfo->morphType]; ;
            typeSize = typeSizeSet[pMorphInfo->morphType] - sizeof(int);    //subtract the max index size
            pMorphInfo->morphData = malloc(typeSizeSet[pMorphInfo->morphType] * pMorphInfo->morphCount);
            memset(pMorphInfo->morphData, 0, typeSizeSet[pMorphInfo->morphType] * pMorphInfo->morphCount);
            pStore = pMorphInfo->morphData;

            printf("indexSize %d, typeSize %d\n", indexSize, typeSize);
            for(ri = 0 ; ri < pMorphInfo->morphCount ; ri++) {
                ret = fread(pStore, indexSize, 1, fModel);
                pStore += sizeof(int);
                ret = fread(pStore, typeSize, 1, fModel);
                pStore += typeSize;
            }

#define PROFILE_DBG
#ifdef PROFILE_DBG
            struct MorphType_Vertex *pVertex     = (struct MorphType_Vertex *)pMorphInfo->morphData;
            struct MorphType_UV *pUV             = (struct MorphType_UV *)pMorphInfo->morphData;
            struct MorphType_Bone *pBone         = (struct MorphType_Bone *)pMorphInfo->morphData;
            struct MorphType_Material *pMaterial = (struct MorphType_Material *)pMorphInfo->morphData;
            struct MorphType_Group *pGroup       = (struct MorphType_Group *)pMorphInfo->morphData;

            printf("Morpth name %s\n", pMorphInfo->name_JP.text_UTF8);
            printf("\tOperator Type : %d\n", pMorphInfo->operatorType);
            printf("\tMorph Type    : %d\n", pMorphInfo->morphType);
            printf("\tMorph Count   : %d\n", pMorphInfo->morphCount);
            for(int di = 0; di < pMorphInfo->morphCount ; di++) {
                switch(pMorphInfo->morphType) {
                case 0:
                    printf("\tmID [%d], offset : %f %f %f %f\n", pGroup->mID, pGroup->morphRate[0], pGroup->morphRate[1], pGroup->morphRate[2], pGroup->morphRate[3]);
                    break;
                case 1:
                    if(di < 5) printf("\tvID [%d], offset : %f %f %f\n", pVertex->vID, pVertex->offset[0], pVertex->offset[1], pVertex->offset[2]);
                    pVertex++;
                    break;
                case 2:
                    printf("\tbID [%d], translation : %f %f %f, rotation %f %f %f\n", pBone->bID,
                           pBone->translation[0], pBone->translation[1], pBone->translation[2],
                           pBone->rotation[0], pBone->rotation[1], pBone->rotation[2],pBone->rotation[3]);
                    break;
                case 3:
                    break;
                case 4:
                    break;
                case 5:
                    break;
                case 6:
                    break;
                case 7:
                    break;
                case 8:
                    printf("\tDiffuse     : R %5.4f, G %5.4f, B %5.4f, A %5.4f\n", pMaterial->diffuse[0], pMaterial->diffuse[1], pMaterial->diffuse[2], pMaterial->diffuse[3]);
                    printf("\tSpecularity : %5.4f\n", pMaterial->Specularity);
                    printf("\tSpecular    : R %5.4f, G %5.4f, B %5.4f, A %5.4f\n", pMaterial->specular[0], pMaterial->specular[1], pMaterial->specular[2], pMaterial->specular[3]);
                    printf("\tAmbient     : R %5.4f, G %5.4f, B %5.4f\n", pMaterial->ambient[0], pMaterial->ambient[1], pMaterial->ambient[2]);
                    break;
                }
            }
#endif
#undef PROFILE_DBG
        }
        return 1;
    }
}
// ---------------------------------------------------------------------------------
int PMX_profile::parse_DisplayNameInformation()
{
    return 1;
}
/* ---------------------------------------------------------------------------------
 *  Alloc and reset a new PMDContext for return
 *      @param ID : Specify id.
 *      @param MaxWidth, MaxHeight, MaxLength : The boundary size of width, height, length.
 */
struct PolygonModeleXtendedContext *PMX_profile::genContext(int ID, float MaxWidth, float MaxHeight, float MaxLength)
{
    struct PolygonModeleXtendedContext *Ctx = (struct PolygonModeleXtendedContext *)malloc(sizeof(struct PolygonModeleXtendedContext));

    if(Ctx == NULL) return NULL;
    this->fModel                    = NULL;
    Ctx->mName                      = NULL;
    Ctx->mPath                      = NULL;
    Ctx->VertexInfo                 = NULL;
    Ctx->MaterialInfo               = NULL;
    Ctx->IndexInfo                  = NULL;
    Ctx->BoneInfo                   = NULL;
    Ctx->MorphInfo              = NULL;
    Ctx->CtxID                      = ID;
    Ctx->VertexInfoCount            = 0;
    Ctx->IndexInfoCount             = 0;
    Ctx->MaterialInfoCount          = 0;
    Ctx->BoneInfoCount              = 0;
    Ctx->InverseKinematicsInfoCount = 0;
    Ctx->MorphInfoCount         = 0;
    memset(&Ctx->ModelGeometryCtx, 0, sizeof(PMX_ModelGeometryContext));
    MaxModelSize[0] =MaxWidth;
    MaxModelSize[1] =MaxHeight;
    MaxModelSize[2] =MaxLength;
    return Ctx;
}
// ---------------------------------------------------------------------------------
void PMX_profile::release(PolygonModeleXtendedContext *relCtx)
{
    if(NULL == relCtx) return;
    if(NULL != relCtx->VertexInfo )     free(relCtx->VertexInfo);
    if(NULL != relCtx->MaterialInfo)    free(relCtx->MaterialInfo);
    if(NULL != relCtx->IndexInfo)       free(relCtx->IndexInfo);
    if(NULL != relCtx->BoneInfo)        free(relCtx->BoneInfo);
    if(NULL != relCtx->MorphInfo)   free(relCtx->MorphInfo);
    if(NULL != relCtx->mPath)           free(relCtx->mPath);
    if(NULL != relCtx->mName)           free(relCtx->mName);
    memset(relCtx, 0, sizeof(PolygonModeleXtendedContext));
}
/* ---------------------------------------------------------------------------------
 * Opoen the .obj file
 *      @param FilePath : The path of target .obj file.
 *      @param TexturePath : The path of target material file.
*/
int PMX_profile::load(struct PolygonModeleXtendedContext *pmxCtx, const char *pmxPath, const char *pmxName, const char *pmxToonTexturePath)
{
    struct timeval pStart, pEnd;
    float uTime;
    const int pathLen = strlen(pmxPath);
    const int nameLen = strlen(pmxName);
    const int FullPathLen = pathLen + nameLen +1;
    char *FullPath = (char *)malloc(FullPathLen);

    // associate the file full path.
    if(FullPath == NULL) goto ANY_ERROR;
    memset(FullPath, 0, FullPathLen);
    strcpy(FullPath, pmxPath);
    if(pmxPath[pathLen -1] != '/') strcat(FullPath, "/");
    strcat(FullPath, pmxName);

    gettimeofday(&pStart, NULL);
    fModel= fopen(FullPath, "r");
    if((fModel != NULL )&& (pmxCtx != NULL)) {
        ProfileCtx_ptr = pmxCtx;
        ProfileCtx_ptr->mName = (char *)malloc(nameLen +1);
        ProfileCtx_ptr->mPath = (char *)malloc(pathLen +1);
        if(!ProfileCtx_ptr->mName) goto ANY_ERROR;
        if(!ProfileCtx_ptr->mPath) goto ANY_ERROR;
        memcpy(ProfileCtx_ptr->mName, pmxName, nameLen + 1);
        memcpy(ProfileCtx_ptr->mPath, pmxPath, pathLen + 1);
        /* parse PMX model data */
        if(!parse_FileHeader()) goto ANY_ERROR;
        if(!parse_VertexInformation()) goto ANY_ERROR;
        if(!parse_IndexInformation()) goto ANY_ERROR;
        if(!parse_TextureInformation()) goto ANY_ERROR;
        if(!parse_MaterialInformation()) goto ANY_ERROR;
        if(!parse_BoneInformation()) goto ANY_ERROR;
        //if(!parse_MorphInformation()) goto ANY_ERROR;
        //if(!parse_DisplayNameInformation()) goto ANY_ERROR;
        fclose(fModel);

        /* paser extend data */
        parse_Boundary();
        resize_Model();
    }
    else {
        printf("Fatal : open model failed, no suck file or directory [%s]\n", FullPath);
    }
    gettimeofday(&pEnd, NULL);
    uTime = (pEnd.tv_sec -pStart.tv_sec)*1000000.0 +(pEnd.tv_usec -pStart.tv_usec);
    printf("Info : Read model file : %s ... time : %lf ms\n", pmxPath, uTime /1000.0f) ;
    printf("--------------------------------------------------------------------\n");

    free(FullPath);
    return 1;

ANY_ERROR:
    printf ("Info : Read object file : %s ... Failed\n", FullPath) ;
    release(pmxCtx);
    return 0 ;
}
// ---------------------------------------------------------------------------------
PMX_TextureContext *PMX_profile::load_Texture(char *fPath, char *fName)
{
    BMP_file Texture;
    Texture.read(fPath, fName);

    PMX_TextureContext *TextureImg = (PMX_TextureContext *)malloc(sizeof(PMX_TextureContext));

    TextureImg->Width = Texture.get_info_header()->width;
    TextureImg->Height = Texture.get_info_header()->height;
    TextureImg->ByteLen = TextureImg->Width * TextureImg->Height * 3;
    TextureImg->RGB888 = (unsigned char *)malloc(TextureImg->ByteLen);

    memcpy(TextureImg->RGB888, Texture.get_color_ptr(), TextureImg->ByteLen);
    return TextureImg;
    /*char FullPath[1024];
    strcpy(FullPath, fPath);
    strcat(FullPath, fName);

    IplImage *TextureImg = cvLoadImage(FullPath, CV_LOAD_IMAGE_COLOR);
    if(!TextureImg) {
        printf(pLIGHT_RED "\t\tFatal" pNONE " : Read texture file: %s ... Failed\n" pNONE, FullPath);
        return NULL ;
    }

    printf(pLIGHT_GREEN "\t\tInfo " pNONE ": Read texture file: %s ... Success\n", FullPath);
    printf("\t\t\tWidth : %d,  Height : %d, Channels : %d\n", TextureImg->width, TextureImg->height, TextureImg->nChannels);
    printf("\t\t\tImageSize : %d, WidthStep : %d\n", TextureImg->imageSize, TextureImg->widthStep );
    return TextureImg ;*/
}
