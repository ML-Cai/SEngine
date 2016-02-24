#ifndef OPENMMD_PROFILE_H_INCLUDED
#define OPENMMD_PROFILE_H_INCLUDED
/* -------------------------------------------------------------------- */
#include <stdio.h>
/* -------------------------------------------------------------------- */
/* The Data buffer of obj files.
 *  This struct is uesed to carry obj data when parse the files.
*/
namespace MMDCppDev {
    /* --------------------------------------------------------------------
     * The Axis Boundary Context.
     *      This struct contain the boundary information of raw PMD model and fixed PMD model
    */
    struct ModelGeometryContext {
        float ModelCenter[3];       /* Center point */
        float ModelSize[3];         /* 0-Width(x), 1-Height(y), 2-Length(2)*/
        float ModelBoundary[3][2];  /* [0][0] Max X, [0][1] Min X
                                       [1][0] Max Y, [1][1] Min Y
                                       [2][0] Max Z, [2][1] Min Z*/
        float RawModelCenter[3];    /* Original Model information */
        float RawModelSize[3];
        float RawModelBoundary[3][2];
        float ModelBoundScaleRate;
    };
    /* ----------------------------------*/
    struct PMD_TextureContext {
        int Width;
        int Height;
        int ByteLen;
        unsigned char *RGB888;
    };

    /* --------------------------------------------------------------------
     * The Sub-Struct of Polygon Model Data
     *  Basic Model Information :
     *      1. FileHeader
     *      2. Model header
     *      3. Vertex information
     *      4. Index information
     *      5. Materials information
     *      6. Bones information
     *      7. Inverse Kinematics information
     *      8. Face Morph information
     *      9. Display Name Information
     *  English Model Information
     */
    /* Basic Model Infoermation set */
    /* ----------------------------------*/
    struct PMD_FileHeader {
        char    PMD[3];
        float   version;
    } __attribute__((packed));

    /* ----------------------------------*/
    struct PMD_ModelHeader {
        char name[20];
        char comment[256];
    } __attribute__((packed));

    /* ----------------------------------*/
    struct PMD_VertexInformation {
        float           Vertex[3];
        float           Normal[3];
        float           Texture[2];
        unsigned short  BoneID_0;
        unsigned short  BoneID_1;
        unsigned char   BoneWeight;
        char   EdgeFlag;
    } __attribute__((packed));

    /* ----------------------------------*/
    struct PMD_MaterialInformation {
        float           Diffuse_RGBA[4];
        float           Specularity;
        float           Specular_RGB[3];
        float           Ambient_RGB[3];
        unsigned char   ToonID;
        unsigned char   EdgeFlag;
        unsigned int    EffectRange;
        char            fname[20];
        int             TextureIndex;
        PMD_TextureContext *TextureImg ;
    } __attribute__((packed));

    /* ----------------------------------*/
    struct PMD_BoneInformation {
        char            Name_shift_jis[20];
        short           bID_Parent;
        short           bID_Child;
        unsigned char   Type;
        short           TargetBone;
        float           BoneAxis[3];
        //external data
        unsigned int NameKey_DKDR;
        unsigned int NameKey_djb2;
    } __attribute__((packed));

    /* ----------------------------------*/
    struct PMD_InverseKinematicsInformation {
        unsigned short  bID_Reach;
        unsigned short  bID_End;
        unsigned char   ChainCount;
        short           MaxIterations;
        float           MaxAngle;
        short           *Chain;
    } __attribute__((packed));

    /* ----------------------------------*/
    struct PMD_FaceVertexList {
        unsigned int    Index;
        float           Vertex[3];
    } __attribute__((packed));

    struct PMD_FaceMorphInformation {
        char Name_shift_jis[20];
        unsigned int EffectCount;
        unsigned char Type;
        struct PMD_FaceVertexList *EffectList;
    } __attribute__((packed));

    /* ----------------------------------*/
    struct PMD_DisplayNameInformation {
        unsigned char FaceCount;
        unsigned char BondGroupCount;
        unsigned int DisplayBoneCount;
    } __attribute__((packed));

    /* English Model Infoermation set */

    struct PolygonModelDataContext {
        unsigned int CtxID;
        int VertexInfoCount;
        int MaterialInfoCount;
        int MaterialTextureCount;
        int IndexInfoCount;
        short BoneInfoCount;
        short InverseKinematicsInfoCount;
        short FaceMorphInfoCount;
        struct PMD_FileHeader FileHeaderInfo;
        struct PMD_ModelHeader ModelHeaderInfo;
        struct PMD_VertexInformation *VertexInfo;
        struct PMD_MaterialInformation *MaterialInfo;
        unsigned short *IndexInfo;
        struct PMD_BoneInformation *BoneInfo;
        struct PMD_InverseKinematicsInformation *InverseKinematicsInfo;
        struct PMD_FaceMorphInformation *FaceMorphInfo;
        struct ModelGeometryContext ModelGeometryCtx;

    };

    /* --------------------------------------------------------------------
     * The Context of Polygon Model Data (.pmd) File.
     *
    */
    class PMD_profile {
    public:
        struct PolygonModelDataContext *genContext(int ID, float MaxWidth, float MaxHeight, float MaxLength);
        int load(struct PolygonModelDataContext *PMDCtx ,const char *FilePath, const char *FileName, const char *ToonTexturePath);
        void release(struct PolygonModelDataContext *relCtx);

    private:
        const static int DefaultToonTextureSize = 10;
        FILE * fModel;
        //struct ToonTextureContext ToonTextureCtx[10];
        struct PolygonModelDataContext *ProfileCtx_ptr;
        float MaxModelSize[3];

        /* parse method */
        int parse_FileHeader();
        int parse_ModelHeader();
        int parse_VertexInformation();
        int parse_IndexInformation();
        int parse_MaterialInformation(const char *FilePath);
        int parse_BoneInformation();
        int parse_InverseKinematicsInformation();
        int parse_FaceMorphInformation();
        int parse_DisplayNameInformation();

        /* system method  */
        PMD_TextureContext *load_ToonTexture();
        PMD_TextureContext *load_Texture(const char *fPath, const char *fName);
        void parse_Boundary();
        void resize_Model();
    };
}
/* -------------------------------------------------------------------- */
#endif
