#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <sys/time.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glext.h>
#ifdef USEING_QT
#include <QGLFunctions>
#endif
#include <sutDataStruct.h>
#include "va.h"
// ---------------------------------------------------------------------------------
/* http://what-when-how.com/advanced-methods-in-computer-graphics/skeletal-animation-advanced-methods-in-computer-graphics-part-2/ */
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
using namespace VisualizationSystem;

static const float IdentityMatrix[16] ={1.0f, 0.0f, 0.0f, 0.0f,
                                        0.0f, 1.0f, 0.0f, 0.0f,
                                        0.0f, 0.0f, 1.0f, 0.0f,
                                        0.0f, 0.0f, 0.0f, 1.0f};
#define BONE_COLOR_NORMAL   0

const float JointColorSet[][4] = {{1.0f, 0.4f, 1.0f, 0.8f}, //0
                                  {0.0f, 1.0f, 0.0f, 1.0f},  //1
                                  {0.7f, 0.0f, 1.0f, 1.0f},  //2
                                  {1.0f, 1.0f, 0.0f, 1.0f},  //3
                                  {1.0f, 0.8f, 0.0f, 0.0f},  //4
                                  {1.0f, 0.0f, 1.0f, 1.0f},  //5
                                  {0.6f, 0.8f, 0.3f, 1.0f},  //6
                                  {0.1f, 0.5f, 0.8f, 1.0f},  //7
                                  {0.8f, 0.3f, 0.5f, 1.0f},  //8
                                  {0.5f, 0.1f, 0.8f, 1.0f},  //9
                                 };
static float DefaultDiffuse_RGBA[4] = {0.8f, 0.8f, 0.8f, 1.0f};
static float DefaultSpecular_RGB[3] = {0.2f, 0.2f, 0.2f};
static float DefaultAmbient_RGB[3] = {0.5f, 0.5f, 0.5f};
static float DefaultSpecularity = 1.0f;

/* ---------------------------------------------------------------------------------
 * Visualize the avatar.
 * This function will init the openGL shader for rendering avatar in first call.
 *
 * @param ProjectionMatrix : the opengl projection matrix in 4x4 float array.
 * @param ModelViewMatrix : the opengl model view matrix in 4x4 float array.
*/
void VisualizationAvatar::
visualize(float *ProjectionMatrix, float *ModelViewMatrix, unsigned int renderingCap)
{
    if(!isReincarnate()) {
        vaVisualization_f->reincarnate(memoryCtx, &visualizationCtx);
        AvatarStatus |= AvatarState_Reincarnation;
    }
    vaVisualization_f->visualize(memoryCtx, &surfaceCtx, &skeletonCtx, &visualizationCtx, ProjectionMatrix, ModelViewMatrix, renderingCap);
}

/* ---------------------------------------------------------------------------------
 * Generate the Slelecton Architecture.
 *
 */
int VisualizationAvatar::
generate_SkeletonArchitecture()
{
    int i;
    const int BoneInfoCount = memoryCtx->BoneMemory.BoneUnitCount;
    int CheckedParent = memoryCtx->BoneMemory.NullParentID; //start from root node.
    sutPoolQueue UnknowQueue;   /* Store the bone which praent unknow */
    sutPoolQueue RecognizeQueue;
    sutPoolQueue ChildSearchQueue;

    printf("generate_SkeletonArchitecture\n");
    RecognizeQueue.init(sutPoolQueue::ModeAuto, 128);
    UnknowQueue.init(sutPoolQueue::ModeAuto, 128);
    ChildSearchQueue.init(sutPoolQueue::ModeAuto, 128);
    skeletonCtx.chainNodeSet = (struct vaSkeletonNodeContext *)malloc(sizeof(struct vaSkeletonNodeContext) * BoneInfoCount);

    /* init unknow queue */
    skeletonCtx.chainNodeCount = memoryCtx->BoneMemory.BoneUnitCount ;
    skeletonCtx.chainNodeVertex = (float *)malloc(sizeof(float) * skeletonCtx.chainNodeCount *3);
    skeletonCtx.chainRoot.QuaternionCtx.init();
    skeletonCtx.chainRoot.globalQuaternion.init();
    skeletonCtx.chainRoot.isTouch =0;
    skeletonCtx.chainRoot.nID = memoryCtx->BoneMemory.NullParentID;
    skeletonCtx.chainRoot.Parnet = NULL;
    skeletonCtx.chainRoot.pRawBoneInfo = NULL;
    memcpy(&skeletonCtx.chainRoot.TransformMatrix, IdentityMatrix, sizeof(float)*16);

    /* init all chain node in the skeleton tree */
    float *pChainNodeVertex = skeletonCtx.chainNodeVertex;
    for(i = 0 ; i < BoneInfoCount ; i++) {
        struct vaSkeletonNodeContext *ChainNode_ptr = skeletonCtx.chainNodeSet +i;
        ChainNode_ptr->pRawBoneInfo = memoryCtx->BoneMemory.BoneUnit +i;
        ChainNode_ptr->nID = i;
        ChainNode_ptr->isTouch = 0;
        ChainNode_ptr->Parnet = NULL;
        ChainNode_ptr->Child = NULL;
        ChainNode_ptr->neighbor = NULL;
        ChainNode_ptr->ChildCount = 0;
        ChainNode_ptr->VertexChainCount =0;
        ChainNode_ptr->QuaternionCtx.init();
        ChainNode_ptr->globalQuaternion.init();
        ChainNode_ptr->pBoneVertex = skeletonCtx.chainNodeVertex + i *3 ;
        *pChainNodeVertex++ = ChainNode_ptr->pRawBoneInfo->BoneVertex[0] ;
        *pChainNodeVertex++ = ChainNode_ptr->pRawBoneInfo->BoneVertex[1];
        *pChainNodeVertex++ = ChainNode_ptr->pRawBoneInfo->BoneVertex[2];
        memcpy(ChainNode_ptr->TransformMatrix, IdentityMatrix, sizeof(float) *16);
        UnknowQueue.enqueue(ChainNode_ptr);
    }

    /* parse skeleton information */
    do {
        int CheckSize = UnknowQueue.count();    /* uncheck node remaining */
        for(i = 0 ; i < CheckSize ; i++) {
            struct vaSkeletonNodeContext *CheckBone = (struct vaSkeletonNodeContext *)UnknowQueue.dequeue();
            if(CheckBone->pRawBoneInfo->bID_Parent == CheckedParent) {
                ChildSearchQueue.enqueue(CheckBone);
                RecognizeQueue.enqueue(CheckBone);
            }
            else {
                UnknowQueue.enqueue(CheckBone);
            }
        }

        /* create the child map in checked parent node */
        if(ChildSearchQueue.count() >0) {
            int ChildCount = ChildSearchQueue.count();
            struct vaSkeletonNodeContext *ParentNode = (CheckedParent == memoryCtx->BoneMemory.NullParentID) ?
                                                          &skeletonCtx.chainRoot :
                                                          skeletonCtx.chainNodeSet +CheckedParent;
            ParentNode->ChildCount = ChildCount;
            ParentNode->Child = (struct vaSkeletonNodeContext **)malloc(sizeof(struct vaSkeletonNodeContext *) *ChildCount);
            if(ParentNode->Child == NULL) return 0;

            for(i = 0 ; i < ChildCount ; i++) {
                struct vaSkeletonNodeContext *pChild = (struct vaSkeletonNodeContext *)ChildSearchQueue.dequeue();
                *(ParentNode->Child +i) = pChild;
                pChild->Parnet = ParentNode;
                if(i > 0) (*(ParentNode->Child +i -1))->neighbor = pChild;
            }
        }

        /* swtich to next parent node which needced to find child */
        CheckedParent  = ((struct vaSkeletonNodeContext *)RecognizeQueue.dequeue())->nID;
    } while(UnknowQueue.count() >0);
    RecognizeQueue.release();
    UnknowQueue.release();
    ChildSearchQueue.release();
    return 1;
}

/* ---------------------------------------------------------------------------------
 * Convert the data buffer and surface buffer into vertex array.
 */
int VisualizationAvatar::
generate_BodySurface()
{
    int i;
    printf("generate_BodySurface %d\n", memoryCtx->VertexMemory.VertexUnitCount);
    surfaceCtx.Vertex = (float *)malloc(sizeof(float) * memoryCtx->VertexMemory.VertexUnitCount * 3);
    surfaceCtx.Normal = (float *)malloc(sizeof(float) * memoryCtx->VertexMemory.VertexUnitCount * 3);
    surfaceCtx.Texture = (float *)malloc(sizeof(float) * memoryCtx->VertexMemory.VertexUnitCount * 2);

    memcpy(surfaceCtx.Vertex, memoryCtx->VertexMemory.Vertex, sizeof(float) * memoryCtx->VertexMemory.VertexUnitCount * 3);
    memcpy(surfaceCtx.Normal, memoryCtx->VertexMemory.Normal, sizeof(float) * memoryCtx->VertexMemory.VertexUnitCount * 3);
    memcpy(surfaceCtx.Texture, memoryCtx->VertexMemory.TexCoord, sizeof(float) * memoryCtx->VertexMemory.VertexUnitCount * 2);

    if(memoryCtx->MaterialMemory.MaterialUnitCount >0) {
        int IndexCounter =0;
        surfaceCtx.GroupCtxCount = memoryCtx->MaterialMemory.MaterialUnitCount;
        surfaceCtx.GroupCtx = (struct vaSurfaceGroupContext *)malloc(sizeof(struct vaSurfaceGroupContext) *surfaceCtx.GroupCtxCount);
        for(i = 0 ; i < surfaceCtx.GroupCtxCount ; i++) {
            struct vaSurfaceGroupContext *Group_ptr = surfaceCtx.GroupCtx +i;
            struct MaterialUnitContext *pMaterialUnit = (memoryCtx->MaterialMemory.MaterialUnit +i);
            Group_ptr->Ambient_RGBA     = pMaterialUnit->Ambient_RGBA;
            Group_ptr->Diffuse_RGBA     = pMaterialUnit->Diffuse_RGBA;
            Group_ptr->Specularity      = pMaterialUnit->Specularity;
            Group_ptr->Specular_RGBA    = pMaterialUnit->Specular_RGBA;
            Group_ptr->IndexCount       = pMaterialUnit->SurfaceCount;
            Group_ptr->TextureReference = pMaterialUnit->ToonReference;
            Group_ptr->Index_ptr        = memoryCtx->IndexMemory.IndexUnit + IndexCounter *memoryCtx->IndexMemory.IndexUnitTypeSize;
            IndexCounter += Group_ptr->IndexCount ;
        }
    }
    else {
        surfaceCtx.GroupCtxCount = 1;
        surfaceCtx.GroupCtx = (struct vaSurfaceGroupContext *)malloc(sizeof(struct vaSurfaceGroupContext));
        struct vaSurfaceGroupContext *Group_ptr = surfaceCtx.GroupCtx;
        Group_ptr->Ambient_RGBA     = DefaultAmbient_RGB;
        Group_ptr->Diffuse_RGBA     = DefaultDiffuse_RGBA;
        Group_ptr->Specularity      = DefaultSpecularity;
        Group_ptr->Specular_RGBA    = DefaultSpecular_RGB;
        Group_ptr->IndexCount       = memoryCtx->IndexMemory.IndexUnitCount;
        Group_ptr->Index_ptr        = memoryCtx->IndexMemory.IndexUnit ;
        printf("Group_ptr->IndexCount  %d\n", Group_ptr->IndexCount );
    }
    return 0;
}
// ---------------------------------------------------------------------------------
int VisualizationAvatar::
generate_SkeletonSurface()
{
    int i, ei;

    printf("generate_SkeletonSurface\n");

    // calculate the count of node which have childs.
    skeletonCtx.renderingCount_Bone  = 0;
    skeletonCtx.renderingCount_Joint = 0;
    for(i = 0 ; i < skeletonCtx.chainNodeCount ; i++) {
        struct vaSkeletonNodeContext *pChainNode = skeletonCtx.chainNodeSet +i;
        if(pChainNode->ChildCount > 0 && pChainNode !=  &skeletonCtx.chainRoot) skeletonCtx.renderingCount_Joint ++;
        if((pChainNode->Parnet != NULL) && (pChainNode->Parnet != &skeletonCtx.chainRoot) && (pChainNode->Parnet->nID !=0) ) skeletonCtx.renderingCount_Bone ++;
    }

    /* Generate data
     * 4 pointer to rendering joint picture, the 4 elements of skeleton vertex are xyz, and the joint offset in glsl.
     * The following comment shows the node vertex, v[NODE_ID]:X,Y,Z,joint offset.
     *
     *  v[1]:x,y,z,-1/+1            v[2]:x,y,z,+1/+1
     *
     *                v[0]:x,y,z,0/0
     *
     *  v[3]:x,y,z,-1/-1            v[4]:x,y,z,+1/-1
    */
    skeletonCtx.renderingVertex = (float *)malloc(sizeof(float) * skeletonCtx.chainNodeCount *SKELETON_RENDERING_NODE_ELEMENTS *SKELETON_RENDERING_VERTEX_ELEMENTS);
    skeletonCtx.renderingColor = (float *)malloc(sizeof(float) * skeletonCtx.chainNodeCount *SKELETON_RENDERING_NODE_ELEMENTS *4);
    skeletonCtx.renderingIndex_Joint = (unsigned short *)malloc(sizeof(unsigned short) * skeletonCtx.renderingCount_Joint *4);    // 4 point as QUADS
    skeletonCtx.renderingIndex_Bone = (unsigned short *)malloc(sizeof(unsigned short) * skeletonCtx.renderingCount_Bone *4);    // 3 point as triangle
    unsigned short *renderingIndex_Bone  = skeletonCtx.renderingIndex_Bone ;
    unsigned short *renderingIndex_Joint = skeletonCtx.renderingIndex_Joint;
    float *renderingColor                = skeletonCtx.renderingColor;
    float *renderingVertex               = skeletonCtx.renderingVertex;
    struct BoneUnitContext *pRawBoneInfo = memoryCtx->BoneMemory.BoneUnit ;

    for(i = 0 ; i < skeletonCtx.chainNodeCount ; i++) {
        struct vaSkeletonNodeContext *pChainNode = skeletonCtx.chainNodeSet +i;
        if(pChainNode->ChildCount > 0 && pChainNode !=  &skeletonCtx.chainRoot) {
            *renderingIndex_Joint++ = SKELETON_RENDERING_NODE_ELEMENTS * i +1;
            *renderingIndex_Joint++ = SKELETON_RENDERING_NODE_ELEMENTS * i +2;
            *renderingIndex_Joint++ = SKELETON_RENDERING_NODE_ELEMENTS * i +4;
            *renderingIndex_Joint++ = SKELETON_RENDERING_NODE_ELEMENTS * i +3;
        }
        if((pChainNode->Parnet != NULL) && (pChainNode->Parnet != &skeletonCtx.chainRoot) && (pChainNode->Parnet->nID !=0)) {
            *renderingIndex_Bone++ = SKELETON_RENDERING_NODE_ELEMENTS * pRawBoneInfo->bID_Parent +3;
            *renderingIndex_Bone++ = SKELETON_RENDERING_NODE_ELEMENTS * i;
            *renderingIndex_Bone++ = SKELETON_RENDERING_NODE_ELEMENTS * pRawBoneInfo->bID_Parent +4;
            *renderingIndex_Bone++ = SKELETON_RENDERING_NODE_ELEMENTS * i;
        }
        for(ei = 0 ; ei < SKELETON_RENDERING_NODE_ELEMENTS ; ei++) {
            if(i == 14)
                memcpy(renderingColor, &JointColorSet[1][0], sizeof(float) *4);
            else
                memcpy(renderingColor, &JointColorSet[0][0], sizeof(float) *4);
            memcpy(renderingVertex, pRawBoneInfo->BoneVertex, sizeof(float) *3);
            renderingVertex[3] = ei;
            renderingColor += 4;
            renderingVertex += SKELETON_RENDERING_VERTEX_ELEMENTS;
        }
        pRawBoneInfo++;
    }
    return 1;
}

// ---------------------------------------------------------------------------------
int VisualizationAvatar::
setBoneQuaternion(int dstBone_ID, \
                  float Quaternion_ax, float Quaternion_by, float Quaternion_cz, float Quaternion_w,
                  float Translate_x, float Translate_y, float Translate_z,
                  PostureModeSet setMode)
{
    sutQuaternionOperator Quaternion;
    struct vaSkeletonNodeContext *fBone = skeletonCtx.chainNodeSet +dstBone_ID;

    if(dstBone_ID > memoryCtx->BoneMemory.BoneUnitCount) {
        printf("Unhandle bone in model : %d\n", dstBone_ID);
        return 0;
    }

    // if quaternion is identity

    fBone->isTouch = 1;
    switch(setMode) {
    case PostureMode_Replace:
        if(!(Quaternion_ax == 0.0f &&  Quaternion_by == 0.0f && Quaternion_cz == 0.0f && Quaternion_w == 1.0f)) {
            fBone->globalQuaternion.setQuaternion(Quaternion_ax, Quaternion_by, Quaternion_cz, Quaternion_w);
            fBone->QuaternionCtx.setQuaternion(Quaternion_ax, Quaternion_by, Quaternion_cz, Quaternion_w);
        }
        fBone->QuaternionCtx.setTranslate(Translate_x, Translate_y, Translate_z);
        break;
    case PostureMode_Overlap_Mul:
        Quaternion.init();
        Quaternion.setQuaternion(Quaternion_ax, Quaternion_by, Quaternion_cz, Quaternion_w);
        Quaternion.setTranslate(Translate_x, Translate_y, Translate_z);
        fBone->QuaternionCtx.mul(&Quaternion, sutQuaternionOperator::INPUT_AS_LEFT);
        break;
    case PostureMode_Overlap_Add:
        Quaternion.setQuaternion(Quaternion_ax, Quaternion_by, Quaternion_cz, Quaternion_w);
        Quaternion.setTranslate(Translate_x, Translate_y, Translate_z);
        fBone->QuaternionCtx.add(&Quaternion);
    }

    //printf("bID %d, set quaternion : ", fBone->nID);
    //fBone->globalQuaternion.dump();
    return 1;
}

/* ---------------------------------------------------------------------------------
 * Setup the posture into avatar.
 */
void VisualizationAvatar::posture(vaPostureMemoryContext *poseCtx)
{
    static float AvgCounter =0;
    static float AvgTime =0;
    int bi, ni;
    struct timeval pStart, pEnd;
    float pTime, dTime;

    if(poseCtx == NULL) return;

    gettimeofday(&pStart, NULL);
    if((memoryCtx != NULL) && (MaterializeElements & MaterializeElement_Skeleton)) {
        for(bi = 0 ; bi < poseCtx->poseUnitCount ; bi++) {
            struct vaPostureMemoryUnit *pPoseUnit = poseCtx->poseUnit +bi ;
            struct vaSkeletonNodeContext *targetBone = (struct vaSkeletonNodeContext *)pPoseUnit->targetBone;
            this->setBoneQuaternion(targetBone->nID,
                                    pPoseUnit->quaternion[0],
                                    pPoseUnit->quaternion[1],
                                    pPoseUnit->quaternion[2],
                                    pPoseUnit->quaternion[3],
                                    pPoseUnit->translate[0] * memoryCtx->GeometryCtx.ScaleRate,
                                    pPoseUnit->translate[1] * memoryCtx->GeometryCtx.ScaleRate,
                                    pPoseUnit->translate[2] * memoryCtx->GeometryCtx.ScaleRate,
                                    PostureMode_Replace);
        }

        // posture
        gettimeofday(&pEnd, NULL);
        pTime = ((pEnd.tv_sec -pStart.tv_sec)*1000000.0 +(pEnd.tv_usec -pStart.tv_usec)) /1000.0f;
        printf("Posture time : %.2lf ms, ", pTime);

        // 1. update the transform matrix in each bone.
        vaMotionFunction_f->update_SkeletonTransformMatrix(&skeletonCtx, NULL);

        // 2. normalize global quaternion.
        for(ni = 0 ; ni < skeletonCtx.chainNodeCount ; ni++) {
            (skeletonCtx.chainNodeSet +ni)->globalQuaternion.normalize();
        }

        // 2. update the bone vertex in skeleton .
        vaMotionFunction_f->update_SkeletonVertex(&skeletonCtx, &skeletonCtx.chainRoot);

        // 3. apply inverse kinematics algorithm, this algorithm will update the bone vertex in IK chain again.
        vaMotionFunction_f->update_InverseKinematics(memoryCtx, &skeletonCtx);

        // 4. update the surface vertex for rendering.
        if(MaterializeElements & MaterializeElement_Body) vaMotionFunction_f->update_BodySurfaceVertex(memoryCtx, &skeletonCtx, &surfaceCtx);
        if(MaterializeElements & MaterializeElement_Skeleton) vaMotionFunction_f->update_SkeletonSurfaceVertex(&skeletonCtx);

        // 5. Distouch the node.
        for(ni = 0 ; ni < skeletonCtx.chainNodeCount ; ni++) {
            (skeletonCtx.chainNodeSet +ni)->isTouch = 0;
            (skeletonCtx.chainNodeSet +ni)->globalQuaternion.init();
        }

        gettimeofday(&pEnd, NULL);
        dTime = ((pEnd.tv_sec -pStart.tv_sec)*1000000.0 +(pEnd.tv_usec -pStart.tv_usec)) /1000.0f;
        AvgTime += dTime;
        AvgCounter++;
        printf("Skeleton transformation time : %.3lf ms , Average time : %.3lfms\n", dTime, AvgTime /AvgCounter );
    }
}

/** ---------------------------------------------------------------------------------
 * Set the GLOBAL ABSOLUTE location of avatar
 */
void VisualizationAvatar::location(float Tx, float Ty, float Tz)
{
    Location[0] = Tx;
    Location[1] = Ty;
    Location[2] = Tz;

    //In android , the origation of model must rotate 180 to fit the rotation vector working.
    //skeletonCtx.chainRoot.QuaternionCtx.asRotation(0, 1, 0, 180);
    skeletonCtx.chainRoot.QuaternionCtx.setTranslate(Tx, Ty, Tz);
}

/* ---------------------------------------------------------------------------------
 * Create the Skeleton and Sureface data from PMD data context
 */
int VisualizationAvatar::
materialize(vaMemoryContext *vMemoryCtx, unsigned int mElements)
{
    struct timeval pStart, pEnd;

    printf("Model Materialize\n");
    if(vMemoryCtx == NULL) return 0;    //param & elements check
    if(!((mElements & MaterializeElement_Body) && (vMemoryCtx->MaterializeElementBitFiled & MemoryElement_Body)))
        mElements &= ~MaterializeElement_Body;
    if(!((mElements & MaterializeElement_Skeleton) && (vMemoryCtx->MaterializeElementBitFiled & MemoryElement_Bone)))
        mElements &= ~MaterializeElement_Skeleton;

    memoryCtx = vMemoryCtx;
    MaterializeElements = mElements;

    gettimeofday(&pStart, NULL);
    if(MaterializeElements & MaterializeElement_Body)
        generate_BodySurface();

    if(MaterializeElements & MaterializeElement_Skeleton) {
        generate_SkeletonArchitecture();
        generate_SkeletonSurface();
    }
    AvatarStatus |= AvatarState_Materilize;
    gettimeofday(&pEnd, NULL);
    float uTime = (pEnd.tv_sec -pStart.tv_sec) *1000000.0f +(pEnd.tv_usec -pStart.tv_usec);
    printf("Materialize time : %lf ms\n", uTime /1000.0f);
    printf("--------------------------------------------------------------------\n");
    return 1;
}
// ---------------------------------------------------------------------------------
#ifdef USEING_QT
VisualizationAvatar::
VisualizationAvatar(QGLContext *GLContext)
{
    create();
    initializeGLFunctions(GLContext);
    vaVisualization_f = new vaVisualizationFunctions(GLContext);
    vaMotionFunction_f = new vaMotionFunctions();
}
#else
VisualizationAvatar::
VisualizationAvatar()
{
    create();
    vaVisualization_f = new vaVisualizationFunctions(GLContext);
    vaMotionFunction_f = new vaMotionFunctions();
}
#endif
// ---------------------------------------------------------------------------------
void VisualizationAvatar::create()
{
    uID = 0;
    MaterializeElements = MaterializeElement_Ghost;
    memoryCtx = NULL;
    AvatarStatus = 0;
    surfaceCtx.GroupCtxCount =0;
    Location[0] = 0;
    Location[1] = 0;
    Location[2] = 0;
    poseCtx.poseUnit = NULL;
    poseCtx.poseUnitCount = 0;
    memset(&surfaceCtx, 0, sizeof(vaSurfaceContext));
    memset(&skeletonCtx, 0, sizeof(vaSkeletonTreeContext));
}
// ---------------------------------------------------------------------------------
void VisualizationAvatar::release()
{

}

/** Set the camera location, this location is   by GLSL.
*/
void VisualizationAvatar::setCameraLocation(float cX, float cY, float cZ)
{
    vaVisualization_f->setCameraLocation(&visualizationCtx, cX, cY, cZ);
}

/** ---------------------------------------------------------------------------------
 * Set the ID in universe .
 *  This function is used in ParallelUniverse system, and it will automatinally setted by WorldLine system in universe.
 *
 *  But, if you don't use the ParallelUniverse system. you could setup the ID by your self to identify the Avatar.
 *
 *      @param uID : The ID of this avatar.
*/
void VisualizationAvatar::setUniverseID(unsigned int uID)
{
    this->uID = uID;
}
unsigned int VisualizationAvatar::getUniverseID()
{
    return uID;
}

/* ---------------------------------------------------------------------------------
 * Return the context of vertex array  in current obj file.
 *
*/
int VisualizationAvatar::isReincarnate()
{
    return (AvatarStatus & AvatarState_Reincarnation);
}

/** Return the internal posture memory
 *  @isFullCopy : Set up this flag will copy the current quaternion & translate data into posture memory,
 *                otherwise, it will return the posture memory with nameKey & bone associate ,but old quaternion & translate.
*/
struct vaPostureMemoryContext *VisualizationAvatar::getPostureMemoryCtx(int isFullCopy)
{
    /* Release & reset the posture memory when data dimension mismatch */
    if(poseCtx.poseUnitCount != skeletonCtx.chainNodeCount) {
        poseCtx.poseUnitCount = skeletonCtx.chainNodeCount;
        if(poseCtx.poseUnit != NULL) free(poseCtx.poseUnit);
        poseCtx.poseUnit = (struct vaPostureMemoryUnit *)malloc(sizeof(vaPostureMemoryUnit) * poseCtx.poseUnitCount);
        isFullCopy = 1;
    }

    /* Copy data into posture memory */
    if(isFullCopy) {
        for(int i = 0 ; i < skeletonCtx.chainNodeCount ; i++) {
            struct vaPostureMemoryUnit *postureMemoryUnit = poseCtx.poseUnit +i;
            struct vaSkeletonNodeContext *chainNode = skeletonCtx.chainNodeSet +i;
            postureMemoryUnit->isEnable = 0;
            postureMemoryUnit->nameKey_UTF8_djb2 = chainNode->pRawBoneInfo->nameKey_djb2;
            postureMemoryUnit->nameKey_UTF8_DKDR = chainNode->pRawBoneInfo->nameKey_DKDR;
            postureMemoryUnit->targetBone = chainNode;
            chainNode->QuaternionCtx.getQuaternion(postureMemoryUnit->quaternion);
            chainNode->QuaternionCtx.getTranslate(postureMemoryUnit->translate);
        }
    }
    return &poseCtx;
}
