#ifndef VisualizationAvatar_H
#define VisualizationAvatar_H
/* ------------------------------------------------------------------ */
#ifdef USEING_QT
#include <QGLFunctions>
#endif
#include <sutQuaternion.h>
#include "PMD_Profile.h"
#include "PostureMemory.h"
#include "PMX_Profile.h"
#include "VMD_Profile.h"
#include "vaMemory.h"
#include "vaPostureMemory.h"
// ------------------------------------------------------------------
//mental model
using namespace MMDCppDev;

namespace VisualizationSystem {

/** --------------------------------------------------------------------
 * The openGL Vertex Array Context
 *  This context include the vertex array, such as vertex, Normal, texture and surface index for openGL of obj files.
 */
struct vaSurfaceGroupContext {
    float *Diffuse_RGBA;
    float Specularity;
    float *Specular_RGBA;
    float *Ambient_RGBA;
    unsigned char *Index_ptr;
    int IndexCount;
    int TextureReference;
};

struct vaSurfaceContext {
    float *Vertex;
    float *Normal;
    float *Texture;
    unsigned char *Index;
    char *Edge;
    struct vaSurfaceGroupContext *GroupCtx;
    int GroupCtxCount;
};

// --------------------------------------------------------------
struct vaSkeletonNodeContext {
    int nID;
    int isTouch;
    struct BoneUnitContext *pRawBoneInfo;
    struct vaSkeletonNodeContext *Parnet;
    struct vaSkeletonNodeContext **Child;
    struct vaSkeletonNodeContext *neighbor;
    int ChildCount;
    int VertexChainCount;

    // transform matrix
    sutQuaternionOperator QuaternionCtx;
    sutQuaternionOperator globalQuaternion;
    float *pBoneVertex;         //point to the rendering vertex of skeletion
    float TransformMatrix[16];  // TransformMatrix = [Rotation Base Matrix] [Quaternion Rotation Matrix] [-Rotation Base Matrix]
};

// --------------------------------------------------------------
#define SKELETON_RENDERING_VERTEX_ELEMENTS  4
#define SKELETON_RENDERING_NODE_ELEMENTS    5
struct vaSkeletonTreeContext {
    int chainNodeCount;
    struct vaSkeletonNodeContext chainRoot;
    struct vaSkeletonNodeContext *chainNodeSet;
    float *chainNodeVertex; //vertex data set of chain node.
    float *renderingVertex;
    float *renderingColor;
    int renderingCount_Joint;
    int renderingCount_Bone;
    unsigned short *renderingIndex_Joint;
    unsigned short *renderingIndex_Bone;
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

/* Rendering Cap */
#define VA_RENDERING_NONE       0x0800
#define VA_RENDERING_BODY       0x0801
#define VA_RENDERING_SKELETON   0x0802
#define VA_RENDERING_NORMAL     0x0804


/** --------------------------------------------------------------
 * Visualization Avatar
*/
#ifdef USEING_QT
class VisualizationAvatar : protected QGLFunctions {
#else
class VisualizationAvatar {
#endif
public :
#ifdef USEING_QT
    VisualizationAvatar(QGLContext *GLContext);
#else
    VisualizationAvatar();
#endif
    void create();
    void release();
    int materialize(struct vaMemoryContext *vMemoryCtx, unsigned int mElements);
    void visualize(float *ProjectionMatrix, float *ModelViewMatrix, unsigned int renderingCap);
    int motion(VocaloidMotionDataContext *vMotionCtx, float presentTimeStamp);
    void location(float Tx, float Ty, float Tz);
    void posture(vaPostureMemoryContext *poseCtx);

    /* get methods */
    vaPostureMemoryContext *getPostureMemoryCtx(int isFullCopy);
    unsigned int getUniverseID();

    /* set methods */
    void setCameraLocation(float cX, float cY, float cZ);
    void setUniverseID(unsigned int uID);
    int setBoneQuaternion(int dstBone_ID, float Quaternion_ax, float Quaternion_by, float Quaternion_cz, float Quaternion_w,
                          float Translate_x, float Translate_y, float Translate_z,
                          PostureModeSet setMode);
private :
    unsigned int uID;   //Universe ID, setup by Parallel universe.
    unsigned int AvatarStatus;
    unsigned int MaterializeElements;
    struct vaMemoryContext          *memoryCtx;
    struct vaSkeletonTreeContext    skeletonCtx;
    struct vaSurfaceContext         surfaceCtx;
    struct vaPostureMemoryContext   poseCtx;
    //struct VocaloidMotionDataContext *CurrentMotionCtx_ptr;    //pointer to the motion data
    float Location[3];

    /** -------------------------------------------------------
     * The Part of visualization functions
    */
    int isReincarnate();

#ifdef USEING_QT
    class vaVisualizationFunctions : protected QGLFunctions {
#else
    class vaVisualizationFunctions {
#endif
    public :
#ifdef USEING_QT
        vaVisualizationFunctions(QGLContext *GLContext);
#else
        vaVisualizationFunctions();
#endif

        struct GLshaderProgram {
            unsigned int VertexShader;
            unsigned int FragmentShader;
            unsigned int Program;
            int L_ProjectionMatrix; //the location value in the shader.
            int L_ModelViewMatrix;
            int L_Vertex;
            int L_Color;
            int L_Normal;
            int L_TexCoord;
            int L_AmbientColor;
            int L_DiffuseColor;
            int L_inLightPosition;
            int L_uTexture;
            int L_CameraLocation;
        };

        struct Context {
            int isVisualize;
            struct GLshaderProgram skeletonShader;
            struct GLshaderProgram bodySurfaceShader;
            unsigned int emptyTexture;
            unsigned int *MaterialTexture;
            float cameraLocation[3];
        };

        void visualize(struct vaMemoryContext *inMemory,
                      struct vaSurfaceContext *inSurfaceCtx,
                      struct vaSkeletonTreeContext *inSkeletonCtx,
                      struct vaVisualizationFunctions::Context *visualizationCtx,
                      float *ProjectionMatrix, float *ModelViewMatrix, unsigned int renderingCap);
        int reincarnate(struct vaMemoryContext *inMemory,
                        struct vaVisualizationFunctions::Context *visualizationCtx);
        void setCameraLocation(struct vaVisualizationFunctions::Context *visualizationCtx,
                               float cX, float cY, float cZ);
    private :
        int loadShaderFromFileSystem(struct GLshaderProgram *dstShader, char *fPath, char *fName);
        int loadShader(const char *srcContent, int Type);
        int genShaderProgram(struct GLshaderProgram *dstShader, const char *srcVertexShder, const char *srcFragmentShader);

        void renderingBody(struct vaMemoryContext *inMemory,
                          struct vaSurfaceContext *inSurfaceCtx,
                          struct vaVisualizationFunctions::Context *visualizationCtx,
                          float *ProjectionMatrix, float *ModelViewMatrix);
        void renderingSkeletion(struct vaSkeletonTreeContext *inSkeletonCtx,
                               struct vaVisualizationFunctions::Context *visualizationCtx,
                               float *ProjectionMatrix, float *ModelViewMatrix);
    };
    vaVisualizationFunctions *vaVisualization_f;
    struct vaVisualizationFunctions::Context visualizationCtx;


    /* generate data information */
    int generate_SkeletonArchitecture();
    int generate_BodySurface();
    int generate_SkeletonSurface();


    /** -------------------------------------------------------
     * The Part of motion functions
    */
    class vaMotionFunctions {
    public :
        /* motion functions */
        void calculate_SkeletonTransformMatrix(struct vaSkeletonNodeContext * fBone);
        void update_SkeletonTransformMatrix(struct vaSkeletonTreeContext *skeletonCtx,
                                            struct vaSkeletonNodeContext * fBone);
        void update_BodySurfaceVertex(struct vaMemoryContext *vaMemory,
                                      struct vaSkeletonTreeContext *skeletonCtx,
                                      struct vaSurfaceContext *surfaceCtx);
        void update_SkeletonSurfaceVertex(struct vaSkeletonTreeContext *inSkeletonCtx);
        void update_SkeletonVertex(struct vaSkeletonTreeContext *skeletonCtx,
                                   struct vaSkeletonNodeContext * fBone);
        void update_InverseKinematics(struct vaMemoryContext *vaMemory,
                                      struct vaSkeletonTreeContext *skeletonCtx);
       // void superpose_SkeletonTransformMatrix(struct vaSkeletonNodeContext * fBone, float *srcMatrix);
    private :

    };
    vaMotionFunctions *vaMotionFunction_f;
};

}
/* ------------------------------------------------------------------ */
#endif // VisualizationAvatar_H
