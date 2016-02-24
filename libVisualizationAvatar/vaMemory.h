#ifndef vaMemory_H
#define vaMemory_H
/* ------------------------------------------------------------------ */
#include "PMD_Profile.h"
#include "PostureMemory.h"
#include "PMX_Profile.h"
#include <sutQuaternion.h>
// ------------------------------------------------------------------
//mental model
using namespace MMDCppDev;

namespace VisualizationSystem {
    /** --------------------------------------------------------------------
     * The Axis Boundary Context.
     *      This struct contain the boundary information of raw PMD model and fixed PMD model
    */
    struct GeometryContext {
        float Center[3];       /* Center point */
        float Size[3];         /* 0-Width(x), 1-Height(y), 2-Length(2)*/
        float Boundary[3][2];  /* [0][0] Max X, [0][1] Min X
                                  [1][0] Max Y, [1][1] Min Y
                                  [2][0] Max Z, [2][1] Min Z */
        float RawCenter[3];    /* Original Model information */
        float RawSize[3];
        float RawBoundary[3][2];
        float ScaleRate;
    };

    /** --------------------------------------------------------------
     * The deform type of vertex point.
     * include 4 types : BDEF 1,
     *                   BDEF 2,
     *                   BDEF 4
     *                   SDEF     The deform function is under constructed.
    */
    struct DeformType_BDEF1 {
        int vID;
        unsigned int BoneIndex;
    };

    struct DeformType_BDEF2 {
        int vID;
        unsigned int BoneIndex_1;
        unsigned int BoneIndex_2;
        float        Weight_1;
    };

    struct DeformType_BDEF4 {
        int vID;
        unsigned int BoneIndex_1;
        unsigned int BoneIndex_2;
        unsigned int BoneIndex_3;
        unsigned int BoneIndex_4;
        float        Weight_1;
        float        Weight_2;
        float        Weight_3;
        float        Weight_4;
    };

    struct DeformType_SDEF {
        int vID;
        unsigned int BoneIndex_1;
        unsigned int BoneIndex_2;
        float        Weight_1;
        float        C[3];
        float        R0[3];
        float        R1[3];
    };

    /* The Vertex Unit to store vertex data, this Unit carried the index only.
     * The data pointer is stored in ElementMemory.
    */
    struct VertexUnitContext {
        int             vID;            // Vertex data Index
        int             addID;          // Additional data index
        int             DeformID;
        unsigned char   DeformType;
        float EdgeScale;
    };

    /** The main context of vertex memory, this struct carried the vertex information of PMX/PMD files
     *  Include : Vertex data
     *            Include data
     *            Deform data
    */
    struct VertexMemoryContext {
        //--------------- Vertex Data ---------------
        struct VertexUnitContext *VertexUnit;
        int     VertexUnitCount;
        float  *Vertex;
        float  *Normal;
        float  *TexCoord;
        //--------------- Deform Data ---------------
        int     BDEF1Count;
        int     BDEF2Count;
        int     BDEF4Count;
        int     SDEFCount;
        struct DeformType_BDEF1 *BDEF1;
        struct DeformType_BDEF2 *BDEF2;
        struct DeformType_BDEF4 *BDEF4;
        struct DeformType_SDEF  *SDEF;
    };

    //--------------- Index Data ---------------
    struct IndexMemoryContext {
        int IndexUnitCount;
        int IndexUnitTypeSize;     // The size of IndexUnit.
        unsigned char *IndexUnit;  //the type of IndexUnit could be char/short/int.
    };

    /** --------------------------------------------------------------------------
     * The Texture Context to store texture data.
     *      include : Toon Texture.
     *                Texture (for associate material)
    */
    struct TextureContext {
        int Width;
        int Height;
        int ByteLen;
        unsigned char *RGBA8888;
    };

    struct TextureMemoryContext {
        int ToontextureCount;
        struct TextureContext **ToonTextureImg ;
        int TextureCount;
        struct TextureContext **TextureImg ;
    };

    /** --------------------------------------------------------------------------
     *  The material Unit to store vertex data, this Unit carried the index only.
     * The data pointer is stored in ElementMemory.
    */
    struct MaterialUnitContext {
        unsigned int    JPnameKey_DKDR;
        unsigned int    JPnameKey_djb2;
        float           Diffuse_RGBA[4];
        float           Specular_RGBA[4];
        float           Ambient_RGBA[4];
        float           Specularity;
        int             SurfaceCount;
        unsigned char   bitFlag;
        float           Edge_RGBA[4];
        float           Edge_Scale;
        int             TextureIndex;
        int             sphereTextureIndex;
        unsigned char   sphereTextureBlendMode;
        unsigned char   ToonReference;
        unsigned int    ToonValue;
    };

    /* The main context of material memory, this struct carried the material information of PMX/PMD files
     *  Include : Material data
     *            Texture
    */
    struct MaterialMemoryContext {
        int MaterialUnitCount;
        struct MaterialUnitContext *MaterialUnit;
    };


    /** --------------------------------------------------------------------------
     *  The Bone Unit to store vertex data,.
    */
    struct IKChainUnit {
        unsigned int    LinkBone;
        unsigned char   isLimit;
        float           UpBound[3];
        float           LowerBound[3];
        /* Augmented matrix/vertex is used to calculate the process matrix
         * in CCD inverse kinematic algorithm. This matrix means the "local"
         * translation/rotation in the CCD process.
        */
        float           cVertex[3];
        float           augmentedMatrix[16];
        sutQuaternionOperator IKQuaternion;
    };

    struct IKChainContext {
        unsigned int    ReachBoneID;
        unsigned int    TargetBoneID;
        int             MaxIterations;
        float           MaxAngle;
        int             IKChainCount;
        struct IKChainUnit *IKChain;
    };

    enum BoneTypeBits {
        BoneType_Linkto         = 0x0001, //0x0001  : 接続先(PMD子ボーン指定)表示方法 -> 0:座標オフセットで指定 1:ボーンで指定
        BoneType_isRotate       = 0x0002, //0x0002  : 回転可能
        BoneType_isMove         = 0x0004, //0x0004  : 移動可能
        BoneType_isShow         = 0x0008, //0x0008  : 表示
        BoneType_isAccess       = 0x0010, //0x0010  : 操作可
        BoneType_IK             = 0x0020, //0x0020  : IK
        BoneType_LocalGrant     = 0x0080, //0x0080  : ローカル付与 | 付与対象 0:ユーザー変形値／IKリンク／多重付与 1:親のローカル変形量
        BoneType_RotateGrant    = 0x0100, //0x0100  : 回転付与
        BoneType_MoveGrant      = 0x0200, //0x0200  : 移動付与
        BoneType_AxisFix        = 0x0400, //0x0400  : 軸固定
        BoneType_LocalAxis      = 0x0800, //0x0800  : ローカル軸
        BoneType_PhysicalDeform = 0x1000, //0x1000  : 物理後変形
        BoneType_ExteranlDeform = 0x2000, //0x2000  : 外部親変形
    };

    struct BoneUnitContext {
        int             bID;
        unsigned int    nameKey_DKDR;
        unsigned int    nameKey_djb2;
        float           BoneVertex[3];
        int             bID_Parent;
        int             DeformableLevel;
        unsigned short  BoneType;
        // -------------- Type data --------------
        float           BoneType_0x0001_BoneOffset[3];
        int             BoneType_0x0001_BoneIndex;
        int             BoneType_0x0100_BoneIndex;
        float           BoneType_0x0100_Percent;
        float           BoneType_0x0400_DirectonVertex[3];
        float           BoneType_0x0800_Vertex_X[3];
        float           BoneType_0x0800_Vertex_Z[3];
        int             BoneType_0x2000_Key;
    };

    struct BoneMemoryContext {
        int NullParentID;
        struct BoneUnitContext *BoneUnit;
        int BoneUnitCount;
        int IKChainCount;
        struct IKChainContext *IKChain;
    };

    /* -------------------------------------------------------------- */
    enum MemoryElementBits {
        MemoryElement_Body      = 0x01,
        MemoryElement_Material  = 0x02,
        MemoryElement_Bone      = 0x04,
        MemoryElement_Face      = 0x10,
        MemoryElement_FULL      = 0xFF
    };

    /* --------------------------------------------------------------------
     * The Context of Visualization Memory.
     * This memory is vreate from PMX or PMX context.
     */
    struct vaMemoryContext {
        unsigned int mID;
        unsigned int MaterializeElementBitFiled;    //The bit setted from MemoryElementBits.
        struct GeometryContext          GeometryCtx;
        struct VertexMemoryContext      VertexMemory;
        struct IndexMemoryContext       IndexMemory;
        struct MaterialMemoryContext    MaterialMemory;
        struct TextureMemoryContext     TextureMemory;
        struct BoneMemoryContext        BoneMemory;
    };

    class vaMemory {
    public:
        struct vaMemoryContext *create(struct PolygonModeleXtendedContext *srcPMX, unsigned int mElements);
        struct vaMemoryContext *create(struct PolygonModelDataContext *srcPMD, unsigned int mElements);
        void release(struct vaMemoryContext *srcVM);

    private :
        struct vaMemoryContext *vMemoryCtx;

        /* generate data information */
        int generate_VertexMemory(void *srcData, int srcMode);
        int generate_VertexMemory_Soruce_PMD(struct PolygonModelDataContext *srcPMD);
        int generate_VertexMemory_Soruce_PMX(struct PolygonModeleXtendedContext *srcPMX);

        int generate_IndexMemory(void *srcData, int srcMode);
        int generate_IndexMemory_Soruce_PMD(struct PolygonModelDataContext *srcPMD);
        int generate_IndexMemory_Soruce_PMX(struct PolygonModeleXtendedContext *srcPMX);

        int generate_MaterialMemory(void *srcData, int srcMode);
        int generate_MaterialMemory_Soruce_PMD(struct PolygonModelDataContext *srcPMD);
        int generate_MaterialMemory_Soruce_PMX(struct PolygonModeleXtendedContext *srcPMX);

        int generate_TextureMemory(void *srcData, int srcMode);
        int generate_TextureMemory_Soruce_PMD(struct PolygonModelDataContext *srcPMD);
        int generate_TextureMemory_Soruce_PMX(struct PolygonModeleXtendedContext *srcPMX);

        int generate_BoneMemory(void *srcData, int srcMode);
        int generate_BoneMemory_Soruce_PMD(struct PolygonModelDataContext *srcPMD);
        int generate_BoneMemory_Soruce_PMX(struct PolygonModeleXtendedContext *srcPMX);
    };
}
/* ------------------------------------------------------------------ */
#endif // vaMemory_H
