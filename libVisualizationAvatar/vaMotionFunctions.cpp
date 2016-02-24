#include <stdio.h>
#include <math.h>
#include "va.h"
// ---------------------------------------------------------------------------------
using namespace MMDCppDev;
using namespace VisualizationSystem;
// ---------------------------------------------------------------------------------
const float IdentityMatrix[16] ={1.0f, 0.0f, 0.0f, 0.0f,
                                 0.0f, 1.0f, 0.0f, 0.0f,
                                 0.0f, 0.0f, 1.0f, 0.0f,
                                 0.0f, 0.0f, 0.0f, 1.0f};
/* ---------------------------------------------------------------------------------
 * Special Matrxi operator
 *
 *  MatrixOperator_Mul_4x4M_3x1V :
 *  | Matrix 0   Matrix 1   Matrix 2   Matrix 3  |     | 1   0   0   Vertex0 |
 *  | Matrix 4   Matrix 5   Matrix 6   Matrix 7  |     | 0   1   0   Vertex1 |
 *  | Matrix 8   Matrix 9   Matrix 10  Matrix 11 |     | 0   0   1   Vertex2 |
 *  | Matrix 12  Matrix 13  Matrix 14  Matrix 15 |     | 0   0   0   1       |
 *
 *  MatrixOperator_Mul_3x1V_4x4M :
 *  | 1   0   0   Vertex0 |     | Matrix 0   Matrix 1   Matrix 2   Matrix 3  |
 *  | 0   1   0   Vertex1 |     | Matrix 4   Matrix 5   Matrix 6   Matrix 7  |
 *  | 0   0   1   Vertex2 |     | Matrix 8   Matrix 9   Matrix 10  Matrix 11 |
 *  | 0   0   0   1       |     | Matrix 12  Matrix 13  Matrix 14  Matrix 15 |
 *
 */
static inline void MatrixOperator_Mul_4x4M_3x1V(float *inMatrix, float *inVertex)
{
    //if(outMatrix != inMatrix) memcpy(outMatrix, inMatrix, sizeof(float) *16);
    inMatrix[3] = (inMatrix[0] * inVertex[0] +
                    inMatrix[1] * inVertex[1] +
                    inMatrix[2] * inVertex[2] +
                    inMatrix[3]);
    inMatrix[7] = (inMatrix[4] * inVertex[0] +
                    inMatrix[5] * inVertex[1] +
                    inMatrix[6] * inVertex[2] +
                    inMatrix[7]);
    inMatrix[11] = (inMatrix[8] * inVertex[0] +
                    inMatrix[9] * inVertex[1] +
                    inMatrix[10] * inVertex[2] +
                    inMatrix[11]);
    inMatrix[15] = (inMatrix[12] * inVertex[0] +
                    inMatrix[13] * inVertex[1] +
                    inMatrix[14] * inVertex[2] +
                    inMatrix[15]);
}

static inline void MatrixOperator_Mul_3x1V_4x4M(float *inVertex, float *inMatrix)
{
    inMatrix[3] += inVertex[0];
    inMatrix[7] += inVertex[1];
    inMatrix[11] += inVertex[2];
}

/* ---------------------------------------------------------------------------------
 * Multiple the transform matrix with vertex
 *
 *      outMatrix = [inMatrix_A][inMatrix_B]
 *
 *  @param inMatrix_A, inMatrix_B : The input 4x4 float matrix.
 *  @param outMatrix : The output 4x4 matrix.
 */
static inline void MatrixOperator_Mul_4x4M_4x4M(float *inMatrix_A, float *inMatrix_B, float *outMatrix)
{
    outMatrix[0] = (inMatrix_A[0] * inMatrix_B[0] +
                    inMatrix_A[1] * inMatrix_B[4] +
                    inMatrix_A[2] * inMatrix_B[8] +
                    inMatrix_A[3] * inMatrix_B[12]);
    outMatrix[1] = (inMatrix_A[0] * inMatrix_B[1] +
                    inMatrix_A[1] * inMatrix_B[5] +
                    inMatrix_A[2] * inMatrix_B[9] +
                    inMatrix_A[3] * inMatrix_B[13]);
    outMatrix[2] = (inMatrix_A[0] * inMatrix_B[2] +
                    inMatrix_A[1] * inMatrix_B[6] +
                    inMatrix_A[2] * inMatrix_B[10] +
                    inMatrix_A[3] * inMatrix_B[14]);
    outMatrix[3] = (inMatrix_A[0] * inMatrix_B[3] +
                    inMatrix_A[1] * inMatrix_B[7] +
                    inMatrix_A[2] * inMatrix_B[11] +
                    inMatrix_A[3] * inMatrix_B[15]);
    //--------------------------------------------------
    outMatrix[4] = (inMatrix_A[4] * inMatrix_B[0] +
                    inMatrix_A[5] * inMatrix_B[4] +
                    inMatrix_A[6] * inMatrix_B[8] +
                    inMatrix_A[7] * inMatrix_B[12]);
    outMatrix[5] = (inMatrix_A[4] * inMatrix_B[1] +
                    inMatrix_A[5] * inMatrix_B[5] +
                    inMatrix_A[6] * inMatrix_B[9] +
                    inMatrix_A[7] * inMatrix_B[13]);
    outMatrix[6] = (inMatrix_A[4] * inMatrix_B[2] +
                    inMatrix_A[5] * inMatrix_B[6] +
                    inMatrix_A[6] * inMatrix_B[10] +
                    inMatrix_A[7] * inMatrix_B[14]);
    outMatrix[7] = (inMatrix_A[4] * inMatrix_B[3] +
                    inMatrix_A[5] * inMatrix_B[7] +
                    inMatrix_A[6] * inMatrix_B[11] +
                    inMatrix_A[7] * inMatrix_B[15]);
    //--------------------------------------------------
    outMatrix[8] = (inMatrix_A[8] * inMatrix_B[0] +
                    inMatrix_A[9] * inMatrix_B[4] +
                    inMatrix_A[10] * inMatrix_B[8] +
                    inMatrix_A[11] * inMatrix_B[12]);
    outMatrix[9] = (inMatrix_A[8] * inMatrix_B[1] +
                    inMatrix_A[9] * inMatrix_B[5] +
                    inMatrix_A[10] * inMatrix_B[9] +
                    inMatrix_A[11] * inMatrix_B[13]);
    outMatrix[10] = (inMatrix_A[8] * inMatrix_B[2] +
                    inMatrix_A[9] * inMatrix_B[6] +
                    inMatrix_A[10] * inMatrix_B[10] +
                    inMatrix_A[11] * inMatrix_B[14]);
    outMatrix[11] = (inMatrix_A[8] * inMatrix_B[3] +
                    inMatrix_A[9] * inMatrix_B[7] +
                    inMatrix_A[10] * inMatrix_B[11] +
                    inMatrix_A[11] * inMatrix_B[15]);
    //--------------------------------------------------
    outMatrix[12] = (inMatrix_A[12] * inMatrix_B[0] +
                     inMatrix_A[13] * inMatrix_B[4] +
                     inMatrix_A[14] * inMatrix_B[8] +
                     inMatrix_A[15] * inMatrix_B[12]);
    outMatrix[13] = (inMatrix_A[12] * inMatrix_B[1] +
                     inMatrix_A[13] * inMatrix_B[5] +
                     inMatrix_A[14] * inMatrix_B[9] +
                     inMatrix_A[15] * inMatrix_B[13]);
    outMatrix[14] = (inMatrix_A[12] * inMatrix_B[2] +
                     inMatrix_A[13] * inMatrix_B[6] +
                     inMatrix_A[14] * inMatrix_B[10] +
                     inMatrix_A[15] * inMatrix_B[14]);
    outMatrix[15] = (inMatrix_A[12] * inMatrix_B[3] +
                     inMatrix_A[13] * inMatrix_B[7] +
                     inMatrix_A[14] * inMatrix_B[11] +
                     inMatrix_A[15] * inMatrix_B[15]);
}

/* ---------------------------------------------------------------------------------
 * Multiple the transform matrix with vertex
 * Formulation :
 *      outVertex = inWeight [inMatrix_A] [inVertex]
 */
static inline void MulTransformMatrix(float *inMatrix, float *inVertex,
                                      float inWeight, float *outVertex)
{
    outVertex[0] = inWeight *(inMatrix[0] * inVertex[0] +
                              inMatrix[1] * inVertex[1] +
                              inMatrix[2] * inVertex[2] +
                              inMatrix[3]);
    outVertex[1] = inWeight *(inMatrix[4] * inVertex[0] +
                              inMatrix[5] * inVertex[1] +
                              inMatrix[6] * inVertex[2] +
                              inMatrix[7]);
    outVertex[2] = inWeight *(inMatrix[8] * inVertex[0] +
                              inMatrix[9] * inVertex[1] +
                              inMatrix[10] * inVertex[2] +
                              inMatrix[11]);
}
/* ---------------------------------------------------------------------------------
 * Multiple the transform matrix with vertex
 * Formulation :
 *      outVertex = inWeight [inMatrix_A] [inVertex]
 */
static inline void matrixMul_RotationOnly(float *inMatrix, float *inVertex, float inWeight, float *outVertex)
{
    outVertex[0] = inWeight *(inMatrix[0] * inVertex[0] +
                              inMatrix[1] * inVertex[1] +
                              inMatrix[2] * inVertex[2]);
    outVertex[1] = inWeight *(inMatrix[4] * inVertex[0] +
                              inMatrix[5] * inVertex[1] +
                              inMatrix[6] * inVertex[2]);
    outVertex[2] = inWeight *(inMatrix[8] * inVertex[0] +
                              inMatrix[9] * inVertex[1] +
                              inMatrix[10] * inVertex[2]);
}

/* ---------------------------------------------------------------------------------
 * Calculate the transform matrix of childs with parent's transform matrix
 * */
void VisualizationAvatar::vaMotionFunctions::
calculate_SkeletonTransformMatrix(struct vaSkeletonNodeContext * fBone)
{
    float matrix[16];
    float PostiveVertex[3] = {fBone->pRawBoneInfo->BoneVertex[0],
                              fBone->pRawBoneInfo->BoneVertex[1],
                              fBone->pRawBoneInfo->BoneVertex[2]};
    float NativeVertex[3] = {-fBone->pRawBoneInfo->BoneVertex[0],
                             -fBone->pRawBoneInfo->BoneVertex[1],
                             -fBone->pRawBoneInfo->BoneVertex[2]};

    memcpy(matrix, fBone->QuaternionCtx.getMatrix(), sizeof(float)*16);
    MatrixOperator_Mul_3x1V_4x4M(PostiveVertex, matrix);
    MatrixOperator_Mul_4x4M_3x1V(matrix, NativeVertex);
    MatrixOperator_Mul_4x4M_4x4M(fBone->Parnet->TransformMatrix, matrix, fBone->TransformMatrix);
}

/* ---------------------------------------------------------------------------------
 * Update the Skeleton Transform matrix and Body Vertex after any bone is setted Quaternion Param by set_BoneQuaternionParam()
 *
 *      @param : fBone : The bone context which is needed to update,
 *                       As NULL will update the skeleton from skeleton root.
*/
void VisualizationAvatar::vaMotionFunctions::
update_SkeletonTransformMatrix(struct vaSkeletonTreeContext *skeletonCtx,
                               struct vaSkeletonNodeContext *fBone)
{
    int ci, ChildCount;

    if(fBone == NULL) fBone = &skeletonCtx->chainRoot;
    if(fBone != &skeletonCtx->chainRoot) {
        if(fBone->Parnet->isTouch) fBone->isTouch = 1;
        calculate_SkeletonTransformMatrix(fBone);
    }
    else {  //The transform matrix in roochain is used to identify Obj asix and rotation.
        fBone->QuaternionCtx.toMatrix();
        memcpy(fBone->TransformMatrix, fBone->QuaternionCtx.getMatrix(), sizeof(float)*16);
    }

    // Recursive, update the child.
    ChildCount = fBone->ChildCount;
    for(ci = 0 ; ci < ChildCount ; ci++) {
        struct vaSkeletonNodeContext *tBone = *(fBone->Child +ci);
        update_SkeletonTransformMatrix(skeletonCtx, tBone);
    }
}

/* ---------------------------------------------------------------------------------
 * Update the vertex of skeleton surface
 * */
void VisualizationAvatar::vaMotionFunctions::
update_SkeletonVertex(struct vaSkeletonTreeContext *skeletonCtx,
                      struct vaSkeletonNodeContext * fBone)
{
    int ci;
    struct vaSkeletonNodeContext *SkeletonChain_ptr = fBone;

    /* Update Skeleton Bone Vertex */
    if(fBone != &skeletonCtx->chainRoot) {
        /* Using the RAW bone vertex to calcualte new vertex data with transform matrix */
        MulTransformMatrix(fBone->TransformMatrix, fBone->pRawBoneInfo->BoneVertex, 1.0f,  fBone->pBoneVertex);
    }

    for(ci = 0; ci < SkeletonChain_ptr->ChildCount ; ci++) {
        struct vaSkeletonNodeContext *tBone = *(SkeletonChain_ptr->Child +ci);
        update_SkeletonVertex(skeletonCtx, tBone);
    }
}

/* ---------------------------------------------------------------------------------
 * Update the Normal of skeleton
 *      This function will recursively update the vertex by bone vertex chain.
 *
 *      @param fBone : updated bone, the child of this bone will be update recursively.
*/
inline void update_SurfaceNormal_BDEF4(float *srcVertexAry,
                                       struct DeformType_BDEF4 *BDEF4_ptr,
                                       struct vaSkeletonNodeContext *SkeketonChain,
                                       float *dstVertexAry)
{
    float nVertex_b1[3], nVertex_b2[3], nVertex_b3[3], nVertex_b4[3];
    struct vaSkeletonNodeContext *sBone_1 = SkeketonChain +BDEF4_ptr->BoneIndex_1;
    struct vaSkeletonNodeContext *sBone_2 = SkeketonChain +BDEF4_ptr->BoneIndex_2;
    struct vaSkeletonNodeContext *sBone_3 = SkeketonChain +BDEF4_ptr->BoneIndex_3 ;
    struct vaSkeletonNodeContext *sBone_4 = SkeketonChain +BDEF4_ptr->BoneIndex_4;

    matrixMul_RotationOnly(sBone_1->TransformMatrix, srcVertexAry, BDEF4_ptr->Weight_1, nVertex_b1);
    matrixMul_RotationOnly(sBone_2->TransformMatrix, srcVertexAry, BDEF4_ptr->Weight_2, nVertex_b2);
    matrixMul_RotationOnly(sBone_3->TransformMatrix, srcVertexAry, BDEF4_ptr->Weight_3, nVertex_b3);
    matrixMul_RotationOnly(sBone_4->TransformMatrix, srcVertexAry, BDEF4_ptr->Weight_4, nVertex_b4);
    *(dstVertexAry)    = nVertex_b1[0] + nVertex_b2[0] + nVertex_b3[0] + nVertex_b4[0];
    *(dstVertexAry +1) = nVertex_b1[1] + nVertex_b2[1] + nVertex_b3[1] + nVertex_b4[1];
    *(dstVertexAry +2) = nVertex_b1[2] + nVertex_b2[2] + nVertex_b3[2] + nVertex_b4[2];
}


inline void update_SurfaceNormal_BDEF2(float *srcVertexAry,
                                       struct DeformType_BDEF2 *BDEF2_ptr,
                                       struct vaSkeletonNodeContext *SkeketonChain,
                                       float *dstVertexAry)
{
    float nVertex_b1[3], nVertex_b2[3];
    struct vaSkeletonNodeContext *sBone_1 = SkeketonChain +BDEF2_ptr->BoneIndex_1;
    struct vaSkeletonNodeContext *sBone_2 = SkeketonChain +BDEF2_ptr->BoneIndex_2;

    matrixMul_RotationOnly(sBone_1->TransformMatrix, srcVertexAry, BDEF2_ptr->Weight_1, nVertex_b1);
    matrixMul_RotationOnly(sBone_2->TransformMatrix, srcVertexAry, 1.0f - BDEF2_ptr->Weight_1, nVertex_b2);
    *(dstVertexAry)    = nVertex_b1[0] + nVertex_b2[0];
    *(dstVertexAry +1) = nVertex_b1[1] + nVertex_b2[1];
    *(dstVertexAry +2) = nVertex_b1[2] + nVertex_b2[2];
}

inline void update_SurfaceNormal_BDEF1(float *srcVertexAry,
                                       struct DeformType_BDEF1 *BDEF1_ptr,
                                       struct vaSkeletonNodeContext *SkeketonChain,
                                       float *dstVertexAry)
{
    float nVertex[3];
    struct vaSkeletonNodeContext *sBone = SkeketonChain +BDEF1_ptr->BoneIndex;
    matrixMul_RotationOnly(sBone->TransformMatrix, srcVertexAry, 1.0f, nVertex);
    *(dstVertexAry)    = nVertex[0];
    *(dstVertexAry +1) = nVertex[1];
    *(dstVertexAry +2) = nVertex[2];
}
inline void update_SurfaceNormal_SDEF(float *srcVertexAry,
                                       struct DeformType_SDEF *SDEF_ptr,
                                       struct vaSkeletonNodeContext *SkeketonChain,
                                       float *dstVertexAry)
{
    float nVertex_b1[3], nVertex_b2[3];
    struct vaSkeletonNodeContext *sBone_1 = SkeketonChain +SDEF_ptr->BoneIndex_1;
    struct vaSkeletonNodeContext *sBone_2 = SkeketonChain +SDEF_ptr->BoneIndex_2;

    matrixMul_RotationOnly(sBone_1->TransformMatrix, srcVertexAry, SDEF_ptr->Weight_1, nVertex_b1);
    matrixMul_RotationOnly(sBone_2->TransformMatrix, srcVertexAry, 1.0f - SDEF_ptr->Weight_1, nVertex_b2);
    *(dstVertexAry)    = nVertex_b1[0] + nVertex_b2[0];
    *(dstVertexAry +1) = nVertex_b1[1] + nVertex_b2[1];
    *(dstVertexAry +2) = nVertex_b1[2] + nVertex_b2[2];
}

inline void update_SurfaceVertex_SDEF(float *srcVertexAry,
                                       struct DeformType_SDEF *SDEF_ptr,
                                       struct vaSkeletonNodeContext *SkeketonChain,
                                       float *dstVertexAry)
{
    float nVertex_b1[3], nVertex_b2[3];
    struct vaSkeletonNodeContext *sBone_1 = SkeketonChain +SDEF_ptr->BoneIndex_1;
    struct vaSkeletonNodeContext *sBone_2 = SkeketonChain +SDEF_ptr->BoneIndex_2;

    MulTransformMatrix(sBone_1->TransformMatrix, srcVertexAry, SDEF_ptr->Weight_1, nVertex_b1);
    MulTransformMatrix(sBone_2->TransformMatrix, srcVertexAry, 1.0f - SDEF_ptr->Weight_1, nVertex_b2);
    *(dstVertexAry)    = nVertex_b1[0] + nVertex_b2[0];
    *(dstVertexAry +1) = nVertex_b1[1] + nVertex_b2[1];
    *(dstVertexAry +2) = nVertex_b1[2] + nVertex_b2[2];
}

inline void update_SurfaceVertex_BDEF4(float *srcVertexAry,
                                       struct DeformType_BDEF4 *BDEF4_ptr,
                                       struct vaSkeletonNodeContext *SkeketonChain,
                                       float *dstVertexAry)
{
    float nVertex_b1[3], nVertex_b2[3], nVertex_b3[3], nVertex_b4[3];
    struct vaSkeletonNodeContext *sBone_1 = SkeketonChain +BDEF4_ptr->BoneIndex_1;
    struct vaSkeletonNodeContext *sBone_2 = SkeketonChain +BDEF4_ptr->BoneIndex_2;
    struct vaSkeletonNodeContext *sBone_3 = SkeketonChain +BDEF4_ptr->BoneIndex_3 ;
    struct vaSkeletonNodeContext *sBone_4 = SkeketonChain +BDEF4_ptr->BoneIndex_4;

    MulTransformMatrix(sBone_1->TransformMatrix, srcVertexAry, BDEF4_ptr->Weight_1, nVertex_b1);
    MulTransformMatrix(sBone_2->TransformMatrix, srcVertexAry, BDEF4_ptr->Weight_2, nVertex_b2);
    MulTransformMatrix(sBone_3->TransformMatrix, srcVertexAry, BDEF4_ptr->Weight_3, nVertex_b3);
    MulTransformMatrix(sBone_4->TransformMatrix, srcVertexAry, BDEF4_ptr->Weight_4, nVertex_b4);
    *(dstVertexAry)    = nVertex_b1[0] + nVertex_b2[0] + nVertex_b3[0] + nVertex_b4[0];
    *(dstVertexAry +1) = nVertex_b1[1] + nVertex_b2[1] + nVertex_b3[1] + nVertex_b4[1];
    *(dstVertexAry +2) = nVertex_b1[2] + nVertex_b2[2] + nVertex_b3[2] + nVertex_b4[2];
}


inline void update_SurfaceVertex_BDEF2(float *srcVertexAry,
                                       struct DeformType_BDEF2 *BDEF2_ptr,
                                       struct vaSkeletonNodeContext *SkeketonChain,
                                       float *dstVertexAry)
{
    float nVertex_b1[3], nVertex_b2[3];
    struct vaSkeletonNodeContext *sBone_1 = SkeketonChain +BDEF2_ptr->BoneIndex_1;
    struct vaSkeletonNodeContext *sBone_2 = SkeketonChain +BDEF2_ptr->BoneIndex_2;

    MulTransformMatrix(sBone_1->TransformMatrix, srcVertexAry, BDEF2_ptr->Weight_1, nVertex_b1);
    MulTransformMatrix(sBone_2->TransformMatrix, srcVertexAry, 1.0f - BDEF2_ptr->Weight_1, nVertex_b2);
    *(dstVertexAry)    = nVertex_b1[0] + nVertex_b2[0];
    *(dstVertexAry +1) = nVertex_b1[1] + nVertex_b2[1];
    *(dstVertexAry +2) = nVertex_b1[2] + nVertex_b2[2];
}

inline void update_SurfaceVertex_BDEF1(float *srcVertexAry,
                                       struct DeformType_BDEF1 *BDEF1_ptr,
                                       struct vaSkeletonNodeContext *SkeketonChain,
                                       float *dstVertexAry)
{
    float nVertex[3];
    struct vaSkeletonNodeContext *sBone = SkeketonChain +BDEF1_ptr->BoneIndex;
    MulTransformMatrix(sBone->TransformMatrix, srcVertexAry, 1.0f, nVertex);
    *(dstVertexAry)    = nVertex[0];
    *(dstVertexAry +1) = nVertex[1];
    *(dstVertexAry +2) = nVertex[2];
}


/* ---------------------------------------------------------------------------------
 * Update the vertex of avatar body surface
 * This function will recursively update the vertex by bone vertex chain.
 *
 * @param vaMemory : source avatar memory.
 * @param skeletonCtx : source skeleton context.
 * @param surfaceCtx : surface data context
 *
*/
void VisualizationAvatar::vaMotionFunctions::
update_BodySurfaceVertex(struct vaMemoryContext *vaMemory,
                         struct vaSkeletonTreeContext *skeletonCtx,
                         struct vaSurfaceContext *surfaceCtx)
{
    int i;
    struct DeformType_BDEF1 *pBDEF1 = vaMemory->VertexMemory.BDEF1;
    for(i = 0 ; i < vaMemory->VertexMemory.BDEF1Count ; i++) {
        float *inVertex  = vaMemory->VertexMemory.Vertex + pBDEF1->vID ;
        float *dstVertex = surfaceCtx->Vertex + pBDEF1->vID;

        update_SurfaceVertex_BDEF1(inVertex,
                                   vaMemory->VertexMemory.BDEF1 + i,
                                   skeletonCtx->chainNodeSet,
                                   dstVertex);
        pBDEF1++;
    }

    struct DeformType_BDEF2 *pBDEF2 = vaMemory->VertexMemory.BDEF2;
    for(i = 0 ; i < vaMemory->VertexMemory.BDEF2Count ; i++) {
        float *inVertex  = vaMemory->VertexMemory.Vertex + pBDEF2->vID ;
        float *dstVertex = surfaceCtx->Vertex + pBDEF2->vID;

        update_SurfaceVertex_BDEF2(inVertex,
                                   vaMemory->VertexMemory.BDEF2 + i,
                                   skeletonCtx->chainNodeSet,
                                   dstVertex);
        pBDEF2++;
    }

    struct DeformType_BDEF4 *pBDEF4 = vaMemory->VertexMemory.BDEF4;
    for(i = 0 ; i < vaMemory->VertexMemory.BDEF4Count ; i++) {
        float *inVertex  = vaMemory->VertexMemory.Vertex + pBDEF4->vID ;
        float *dstVertex = surfaceCtx->Vertex + pBDEF4->vID;

        update_SurfaceVertex_BDEF4(inVertex,
                                   vaMemory->VertexMemory.BDEF4 + i,
                                   skeletonCtx->chainNodeSet,
                                   dstVertex);
        pBDEF4++;
    }

    struct DeformType_SDEF *pSDEF = vaMemory->VertexMemory.SDEF;
    for(i = 0 ; i < vaMemory->VertexMemory.SDEFCount ; i++) {
        float *inVertex  = vaMemory->VertexMemory.Vertex + pSDEF->vID ;
        float *dstVertex = surfaceCtx->Vertex + pSDEF->vID;

        update_SurfaceVertex_SDEF(inVertex,
                                   vaMemory->VertexMemory.SDEF + i,
                                   skeletonCtx->chainNodeSet,
                                   dstVertex);
        pSDEF++;
    }

    /** -----------------------------------------------------------------
     * Normal
     */
    pBDEF1 = vaMemory->VertexMemory.BDEF1;
    for(i = 0 ; i < vaMemory->VertexMemory.BDEF1Count ; i++) {
        float *inNormal  = vaMemory->VertexMemory.Normal + pBDEF1->vID ;
        float *dstNormal = surfaceCtx->Normal + pBDEF1->vID;
        update_SurfaceNormal_BDEF1(inNormal,
                                   vaMemory->VertexMemory.BDEF1 + i,
                                   skeletonCtx->chainNodeSet,
                                   dstNormal);
        pBDEF1++;
    }

    pBDEF2 = vaMemory->VertexMemory.BDEF2;
    for(i = 0 ; i < vaMemory->VertexMemory.BDEF2Count ; i++) {
        float *inNormal  = vaMemory->VertexMemory.Normal + pBDEF2->vID ;
        float *dstNormal = surfaceCtx->Normal + pBDEF2->vID;
        update_SurfaceNormal_BDEF2(inNormal,
                                   vaMemory->VertexMemory.BDEF2 + i,
                                   skeletonCtx->chainNodeSet,
                                   dstNormal);
        pBDEF2++;
    }

    pBDEF4 = vaMemory->VertexMemory.BDEF4;
    for(i = 0 ; i < vaMemory->VertexMemory.BDEF4Count ; i++) {
        float *inNormal  = vaMemory->VertexMemory.Normal + pBDEF4->vID ;
        float *dstNormal = surfaceCtx->Normal + pBDEF4->vID;
        update_SurfaceNormal_BDEF4(inNormal,
                                   vaMemory->VertexMemory.BDEF4 + i,
                                   skeletonCtx->chainNodeSet,
                                   dstNormal);
        pBDEF4++;
    }

    pSDEF = vaMemory->VertexMemory.SDEF;
    for(i = 0 ; i < vaMemory->VertexMemory.SDEFCount ; i++) {
        float *inNormal  = vaMemory->VertexMemory.Normal + pSDEF->vID ;
        float *dstNormal = surfaceCtx->Normal + pSDEF->vID;
        update_SurfaceNormal_SDEF(inNormal,
                                   vaMemory->VertexMemory.SDEF + i,
                                   skeletonCtx->chainNodeSet,
                                   dstNormal);
        pSDEF++;
    }

    /*int i;
    struct VertexUnitContext *VertexCtx = vaMemory->VertexMemory.VertexUnit + i;
    for(i = 0 ; i < vaMemory->VertexMemory.VertexUnitCount ; i++) {
        float *inVertex  = vaMemory->VertexMemory.Vertex + VertexCtx->vID *3;
        float *dstVertex = surfaceCtx->Vertex + VertexCtx->vID *3;

        switch(VertexCtx->DeformType) {
        case 0: // 0:BDEF1
            update_SurfaceVertex_BDEF1(inVertex,
                                       vaMemory->VertexMemory.BDEF1 + VertexCtx->DeformID,
                                       skeletonCtx->chainNodeSet,
                                       dstVertex);
            break;
        case 1: // 1:BDEF2
            update_SurfaceVertex_BDEF2(inVertex,
                                       vaMemory->VertexMemory.BDEF2 + VertexCtx->DeformID,
                                       skeletonCtx->chainNodeSet,
                                       dstVertex);
            break;
        case 2: // 2:BDEF4
            update_SurfaceVertex_BDEF4(inVertex,
                                       vaMemory->VertexMemory.BDEF4 + VertexCtx->DeformID,
                                       skeletonCtx->chainNodeSet,
                                       dstVertex);
            break;
        case 3: // 3:SDEF
            update_SurfaceVertex_SDEF(inVertex,
                                       vaMemory->VertexMemory.SDEF + VertexCtx->DeformID,
                                       skeletonCtx->chainNodeSet,
                                       dstVertex);
            break;
        }
        VertexCtx++;    //switch to next vertex
    }*/
}

/* ---------------------------------------------------------------------------------
 * Update the vertex of avatar skeleton surface
 * This function only update(copy) the vertex used to rendering. All calculation of vertex was done by update_SkeletonVertex() & update_InverseKinematics(),
 * and the new vertex data will stored in the pBoneVertex of chainNodeSet or the chainNodeVertex of inSkeletonCtx.
 *
 * @param inSkeletonCtx : Target skeleton context.
*/
void VisualizationAvatar::vaMotionFunctions::
update_SkeletonSurfaceVertex(struct vaSkeletonTreeContext *inSkeletonCtx)
{
    int counter = inSkeletonCtx->chainNodeCount, ei;
    float *renderingColor = inSkeletonCtx->renderingColor;
    float *renderingVertex = inSkeletonCtx->renderingVertex;
    struct vaSkeletonNodeContext *pChainNode = inSkeletonCtx->chainNodeSet;
    while(counter >0) {
        for(ei = 0 ; ei < SKELETON_RENDERING_NODE_ELEMENTS ; ei++) {
            memcpy(renderingVertex, pChainNode->pBoneVertex, sizeof(float) *3);
            renderingVertex[3] = ei;
            renderingColor += 4;
            renderingVertex += SKELETON_RENDERING_VERTEX_ELEMENTS;
        }
        pChainNode++;
        counter--;
    }
}

/** ---------------------------------------------------------------------------------
 * Update the vertex of bone with Inverse Kinematics
 * The algorithm implemented in this function is :
 *  1. Choose 1 chain.
 *  2. Store the original bone vertex in each chain unit.
 *  3. Calculated the augmented matrix of each unit.
 *  4. Apply CCD algoithm, repeat 2~4 until CCD finish.
 *  5. Superpose the augmented matrix recursively with the transform matrix and vertex to
 *      the bone associated with chain unit and it's child by function superpose_IKAugmentedMatrix().
 *
 * superpose_IKAugmentedMatrix:
 *      This function superpose the augmented matrix to all childs,
 *      except the child in the same IK chain to prevent dupiclate multiplication.
 *      The deatail shown below :
 *
 * Note :
 *  In CCD inverse kinematics algorithm, every bone unit in IK chain may apply an
 *  augmented matrix to transform target vertex in IK chain into the reach vertex,
 *  The augmented matrix is defined as a local matrix in this library.

 *          =[Am'0]      =[Am'1][Am0]        =[Am'2][Am1][Am0]
 *            Am0             Am1                    Am2
 *  IK n : (Bone 0)-------->(Bone 1)------------->(Bone 2)
 *            Tm0             Tm1                    Tm2
 *
 *           ^^^^^
 *         TargetBone
 *
 *    @Tmx is the transform matrix of bone, x is bone ID.
 *    @Amx is the augmented matrix of bone, x is bone ID.
 *    @Am'x is the CCD matrix of bone, x is bone ID.
 *
 * Accroding to the skeleton sturcture, any bone mutiled a mathmatica matrix will effect
 * all child associated with this bone. due to the mathmatical define, if multiple the
 * augmented matrix into child directly, the transfrom matrix in IK bone will mutiple this
 * matrix twice. For this reason, the function superpose_IKAugmentedMatrix() design a param
 * $disable_bID to prevent the augmented matrix superpose into child bond which is in the
 * same IK chain with parent bone.
*/
inline void getVector(float *targetVertex, float *baseVertex, float *dstVector)
{
    dstVector[0] = targetVertex[0] - baseVertex[0];
    dstVector[1] = targetVertex[1] - baseVertex[1];
    dstVector[2] = targetVertex[2] - baseVertex[2];
}
inline void normalizeVector(float *dstVector)
{
    float len = dstVector[0] * dstVector[0] + dstVector[1] * dstVector[1] + dstVector[2] * dstVector[2];
    len = (len > 0.0f) ? 1.0f / sqrtf(len) : 1.0f;
    dstVector[0] *= len;
    dstVector[1] *= len;
    dstVector[2] *= len;
}
inline float getDotProduct(float *VectorA, float *VectorB)
{
    float DotProduct = VectorA[0] * VectorB[0] + VectorA[1] * VectorB[1] + VectorA[2] * VectorB[2];
    if(DotProduct > 1.0f) DotProduct = 1.0f;
    if(DotProduct < -1.0f) DotProduct = -1.0f;
    return DotProduct;
}
inline void getOuterProduct(float *VectorA, float *VectorB, float *dstOuterProduct)
{
    dstOuterProduct[0] = VectorA[1]* VectorB[2] - VectorA[2]* VectorB[1];
    dstOuterProduct[1] = VectorA[2]* VectorB[0] - VectorA[0]* VectorB[2];
    dstOuterProduct[2] = VectorA[0]* VectorB[1] - VectorA[1]* VectorB[0];
}

static float getVertex(float *baseVertex, float *parentVertex, float *dstVertex)
{

}

static float getRotationVector(float *targetVertex, float *reachVertex, float *baseVertex, float *dstOuterProduct)
{
    float vertexTR[3], vectorBR[3];
    float DotProduct ;
    getVector(targetVertex, baseVertex, vertexTR);
    getVector(reachVertex, baseVertex, vectorBR);
    normalizeVector(vertexTR);
    normalizeVector(vectorBR);

    DotProduct = getDotProduct(vertexTR, vectorBR);
    getOuterProduct(vertexTR, vectorBR, dstOuterProduct);
    return DotProduct;
}

static void getRotationSupportVertex(float *baseVertex, float *parentVertex, float *outerProduct, float dotProduct, float *dstsupportVertex)
{
    sutQuaternionOperator IKQuaternion;
    float matrix4x4[16];
    float BoneVertexMatrix[16];
    float invBoneVertexMatrix[16], augmentedMatrix[16], vertex[3];

    IKQuaternion.init();
    IKQuaternion.asRotation(outerProduct[0], outerProduct[1], outerProduct[2], acos(dotProduct) * 180.0f /3.1415926);
    IKQuaternion.toMatrix();

    dstsupportVertex[0] = -parentVertex[0];
    dstsupportVertex[1] = -parentVertex[1];
    dstsupportVertex[2] = -parentVertex[2];
    memcpy(BoneVertexMatrix, IdentityMatrix, sizeof(float)*16);
    memcpy(invBoneVertexMatrix, IdentityMatrix, sizeof(float)*16);
    BoneVertexMatrix[3]     = baseVertex[0];
    BoneVertexMatrix[7]     = baseVertex[1];
    BoneVertexMatrix[11]    = baseVertex[2];
    invBoneVertexMatrix[3]  = -baseVertex[0];
    invBoneVertexMatrix[7]  = -baseVertex[1];
    invBoneVertexMatrix[11] = -baseVertex[2];
    MatrixOperator_Mul_4x4M_4x4M(BoneVertexMatrix, IKQuaternion.getMatrix(), matrix4x4);
    MatrixOperator_Mul_4x4M_4x4M(matrix4x4, invBoneVertexMatrix, augmentedMatrix);
    MulTransformMatrix(augmentedMatrix, dstsupportVertex ,1, vertex);
    dstsupportVertex[0] = vertex[0];
    dstsupportVertex[1] = vertex[1];
    dstsupportVertex[2] = vertex[2];
}

static void truncateRotationAngle(float *baseVertex, float *parentVertex, float *supportVertex,
                                  struct IKChainUnit *pIKUnit, float *rotationVector, float rotationDot,
                                  float *augmentedMatrix)
{
    int xyz;
    float vectorParentBase[3], vectorOriBase[3], vectorSupportBase[3];
    float OuterProduct[3], vectorParent[3], vertexOri[3] = {-parentVertex[0], -parentVertex[1], -parentVertex[2]} ;
    float componentsXYZ_ParentOri[3][3]={0.0f};
    float componentsXYZ_ParentSupport[3]={0.0f} ;
    float componentsA[3] = {0.0f}, componentsB[3] = {0.0f};
    float dotProduct, outerProductParent[3], dotProductParent;
    sutQuaternionOperator IKQuaternion, componentsQuaternion[3];

    getVector(parentVertex, baseVertex, vectorParentBase);
    normalizeVector(vectorParentBase);
    getVector(vertexOri, baseVertex, vectorOriBase);
    normalizeVector(vectorOriBase);
    getVector(supportVertex, baseVertex, vectorSupportBase);
    normalizeVector(vectorSupportBase);

    // get each compoments XYZ
    for(xyz = 0 ; xyz < 3 ; xyz++) { //parse each compoments
        componentsA[0] = vectorOriBase[0];
        componentsA[1] = vectorOriBase[1];
        componentsA[2] = vectorOriBase[2];
        componentsA[xyz] = 0.0f;
        componentsB[0] = vectorParentBase[0];
        componentsB[1] = vectorParentBase[1];
        componentsB[2] = vectorParentBase[2];
        componentsB[xyz] = 0.0f;

        normalizeVector(componentsA);
        normalizeVector(componentsB);
        getOuterProduct(componentsA, componentsB, OuterProduct);
        normalizeVector(OuterProduct);
        /*if(OuterProduct[xyz] < 0) / cross product constant the normal as (1,0,0), (0,1,0) or (0,0,1)
            dotProduct = 1.0f - getDotProduct(componentsA, componentsB);
        else
            dotProduct = getDotProduct(componentsA, componentsB);*/
        dotProduct = getDotProduct(componentsA, componentsB);
        printf("-----------------------------------------------------------\n");
        printf("\tvectorOriBase %f %f %f,  vectorParentBase %f %f %f\n", vectorOriBase[0], vectorOriBase[1], vectorOriBase[2], vectorParentBase[0], vectorParentBase[1], vectorParentBase[2]);
        printf("\tcomponentsA %f %f %f,  componentsB %f %f %f\n", componentsA[0], componentsA[1], componentsA[2], componentsB[0], componentsB[1], componentsB[2]);
        printf("\tdotProduct %f , angle %f, Outerproduct %f %f %f\n",
               dotProduct, acos(dotProduct) * 180.0f /3.1415926,
               OuterProduct[0], OuterProduct[1], OuterProduct[2]);
    }


    printf("\n-------------------------------------------------------\n");

    IKQuaternion.init();
    IKQuaternion.asRotation(rotationVector[0], rotationVector[1], rotationVector[2], acos(rotationDot) * 180.0f /3.1415926);
    IKQuaternion.toMatrix();


    float matrix4x4[16], matrix4x4tmp[16];
    float BoneVertexMatrix[16];
    float invBoneVertexMatrix[16];
    memcpy(BoneVertexMatrix, IdentityMatrix, sizeof(float)*16);
    memcpy(invBoneVertexMatrix, IdentityMatrix, sizeof(float)*16);
    BoneVertexMatrix[3]     = baseVertex[0];
    BoneVertexMatrix[7]     = baseVertex[1];
    BoneVertexMatrix[11]    = baseVertex[2];
    invBoneVertexMatrix[3]  = -baseVertex[0];
    invBoneVertexMatrix[7]  = -baseVertex[1];
    invBoneVertexMatrix[11] = -baseVertex[2];

    // calculate the CCD matrix of bone
    MatrixOperator_Mul_4x4M_4x4M(componentsQuaternion[0].getMatrix(), IKQuaternion.getMatrix(), matrix4x4tmp);
    MatrixOperator_Mul_4x4M_4x4M(componentsQuaternion[1].getMatrix(), matrix4x4tmp, matrix4x4);
    MatrixOperator_Mul_4x4M_4x4M(componentsQuaternion[2].getMatrix(), matrix4x4, matrix4x4tmp);
    MatrixOperator_Mul_4x4M_4x4M(BoneVertexMatrix, matrix4x4tmp, matrix4x4);
    MatrixOperator_Mul_4x4M_4x4M(matrix4x4, invBoneVertexMatrix, augmentedMatrix);

    // calculate the CCD matrix of bone
    //MatrixOperator_Mul_4x4M_4x4M(BoneVertexMatrix, IKQuaternion.getMatrix(), matrix4x4);
    //MatrixOperator_Mul_4x4M_4x4M(matrix4x4, invBoneVertexMatrix, augmentedMatrix);
    exit(0);
}

static void getCCDAugmentedMatrix(float *baseVertex, float *rotationVector, float rotationDot, struct IKChainUnit *pIKUnit, float *augmentedMatrix)
{
    sutQuaternionOperator IKQuaternion, IKQuaternionResult, IKQuaternionTmp, IKQuaternionX, IKQuaternionY, IKQuaternionZ;
    sutQuaternionOperator quaternionYawPitchRoll;
    IKQuaternion.init();
    IKQuaternion.asRotation(rotationVector[0], rotationVector[1], rotationVector[2], acos(rotationDot) * 180.0f /3.1415926);
    IKQuaternion.toMatrix();

   /* float quaternion[4] ;
    IKQuaternion.getQuaternion(quaternion);
    float x = quaternion[0], y = quaternion[1], z = quaternion[2], w = quaternion[3];
    float xRadius = atan2(2*y*w - 2*x*z, 1 - 2*y*y - 2*z*z);
    float yRadius = atan2(2*x*w - 2*y*z, 1 - 2*x*x - 2*z*z);
    float zRadius =  asin(2*x*y + 2*z*w);
    printf("xRadius %f , yRadius %f , zRadius %f    ", xRadius, yRadius, zRadius);
    printf("xRadius %f , yRadius %f , zRadius %f\n", xRadius /3.1415926535f, yRadius /3.1415926535f, zRadius /3.1415926535f);

    if(xRadius > pIKUnit->UpBound[0]) xRadius =  pIKUnit->UpBound[0];
    if(yRadius > pIKUnit->UpBound[1]) yRadius =  pIKUnit->UpBound[1];
    if(zRadius > pIKUnit->UpBound[2]) zRadius =  pIKUnit->UpBound[2];
    if(xRadius < pIKUnit->LowerBound[0]) xRadius =  pIKUnit->LowerBound[0];
    if(yRadius < pIKUnit->LowerBound[1]) yRadius =  pIKUnit->LowerBound[1];
    if(zRadius < pIKUnit->LowerBound[2]) zRadius =  pIKUnit->LowerBound[2];
    IKQuaternionX.init();
    IKQuaternionY.init();
    IKQuaternionZ.init();
    IKQuaternionX.asRotation(1.0f, 0.0f, 0.0f, xRadius * 180.0f /3.1415926);
    IKQuaternionY.asRotation(0.0f, 1.0f, 0.0f, yRadius * 180.0f /3.1415926);
    IKQuaternionZ.asRotation(0.0f, 0.0f, 1.0f, zRadius * 180.0f /3.1415926);
    IKQuaternionX.toMatrix();
    IKQuaternionY.toMatrix();
    IKQuaternionZ.toMatrix();*/
    float quaternion[4], quaternionOld[4]  ;

    pIKUnit->IKQuaternion.getQuaternion(quaternionOld);
    pIKUnit->IKQuaternion.mul(&IKQuaternion);
    float x = quaternion[0], y = quaternion[1], z = quaternion[2], w = quaternion[3];
    float radiusYaw =  atan2(2*x*y + 2*w*z, w*w + x*x - y*y - z*z);
    float radiusPitch = -asin(2*w*y - 2*x*z);
    float radiusRoll  = -atan2(2*y*z + 2*w*x, -w*w + x*x + y*y - z*z);

    printf("roll %f , pitch %f , yaw %f    ", radiusRoll, radiusPitch, radiusYaw);
    radiusRoll = 0.0f;
    radiusPitch = 0.0f;
    //radiusYaw = 0.0f;
    if(radiusYaw > pIKUnit->UpBound[0])            radiusYaw =  pIKUnit->UpBound[0];
    else if(radiusYaw < pIKUnit->LowerBound[0])    radiusYaw =  pIKUnit->LowerBound[0];
    /*if(radiusRoll > pIKUnit->UpBound[2])        radiusRoll =  pIKUnit->UpBound[2];
    if(radiusRoll < pIKUnit->LowerBound[2])     radiusRoll =  pIKUnit->LowerBound[2];
    if(radiusPitch > pIKUnit->UpBound[1])       radiusPitch =  pIKUnit->UpBound[1];
    if(radiusPitch < pIKUnit->LowerBound[1])    radiusPitch =  pIKUnit->LowerBound[1];
    if(radiusYaw > pIKUnit->UpBound[0])         radiusYaw =  pIKUnit->UpBound[0];
    if(radiusYaw < pIKUnit->LowerBound[0])      radiusYaw =  pIKUnit->LowerBound[0];*/
    printf("roll %f , pitch %f , yaw %f  \n", radiusRoll, radiusPitch, radiusYaw);

    quaternion[0] = cos(radiusRoll/2) *cos(radiusPitch/2) *cos(radiusYaw/2) +sin(radiusRoll/2) *sin(radiusPitch/2) *sin(radiusYaw/2);
    quaternion[1] = sin(radiusRoll/2) *cos(radiusPitch/2) *cos(radiusYaw/2) -cos(radiusRoll/2) *sin(radiusPitch/2) *sin(radiusYaw/2);
    quaternion[2] = cos(radiusRoll/2) *sin(radiusPitch/2) *cos(radiusYaw/2) +sin(radiusRoll/2) *cos(radiusPitch/2) *sin(radiusYaw/2);
    quaternion[3] = cos(radiusRoll/2) *cos(radiusPitch/2) *sin(radiusYaw/2) -sin(radiusRoll/2) *sin(radiusPitch/2) *cos(radiusYaw/2);
    pIKUnit->IKQuaternion.init();
    pIKUnit->IKQuaternion.setRotation(quaternion[1], quaternion[2], quaternion[3], quaternion[0]);
    pIKUnit->IKQuaternion.toMatrix();
    pIKUnit->IKQuaternion.getQuaternion(quaternion);

    /* pIKUnit->IKQuaternion =  IKQuaternionResult * pIKUnit->IKQuaternion(Old)
     * IKQuaternionResult =  pIKUnit->IKQuaternion * Conjugated(pIKUnit->IKQuaternion(Old))
    */
    IKQuaternionTmp.init();
    IKQuaternionTmp.setRotation(quaternionOld[0], quaternionOld[1],quaternionOld[2],quaternionOld[3]);
    IKQuaternionTmp.asConjugated();
    IKQuaternionResult.init();
    IKQuaternionResult.setRotation(quaternion[0], quaternion[1],quaternion[2], quaternion[3]);
    IKQuaternionResult.mul(&IKQuaternionTmp);
    IKQuaternionResult.toMatrix();


    float matrix4x4[16];
    float BoneVertexMatrix[16];
    float invBoneVertexMatrix[16];
    memcpy(BoneVertexMatrix, IdentityMatrix, sizeof(float)*16);
    memcpy(invBoneVertexMatrix, IdentityMatrix, sizeof(float)*16);
    BoneVertexMatrix[3]     = baseVertex[0];
    BoneVertexMatrix[7]     = baseVertex[1];
    BoneVertexMatrix[11]    = baseVertex[2];
    invBoneVertexMatrix[3]  = -baseVertex[0];
    invBoneVertexMatrix[7]  = -baseVertex[1];
    invBoneVertexMatrix[11] = -baseVertex[2];

    // calculate the CCD matrix of bone
    MatrixOperator_Mul_4x4M_4x4M(BoneVertexMatrix, IKQuaternionResult.getMatrix(), matrix4x4);
    MatrixOperator_Mul_4x4M_4x4M(matrix4x4, invBoneVertexMatrix, augmentedMatrix);
}

static void CCD_calcAugmentedMatrix(float *targetVertex, float *reachVertex, float *baseVertex, float maxAngle, float *augmentedMatrix)
{
    /*                        (Bone base)
     *  (Bone Target)-------->(Bone IK[0])------------->(Bone IK[1])
     *        |_                   _|
     *          |_               _|
     *  vertexTR  |_            _|    vertexBR
     *              |           |
     *             (Bone reache )
    */

    sutQuaternionOperator IKQuaternion;
    float vertexTR[3], vectorBR[3], OuterProduct[3] ;
    float DotProduct ;
    getVector(targetVertex, baseVertex, vertexTR);
    normalizeVector(vertexTR);
    getVector(reachVertex, baseVertex, vectorBR);
    normalizeVector(vectorBR);

    DotProduct = getDotProduct(vertexTR, vectorBR);
    getOuterProduct(vertexTR, vectorBR, OuterProduct);

   // if(DotProduct > maxAngle) DotProduct = maxAngle;
    IKQuaternion.init();
    IKQuaternion.asRotation(OuterProduct[0], OuterProduct[1], OuterProduct[2], acos(DotProduct) * 180.0f /3.1415926);
    IKQuaternion.toMatrix();

    float matrix4x4[16];
    float BoneVertexMatrix[16];
    float invBoneVertexMatrix[16];
    memcpy(BoneVertexMatrix, IdentityMatrix, sizeof(float)*16);
    memcpy(invBoneVertexMatrix, IdentityMatrix, sizeof(float)*16);
    BoneVertexMatrix[3]     = baseVertex[0];
    BoneVertexMatrix[7]     = baseVertex[1];
    BoneVertexMatrix[11]    = baseVertex[2];
    invBoneVertexMatrix[3]  = -baseVertex[0];
    invBoneVertexMatrix[7]  = -baseVertex[1];
    invBoneVertexMatrix[11] = -baseVertex[2];

    // calculate the CCD matrix of bone
    MatrixOperator_Mul_4x4M_4x4M(BoneVertexMatrix, IKQuaternion.getMatrix(), matrix4x4);
    MatrixOperator_Mul_4x4M_4x4M(matrix4x4, invBoneVertexMatrix, augmentedMatrix);
}

static void superpose_IKAugmentedMatrix(struct vaSkeletonNodeContext *IKBone, float *augMatrix, int disable_bID);

void VisualizationAvatar::vaMotionFunctions::
update_InverseKinematics(struct vaMemoryContext *vaMemory,
                         struct vaSkeletonTreeContext *skeletonCtx)
{
    int iki, ui, ci;
    float matrix4x4[16], vertex3[3];
    float tVertex[3], tAugmentedMatrix[16]; // the vertex and augmented matrix of target bone.

    for(iki = 0 ; iki < vaMemory->BoneMemory.IKChainCount ; iki++) {
        struct IKChainContext *pIKChain = vaMemory->BoneMemory.IKChain + iki;
        struct vaSkeletonNodeContext *targetBone = (skeletonCtx->chainNodeSet + pIKChain->TargetBoneID);
        float *ReachVertex = (skeletonCtx->chainNodeSet + pIKChain->ReachBoneID)->pBoneVertex;
        int maxIterations = pIKChain->MaxIterations;

        //assign the vertex value of bone which are computed before inverse kinematics calculation.
        memcpy(tVertex, targetBone->pBoneVertex, sizeof(float) *3);
        memcpy(tAugmentedMatrix, IdentityMatrix, sizeof(float) *16);
        for(ci = 0 ; ci < pIKChain->IKChainCount ; ci++) {
            struct IKChainUnit *pIKUnit = (pIKChain->IKChain + ci);
            struct vaSkeletonNodeContext *pBone = (skeletonCtx->chainNodeSet + pIKUnit->LinkBone);
            memcpy(pIKUnit->cVertex, pBone->pBoneVertex, sizeof(float) *3);
            memcpy(pIKUnit->augmentedMatrix, IdentityMatrix, sizeof(float) *16);
        }

        // apply CCD algorithm (IK algorithm)
        while(maxIterations >0) {
            maxIterations--;
            for(ci = 0 ; ci < pIKChain->IKChainCount ; ci++) {
                struct IKChainUnit *pIKUnit = (pIKChain->IKChain + ci);
                float *baseVertex = pIKUnit->cVertex;
                float CCDMatrix[16];

                // Calculate the augmented matrix
                if(pIKUnit->isLimit ) {

                    /** Below algorithm is under construct.
                    */
                    struct vaSkeletonNodeContext *pIKBone = (skeletonCtx->chainNodeSet + pIKUnit->LinkBone);
                    float dotProduct, outerProduct[3], supportVertex[3];
                    dotProduct = getRotationVector(tVertex, ReachVertex, baseVertex, outerProduct);
                    getRotationSupportVertex(baseVertex, pIKBone->Parnet->pBoneVertex, outerProduct, dotProduct, supportVertex);
                    getCCDAugmentedMatrix(baseVertex, outerProduct, dotProduct, pIKUnit, CCDMatrix);
                }
                else {
                    CCD_calcAugmentedMatrix(tVertex, ReachVertex, baseVertex, pIKChain->MaxAngle, CCDMatrix);
                }

                // Superpose the matrix into IK vertex & matrix
                for(ui = 0 ; ui <= ci ; ui++) {
                    struct IKChainUnit *pUnit = (pIKChain->IKChain + ui);
                    MatrixOperator_Mul_4x4M_4x4M(CCDMatrix, pUnit->augmentedMatrix, matrix4x4);
                    memcpy(pUnit->augmentedMatrix, matrix4x4, sizeof(float)*16);
                    MulTransformMatrix(CCDMatrix, pUnit->cVertex ,1 ,vertex3);
                    memcpy(pUnit->cVertex, vertex3, sizeof(float)*3);
                }

                //superpose the matrix into target vertex & matrix
                MatrixOperator_Mul_4x4M_4x4M(CCDMatrix, tAugmentedMatrix ,matrix4x4);
                memcpy(tAugmentedMatrix, matrix4x4, sizeof(float)*16);
                MulTransformMatrix(CCDMatrix, tVertex ,1, vertex3);
                memcpy(tVertex, vertex3, sizeof(float)*3);
            }
        }

        // feedback the augmented matrix and new vertex into the child of IK chain.
        for(ci = 0 ; ci < pIKChain->IKChainCount  ; ci++) {
            struct IKChainUnit *pIKUnit = (pIKChain->IKChain + ci);
            struct IKChainUnit *pIKUnitNext = (pIKChain->IKChain + ci - 1);
            struct vaSkeletonNodeContext *pBone = (skeletonCtx->chainNodeSet + pIKUnit->LinkBone);
            int bIDNext = (ci >0)? pIKUnitNext->LinkBone : pIKChain->TargetBoneID;
            superpose_IKAugmentedMatrix(pBone, pIKUnit->augmentedMatrix, bIDNext);
        }

        // feedback the augmented matrix and new vertex into "target" IK node.
        superpose_IKAugmentedMatrix(targetBone, tAugmentedMatrix, -1);
    }
}

/* ---------------------------------------------------------------------------------
 * Superpose the input matrix to target bone's transform matrix
 *  This function will superpose the input matrix with fBone's child.
 *
 *      [fBone->TransformMatrix] = [srcMatrix] [fBone->TransformMatrix]
 *
 *  @param fBone : The bone which is needed to superpose.
 *  @param srcMatrix : Superposed 4*4 matrix.
 */
static void superpose_IKAugmentedMatrix(struct vaSkeletonNodeContext *IKBone, float *augMatrix, int disable_bID)
{
    int i;
    float matrix4x4f[16], vertrex3f[3];
    MatrixOperator_Mul_4x4M_4x4M(augMatrix, IKBone->TransformMatrix, matrix4x4f);
    memcpy(IKBone->TransformMatrix, matrix4x4f, sizeof(float) *16);
    MulTransformMatrix(augMatrix, IKBone->pBoneVertex, 1.0f, vertrex3f);
    memcpy(IKBone->pBoneVertex, vertrex3f, sizeof(float) *3);

    for(i = 0 ; i < IKBone->ChildCount ; i++) {
        struct vaSkeletonNodeContext *pChild = *(IKBone->Child +i);
        if(pChild->pRawBoneInfo->bID != disable_bID)
            superpose_IKAugmentedMatrix(pChild, augMatrix, disable_bID);
    }
}
