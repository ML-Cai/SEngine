#ifndef VisualizationAvatar_H
#define VisualizationAvatar_H
/* ------------------------------------------------------------------ */
#include <sutQuaternion.h>
#include "PMD_Profile.h"
#include "PostureMemory.h"
#include "PMX_Profile.h"
#include "VMD_Profile.h"
#include "VisualizationMemory.h"
// ------------------------------------------------------------------
//mental model
namespace MMDCppDev {
    // --------------------------------------------------------------------
    struct VisualizationShaderProgram {
        unsigned int VertexShader;
        unsigned int FragmentShader;
        unsigned int Program;
    };

    struct VisualizationContext {
        int isVisualize;
        struct VisualizationShaderProgram DefaultShader;
        struct VisualizationShaderProgram ToonTextureShader;
        struct VisualizationShaderProgram MaterialTextureShader;
        unsigned int *MaterialTexture;
    };

    /* --------------------------------------------------------------------
     * The openGL Vertex Array Context
     *  This context include the vertex array, such as vertex, normal, texture and surface index for openGL of obj files.
    */
    struct SurfaceGroupContext {
        float *Diffuse_RGBA;
        float *Specularity;
        float *Specular_RGB;
        float *Ambient_RGB;
        unsigned char *Index_ptr;
        int IndexCount;
        int TextureReference;
    };

    struct GLSurfaceRenderingContext {
        float *Vertex;
        float *Noraml;
        float *Texture;
        unsigned char *Index;
        char *Edge;
        struct SurfaceGroupContext *GroupCtx;
        int GroupCtxCount;
    };
    /* ----------------------------------*/
    struct GLSkeletonRenderingContext {
        int JointCount;
        float *JointVertex;
        float *JointColor;
        int BoneCount;
        unsigned short *BoneIndex;
    };

    // --------------------------------------------------------------
    struct SkeletonChainContext {
        int nID;
        int isTouch;
        struct BoneUnitContext *RawBoneInfo_ptr;
        struct SkeletonChainContext *Parnet;
        struct SkeletonChainContext **Child;
        int ChildCount;
        int VertexChainCount;

        // motion data
        void *Motion_ptr;
        void *Frame1_ptr;
        void *Frame2_ptr;
        int FrameInMotion_Index;

        /* transform matrix */
        sutQuaternionOperator WorldQuaternion;
        sutQuaternionOperator QuaternionCtx;
        float *BoneVertex;
        float TransformMatrix[16];  // TransformMatrix = [Rotation Base Matrix] [Quaternion Rotation Matrix] [-Rotation Base Matrix]
    };

    // --------------------------------------------------------------
    struct ModelSkeletonArchitectureContext {
        struct SkeletonChainContext ChainRoot;
        struct SkeletonChainContext *ChainNodeSet;
        int JointCount;
        int ChainNodeCount;
    };

    /* -------------------------------------------------------------- */
    enum MaterializeElementSet{
        MaterializeElement_Ghost       =0x0,
        MaterializeElement_Skeleton    =0x1,
        MaterializeElement_Body        =0x2,
        MaterializeElement_Face        =0x4,
        MaterializeElement_FULL        =0xF
    };
    enum PostureModeSet{
        PostureMode_Replace        = 0x1,
        PostureMode_Overlap_Mul    = 0x2,
        PostureMode_Overlap_Add    = 0x4,
    };
    enum {
        AvatarState_Materilize      = 0x01,
        AvatarState_Reincarnation   = 0x02
    };

    /* -------------------------------------------------------------- */
#ifdef USEING_QT
    class VisualizationAvatar : protected QGLFunctions {
#else
    class VisualizationAvatar {
#endif
    public:
#ifdef USEING_QT
        VisualizationAvatar(QGLContext *GLContext);
#else
        VisualizationAvatar();
#endif
        void create();
        void release();
        int materialize(struct VisualizationMemoryContext *vMemoryCtx, unsigned int mElements);
        int reincarnate();
        int visualize(float *ProjectionMatrix, float *ModelViewMatrix);
        int motion(VocaloidMotionDataContext *vMotionCtx, float presentTimeStamp);
        void location(float Tx, float Ty, float Tz);
        int posture(PostureMemoryContext *PoseCtx, PostureModeSet pMode);
        int posture(PostureMemoryContext *PoseCtx_op, PostureMemoryContext *PoseCtx_ed, float InterpolationT);
        int posture(float FramID);

        int isReincarnate();
        int isMaterialize();

        struct VisualizationMemoryContext *get_Spirit();
        GLSurfaceRenderingContext *get_VertexArrayCtx();
        GLSkeletonRenderingContext *get_SkeletonRenderingCtx();

        /* set fiunction */
        void setUniverseID(unsigned int uID);
        unsigned int getUniverseID();
        int set_BoneQuaternionParam(int TargetDone_ID, float Quaternion_ax, float Quaternion_by, float Quaternion_cz, float Quaternion_w,
                                    float Translate_x, float Translate_y, float Translate_z,
                                    PostureModeSet setMode);
    private :
        unsigned int aID;   //Avatar ID, setup by programmer.
        unsigned int uID;   //Universe ID, setup by Parallel universe.
        unsigned int AvatarStatus;
        unsigned int MaterializeElements;
        struct VisualizationContext VisualCtx ;
        struct VisualizationMemoryContext *Spirit;
        struct ModelSkeletonArchitectureContext MasterSkeletonCtx;
        struct GLSurfaceRenderingContext BodyRederingCtx;
        struct GLSkeletonRenderingContext SkeletonRenderingCtx ;
        struct VocaloidMotionDataContext *CurrentMotionCtx_ptr;    //pointer to the motion data
        float TimeStamp_CurrentPresent;
        float TimeStamp_Last;
        float Location[3];

        /* generate data information */
        int loadShader(const char *srcContent, int Type);
        int genShaderProgram(struct VisualizationShaderProgram *dstShader, const char *srcVertexShder, const char *srcFragmentShader);
        int generate_SkeletonArchitecture();
        int generate_BodySurface();
        int generate_SkeletonSurface();

        void calculate_SkeletonTransformMatrix(struct SkeletonChainContext * fBone);
        void update_SkeletonTransformMatrix(struct SkeletonChainContext * fBone);
        void update_BodySurfaceVertex(struct SkeletonChainContext * fBone);
        void update_SkeletonSurfaceVertex(struct SkeletonChainContext * fBone);
        void update_InverseKinematics();
        void superpose_SkeletonTransformMatrix(struct SkeletonChainContext * fBone, float *srcMatrix);
        void reset_BodySurfaceVertex(struct SkeletonChainContext * fBone);
        void reset_SkeletonSurfaceVertex(struct SkeletonChainContext * fBone);
    };
}
/* ------------------------------------------------------------------ */
#endif // VisualizationAvatar_H
