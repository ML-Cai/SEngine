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
    fBone->globalQuaternion.mul(&fBone->Parnet->globalQuaternion, sutQuaternionOperator::INPUT_AS_RIGHT);

/*
    printf("---------------- [ %2d , Parent %2d ]-------------------\n", fBone->nID, fBone->Parnet->nID);
    printf("parent       : ");
    fBone->Parnet->globalQuaternion.dump();
    printf("bone         : ");
    fBone->globalQuaternion.dump();

    sutQuaternionContext *gga = fBone->Parnet->globalQuaternion.getQuaternionCtx();
    sutQuaternionOperator globalInverseQuaternion;
    globalInverseQuaternion.init();
    if(fBone->globalQuaternion.isIdentity()) {

    }
    else {
        if(!fBone->Parnet->globalQuaternion.isIdentity()) {
            globalInverseQuaternion.setQuaternion(gga->a, gga->b, gga->c, gga->w);
            globalInverseQuaternion.asInverse();
        }
    }

    printf("*Q^-1        : ");
    globalInverseQuaternion.dump();
    fBone->globalQuaternion.mul(&globalInverseQuaternion, sutQuaternionOperator::INPUT_AS_RIGHT);
    printf("new[*Q^-1]   : ");
    fBone->globalQuaternion.dump();
    fBone->globalQuaternion.mul(&fBone->Parnet->globalQuaternion, sutQuaternionOperator::INPUT_AS_LEFT);
    printf("new[*Q   ]   : ");
    fBone->globalQuaternion.dump();*/
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
static inline void getVector(float *targetVertex, float *baseVertex, float *dstVector)
{
    dstVector[0] = targetVertex[0] - baseVertex[0];
    dstVector[1] = targetVertex[1] - baseVertex[1];
    dstVector[2] = targetVertex[2] - baseVertex[2];
}
static inline void normalizeVector(float *dstVector)
{
    float len = dstVector[0] * dstVector[0] + dstVector[1] * dstVector[1] + dstVector[2] * dstVector[2];
    len = (len > 0.0f) ? 1.0f / sqrtf(len) : 1.0f;
    dstVector[0] *= len;
    dstVector[1] *= len;
    dstVector[2] *= len;
}
static inline float getDotProduct(float *VectorA, float *VectorB)
{
    float DotProduct = VectorA[0] * VectorB[0] + VectorA[1] * VectorB[1] + VectorA[2] * VectorB[2];
    if(DotProduct > 1.0f) DotProduct = 1.0f;
    if(DotProduct < -1.0f) DotProduct = -1.0f;
    return DotProduct;
}
static inline void getOuterProduct(float *VectorA, float *VectorB, float *dstOuterProduct)
{
    dstOuterProduct[0] = VectorA[1]* VectorB[2] - VectorA[2]* VectorB[1];
    dstOuterProduct[1] = VectorA[2]* VectorB[0] - VectorA[0]* VectorB[2];
    dstOuterProduct[2] = VectorA[0]* VectorB[1] - VectorA[1]* VectorB[0];
}

static inline void CCD_getStepAugmentedMatrix(float *baseVertex, sutQuaternionOperator *IKQuaternionResult, float *augmentedMatrix)
{
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
    MatrixOperator_Mul_4x4M_4x4M(BoneVertexMatrix, IKQuaternionResult->getMatrix(), matrix4x4);
    MatrixOperator_Mul_4x4M_4x4M(matrix4x4, invBoneVertexMatrix, augmentedMatrix);
}

static inline float CCD_getRotationVector2(float *targetVertex, float *reachVertex, float *baseVertex,
                                           struct IKChainUnit *pIKUnit, struct vaSkeletonNodeContext *pBone,
                                           float *dstOuterProduct)
{
    /*                  vertexTB     (Bone base)
     *  (Bone Target)<--------------(Bone IK[0])-------------(Bone IK[1])
     *                                 _|
     *                               _|
     *                             _|    vertexRB
     *                            |
     *                      (Bone reache )
    */
    sutQuaternionOperator globalInverseQuaternion;

    struct sutQuaternionContext *QCtx = pBone->globalQuaternion.getQuaternionCtx();
    globalInverseQuaternion.init();
    if(QCtx != NULL) {
        globalInverseQuaternion.setQuaternion(QCtx->a, QCtx->b, QCtx->c, QCtx->w);
        globalInverseQuaternion.asInverse();
    }

    float vector[3], vertexTB[3], vectorRB[3];
    getVector(targetVertex, baseVertex, vector);
    normalizeVector(vector);
    MulTransformMatrix(globalInverseQuaternion.getMatrix(), vector, 1, vertexTB);
    getVector(reachVertex, baseVertex, vector);
    normalizeVector(vector);
    MulTransformMatrix(globalInverseQuaternion.getMatrix(), vector, 1, vectorRB);
    getOuterProduct(vertexTB, vectorRB, dstOuterProduct);
    return getDotProduct(vertexTB, vectorRB);
}

static void CCD_truncateIKQuaternion2(int ikID, float *rotationVector, float rotationDot, float maxStepRotationAngle,
                                     struct IKChainUnit *pIKUnit, struct vaSkeletonNodeContext *pBone,
                                     sutQuaternionOperator *dstIKQuaternion)
{
    float rotationRadius = acos(rotationDot);
    float localRotationVector[3];

    //printf("rotationDot %f , rotationRadius %f, maxStepRotationAngle %f", rotationDot, rotationRadius, maxStepRotationAngle);
    if(rotationRadius > maxStepRotationAngle) rotationRadius = maxStepRotationAngle;
    else if(rotationRadius < -maxStepRotationAngle) rotationRadius = -maxStepRotationAngle;
    //printf(" .....rotationRadius %f\n",  rotationRadius);

    if(!pIKUnit->isLimit) { // The bone without angle limit will used the original rotation vector to rotate directly.
        dstIKQuaternion->init();
        dstIKQuaternion->asRotation(rotationVector[0], rotationVector[1], rotationVector[2], rotationRadius * 180.0f /3.1415926f);
    }
    else {
        sutQuaternionOperator globalInverseQuaternion, oldLocalQuaternion, newFullQuaternion;
        float rotationRadius = acos(rotationDot);

        if(rotationRadius > maxStepRotationAngle) rotationRadius = maxStepRotationAngle;
        else if(rotationRadius < -maxStepRotationAngle) rotationRadius = -maxStepRotationAngle;

        /* get the inverse global quaternion */
        struct sutQuaternionContext *QCtx = pBone->globalQuaternion.getQuaternionCtx();
        globalInverseQuaternion.init();
        if(QCtx != NULL) {
            globalInverseQuaternion.setQuaternion(QCtx->a, QCtx->b, QCtx->c, QCtx->w);
            globalInverseQuaternion.asInverse();
        }
        //printf("\nglobalInverseQuaternion    ");
        //globalInverseQuaternion.dump();

        /* get inverse local quaternion */
        QCtx = pIKUnit->localQuaternion.getQuaternionCtx();
        oldLocalQuaternion.init();
        if(QCtx != NULL) oldLocalQuaternion.setQuaternion(QCtx->a, QCtx->b, QCtx->c, QCtx->w);

        /* get the specify rotation quaternion */
        newFullQuaternion.init();
        newFullQuaternion.asRotation(rotationVector[0], rotationVector[1], rotationVector[2], rotationRadius * 180.0f /3.1415926f);
        newFullQuaternion.mul(&pIKUnit->localQuaternion, sutQuaternionOperator::INPUT_AS_RIGHT);    //---------> get new local rotation in this bone
        newFullQuaternion.truncate(pIKUnit->UpBound[0], pIKUnit->LowerBound[0],       //----------> new local quaternion
                                   pIKUnit->UpBound[1], pIKUnit->LowerBound[1],
                                   pIKUnit->UpBound[2], pIKUnit->LowerBound[2]);
        newFullQuaternion.normalize();
        //newFullQuaternion.dump();

        /*QCtx = newFullQuaternion.getQuaternionCtx();
        dstIKQuaternion->init();
        dstIKQuaternion->setQuaternion(QCtx->a, QCtx->b, QCtx->c, QCtx->w);
        return;*/
        //printf("oldLocalQuaternion 4    ");  newFullQuaternion.dump();

        QCtx = newFullQuaternion.getQuaternionCtx();
        pIKUnit->localQuaternion.setQuaternion(QCtx->a, QCtx->b, QCtx->c, QCtx->w);
       // printf("pIKUnit->localQuaternion.dump   "); pIKUnit->localQuaternion.dump();

        newFullQuaternion.mul(&pBone->globalQuaternion, sutQuaternionOperator::INPUT_AS_RIGHT);    //----------> new Full rotation quaternion

        /*printf("ikID %d\n", ikID);
        if(ikID !=0) {
            QCtx = newFullQuaternion.getQuaternionCtx();
            dstIKQuaternion->init();
            dstIKQuaternion->setQuaternion(QCtx->a, QCtx->b, QCtx->c, QCtx->w);
            printf("return\n");
            return;
        }*/

       // printf("oldLocalQuaternion 5    ");  newFullQuaternion.dump();

        /** Get the result quaternion.
         * The result accumlate quaternion is defined as :
         *
         *      New Full Quaterion = Result Global Accumlate Quaternion * (Old local Quaternion * Global Quaternion).
         *                                                                  |_________Old global rotation._________|
         *      Assume :
         *          Old local Quaternion : is unit quaternion.
         *          Global Quaternion    : is unit quaternion.
         *
         *      New Global Accumlate Quaterion * (Old local Quaternion * Global Quaternion)^-1  =  Result Global Accumlate Quaternion * (Old local Quaternion * Global Quaternion) * (Old local Quaternion * Global Quaternion)^-1.
         *
         *      Result Accumlate Quaternion  = New Global Quaterion * (Old local Quaternion * Global Quaternion)^-1 ;
        */
        QCtx = newFullQuaternion.getQuaternionCtx();
        dstIKQuaternion->init();
        dstIKQuaternion->setQuaternion(QCtx->a, QCtx->b, QCtx->c, QCtx->w);
        oldLocalQuaternion.mul(&pBone->globalQuaternion, sutQuaternionOperator::INPUT_AS_RIGHT);
        oldLocalQuaternion.normalize();
        oldLocalQuaternion.asInverse();
        dstIKQuaternion->mul(&oldLocalQuaternion, sutQuaternionOperator::INPUT_AS_RIGHT);
    }
}

static inline float CCD_getRotationVector(float *targetVertex, float *reachVertex, float *baseVertex, float *dstOuterProduct)
{
    /*                  vertexTB     (Bone base)
     *  (Bone Target)<--------------(Bone IK[0])-------------(Bone IK[1])
     *                                 _|
     *                               _|
     *                             _|    vertexRB
     *                            |
     *                      (Bone reache )
    */
    float vertexTB[3], vectorRB[3];
    getVector(targetVertex, baseVertex, vertexTB);
    getVector(reachVertex, baseVertex, vectorRB);
    normalizeVector(vertexTB);
    normalizeVector(vectorRB);
    getOuterProduct(vertexTB, vectorRB, dstOuterProduct);
    return getDotProduct(vertexTB, vectorRB);
}


static void CCD_truncateIKQuaternion(float *rotationVector, float rotationDot, float maxStepRotationAngle,
                                     struct IKChainUnit *pIKUnit, struct vaSkeletonNodeContext *pBone,
                                     sutQuaternionOperator *dstIKQuaternion)
{
    float rotationRadius = acos(rotationDot);
    float localRotationVector[3];

    if(rotationRadius > maxStepRotationAngle) rotationRadius = maxStepRotationAngle;
    else if(rotationRadius < -maxStepRotationAngle) rotationRadius = -maxStepRotationAngle;

    if(!pIKUnit->isLimit) { // The bone without angle limit will used the original rotation vector to rotate directly.
        dstIKQuaternion->init();
        dstIKQuaternion->asRotation(rotationVector[0], rotationVector[1], rotationVector[2], rotationRadius * 180.0f /3.1415926f);
    }
    else {
        sutQuaternionOperator globalInverseQuaternion, oldLocalQuaternion, newFullQuaternion;
        float rotationRadius = acos(rotationDot);

        if(rotationRadius > maxStepRotationAngle) rotationRadius = maxStepRotationAngle;
        else if(rotationRadius < -maxStepRotationAngle) rotationRadius = -maxStepRotationAngle;

        /* get the inverse global quaternion */
        struct sutQuaternionContext *QCtx = pBone->globalQuaternion.getQuaternionCtx();
        globalInverseQuaternion.init();
        if(QCtx != NULL) {
            globalInverseQuaternion.setQuaternion(QCtx->a, QCtx->b, QCtx->c, QCtx->w);
            globalInverseQuaternion.asInverse();
        }
        printf("\nglobalInverseQuaternion    ");
        globalInverseQuaternion.dump();

        /* get inverse local quaternion */
        QCtx = pIKUnit->localQuaternion.getQuaternionCtx();
        oldLocalQuaternion.init();
        if(QCtx != NULL) oldLocalQuaternion.setQuaternion(QCtx->a, QCtx->b, QCtx->c, QCtx->w);
        //printf("oldLocalQuaternion    ");
        //oldLocalQuaternion.dump();

        /* get the specify rotation quaternion */
        newFullQuaternion.init();
        //newFullQuaternion.asRotation(rotationVector[0], rotationVector[1], rotationVector[2], rotationRadius * 180.0f /3.1415926f);
        //printf("newFullQuaternion 1    ");  newFullQuaternion.dump();

        newFullQuaternion.mul(&pIKUnit->localQuaternion, sutQuaternionOperator::INPUT_AS_RIGHT);  //---------> get new local rotation in this bone
        newFullQuaternion.mul(&globalInverseQuaternion, sutQuaternionOperator::INPUT_AS_RIGHT);   //---------> get accumlate local rotation in this step

        MulTransformMatrix(newFullQuaternion.getMatrix(), rotationVector, 1, localRotationVector);
        newFullQuaternion.asRotation(localRotationVector[0], localRotationVector[1], localRotationVector[2], rotationRadius * 180.0f /3.1415926f);
        printf("localRotationVector %f %f %f\n", rotationVector[0], rotationVector[1], rotationVector[2]);

        newFullQuaternion.truncate(pIKUnit->UpBound[0], pIKUnit->LowerBound[0],       //----------> new local quaternion
                                   pIKUnit->UpBound[1], pIKUnit->LowerBound[1],
                                   pIKUnit->UpBound[2], pIKUnit->LowerBound[2]);
        newFullQuaternion.normalize();
        //printf("oldLocalQuaternion 4    ");  newFullQuaternion.dump();

        QCtx = newFullQuaternion.getQuaternionCtx();
        pIKUnit->localQuaternion.setQuaternion(QCtx->a, QCtx->b, QCtx->c, QCtx->w);
       // printf("pIKUnit->localQuaternion.dump   "); pIKUnit->localQuaternion.dump();

        newFullQuaternion.mul(&pBone->globalQuaternion, sutQuaternionOperator::INPUT_AS_RIGHT);    //----------> new Full rotation quaternion
       // printf("oldLocalQuaternion 5    ");  newFullQuaternion.dump();

        /** Get the result quaternion.
         * The result accumlate quaternion is defined as :
         *
         *      New Full Quaterion = Result Global Accumlate Quaternion * (Old local Quaternion * Global Quaternion).
         *                                                                  |_________Old global rotation._________|
         *      Assume :
         *          Old local Quaternion : is unit quaternion.
         *          Global Quaternion    : is unit quaternion.
         *
         *      New Global Accumlate Quaterion * (Old local Quaternion * Global Quaternion)^-1  =  Result Global Accumlate Quaternion * (Old local Quaternion * Global Quaternion) * (Old local Quaternion * Global Quaternion)^-1.
         *
         *      Result Accumlate Quaternion  = New Global Quaterion * (Old local Quaternion * Global Quaternion)^-1 ;
        */
        QCtx = newFullQuaternion.getQuaternionCtx();
        dstIKQuaternion->init();
        dstIKQuaternion->setQuaternion(QCtx->a, QCtx->b, QCtx->c, QCtx->w);
        oldLocalQuaternion.mul(&pBone->globalQuaternion, sutQuaternionOperator::INPUT_AS_RIGHT);
        oldLocalQuaternion.normalize();
        oldLocalQuaternion.asInverse();
        dstIKQuaternion->mul(&oldLocalQuaternion, sutQuaternionOperator::INPUT_AS_RIGHT);
    }
}

static void getCCDAugmentedMatrix(float *baseVertex, float *rotationVector, float rotationDot, float maxAngle,
                                  struct IKChainUnit *pIKUnit, struct vaSkeletonNodeContext *pBone,
                                  float *augmentedMatrix, sutQuaternionOperator *IKQuaternionResult)
{
    /** Reference from :
     * https://en.wikipedia.org/wiki/Conversion_between_quaternions_and_Euler_angles
    */
    sutQuaternionOperator globalInverseQuaternion, IKQuaternion, IKQuaternionTmp;
    float quaternionTruncate[4], quaternionNew[4], quaternionOld[4]  ;
    float rotationRadius = acos(rotationDot);

    if(rotationRadius > maxAngle) rotationRadius = maxAngle;
    else if(rotationRadius < -maxAngle) rotationRadius = -maxAngle;

    /* get the inverse global quaternion */
    struct sutQuaternionContext *gga = pBone->globalQuaternion.getQuaternionCtx();
    globalInverseQuaternion.init();
    globalInverseQuaternion.setQuaternion(gga->a, gga->b, gga->c, gga->w);
    globalInverseQuaternion.asInverse();

    /* temporate the last local quaternion */
    pIKUnit->localQuaternion.getQuaternion(quaternionOld);

    /* get the specify rotation quaternion */
    IKQuaternion.init();
    IKQuaternion.asRotation(rotationVector[0], rotationVector[1], rotationVector[2], rotationRadius * 180.0f /3.1415926f);
    IKQuaternion.mul(&globalInverseQuaternion, sutQuaternionOperator::INPUT_AS_RIGHT);
    IKQuaternion.mul(&pIKUnit->localQuaternion, sutQuaternionOperator::INPUT_AS_RIGHT); //---------> get the local rotation in this bone
    IKQuaternion.getQuaternion(quaternionNew);
    float x = quaternionNew[0];
    float y = quaternionNew[1];
    float z = quaternionNew[2];
    float w = quaternionNew[3];
    float radiusRoll  = atan2(2.0f *(w *x + y *z), 1.0f -2.0f *(x *x +y *y));
    float radiusPitch = asin (2.0f *(w *y - z *x));
    float radiusYaw   = atan2(2.0f *(w *z + x *y), 1.0f -2.0f *(y *y +z *z));

    if(pIKUnit->isLimit) {  // truncate rotation angle
        if(radiusRoll  > pIKUnit->UpBound[0])       radiusRoll  =  pIKUnit->UpBound[0];
        if(radiusRoll  < pIKUnit->LowerBound[0])    radiusRoll  =  pIKUnit->LowerBound[0];
        if(radiusPitch > pIKUnit->UpBound[1])       radiusPitch =  pIKUnit->UpBound[1];
        if(radiusPitch < pIKUnit->LowerBound[1])    radiusPitch =  pIKUnit->LowerBound[1];
        if(radiusYaw   > pIKUnit->UpBound[2])       radiusYaw   =  pIKUnit->UpBound[2];
        if(radiusYaw   < pIKUnit->LowerBound[2])    radiusYaw   =  pIKUnit->LowerBound[2];
    }
    quaternionTruncate[0] = cos(radiusRoll/2) *cos(radiusPitch/2) *cos(radiusYaw/2) +sin(radiusRoll/2) *sin(radiusPitch/2) *sin(radiusYaw/2);
    quaternionTruncate[1] = sin(radiusRoll/2) *cos(radiusPitch/2) *cos(radiusYaw/2) -cos(radiusRoll/2) *sin(radiusPitch/2) *sin(radiusYaw/2);
    quaternionTruncate[2] = cos(radiusRoll/2) *sin(radiusPitch/2) *cos(radiusYaw/2) +sin(radiusRoll/2) *cos(radiusPitch/2) *sin(radiusYaw/2);
    quaternionTruncate[3] = cos(radiusRoll/2) *cos(radiusPitch/2) *sin(radiusYaw/2) -sin(radiusRoll/2) *sin(radiusPitch/2) *cos(radiusYaw/2);
    pIKUnit->localQuaternion.init();
    pIKUnit->localQuaternion.setQuaternion(quaternionTruncate[1], quaternionTruncate[2], quaternionTruncate[3], quaternionTruncate[0]);
    pIKUnit->localQuaternion.normalize();

    /**
     * Assume :
     *      pIKUnit->localQuaternion(Truncate) = IKQuaternionResult * pIKUnit->localQuaternion(Old)
     *
     * Multiple the Inverse(pIKUnit->localQuaternion(Old)) :
     *      pIKUnit->localQuaternion(Truncate) * Inverse(pIKUnit->localQuaternion(Old)) = IKQuaternionResult * pIKUnit->localQuaternion(Old) * Inverse(pIKUnit->localQuaternion(Old))
     *
     * Then :
     *      IKQuaternionResult = pIKUnit->localQuaternion(Truncate) * Inverse(pIKUnit->localQuaternion(Old))
    */
    IKQuaternionTmp.init();
    IKQuaternionTmp.setQuaternion(quaternionOld[0], quaternionOld[1],quaternionOld[2],quaternionOld[3]);
    IKQuaternionTmp.asInverse();
    IKQuaternionResult->init();
    IKQuaternionResult->setQuaternion(quaternionTruncate[1], quaternionTruncate[2], quaternionTruncate[3], quaternionTruncate[0]);
    IKQuaternionResult->mul(&IKQuaternionTmp, sutQuaternionOperator::INPUT_AS_RIGHT);
    IKQuaternionResult->mul(&pBone->globalQuaternion, sutQuaternionOperator::INPUT_AS_LEFT);

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
    MatrixOperator_Mul_4x4M_4x4M(BoneVertexMatrix, IKQuaternionResult->getMatrix(), matrix4x4);
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


    /*for(iki = 0 ; iki < vaMemory->BoneMemory.IKChainCount ; iki++) {
        struct IKChainContext *pIKChain = vaMemory->BoneMemory.IKChain + iki;
        for(ci = 0 ; ci < pIKChain->IKChainCount  ; ci++) {
            struct IKChainUnit *pIKUnit = (pIKChain->IKChain + ci);
            struct IKChainUnit *pIKUnitNext = (pIKChain->IKChain + ci - 1);
            struct vaSkeletonNodeContext *pBone = (skeletonCtx->chainNodeSet + pIKUnit->LinkBone);
            //superpose_IKAugmentedMatrix(pBone, pIKUnit->augmentedMatrix, bIDNext);

            sutQuaternionOperator globalInverseQuaternion;
            struct sutQuaternionContext *gga = pBone->globalQuaternion.getQuaternionCtx();
            globalInverseQuaternion.init();
            globalInverseQuaternion.setQuaternion(gga->a, gga->b, gga->c, gga->w);
            globalInverseQuaternion.asInverse();
            globalInverseQuaternion.dump();

            float matrix4x4f[16], vertrex3f[3];
            MatrixOperator_Mul_4x4M_4x4M(globalInverseQuaternion.getMatrix(), pBone->TransformMatrix, matrix4x4f);
            memcpy(pBone->TransformMatrix, matrix4x4f, sizeof(float) *16);
            MulTransformMatrix(globalInverseQuaternion.getMatrix(), pBone->pBoneVertex, 1.0f, vertrex3f);
            memcpy(pBone->pBoneVertex, vertrex3f, sizeof(float) *3);
        }
    }
    return;*/

    for(iki = 0 ; iki < vaMemory->BoneMemory.IKChainCount ; iki++) {
   // for(iki = 0 ; iki < 1 ; iki++) {
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
            pIKUnit->localQuaternion.init();
        }

        // apply CCD algorithm (IK algorithm)
       // maxIterations = 1;
        while(maxIterations >0) {
            maxIterations--;
            //printf("\nmaxIterations %d\n", maxIterations);
            for(ci = 0 ; ci < pIKChain->IKChainCount ; ci++) {
                //if(ci >2) break;
                struct IKChainUnit *pIKUnit = (pIKChain->IKChain + ci);
                struct vaSkeletonNodeContext *pBone = (skeletonCtx->chainNodeSet + pIKUnit->LinkBone);
                float *baseVertex = pIKUnit->cVertex;
                float dotProduct, outerProduct[3], CCDStepMatrix[16];
                sutQuaternionOperator IKStepQuaternion;

                /** Test1 Calculate the step matrix by CCD algorithm.
                //CCD_calcAugmentedMatrix(tVertex, ReachVertex, baseVertex, pIKChain->MaxAngle, CCDStepMatrix);
                */

                /** Test 2
                dotProduct = getRotationVector(tVertex, ReachVertex, baseVertex, pBone, outerProduct);
                getCCDAugmentedMatrix(baseVertex, outerProduct, dotProduct, pIKChain->MaxAngle, pIKUnit, pBone, CCDStepMatrix, &IKStepQuaternion);
                */

                //dotProduct = CCD_getRotationVector(tVertex, ReachVertex, baseVertex, outerProduct);
                //IKStepQuaternion.asRotation(outerProduct[0], outerProduct[1], outerProduct[2], acos(dotProduct) * 180.0f /3.14152);
                //CCD_getStepAugmentedMatrix(baseVertex, &IKStepQuaternion, CCDStepMatrix);

                //** Test 3
               // dotProduct = CCD_getRotationVector(tVertex, ReachVertex, baseVertex, outerProduct);
                //CCD_truncateIKQuaternion(outerProduct, dotProduct, pIKChain->MaxAngle, pIKUnit, pBone, &IKStepQuaternion);
                if(pIKUnit->isLimit)
                    dotProduct = CCD_getRotationVector2(tVertex, ReachVertex, baseVertex,  pIKUnit, pBone, outerProduct);
                else
                    dotProduct = CCD_getRotationVector(tVertex, ReachVertex, baseVertex, outerProduct);
                CCD_truncateIKQuaternion2(ci, outerProduct, dotProduct, pIKChain->MaxAngle, pIKUnit, pBone, &IKStepQuaternion);
                CCD_getStepAugmentedMatrix(baseVertex, &IKStepQuaternion, CCDStepMatrix);
             //   printf("rotation : "); IKStepQuaternion.dump();


                //dotProduct = CCD_getRotationVector(tVertex, ReachVertex, baseVertex, outerProduct);
                //getCCDAugmentedMatrix(baseVertex, outerProduct, dotProduct, pIKChain->MaxAngle, pIKUnit, pBone, CCDStepMatrix, &IKStepQuaternion);
                //CCD_calcAugmentedMatrix(tVertex, ReachVertex, baseVertex, pIKChain->MaxAngle, CCDStepMatrix);

                // Superpose the matrix into the vertex & matrix on "nodes in the IK chain".
                for(ui = 0 ; ui <= ci ; ui++) {
                    struct IKChainUnit *pUnit = (pIKChain->IKChain + ui);
                    struct vaSkeletonNodeContext *pIKBone = (skeletonCtx->chainNodeSet + pUnit->LinkBone);
                    MatrixOperator_Mul_4x4M_4x4M(CCDStepMatrix, pUnit->augmentedMatrix, matrix4x4);
                    memcpy(pUnit->augmentedMatrix, matrix4x4, sizeof(float)*16);
                    MulTransformMatrix(CCDStepMatrix, pUnit->cVertex ,1 ,vertex3);
                    memcpy(pUnit->cVertex, vertex3, sizeof(float)*3);

                    if(ui != ci) {
                        pIKBone->globalQuaternion.mul(&IKStepQuaternion, sutQuaternionOperator::INPUT_AS_LEFT);
                    }
                }
                //superpose the matrix into vertex & matrix of "target node"
                MatrixOperator_Mul_4x4M_4x4M(CCDStepMatrix, tAugmentedMatrix ,matrix4x4);
                memcpy(tAugmentedMatrix, matrix4x4, sizeof(float)*16);
                MulTransformMatrix(CCDStepMatrix, tVertex ,1, vertex3);
                memcpy(tVertex, vertex3, sizeof(float)*3);
            }
            /* check the offset between reach and target node */
            float offset[3] = {tVertex[0] - ReachVertex[0], tVertex[1]- ReachVertex[1], tVertex[2]- ReachVertex[2]};
            float accuracy = offset[0] * offset[0] + offset[1] * offset[1] + offset[2] * offset[2];
            if(accuracy < 0.001f) break;
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
