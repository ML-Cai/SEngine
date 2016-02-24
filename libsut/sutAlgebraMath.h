#ifndef SUTALGEBRAMATH_H
#define SUTALGEBRAMATH_H
//--------------------------------------------------------------------------
#ifdef  __cplusplus
extern "C" {
#endif
// ----------------------------------------------------------------------------------
inline void sutNormalizeVector3(float *inVector)
{
    float len = sqrtf(inVector[0] *inVector[0] + inVector[1] *inVector[1] + inVector[2] *inVector[2]);
    float invLen = (len == 0.0f) ? 1.0f : 1.0f/len ;
    inVector[0] *=invLen;
    inVector[1] *=invLen;
    inVector[2] *=invLen;
}
// ----------------------------------------------------------------------------------
inline float sutDotProduct_3x3(float *VectorA, float *VectorB)
{
   return VectorA[0] * VectorB[0] +  VectorA[1] * VectorB[1] + VectorA[2] * VectorB[2];
}
// ----------------------------------------------------------------------------------
inline void sutCrossProduct_3x3(float *VectorA, float *VectorB, float *dstVector)
{
    dstVector[0] = VectorA[1]* VectorB[2] - VectorA[2]* VectorB[1];
    dstVector[1] = VectorA[2]* VectorB[0] - VectorA[0]* VectorB[2];
    dstVector[2] = VectorA[0]* VectorB[1] - VectorA[1]* VectorB[0];
}
// ----------------------------------------------------------------------------------
inline void sutMatrixOperator_Mul_4x4M_4x4M(float *inMatrix_A, float *inMatrix_B, float *outMatrix)
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
//--------------------------------------------------------------------------
#ifdef  __cplusplus
}
#endif
//--------------------------------------------------------------------------
#endif // SUTALGEBRAMATH_H
