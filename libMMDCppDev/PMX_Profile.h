#ifndef PMX_PROFILE_H_INCLUDED
#define PMX_PROFILE_H_INCLUDED
/* -------------------------------------------------------------------- */
#include <stdio.h>
/* -------------------------------------------------------------------- */
/* The Data buffer of obj files.
 *  This struct is uesed to carry obj data when parse the files.
*/
namespace MMDCppDev {
struct pmxVec2 {
    float x;
    float y;
};
struct pmxVec3 {
    float x;
    float y;
    float z;
};
struct pmxVec4 {
    float x;
    float y;
    float z;
    float w;
};

/* --------------------------------------------------------------------
 * The Axis Boundary Context.
 *      This struct contain the boundary information of raw PMD model and fixed PMD model
 */
struct PMX_ModelGeometryContext {
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
struct PMX_TextureContext {
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
struct PMX_GlobalParamSet {
    unsigned char GlobalParamCount;
    unsigned char TextEncoding; // 0 is UTF16, 1 is UTF8
    unsigned char AdditionalVec4Count;
    unsigned char VertexIndexSize;
    unsigned char TextureIndexSize;
    unsigned char MaterialIndexSize;
    unsigned char BoneIndexSize;
    unsigned char MorphIndexSize;
    unsigned char RigidbodyIndexSize;
};

struct PMX_Text {
    int Length;
    char *text_UTF8;
};

/* ----------------------------------*/
struct PMX_FileHeader {
    char    PMX[4];
    float   version;
    struct PMX_Text name_JP;
    struct PMX_Text name_UTF;
    struct PMX_Text comment_JP;
    struct PMX_Text comment_UTF;
} __attribute__((packed));


struct PMX_BDEF1 {
    unsigned int BoneIndex;
}__attribute__((packed));

struct PMX_BDEF2 {
    unsigned int BoneIndex_1;
    unsigned int BoneIndex_2;
    float Weight_1;
} __attribute__((packed));

struct PMX_BDEF4 {
    unsigned int BoneIndex_1;
    unsigned int BoneIndex_2;
    unsigned int BoneIndex_3;
    unsigned int BoneIndex_4;
    float Weight_1;
    float Weight_2;
    float Weight_3;
    float Weight_4;
} __attribute__((packed));

struct PMX_SDEF {
    unsigned int BoneIndex_1;
    unsigned int BoneIndex_2;
    float Weight_1;
    struct pmxVec3 C;
    struct pmxVec3 R0;
    struct pmxVec3 R1;
} __attribute__((packed));

/* ----------------------------------*/
struct PMX_VertexInformation {
    float           Vertex[3];
    float           Normal[3];
    float           Texture[2];
    float           Additional_UV[4 * 4];   //the Max size of additional UV
    unsigned char   DeformType;
    union {
        struct PMX_BDEF1 BDEF1;
        struct PMX_BDEF2 BDEF2;
        struct PMX_BDEF4 BDEF4;
        struct PMX_SDEF SDEF;
    };
    float EdgeScale;
} __attribute__((packed));

/* ----------------------------------*/
struct PMX_TextureInformation {
    struct PMX_Text name_US;
};

/* ----------------------------------*/
struct PMX_MaterialInformation {
    struct PMX_Text name_JP;
    struct PMX_Text name_US;
    float           Diffuse_RGBA[4];
    float           Specular_RGB[3];
    float           Specularity;
    float           Ambient_RGB[3];
    unsigned char   bitFlag;
    float           Edge_RGBA[4];
    float           Edge_Scale;
    int             TextureIndex;
    int             sphereTextureIndex;
    unsigned char   sphereTextureBlendMode;
    unsigned char   ToonReference;
    unsigned int    ToonValue;
    struct PMX_Text MetaData;
    int             SurfaceCount;
} __attribute__((packed));

/* ----------------------------------*/
struct PMX_InverseKinematicsChainContext {
    unsigned int    LinkBone;
    unsigned char   isLimit;
    float           UpBound[3];
    float           LowerBound[3];
} __attribute__((packed));

struct PMX_InverseKinematicsInformation {
    unsigned int    TargetBoneID;
    int             MaxIterations;
    float           MaxAngle;
    int             IKChainCount;
    struct PMX_InverseKinematicsChainContext *IKChain;
} __attribute__((packed));

/* ----------------------------------*/
struct PMX_BoneInformation {
    int             bID;
    struct PMX_Text name_JP;
    struct PMX_Text name_US;
    float           BoneVertex[3];
    int             bID_Parent;
    int             DeformableLevel;
    unsigned short  BoneType;
    //Type data
    float           TypeParam_0x0001_BoneOffset[3];
    int             TypeParam_0x0001_BoneIndex;
    int             TypeParam_0x0100_BoneIndex;
    float           TypeParam_0x0100_Percent;
    float           TypeParam_0x0400_DirectonVertex[3];
    float           TypeParam_0x0800_Vertex_X[3];
    float           TypeParam_0x0800_Vertex_Z[3];
    int             TypeParam_0x2000_Key;
    struct PMX_InverseKinematicsInformation TypeParam_0x0020_IK;
} __attribute__((packed));

/** -------------------------------------------------------------
 * Morph data parts
 */
struct MorphType_Vertex {
    unsigned int    vID;    //vertex id
    float           offset[3];
} __attribute__((packed));

struct MorphType_UV {
    unsigned int    vID;    //vertex id
    float           offset[4];
} __attribute__((packed));

struct MorphType_Bone {
    unsigned int    bID;    //bone id
    float           translation[3];
    float           rotation[4];
} __attribute__((packed));

struct MorphType_Material {
    unsigned int    mID;    //material id
    unsigned char   algorithType;
    float           diffuse[4];
    float           specular[3];
    float           Specularity;
    float           ambient[3];
    float           edgeColor[4];
    float           edgeSize;
    float           textureParam[4];
    float           sphereTextureParam[4];
    float           toonTextureParam[4];
} __attribute__((packed));

struct MorphType_Group {
    unsigned int    mID;    //morph id
    float           morphRate[4];
} __attribute__((packed));

struct PMX_MorphInformation {
    struct PMX_Text name_JP;
    struct PMX_Text name_US;
    unsigned char operatorType;
    unsigned char morphType;
    int morphCount;
    void *morphData;
} __attribute__((packed));



/* ----------------------------------*/
struct PolygonModeleXtendedContext {
    unsigned int CtxID;
    char *mPath;
    char *mName;
    struct PMX_ModelGeometryContext ModelGeometryCtx;
    struct PMX_GlobalParamSet GlobalParamSet;
    struct PMX_FileHeader FileHeaderInfo;

    int VertexInfoCount;
    struct PMX_VertexInformation *VertexInfo;

    int IndexInfoCount;
    unsigned char *IndexInfo;

    int MaterialInfoCount;
    struct PMX_MaterialInformation *MaterialInfo;
    int TextureInfoCount;
    struct PMX_TextureInformation *TextureInfo;

    int BoneInfoCount;
    struct PMX_BoneInformation *BoneInfo;
    //---------------------------------------------------------
    int InverseKinematicsInfoCount;

    //struct PMX_InverseKinematicsInformation *InverseKinematicsInfo;
    int MorphInfoCount;
    struct PMX_MorphInformation *MorphInfo;
};

/* --------------------------------------------------------------------
     * The Context of Polygon Model eXtended (.pmx) File.
     *
    */
class PMX_profile {
public:
    struct PolygonModeleXtendedContext *genContext(int ID, float MaxWidth, float MaxHeight, float MaxLength);
    int load(struct PolygonModeleXtendedContext *pmxCtx, const char *pmxPath, const char *pmxName, const char *pmxToonTexturePath);
    void release(struct PolygonModeleXtendedContext *relCtx);

private:
    const static int DefaultToonTextureSize = 10;
    FILE * fModel;
    struct PMX_TextureContext ToonTextureCtx[10];
    struct PolygonModeleXtendedContext *ProfileCtx_ptr;
    float MaxModelSize[3];

    /* parse method */
    int parse_FileHeader();
    int parse_VertexInformation();
    int parse_IndexInformation();
    int parse_MaterialInformation();
    int parse_TextureInformation();
    int parse_BoneInformation();
    int parse_MorphInformation();
    int parse_DisplayNameInformation();
    int Read_PMX_Text(FILE *fptr, struct PMX_Text *dstText);

    /* system method  */
    PMX_TextureContext *load_ToonTexture();
    PMX_TextureContext *load_Texture(char *fPath, char *fName);
    void parse_Boundary();
    void resize_Model();
};
}
/* -------------------------------------------------------------------- */
#endif
