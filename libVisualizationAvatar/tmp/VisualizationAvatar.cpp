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
#include "sutDataStruct.h"
#include "VisualizationAvatar.h"
#include "PMD_Profile.h"
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
// ----------------------------------------------------------------------------------
// Default Shader
const char VertexShaderContext_Default[] = " \
uniform mat4 inProjectionMatrix; \
uniform mat4 inModelViewMatrix; \
attribute vec3 inVertex; \
attribute vec4 inColor; \
varying vec4 vColor; \
void main() \
{ \
    vColor = inColor;\
    gl_Position = inProjectionMatrix * inModelViewMatrix * vec4(inVertex.x, inVertex.y, inVertex.z, 1.0); \
}";

const char FragmentShaderContext_Default[] = " \
varying vec4 vColor; \
void main() \
{ \
    gl_FragColor = vColor;\
}";
/* -------------------------------------------------
 *  Toon Shader
 */
const char ToonVertexShaderContext[] = " \
uniform mat4 inProjectionMatrix; \
uniform mat4 inModelViewMatrix; \
uniform vec3 inLightPosition; \
attribute vec3 inVertex; \
attribute vec3 inNormal; \
varying vec3 vLightVector_0 ; \
varying vec3 vNormal; \
void main() \
{ \
    vec4 ModelView_glVertex = inModelViewMatrix *  vec4(inVertex.x, inVertex.y, inVertex.z, 1.0); \
    vLightVector_0 = normalize(inLightPosition - inVertex); \
    vNormal = normalize(inNormal); \
    gl_Position = inProjectionMatrix * ModelView_glVertex; \
}";

const char ToonFragmentShaderContext[] = " \
varying vec3 vLightVector_0 ; \
varying vec3 vNormal; \
uniform vec4 AmbientColor; \
uniform vec4 DiffuseColor; \
void main() \
{ \
    vec4 color; \
    float intensity = dot(vLightVector_0, vNormal); \
    if (intensity > 0.8) \
        color = vec4(0.8, 0.8, 0.8, 1.0); \
    else if (intensity > 0.5) \
        color = vec4(0.5, 0.5, 0.5, 1.0); \
    else if (intensity > 0.2) \
        color = vec4(0.2, 0.2, 0.2, 1.0); \
    else \
        color = vec4(0.1, 0.1, 0.1, 1.0); \
    gl_FragColor = (color * AmbientColor +DiffuseColor ) ;\
}";
/* -------------------------------------------------
 *  Toon Shader
 */
const char ToonTextureVertexShaderContext[] = " \
uniform mat4 inProjectionMatrix; \
uniform mat4 inModelViewMatrix; \
uniform vec3 inLightPosition; \
attribute vec2 inTexCoord; \
varying vec2 TexCoord; \
attribute vec3 inVertex; \
attribute vec3 inNormal; \
varying vec3 vLightVector_0 ; \
varying vec3 vNormal; \
void main() \
{ \
    vec4 ModelView_glVertex = inModelViewMatrix *  vec4(inVertex.x, inVertex.y, inVertex.z, 1.0); \
    vLightVector_0 = normalize(inLightPosition - inVertex); \
    vNormal = normalize(inNormal); \
    gl_Position = inProjectionMatrix * ModelView_glVertex; \
    TexCoord = inTexCoord; \
}";

const char ToonTextureFragmentShaderContext[] = " \
varying vec2 TexCoord; \
varying vec3 vNormal; \
varying vec3 vLightVector_0; \
uniform sampler2D uTexture_1; \
void main() \
{ \
    vec4 color; \
    float intensity = dot(vLightVector_0, vNormal); \
    if (intensity > 0.8) \
        color = vec4(1.5, 1.5, 1.5, 1.0); \
    else if (intensity > 0.5) \
        color = vec4(1.0, 1.0, 1.0, 1.0); \
    else \
        color = vec4(0.7, 0.7, 0.7, 1.0); \
    gl_FragColor = color * texture2D(uTexture_1, TexCoord ); \
}";
// ---------------------------------------------------------------------------------
using namespace MMDCppDev;
const float IdentityMatrix[16] ={1.0f, 0.0f, 0.0f, 0.0f,
                                 0.0f, 1.0f, 0.0f, 0.0f,
                                 0.0f, 0.0f, 1.0f, 0.0f,
                                 0.0f, 0.0f, 0.0f, 1.0f};
const float JointColorSet[][4] = {{1.0f, 0.0f, 0.0f, 1.0f}, //0
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
float DefaultDiffuse_RGBA[4] = {0.8f, 0.8f, 0.8f, 1.0f};
float DefaultSpecular_RGB[3] = {0.2f, 0.2f, 0.2f};
float DefaultAmbient_RGB[3] = {0.5f, 0.5f, 0.5f};
float DefaultSpecularity = 1.0f;
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

    /*outVertex[0] = inWeight *(inMatrix_A[0] * inMatrix_B[0] +
                              inMatrix_A[1] * inMatrix_B[4] +
                              inMatrix_A[2] * inMatrix_B[8] +
                              inMatrix_A[3] * inMatrix_B[12]);
    outVertex[1] = inWeight *(inMatrix_A[0] * inMatrix_B[1] +
                              inMatrix_A[1] * inMatrix_B[5] +
                              inMatrix_A[2] * inMatrix_B[9] +
                              inMatrix_A[3] * inMatrix_B[13]);
    outVertex[1] = inWeight *(inMatrix_A[0] * inMatrix_B[2] +
                              inMatrix_A[1] * inMatrix_B[6] +
                              inMatrix_A[2] * inMatrix_B[10] +
                              inMatrix_A[3] * inMatrix_B[14]);
    outVertex[2] = inWeight *(inMatrix_A[0] * inMatrix_B[3] +
                              inMatrix_A[1] * inMatrix_B[7] +
                              inMatrix_A[2] * inMatrix_B[11] +
                              inMatrix_A[3] * inMatrix_B[15]);*/
}
/* ---------------------------------------------------------------------------------
 * Multiple the transform matrix with vertex
 */
static inline void MulTransformMatrix(float *inMatrix, float *inVertex, float inWeight, float *outVertex)
{
    outVertex[0] = inWeight *(inMatrix[0] * inVertex[0] +
                              inMatrix[1] * inVertex[1] +
                              inMatrix[2] * inVertex[2] +
                              inMatrix[3]);
    outVertex[1] = inWeight *(inMatrix[4] * inVertex[0] +
                              inMatrix[5] * inVertex[1] +
                              inMatrix[6] * inVertex[2] +
                              inMatrix[7]) ;
    outVertex[2] = inWeight *(inMatrix[8] * inVertex[0] +
                              inMatrix[9] * inVertex[1] +
                              inMatrix[10] * inVertex[2] +
                              inMatrix[11]);
}
//---------------------------------------------------------------------------------
int VisualizationAvatar::loadShader(const char *srcContent, int Type)
{
    int isSuccess, LogLen;
    char *LogStr;
    unsigned int allocShader = 0;

    printf("Load Shader\n");

    allocShader = glCreateShader(Type);
    if(allocShader ==0) {
        printf("Fatal : Failed to create shader, maybe you run the openGL function in diffect thread ?\n");
    }
    else {
        glShaderSource(allocShader, 1, &srcContent, NULL);
        glCompileShader(allocShader);
        glGetShaderiv(allocShader, GL_COMPILE_STATUS, &isSuccess);
        if (isSuccess != GL_TRUE) {
            printf("Fatal : Failed to compile vertex shader.\n");
            glGetShaderiv(allocShader, GL_INFO_LOG_LENGTH, &LogLen);
            LogStr = (char *)malloc(LogLen +1);
            if(LogStr != NULL){
                glGetShaderInfoLog(allocShader, LogLen, NULL, LogStr);
                printf("Compilation error in shader  %s\n", LogStr);
                free(LogStr);
                glDeleteShader(allocShader);
                return 0;
            }
            else {
                printf("No memory space to list compilation error message\n");
            }
        }
        else
            printf("Info : Compile vertex shader success.\n");
    }
    return allocShader;
}
//---------------------------------------------------------------------------------
int VisualizationAvatar::genShaderProgram(struct VisualizationShaderProgram *dstShader, const char *srcVertexShder, const char *srcFragmentShader)
{
    dstShader->VertexShader = loadShader(srcVertexShder, GL_VERTEX_SHADER);
    if(dstShader->VertexShader ==0) return 0;
    dstShader->FragmentShader = loadShader(srcFragmentShader, GL_FRAGMENT_SHADER);
    if(dstShader->FragmentShader ==0) return 0;
    dstShader->Program = glCreateProgram();
    glAttachShader(dstShader->Program, dstShader->VertexShader);
    glAttachShader(dstShader->Program, dstShader->FragmentShader);
    glLinkProgram(dstShader->Program);
    return 1;
}

// ---------------------------------------------------------------------------------
int VisualizationAvatar::reincarnate()
{
    //prepare shader
    int ret = 1;
    ret &= genShaderProgram(&VisualCtx.DefaultShader, VertexShaderContext_Default, FragmentShaderContext_Default);
    ret &= genShaderProgram(&VisualCtx.ToonTextureShader, ToonVertexShaderContext, ToonFragmentShaderContext);
    ret &= genShaderProgram(&VisualCtx.MaterialTextureShader, ToonTextureVertexShaderContext, ToonTextureFragmentShaderContext);

    if(ret ==0) exit(0);
    //load mterial texture/ toon texture
    int i;
    int Tcount = Spirit->TextureMemory.TextureCount ;
    TextureContext *Image_ptr;

    VisualCtx.MaterialTexture = (GLuint *)malloc(sizeof(GLuint) * Tcount);
    glGenTextures(Tcount, VisualCtx.MaterialTexture);
    for(i = 0 ; i < Tcount ; i++) {
        Image_ptr = *(Spirit->TextureMemory.TextureImg +i) ;
        glBindTexture(GL_TEXTURE_2D, *(VisualCtx.MaterialTexture +i));
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, Image_ptr->Width, Image_ptr->Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, Image_ptr->RGBA8888);
        glBindTexture(GL_TEXTURE_2D, 0);
        printf("Create Texture[%d] in openGL ... result : %d\n", i, *(VisualCtx.MaterialTexture +i));
    }
    AvatarStatus |= AvatarState_Reincarnation;
}

/** ---------------------------------------------------------------------------------
 * Visualization the avatar
 *  Note : In QT , the shader must inherentde form QGLFunctions. unmark the USEING_QT in VisualizationAvatar.h will enable that.
 *  Note : This function must called in the thread with openGL context.
 * */
int VisualizationAvatar::visualize(float *ProjectionMatrix, float *ModelViewMatrix)
{
     GLfloat LightPosition[] = {2000.0f, 2000.0f, -2000.0f};
    int i;
    int ProjectionMatrix_L  = glGetUniformLocation(VisualCtx.ToonTextureShader.Program, "inProjectionMatrix");
    int ModleViewMatrix_L   = glGetUniformLocation(VisualCtx.ToonTextureShader.Program, "inModelViewMatrix");
    int AmbientColor_L      = glGetUniformLocation(VisualCtx.ToonTextureShader.Program, "AmbientColor");
    int DiffuseColor_L      = glGetUniformLocation(VisualCtx.ToonTextureShader.Program, "DiffuseColor");
    int inVertex_L          = glGetAttribLocation(VisualCtx.ToonTextureShader.Program, "inVertex");
    int inNormal_L          = glGetAttribLocation(VisualCtx.ToonTextureShader.Program, "inNormal");
    int inLightPosition_L   = glGetUniformLocation(VisualCtx.ToonTextureShader.Program, "inLightPosition");

    int ProjectionMatrix_L_T    = glGetUniformLocation(VisualCtx.MaterialTextureShader.Program, "inProjectionMatrix");
    int ModleViewMatrix_L_T     = glGetUniformLocation(VisualCtx.MaterialTextureShader.Program, "inModelViewMatrix");
    int inVertex_L_T            = glGetAttribLocation(VisualCtx.MaterialTextureShader.Program, "inVertex");
    int inNormal_L_T            = glGetAttribLocation(VisualCtx.MaterialTextureShader.Program, "inNormal");
    int inTexCoord_L_T          = glGetAttribLocation(VisualCtx.MaterialTextureShader.Program, "inTexCoord");
    int uTexture_1              = glGetUniformLocation(VisualCtx.MaterialTextureShader.Program, "uTexture_1");
    int inLightPosition_L_T     = glGetUniformLocation(VisualCtx.MaterialTextureShader.Program, "inLightPosition");

    struct MaterialUnitContext *Material_ptr = Spirit->MaterialMemory.MaterialUnit ;
    struct GLSurfaceRenderingContext *RenderingCtx = &BodyRederingCtx;
    if(RenderingCtx == NULL) return 0;
    if(!isReincarnate()) this->reincarnate();

    for(i = 0 ; i < RenderingCtx->GroupCtxCount ; i++) {
        struct SurfaceGroupContext *Group_ptr = RenderingCtx->GroupCtx +i ;
        if((Material_ptr +i)->TextureIndex != 255 ) {   // with material texture
            glUseProgram(VisualCtx.MaterialTextureShader.Program);
            glUniformMatrix4fv(ProjectionMatrix_L_T, 1, false, ProjectionMatrix);
            glUniformMatrix4fv(ModleViewMatrix_L_T, 1, false, ModelViewMatrix);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, *(VisualCtx.MaterialTexture + (Material_ptr +i)->TextureIndex));
            glUniform1i(uTexture_1, 0);
            glEnableVertexAttribArray(inTexCoord_L_T);
            glEnableVertexAttribArray(inVertex_L_T);
            glEnableVertexAttribArray(inNormal_L_T);
            glVertexAttribPointer(inTexCoord_L_T, 2, GL_FLOAT, false, 0, RenderingCtx->Texture);
            glVertexAttribPointer(inNormal_L_T, 3, GL_FLOAT, false, 0, RenderingCtx->Noraml);
            glVertexAttribPointer(inVertex_L_T, 3, GL_FLOAT, false, 0, RenderingCtx->Vertex);
            glUniform3fv(inLightPosition_L_T, 1, LightPosition);
        }
        else {  // default toon texture
            glUseProgram(VisualCtx.ToonTextureShader.Program);
            glEnableVertexAttribArray(inVertex_L);
            glEnableVertexAttribArray(inNormal_L);
            glVertexAttribPointer(inNormal_L, 3, GL_FLOAT, false, 0, RenderingCtx->Noraml);
            glVertexAttribPointer(inVertex_L, 3, GL_FLOAT, false, 0, RenderingCtx->Vertex);
            glUniformMatrix4fv(ProjectionMatrix_L, 1, false, ProjectionMatrix);
            glUniformMatrix4fv(ModleViewMatrix_L, 1, false, ModelViewMatrix);
            glUniform3fv(inLightPosition_L, 1, LightPosition);
        }
        glUniform4fv(AmbientColor_L, 1, Group_ptr->Ambient_RGB);
        glUniform4fv(DiffuseColor_L, 1, Group_ptr->Diffuse_RGBA);

        switch(Spirit->IndexMemory.IndexUnitTypeSize) {
        case 1:
            glDrawElements(GL_TRIANGLES, Group_ptr->IndexCount, GL_UNSIGNED_BYTE, Group_ptr->Index_ptr);
            break;
        case 2:
            glDrawElements(GL_TRIANGLES, Group_ptr->IndexCount, GL_UNSIGNED_SHORT, Group_ptr->Index_ptr);
            break;
        case 4:
            glDrawElements(GL_TRIANGLES, Group_ptr->IndexCount, GL_UNSIGNED_INT, Group_ptr->Index_ptr);
            break;
        }
        if((Material_ptr +i)->TextureIndex !=NULL) {
            glBindTexture(GL_TEXTURE_2D, 0);
        }
    }
}

/* ---------------------------------------------------------------------------------
 * Generate the Slelecton Architecture.
 *
 */
int VisualizationAvatar::
generate_SkeletonArchitecture()
{
    int i;
    const int BoneInfoCount = Spirit->BoneMemory.BoneUnitCount;
    int CheckedParent = Spirit->BoneMemory.NullParentID;
    sutPoolQueue UnknowQueue;   /* Store the bone which praent unknow */
    sutPoolQueue RecognizeQueue;
    sutPoolQueue ChildSearchQueue;
    printf("generate_SkeletonArchitecture\n");
    RecognizeQueue.init(sutPoolQueue::ModeAuto, 128);
    UnknowQueue.init(sutPoolQueue::ModeAuto, 128);
    ChildSearchQueue.init(sutPoolQueue::ModeAuto, 128);
    MasterSkeletonCtx.ChainNodeSet = (struct SkeletonChainContext *)malloc(sizeof(struct SkeletonChainContext) * BoneInfoCount);

    /* init unknow queue */
    MasterSkeletonCtx.ChainNodeCount =BoneInfoCount;
    MasterSkeletonCtx.JointCount =0;
    MasterSkeletonCtx.ChainRoot.QuaternionCtx.init();
    MasterSkeletonCtx.ChainRoot.QuaternionCtx.toMatrix();
    MasterSkeletonCtx.ChainRoot.isTouch =0;
    MasterSkeletonCtx.ChainRoot.nID = Spirit->BoneMemory.NullParentID;
    MasterSkeletonCtx.ChainRoot.Parnet = NULL;
    MasterSkeletonCtx.ChainRoot.RawBoneInfo_ptr = NULL;
    memcpy(&MasterSkeletonCtx.ChainRoot.TransformMatrix, IdentityMatrix, sizeof(float)*16);

    for(i = 0 ; i < BoneInfoCount ; i++) {
        struct SkeletonChainContext *ChainNode_ptr = MasterSkeletonCtx.ChainNodeSet +i;
        ChainNode_ptr->RawBoneInfo_ptr =Spirit->BoneMemory.BoneUnit +i;
        ChainNode_ptr->nID = i;
        ChainNode_ptr->isTouch = 0;
        ChainNode_ptr->Parnet = NULL;
        ChainNode_ptr->Child = NULL;
        ChainNode_ptr->ChildCount = 0;
        ChainNode_ptr->Motion_ptr = NULL;
        ChainNode_ptr->Frame1_ptr = NULL;
        ChainNode_ptr->Frame2_ptr = NULL;
        ChainNode_ptr->FrameInMotion_Index = 0;
        ChainNode_ptr->VertexChainCount =0;
        ChainNode_ptr->WorldQuaternion.init();
        ChainNode_ptr->WorldQuaternion.toMatrix();
        ChainNode_ptr->QuaternionCtx.init();
        ChainNode_ptr->QuaternionCtx.toMatrix();
        ChainNode_ptr->BoneVertex = SkeletonRenderingCtx.JointVertex + i*3 ;
        memcpy(ChainNode_ptr->TransformMatrix, IdentityMatrix, sizeof(float)*16);
        UnknowQueue.enqueue(ChainNode_ptr);
    }

    /* parse skeleton */
    do {
        int CheckSize = UnknowQueue.count();    /* uncheck node remaining */
        //printf("------------------------------------------\n");
        //printf("Parent %d\n", CheckedParent);
        for(i = 0 ; i < CheckSize ; i++) {
            struct SkeletonChainContext *CheckBone = (struct SkeletonChainContext *)UnknowQueue.dequeue();
            if(CheckBone->RawBoneInfo_ptr->bID_Parent == CheckedParent) {
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
            struct SkeletonChainContext *ParentNode = (CheckedParent == Spirit->BoneMemory.NullParentID) ?
                                                          &MasterSkeletonCtx.ChainRoot :
                                                          MasterSkeletonCtx.ChainNodeSet +CheckedParent;
            MasterSkeletonCtx.JointCount++;  // any node have child is recognited as jonit
            ParentNode->ChildCount = ChildCount;
            ParentNode->Child = (struct SkeletonChainContext **)malloc(sizeof(struct SkeletonChainContext *) *ChildCount);

            //printf("Child Count %d\n", ChildCount);
            for(i = 0 ; i < ChildCount ; i++) {
                *(ParentNode->Child +i) = (struct SkeletonChainContext *)ChildSearchQueue.dequeue();
                (*(ParentNode->Child +i))->Parnet = ParentNode;
                //printf("\t Child %d\n", (*(ParentNode->Child +i))->nID );
            }
        }

        /* swtich to next parent node which needced to find child */
        CheckedParent  = ((struct SkeletonChainContext *)RecognizeQueue.dequeue())->nID;
    } while(UnknowQueue.count() >0);

    printf("Bond Node Count : %d\n", MasterSkeletonCtx.ChainNodeCount);
    printf("Joint Count : %d\n", MasterSkeletonCtx.JointCount);

    RecognizeQueue.release();
    UnknowQueue.release();
    ChildSearchQueue.release();
}

/* ---------------------------------------------------------------------------------
 * Convert the data buffer and surface buffer into vertex array.
 */
int VisualizationAvatar::
generate_BodySurface()
{
    int i;
    printf("generate_BodySurface %d\n", Spirit->VertexMemory.VertexUnitCount);
    BodyRederingCtx.Vertex = (float *)malloc(sizeof(float) * Spirit->VertexMemory.VertexUnitCount * 3);
    BodyRederingCtx.Noraml = (float *)malloc(sizeof(float) * Spirit->VertexMemory.VertexUnitCount * 3);
    BodyRederingCtx.Texture = (float *)malloc(sizeof(float) * Spirit->VertexMemory.VertexUnitCount * 2);

    memcpy(BodyRederingCtx.Vertex, Spirit->VertexMemory.Vertex, sizeof(float) * Spirit->VertexMemory.VertexUnitCount * 3);
    memcpy(BodyRederingCtx.Noraml, Spirit->VertexMemory.Normal, sizeof(float) * Spirit->VertexMemory.VertexUnitCount * 3);
    memcpy(BodyRederingCtx.Texture, Spirit->VertexMemory.TexCoord, sizeof(float) * Spirit->VertexMemory.VertexUnitCount * 2);

    if(Spirit->MaterialMemory.MaterialUnitCount >0) {
        int IndexCounter =0;
        BodyRederingCtx.GroupCtxCount = Spirit->MaterialMemory.MaterialUnitCount;
        BodyRederingCtx.GroupCtx = (struct SurfaceGroupContext *)malloc(sizeof(struct SurfaceGroupContext) *BodyRederingCtx.GroupCtxCount);
        for(i = 0 ; i < BodyRederingCtx.GroupCtxCount ; i++) {
            struct SurfaceGroupContext *Group_ptr = BodyRederingCtx.GroupCtx +i;
            Group_ptr->Ambient_RGB      = (Spirit->MaterialMemory.MaterialUnit +i)->Ambient_RGB;
            Group_ptr->Diffuse_RGBA     = (Spirit->MaterialMemory.MaterialUnit +i)->Diffuse_RGBA;
            Group_ptr->Specularity      = &(Spirit->MaterialMemory.MaterialUnit +i)->Specularity;
            Group_ptr->Specular_RGB     = (Spirit->MaterialMemory.MaterialUnit +i)->Specular_RGB;
            Group_ptr->IndexCount       = (Spirit->MaterialMemory.MaterialUnit +i)->SurfaceCount;
            Group_ptr->TextureReference = (Spirit->MaterialMemory.MaterialUnit +i)->ToonReference;
            Group_ptr->Index_ptr        = Spirit->IndexMemory.IndexUnit + IndexCounter *Spirit->IndexMemory.IndexUnitTypeSize;
            IndexCounter += Group_ptr->IndexCount ;
        }
    }
    else {
        BodyRederingCtx.GroupCtxCount = 1;
        BodyRederingCtx.GroupCtx = (struct SurfaceGroupContext *)malloc(sizeof(struct SurfaceGroupContext));
        struct SurfaceGroupContext *Group_ptr = BodyRederingCtx.GroupCtx;
        Group_ptr->Ambient_RGB      = DefaultAmbient_RGB;
        Group_ptr->Diffuse_RGBA     = DefaultDiffuse_RGBA;
        Group_ptr->Specularity      = &DefaultSpecularity;
        Group_ptr->Specular_RGB     = DefaultSpecular_RGB;
        Group_ptr->IndexCount       = Spirit->IndexMemory.IndexUnitCount;
        Group_ptr->Index_ptr        = Spirit->IndexMemory.IndexUnit ;
        printf("Group_ptr->IndexCount  %d\n", Group_ptr->IndexCount );
    }
    return 0;
}
// ---------------------------------------------------------------------------------
int VisualizationAvatar::
generate_SkeletonSurface()
{
    int i;
    printf("generate_SkeletonSurface\n");
    SkeletonRenderingCtx.JointCount = Spirit->BoneMemory.BoneUnitCount ;
    SkeletonRenderingCtx.JointVertex = (float *)malloc(sizeof(float) * SkeletonRenderingCtx.JointCount *3);
    SkeletonRenderingCtx.JointColor  = (float *)malloc(sizeof(float) * SkeletonRenderingCtx.JointCount *4);
    struct BoneUnitContext *RawBoneInfo_ptr = Spirit->BoneMemory.BoneUnit ;
    float *StoreVertex_ptr = SkeletonRenderingCtx.JointVertex;
    float *StoreColor_ptr = SkeletonRenderingCtx.JointColor;

    for(i = 0 ; i < Spirit->BoneMemory.BoneUnitCount ; i++) {
        *StoreVertex_ptr++ = RawBoneInfo_ptr->BoneVertex[0] ;
        *StoreVertex_ptr++ = RawBoneInfo_ptr->BoneVertex[1];
        *StoreVertex_ptr++ = RawBoneInfo_ptr->BoneVertex[2];
        *StoreColor_ptr++ = JointColorSet[0][0];
        *StoreColor_ptr++ = JointColorSet[0][1];
        *StoreColor_ptr++ = JointColorSet[0][2];
        *StoreColor_ptr++ = JointColorSet[0][3];
        RawBoneInfo_ptr++;
    }

    SkeletonRenderingCtx.BoneCount =0;
    for(int i =0 ; i < Spirit->BoneMemory.BoneUnitCount ; i++) {
        struct BoneUnitContext *RawBoneInfo_ptr = Spirit->BoneMemory.BoneUnit +i;

        if(RawBoneInfo_ptr->bID_Parent != Spirit->BoneMemory.NullParentID) {
            SkeletonRenderingCtx.BoneCount +=2;
        }
    }

    SkeletonRenderingCtx.BoneIndex = (unsigned short *)malloc(sizeof(unsigned short) * SkeletonRenderingCtx.BoneCount);
    unsigned short *store_ptr = SkeletonRenderingCtx.BoneIndex;
    for(int i =0 ; i < Spirit->BoneMemory.BoneUnitCount ; i++) {
        struct BoneUnitContext *RawBoneInfo_ptr = Spirit->BoneMemory.BoneUnit +i;

        if(RawBoneInfo_ptr->bID_Parent != Spirit->BoneMemory.NullParentID) {
            *store_ptr++ = RawBoneInfo_ptr->bID_Parent;
            *store_ptr++ = i;
        }
    }
}
// ---------------------------------------------------------------------------------
int VisualizationAvatar::
set_BoneQuaternionParam(int TargetDone_ID, \
                        float Quaternion_ax, float Quaternion_by, float Quaternion_cz, float Quaternion_w,
                        float Translate_x, float Translate_y, float Translate_z,
                        PostureModeSet setMode)
{
    sutQuaternionOperator Quaternion;
    struct SkeletonChainContext *fBone = MasterSkeletonCtx.ChainNodeSet +TargetDone_ID;

    if(TargetDone_ID > Spirit->BoneMemory.BoneUnitCount) {
        printf("Unhandle bone in model : %d\n", TargetDone_ID);
        return 0;
    }

    fBone->isTouch = 1;
    switch(setMode) {
    case PostureMode_Replace:
        fBone->QuaternionCtx.setRotation(Quaternion_ax, Quaternion_by, Quaternion_cz, Quaternion_w);
        fBone->QuaternionCtx.setTranslate(Translate_x, Translate_y, Translate_z);
        fBone->QuaternionCtx.toMatrix();
        break;
    case PostureMode_Overlap_Mul:
        Quaternion.init();
        Quaternion.setRotation(Quaternion_ax, Quaternion_by, Quaternion_cz, Quaternion_w);
        Quaternion.setTranslate(Translate_x, Translate_y, Translate_z);
        fBone->QuaternionCtx.mul(&Quaternion);
        fBone->QuaternionCtx.toMatrix();
        break;
    case PostureMode_Overlap_Add:
        Quaternion.setRotation(Quaternion_ax, Quaternion_by, Quaternion_cz, Quaternion_w);
        Quaternion.setTranslate(Translate_x, Translate_y, Translate_z);
        fBone->QuaternionCtx.add(&Quaternion);
        fBone->QuaternionCtx.toMatrix();
    }

    /*if(TargetDone_ID == 1) {
        printf("%.6f %.6f %.6f %.6f\n", fBone->QuaternionCtx.getQuaternionCtx()->a,
               fBone->QuaternionCtx.getQuaternionCtx()->b,
               fBone->QuaternionCtx.getQuaternionCtx()->c,
               fBone->QuaternionCtx.getQuaternionCtx()->w);
    }*/
    return 1;
}
/* ---------------------------------------------------------------------------------
 * Calculate the transform matrix of childs with parent's transform matrix
 * */
void VisualizationAvatar::
calculate_SkeletonTransformMatrix(struct SkeletonChainContext * fBone)
{
    float CurrentMatrix[16];
    float tmp[16], tmp_1[16];
    float PostiveVertex[3] = {fBone->RawBoneInfo_ptr->BoneVertex[0],
                              fBone->RawBoneInfo_ptr->BoneVertex[1],
                              fBone->RawBoneInfo_ptr->BoneVertex[2]};
    float NativeVertex[3] = {-fBone->RawBoneInfo_ptr->BoneVertex[0],
                             -fBone->RawBoneInfo_ptr->BoneVertex[1],
                             -fBone->RawBoneInfo_ptr->BoneVertex[2]};

    memcpy(tmp, fBone->QuaternionCtx.getMatrix(), sizeof(float)*16);
    MatrixOperator_Mul_3x1V_4x4M(PostiveVertex, tmp);
    MatrixOperator_Mul_4x4M_3x1V(tmp, NativeVertex);
    MatrixOperator_Mul_4x4M_4x4M(fBone->Parnet->TransformMatrix, tmp, fBone->TransformMatrix);

    /*memcpy(tmp, fBone->QuaternionCtx.getMatrix(), sizeof(float)*16);
    MatrixOperator_Mul_3x1V_4x4M(PostiveVertex, tmp);
    MatrixOperator_Mul_4x4M_3x1V(tmp, NativeVertex);
    MatrixOperator_Mul_4x4M_4x4M(fBone->Parnet->TransformMatrix, tmp, fBone->TransformMatrix);*/

   /*float tmp_1[16];
    float BoneVertexMatrix[16];
    float invBoneVertexMatrix[16];
    memcpy(BoneVertexMatrix, IdentityMatrix, sizeof(float)*16);
    memcpy(invBoneVertexMatrix, IdentityMatrix, sizeof(float)*16);
    BoneVertexMatrix[3]     = fBone->BoneVertex[0];
    BoneVertexMatrix[7]     = fBone->BoneVertex[1];
    BoneVertexMatrix[11]    = fBone->BoneVertex[2];
    invBoneVertexMatrix[3]  = -fBone->BoneVertex[0];
    invBoneVertexMatrix[7]  = -fBone->BoneVertex[1];
    invBoneVertexMatrix[11] = -fBone->BoneVertex[2];

    // calculate the transform matrix of bone
    MatrixOperator_Mul_4x4M_4x4M(BoneVertexMatrix, fBone->QuaternionCtx.getMatrix(), tmp);
    MatrixOperator_Mul_4x4M_4x4M(tmp, invBoneVertexMatrix, tmp_1);
    MatrixOperator_Mul_4x4M_4x4M(fBone->Parnet->TransformMatrix, tmp_1, fBone->TransformMatrix);*/

    /*printf("---------------------------------------------------\n");
    printf("fBone %d\n", fBone->nID);
    for(int x = 0 ; x < 4 ; x++) {
        for(int y = 0 ; y < 4 ; y++) {
            printf("%f ,",  fBone->TransformMatrix[x*4+y]);
        }
        printf("\n");
    }*/
}
/* ---------------------------------------------------------------------------------
 * Update the Skeleton Transform matrix and Body Vertex after any bone is setted Quaternion Param by set_BoneQuaternionParam()
 *
 *      @param : fBone : The bone context which is needed to update,
 *                       As NULL will update the skeleton from skeleton root.
*/
void VisualizationAvatar::
update_SkeletonTransformMatrix(struct SkeletonChainContext * fBone)
{
    int ci, ChildCount;

    // Update the transfrom matrix of bone which is setted the new quaternion param.
    if(fBone == NULL) fBone = &MasterSkeletonCtx.ChainRoot;
    if(fBone != &MasterSkeletonCtx.ChainRoot) {
        calculate_SkeletonTransformMatrix(fBone);
        fBone->isTouch = 0;
    }
    else {  //The transform matrix in roochain is used to identify Obj asix and rotation.
        fBone->QuaternionCtx.toMatrix();
        memcpy(fBone->TransformMatrix, fBone->QuaternionCtx.getMatrix(), sizeof(float)*16);
    }

    // Recursive, update the child.
    ChildCount = fBone->ChildCount;
    for(ci = 0 ; ci < ChildCount ; ci++) {
        struct SkeletonChainContext *tBone = *(fBone->Child +ci);
        update_SkeletonTransformMatrix(tBone);
        tBone->WorldQuaternion.mul(&fBone->QuaternionCtx);
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
void VisualizationAvatar::
superpose_SkeletonTransformMatrix(struct SkeletonChainContext * fBone, float *srcMatrix)
{
    int i;
    float tmp[16], TranslateVertex[3];
    float *TranslateJoint;
    MatrixOperator_Mul_4x4M_4x4M(srcMatrix, fBone->TransformMatrix, tmp);
    memcpy(fBone->TransformMatrix, tmp, sizeof(float)*16);

    TranslateJoint = fBone->BoneVertex;
    MulTransformMatrix(srcMatrix, TranslateJoint ,1, TranslateVertex);
    memcpy(TranslateJoint, TranslateVertex, sizeof(float)*3);

    for(i = 0 ; i < fBone->ChildCount ; i++) {
        superpose_SkeletonTransformMatrix(*(fBone->Child +i), srcMatrix);
    }
}

/* ---------------------------------------------------------------------------------
 * Update the vertex of skeleton surface
 * */
void VisualizationAvatar::
update_SkeletonSurfaceVertex(struct SkeletonChainContext * fBone)
{
    int ci;
    struct SkeletonChainContext *SkeletonChain_ptr = fBone;

    /* Update Skeleton Bone Vertex */
    if(fBone != &MasterSkeletonCtx.ChainRoot) {
        //int nID = fBone->nID *3;
        float nVertex[3];
        // Using the RAW bone vertex to calcualte new vertex data with transform matrix
        // MulTransformMatrix(fBone->TransformMatrix, fBone->BoneVertex, 1.0f, nVertex);
        MulTransformMatrix(fBone->TransformMatrix, fBone->RawBoneInfo_ptr->BoneVertex, 1.0f,  fBone->BoneVertex);
        /*fBone->BoneVertex[0] = nVertex[0];
        fBone->BoneVertex[1] = nVertex[1];
        fBone->BoneVertex[2] = nVertex[2];*/ // <<< Same as Below
        //*(SkeletonRenderingCtx.JointVertex +nID)    = nVertex[0] ;
        //*(SkeletonRenderingCtx.JointVertex +nID +1) = nVertex[1] ;
        //*(SkeletonRenderingCtx.JointVertex +nID +2) = nVertex[2] ;
    }

    for(ci = 0; ci < SkeletonChain_ptr->ChildCount ; ci++) {
        struct SkeletonChainContext *tBone = *(SkeletonChain_ptr->Child +ci);
        update_SkeletonSurfaceVertex(tBone);
    }
}
/* ---------------------------------------------------------------------------------
 * Update the vertex of skeleton
 *      This function will recursively update the vertex by bone vertex chain.
 *
 *      @param fBone : updated bone, the child of this bone will be update recursively.
*/
inline void update_SurfaceVertex_SDEF(float *rawVertexAry,
                                       struct DeformType_SDEF *SDEF_ptr,
                                       struct SkeletonChainContext *SkeketonChain,
                                       float *dstVertexAry)
{
    float nVertex_b1[3], nVertex_b2[3];
    struct SkeletonChainContext *sBone_1 = SkeketonChain +SDEF_ptr->BoneIndex_1;
    struct SkeletonChainContext *sBone_2 = SkeketonChain +SDEF_ptr->BoneIndex_2;

    MulTransformMatrix(sBone_1->TransformMatrix, rawVertexAry, SDEF_ptr->Weight_1, nVertex_b1);
    MulTransformMatrix(sBone_2->TransformMatrix, rawVertexAry, 1.0f - SDEF_ptr->Weight_1, nVertex_b2);
    *(dstVertexAry)    = nVertex_b1[0] + nVertex_b2[0];
    *(dstVertexAry +1) = nVertex_b1[1] + nVertex_b2[1];
    *(dstVertexAry +2) = nVertex_b1[2] + nVertex_b2[2];
}

inline void update_SurfaceVertex_BDEF4(float *rawVertexAry,
                                       struct DeformType_BDEF4 *BDEF4_ptr,
                                       struct SkeletonChainContext *SkeketonChain,
                                       float *dstVertexAry)
{
    float nVertex_b1[3], nVertex_b2[3], nVertex_b3[3], nVertex_b4[3];
    struct SkeletonChainContext *sBone_1 = SkeketonChain +BDEF4_ptr->BoneIndex_1;
    struct SkeletonChainContext *sBone_2 = SkeketonChain +BDEF4_ptr->BoneIndex_2;
    struct SkeletonChainContext *sBone_3 = SkeketonChain +BDEF4_ptr->BoneIndex_3 ;
    struct SkeletonChainContext *sBone_4 = SkeketonChain +BDEF4_ptr->BoneIndex_4;

    MulTransformMatrix(sBone_1->TransformMatrix, rawVertexAry, BDEF4_ptr->Weight_1, nVertex_b1);
    MulTransformMatrix(sBone_2->TransformMatrix, rawVertexAry, BDEF4_ptr->Weight_2, nVertex_b2);
    MulTransformMatrix(sBone_3->TransformMatrix, rawVertexAry, BDEF4_ptr->Weight_3, nVertex_b3);
    MulTransformMatrix(sBone_4->TransformMatrix, rawVertexAry, BDEF4_ptr->Weight_4, nVertex_b4);

    *(dstVertexAry)    = nVertex_b1[0] + nVertex_b2[0] + nVertex_b3[0] + nVertex_b4[0];
    *(dstVertexAry +1) = nVertex_b1[1] + nVertex_b2[1] + nVertex_b3[1] + nVertex_b4[1];
    *(dstVertexAry +2) = nVertex_b1[2] + nVertex_b2[2] + nVertex_b3[2] + nVertex_b4[2];
}


inline void update_SurfaceVertex_BDEF2(float *rawVertexAry,
                                       struct DeformType_BDEF2 *BDEF2_ptr,
                                       struct SkeletonChainContext *SkeketonChain,
                                       float *dstVertexAry)
{
    float nVertex_b1[3], nVertex_b2[3];
    struct SkeletonChainContext *sBone_1 = SkeketonChain +BDEF2_ptr->BoneIndex_1;
    struct SkeletonChainContext *sBone_2 = SkeketonChain +BDEF2_ptr->BoneIndex_2;

    MulTransformMatrix(sBone_1->TransformMatrix, rawVertexAry, BDEF2_ptr->Weight_1, nVertex_b1);
    MulTransformMatrix(sBone_2->TransformMatrix, rawVertexAry, 1.0f - BDEF2_ptr->Weight_1, nVertex_b2);
    *(dstVertexAry)    = nVertex_b1[0] + nVertex_b2[0];
    *(dstVertexAry +1) = nVertex_b1[1] + nVertex_b2[1];
    *(dstVertexAry +2) = nVertex_b1[2] + nVertex_b2[2];
}

inline void update_SurfaceVertex_BDEF1(float *rawVertexAry,
                                       struct DeformType_BDEF1 *BDEF1_ptr,
                                       struct SkeletonChainContext *SkeketonChain,
                                       float *dstVertexAry)
{
    float nVertex[3];
    struct SkeletonChainContext *sBone = SkeketonChain +BDEF1_ptr->BoneIndex;

    MulTransformMatrix(sBone->TransformMatrix, rawVertexAry, 1.0f, nVertex);
    *(dstVertexAry)    = nVertex[0] ;
    *(dstVertexAry +1) = nVertex[1] ;
    *(dstVertexAry +2) = nVertex[2] ;
}

void VisualizationAvatar::
update_BodySurfaceVertex(struct SkeletonChainContext * fBone)
{
    int i;

    struct VertexUnitContext *VertexCtx = Spirit->VertexMemory.VertexUnit + i;
    for(i = 0 ; i < Spirit->VertexMemory.VertexUnitCount ; i++) {
        float *inVertex  = Spirit->VertexMemory.Vertex + VertexCtx->vID *3;
        float *dstVertex = BodyRederingCtx.Vertex + VertexCtx->vID *3;

        switch(VertexCtx->DeformType) {
        case 0: // 0:BDEF1
            update_SurfaceVertex_BDEF1(inVertex,
                                       Spirit->VertexMemory.BDEF1 + VertexCtx->DeformID,
                                       MasterSkeletonCtx.ChainNodeSet,
                                       dstVertex);
            break;
        case 1: // 1:BDEF2
            update_SurfaceVertex_BDEF2(inVertex,
                                       Spirit->VertexMemory.BDEF2 + VertexCtx->DeformID,
                                       MasterSkeletonCtx.ChainNodeSet,
                                       dstVertex);
            break;
        case 2: // 2:BDEF4
            update_SurfaceVertex_BDEF4(inVertex,
                                       Spirit->VertexMemory.BDEF4 + VertexCtx->DeformID,
                                       MasterSkeletonCtx.ChainNodeSet,
                                       dstVertex);
            break;
        case 3: // 3:SDEF
            update_SurfaceVertex_SDEF(inVertex,
                                       Spirit->VertexMemory.SDEF + VertexCtx->DeformID,
                                       MasterSkeletonCtx.ChainNodeSet,
                                       dstVertex);
            break;
        }
        VertexCtx++;    //switch to next vertex
    }
}

/** ---------------------------------------------------------------------------------
 * Set the ABSOLUTE location of avatar
 */
void VisualizationAvatar::location(float Tx, float Ty, float Tz)
{
    Location[0] = Tx;
    Location[1] = Ty;
    Location[2] = Tz;

    //In android , the origation of model must rotate 180 to fit the rotation vector working.
    //MasterSkeletonCtx.ChainRoot.QuaternionCtx.asRotation(0, 1, 0, 180);
    MasterSkeletonCtx.ChainRoot.QuaternionCtx.setTranslate(Tx, Ty, Tz);
}

/* ---------------------------------------------------------------------------------
 * Update the vertex of bone with Inverse Kinematics
 */
void VisualizationAvatar::
update_InverseKinematics()
{
    int iki;
    sutQuaternionOperator IKQuaternion;

    for(iki = 0 ; iki < Spirit->BoneMemory.IKUnitCount ; iki++) {
        struct IKUnitContext *IKInfo_ptr = Spirit->BoneMemory.IKUnit +iki;
        int ci, j;
        float *ReachVertex = (MasterSkeletonCtx.ChainNodeSet + IKInfo_ptr->ReachBoneID)->BoneVertex;
        float *TargetVertex = (MasterSkeletonCtx.ChainNodeSet + IKInfo_ptr->TargetBoneID)->BoneVertex;

        for(j = 0 ;j < IKInfo_ptr->MaxIterations ; j++) {
            for(ci = 0 ; ci < IKInfo_ptr->IKChainCount  ; ci++) {
                int RotationID = (IKInfo_ptr->IKChain + ci)->LinkBone ;
                float *RotationBaseJoint =  (MasterSkeletonCtx.ChainNodeSet + RotationID)->BoneVertex;
                //-----------------------------------------------------------
                float Vector_1[3] = {TargetVertex[0] - RotationBaseJoint[0],
                                     TargetVertex[1] - RotationBaseJoint[1],
                                     TargetVertex[2] - RotationBaseJoint[2]};
                float VectorLen_1 = sqrtf(Vector_1[0] * Vector_1[0] +
                                          Vector_1[1] * Vector_1[1] +
                                          Vector_1[2] * Vector_1[2]);
                float Vector_2[3] = {ReachVertex[0] - RotationBaseJoint[0],
                                     ReachVertex[1] - RotationBaseJoint[1],
                                     ReachVertex[2] - RotationBaseJoint[2]};
                float VectorLen_2 = sqrtf(Vector_2[0] * Vector_2[0] +
                                          Vector_2[1] * Vector_2[1] +
                                          Vector_2[2] * Vector_2[2]);
                float DotProduct = Vector_1[0] * Vector_2[0] +
                                   Vector_1[1] * Vector_2[1] +
                                   Vector_1[2] * Vector_2[2];
                float OuterProduct[3] = {Vector_1[1]* Vector_2[2] - Vector_1[2]* Vector_2[1],
                                         Vector_1[2]* Vector_2[0] - Vector_1[0]* Vector_2[2],
                                         Vector_1[0]* Vector_2[1] - Vector_1[1]* Vector_2[0]};
                DotProduct /= (VectorLen_1 * VectorLen_2);
                if(DotProduct > 1) DotProduct = 1;
                if(DotProduct < -1) DotProduct = -1;

                float RotationAngle = acos(DotProduct) ;
                if(RotationAngle > IKInfo_ptr->MaxAngle) RotationAngle = IKInfo_ptr->MaxAngle;
                //if(RotationAngle < -IKInfo_ptr->MaxAngle)  RotationAngle = -IKInfo_ptr->MaxAngle;
                RotationAngle *= 180.0f /3.1415926;

                IKQuaternion.init();
                IKQuaternion.asRotation(OuterProduct[0], OuterProduct[1], OuterProduct[2], RotationAngle);
                IKQuaternion.toMatrix();

                (MasterSkeletonCtx.ChainNodeSet + RotationID)->WorldQuaternion.mul(&IKQuaternion);

                float tmp[16], tmp_1[16];
                float BoneVertexMatrix[16];
                float invBoneVertexMatrix[16];
                memcpy(BoneVertexMatrix, IdentityMatrix, sizeof(float)*16);
                memcpy(invBoneVertexMatrix, IdentityMatrix, sizeof(float)*16);
                BoneVertexMatrix[3]     = RotationBaseJoint[0];
                BoneVertexMatrix[7]     = RotationBaseJoint[1];
                BoneVertexMatrix[11]    = RotationBaseJoint[2];
                invBoneVertexMatrix[3]  = -RotationBaseJoint[0];
                invBoneVertexMatrix[7]  = -RotationBaseJoint[1];
                invBoneVertexMatrix[11] = -RotationBaseJoint[2];

                // calculate the transform matrix of bone
                MatrixOperator_Mul_4x4M_4x4M(BoneVertexMatrix, IKQuaternion.getMatrix(), tmp);
                MatrixOperator_Mul_4x4M_4x4M(tmp, invBoneVertexMatrix, tmp_1);

                /*printf("----------------------------------------------------------\n");
                printf("%d\n", ci);
                printf("TranslateJoint %f %f %f\n", TranslateJoint[0], TranslateJoint[1], TranslateJoint[2]);
                printf("RotationBaseJoint %f %f %f\n", RotationBaseJoint[0], RotationBaseJoint[1], RotationBaseJoint[2]);
                printf("Vector_1 %f %f %f, VectorLen_1 %f\n", Vector_1[0], Vector_1[1], Vector_1[2], VectorLen_1);
                printf("Vector_2 %f %f %f, VectorLen_2 %f\n", Vector_2[0], Vector_2[1], Vector_2[2], VectorLen_2);
                printf("ReachVertex %f %f %f\n", ReachVertex[0], ReachVertex[1], ReachVertex[2]);
                printf("DotProduct %f\n", DotProduct);
                printf("OuterProduct %f %f %f , %f\n", OuterProduct[0], OuterProduct[1], OuterProduct[2], RotationAngle);
                for(int x = 0 ; x < 4 ; x++) {
                    for(int y = 0 ; y < 4 ; y++) {
                        printf("%lf , ", tmp_1[x*4+y]);
                    }
                    printf("\n");
                }*/
                superpose_SkeletonTransformMatrix(MasterSkeletonCtx.ChainNodeSet + RotationID, tmp_1);
            }
        }
    }
}
/* ---------------------------------------------------------------------------------
 * Create the Skeleton and Sureface data from PMD data context
 *  This function will create the full pose from PMD data.
 */
int VisualizationAvatar::
posture(PostureMemoryContext *PoseCtx, PostureModeSet pMode)
{
    int i, bi, ci;
    struct timeval pStart, pEnd;

    if(PoseCtx == NULL) return 0;

    gettimeofday(&pStart, NULL);
    if((Spirit != NULL) && (MaterializeElements & MaterializeElement_Skeleton)) {
        for(ci = 0 ; ci < MasterSkeletonCtx.ChainNodeCount ; ci++) {    // reset quaternion
            struct SkeletonChainContext *fBone = MasterSkeletonCtx.ChainNodeSet +ci;
            fBone->WorldQuaternion.init();
        }

        for(bi = 0 ; bi < PoseCtx->PoseUnitCount ; bi++) {
            struct PostureMemoryUnit *PoseUnit_ptr = PoseCtx->PoseUnit +bi ;
            for(ci = 0 ; ci < MasterSkeletonCtx.ChainNodeCount ; ci++) {
                struct SkeletonChainContext *fBone = MasterSkeletonCtx.ChainNodeSet +ci;
                if((PoseUnit_ptr->Key_ShiftJIS_djb2 == fBone->RawBoneInfo_ptr->nameKey_djb2) && \
                   (PoseUnit_ptr->Key_ShiftJIS_DKDR == fBone->RawBoneInfo_ptr->nameKey_DKDR)) {
                    this->set_BoneQuaternionParam(fBone->nID,
                                                  PoseUnit_ptr->Quaternion[0],
                                                  PoseUnit_ptr->Quaternion[1],
                                                  PoseUnit_ptr->Quaternion[2],
                                                  PoseUnit_ptr->Quaternion[3],
                                                  PoseUnit_ptr->Translate[0] * Spirit->GeometryCtx.ScaleRate,
                                                  PoseUnit_ptr->Translate[1] * Spirit->GeometryCtx.ScaleRate,
                                                  PoseUnit_ptr->Translate[2] * Spirit->GeometryCtx.ScaleRate,
                                                  pMode);
                    break;
                }
            }
        }
        // posture
        update_SkeletonTransformMatrix(NULL);
        update_SkeletonSurfaceVertex(&MasterSkeletonCtx.ChainRoot);
        update_InverseKinematics();
        if(MaterializeElements & MaterializeElement_Body) update_BodySurfaceVertex(&MasterSkeletonCtx.ChainRoot);
    }
    gettimeofday(&pEnd, NULL);
    float uTime = (pEnd.tv_sec -pStart.tv_sec)*1000000.0 +(pEnd.tv_usec -pStart.tv_usec);
    printf("Posture time : %lf ms\n", uTime /1000.0f);
    printf("--------------------------------------------------------------------\n");
}

/* ---------------------------------------------------------------------------------
 * Create the Skeleton and Sureface data from PMD data context
 *  This function will interpolate the pose between VPDCtx_op and VPDCtx_ed by interpolation step InterpolationT.
 *
 *  @param VPDCtx_op : Start pose.
 *  @param VPDCtx_ed : End pose.
 *  @param InterpolationT : The interpolation step, [0.0f ~ 1.0f].
 */
int VisualizationAvatar::
posture(PostureMemoryContext *PoseCtx_op,
        PostureMemoryContext *PoseCtx_ed,
        float InterpolationT)
{
    int i, bi;
    struct timeval pStart, pEnd;
    struct PostureMemoryUnit DummyUnit;
    DummyUnit.Quaternion[0] = 0;
    DummyUnit.Quaternion[1] = 0;
    DummyUnit.Quaternion[2] = 0;
    DummyUnit.Quaternion[3] = 1;
    DummyUnit.Translate[0] = 0;
    DummyUnit.Translate[1] = 0;
    DummyUnit.Translate[2] = 0;

    gettimeofday(&pStart, NULL);
    for(bi = 0 ; bi < MasterSkeletonCtx.ChainNodeCount ; bi++) {
        struct SkeletonChainContext *fBone = MasterSkeletonCtx.ChainNodeSet +bi;
        fBone->RawBoneInfo_ptr->BoneVertex;
        struct PostureMemoryUnit *PoseOP_ptr = PoseCtx_op->PoseUnit;
        struct PostureMemoryUnit *PoseED_ptr = PoseCtx_ed->PoseUnit;

        for(i = 0 ; i < PoseCtx_op->PoseUnitCount ; i++) {
            if((PoseOP_ptr->Key_ShiftJIS_djb2  == fBone->RawBoneInfo_ptr->nameKey_djb2) &&
               (PoseOP_ptr->Key_ShiftJIS_DKDR  == fBone->RawBoneInfo_ptr->nameKey_DKDR)) break;
            PoseOP_ptr++;
        }
        for(i = 0 ; i < PoseCtx_ed->PoseUnitCount ; i++) {
            if((PoseED_ptr->Key_ShiftJIS_djb2 == fBone->RawBoneInfo_ptr->nameKey_djb2) &&
               (PoseED_ptr->Key_ShiftJIS_DKDR == fBone->RawBoneInfo_ptr->nameKey_DKDR)) break;
            PoseED_ptr++;
        }
        if(PoseOP_ptr >= PoseCtx_op->PoseUnit +PoseCtx_op->PoseUnitCount) PoseOP_ptr =&DummyUnit;
        if(PoseED_ptr >= PoseCtx_ed->PoseUnit +PoseCtx_ed->PoseUnitCount) PoseED_ptr =&DummyUnit;

        float DotProduct = PoseOP_ptr->Quaternion[0] * PoseED_ptr->Quaternion[0] +
                           PoseOP_ptr->Quaternion[1] * PoseED_ptr->Quaternion[1] +
                           PoseOP_ptr->Quaternion[2] * PoseED_ptr->Quaternion[2] +
                           PoseOP_ptr->Quaternion[3] * PoseED_ptr->Quaternion[3] ;
        if(DotProduct > 1) DotProduct = 1;
        if(DotProduct <-1) DotProduct =-1;
        float Angle = acos(DotProduct);
        float p1 = (Angle ==0) ? 0 : sin((1-InterpolationT) *Angle) / sin(Angle);
        float p2 = (Angle ==0) ? 1 : sin(InterpolationT *Angle) / sin(Angle);

        this->set_BoneQuaternionParam(
                fBone->nID,
                p1 *PoseOP_ptr->Quaternion[0] + p2 *PoseED_ptr->Quaternion[0],
                p1 *PoseOP_ptr->Quaternion[1] + p2 *PoseED_ptr->Quaternion[1],
                p1 *PoseOP_ptr->Quaternion[2] + p2 *PoseED_ptr->Quaternion[2],
                p1 *PoseOP_ptr->Quaternion[3] + p2 *PoseED_ptr->Quaternion[3],
                ((1-InterpolationT) *PoseOP_ptr->Translate[0] + InterpolationT *PoseED_ptr->Translate[0]) * Spirit->GeometryCtx.ScaleRate,
                ((1-InterpolationT) *PoseOP_ptr->Translate[1] + InterpolationT *PoseED_ptr->Translate[1]) * Spirit->GeometryCtx.ScaleRate,
                ((1-InterpolationT) *PoseOP_ptr->Translate[2] + InterpolationT *PoseED_ptr->Translate[2]) * Spirit->GeometryCtx.ScaleRate,
                PostureMode_Replace);
    }
    // posture
    update_SkeletonTransformMatrix(NULL);
    update_SkeletonSurfaceVertex(&MasterSkeletonCtx.ChainRoot);
    update_InverseKinematics();
    if(MaterializeElements & MaterializeElement_Body) update_BodySurfaceVertex(&MasterSkeletonCtx.ChainRoot);

    gettimeofday(&pEnd, NULL);
    float uTime = (pEnd.tv_sec -pStart.tv_sec)*1000000.0 +(pEnd.tv_usec -pStart.tv_usec);
   // printf("Posture time : %lf ms\n", uTime /1000.0f);
    //printf("--------------------------------------------------------------------\n");


#ifdef ASD
    int i, bi;
    PostureMemoryContext *PoseCtx;
    struct PostureMemoryUnit DummyUnit;
    DummyUnit.Quaternion[0] = 0;
    DummyUnit.Quaternion[1] = 0;
    DummyUnit.Quaternion[2] = 0;
    DummyUnit.Quaternion[3] = 1;
    DummyUnit.Translate[0] = 0;
    DummyUnit.Translate[1] = 0;
    DummyUnit.Translate[2] = 0;

    for(bi = 0 ; bi < MasterSkeletonCtx.ChainNodeCount ; bi++) {
        struct SkeletonChainContext *fBone = MasterSkeletonCtx.ChainNodeSet +bi;
        struct PostureMemoryUnit *PoseOP_ptr = PoseCtx_op->PoseUnit;
        struct PostureMemoryUnit *PoseED_ptr = PoseCtx_ed->PoseUnit;

        for(i = 0 ; i < PoseCtx_op->PoseUnitCount ; i++) {
            if((PoseOP_ptr->Key_ShiftJIS_djb2  == fBone->RawBoneInfo_ptr->nameKey_djb2 ) &&
               (PoseOP_ptr->Key_ShiftJIS_DKDR  == fBone->RawBoneInfo_ptr->nameKey_DKDR )) break;
            PoseOP_ptr++;
        }
        for(i = 0 ; i < PoseCtx_ed->PoseUnitCount ; i++) {
            if((PoseED_ptr->Key_ShiftJIS_djb2 == fBone->RawBoneInfo_ptr->nameKey_djb2) &&
               (PoseED_ptr->Key_ShiftJIS_DKDR == fBone->RawBoneInfo_ptr->nameKey_DKDR)) break;
            PoseED_ptr++;
        }

        if(PoseOP_ptr >= PoseCtx_op->PoseUnit +PoseCtx_op->PoseUnitCount ) PoseOP_ptr =&DummyUnit;
        if(PoseED_ptr >= PoseCtx_ed->PoseUnit +PoseCtx_ed->PoseUnitCount ) PoseED_ptr =&DummyUnit;

        float DotProduct = PoseOP_ptr->Quaternion[0] * PoseED_ptr->Quaternion[0] +
                           PoseOP_ptr->Quaternion[1] * PoseED_ptr->Quaternion[1] +
                           PoseOP_ptr->Quaternion[2] * PoseED_ptr->Quaternion[2] +
                           PoseOP_ptr->Quaternion[3] * PoseED_ptr->Quaternion[3] ;
        if(DotProduct > 1) DotProduct = 1;
        if(DotProduct <-1) DotProduct =-1;
        float Angle = acos(DotProduct);
        //printf("<%f>\n", Angle);

        float p1 = (Angle ==0) ? 0 : sin((1-InterpolationT) *Angle) / sin(Angle);
        float p2 = (Angle ==0) ? 1 : sin(InterpolationT *Angle) / sin(Angle);
       // printf("P1 %f , P2 %f\n", p1, p2);

        this->set_BoneQuaternionParam(
                fBone->nID,
                p1 *PoseOP_ptr->Quaternion[0] + p2 *PoseED_ptr->Quaternion[0],
                p1 *PoseOP_ptr->Quaternion[1] + p2 *PoseED_ptr->Quaternion[1],
                p1 *PoseOP_ptr->Quaternion[2] + p2 *PoseED_ptr->Quaternion[2],
                p1 *PoseOP_ptr->Quaternion[3] + p2 *PoseED_ptr->Quaternion[3],
                (p1 *PoseOP_ptr->Translate[0] + p2 *PoseED_ptr->Translate[0]) * Spirit->GeometryCtx.ScaleRate,
                (p1 *PoseOP_ptr->Translate[1] + p2 *PoseED_ptr->Translate[1]) * Spirit->GeometryCtx.ScaleRate,
                (p1 *PoseOP_ptr->Translate[2] + p2 *PoseED_ptr->Translate[2]) * Spirit->GeometryCtx.ScaleRate,
                PostureMode_Replace, 1.0f);
    }
    // posture
    update_SkeletonTransformMatrix(NULL);
    update_SkeletonSurfaceVertex(&MasterSkeletonCtx.ChainRoot);
    update_InverseKinematics();
    if(MaterializeElements & MaterializeElement_Body) update_BodySurfaceVertex(&MasterSkeletonCtx.ChainRoot);
#endif
}

/* ---------------------------------------------------------------------------------
 * Create the Skeleton and Sureface data from PMD data context
 *  This function will interpolate the pose between VPDCtx_op and VPDCtx_ed by interpolation step InterpolationT.
 *
 *  @param VPDCtx_op : Start pose.
 *  @param VPDCtx_ed : End pose.
 *  @param InterpolationT : The interpolation step, [0.0f ~ 1.0f].
 */
int VisualizationAvatar::
posture(float TimeStamp)
{
    int i, bi;
    struct timeval pStart, pEnd;
    struct VocaloidMotionDataContext *MotionCtx = this->CurrentMotionCtx_ptr;

    if((TimeStamp -this->TimeStamp_Last) < 0.0f) return 0; // It's not the time.

    gettimeofday(&pStart, NULL);
    if(this->CurrentMotionCtx_ptr != NULL) {   // no moation assign, but the location may changed;
        for(bi = 0 ; bi < MasterSkeletonCtx.ChainNodeCount ; bi++) {
            struct SkeletonChainContext *fBone = MasterSkeletonCtx.ChainNodeSet +bi;
            struct KeyFrameUnit *FrameOP_ptr = NULL;
            struct KeyFrameUnit *FrameED_ptr = NULL;
            float TimeStamp_OP = 0.0f;
            float TimeStamp_ED = 0.0f;
            float InterpolationT = 0.0f;

            //search the fit BoneMotion of each bone.
            struct BoneMotionUnit *BoneMotion_ptr = NULL;
            if(fBone->Motion_ptr == NULL) {
                for(i = 0 ; i < MotionCtx->BoneMotionCount ; i++) {
                    struct BoneMotionUnit *sBoneMotion_ptr = MotionCtx->BoneMotion +i;
                    if((sBoneMotion_ptr->nameKey_djb2 == fBone->RawBoneInfo_ptr->nameKey_djb2) &&
                            (sBoneMotion_ptr->nameKey_DKDR == fBone->RawBoneInfo_ptr->nameKey_DKDR)) {
                        BoneMotion_ptr = sBoneMotion_ptr;
                        break;
                    }
                }
                fBone->Motion_ptr = BoneMotion_ptr;
            }
            else {
                /* Register the BoneMotion to avoid next search
                 * The register will be reseted when the motion switch to next motion set. */
                BoneMotion_ptr = (struct BoneMotionUnit *)fBone->Motion_ptr;
            }
            if(BoneMotion_ptr == NULL) continue;    //the bone without any motion


            /* Located the Frame in the motion .
             *  The FrameOP_ptr is used to point the last frame in time stamp ${FrameID}.
             *  The FrameED_ptr is used to point the next frame in time stamp ${FrameID}.
             *
             * The FrameOP_ptr and FrameED_ptr will be memorize in ${Bone->Frame1_ptr} and ${Bone->Frame2_ptr};
             */

            /* It's not the time to present motion of ${CurrentMotionCtx_ptr}.
             *  But check the last time stamp to smooth motion
             */
            if(TimeStamp < this->TimeStamp_CurrentPresent) {
                //printf("It's not time, %X , %X\n",fBone->Frame1_ptr, fBone->Frame2_ptr);
                if((fBone->Frame1_ptr != NULL) && (fBone->Frame2_ptr == NULL)) {
                    FrameOP_ptr = (struct KeyFrameUnit *)fBone->Frame1_ptr;
                    FrameED_ptr = (BoneMotion_ptr->KeyFrame +0);
                    TimeStamp_OP = this->TimeStamp_Last;
                    TimeStamp_ED = this->TimeStamp_CurrentPresent;
                    //printf("FrameOP_ptr %X, FrameED_ptr %X, TimeStamp_OP %f, TimeStamp_ED %f\n",FrameOP_ptr,FrameED_ptr, TimeStamp_OP, TimeStamp_ED);
                }
            }
            else {
                // translate the world time stamp to local time stamp.
                float LocalTimeStamp = TimeStamp - this->TimeStamp_CurrentPresent;

                // motion is specified, and the ${LocalTimeStamp} is in the last region, just keep the old key frame.
                if((fBone->Frame1_ptr !=NULL) && (fBone->Frame2_ptr !=NULL) &&  // situation 2
                        (LocalTimeStamp <= ((struct KeyFrameUnit *)fBone->Frame2_ptr)->FrameID)) {
                    FrameOP_ptr = (struct KeyFrameUnit *)fBone->Frame1_ptr;
                    FrameED_ptr = (struct KeyFrameUnit *)fBone->Frame2_ptr;
                    TimeStamp_OP = this->TimeStamp_CurrentPresent + FrameOP_ptr->FrameID ;
                    TimeStamp_ED = this->TimeStamp_CurrentPresent + FrameED_ptr->FrameID;
                }
                else {  // Time stamp is newer, search the next region in the motion.
                    for(i = fBone->FrameInMotion_Index ; i < BoneMotion_ptr->KeyFrameCount ; i++) {
                        struct KeyFrameUnit *Frame_ptr =(BoneMotion_ptr->KeyFrame +i);
                        if(Frame_ptr->FrameID <= LocalTimeStamp) FrameOP_ptr = Frame_ptr;
                        if(Frame_ptr->FrameID >= LocalTimeStamp) {  //any bone have end frame will be displied
                            FrameED_ptr = Frame_ptr;
                            fBone->FrameInMotion_Index = i;
                            break;
                        }
                    }
                    if(FrameED_ptr == NULL) FrameED_ptr = FrameOP_ptr;
                    if(FrameOP_ptr == NULL) FrameOP_ptr = FrameED_ptr;
                    TimeStamp_OP = this->TimeStamp_CurrentPresent + FrameOP_ptr->FrameID ;
                    TimeStamp_ED = this->TimeStamp_CurrentPresent + FrameED_ptr->FrameID;
                }
                fBone->Frame1_ptr = FrameOP_ptr;
                fBone->Frame2_ptr = FrameED_ptr;
            }

            if((FrameOP_ptr == NULL) && (FrameED_ptr == NULL)) continue;
            if(FrameOP_ptr == FrameED_ptr)
                InterpolationT = 0.0f;
            else
                InterpolationT = (float)(TimeStamp -TimeStamp_OP) / (float)abs(TimeStamp_ED - TimeStamp_OP);

            float DotProduct = FrameOP_ptr->Quaternion[0] * FrameED_ptr->Quaternion[0] +
                    FrameOP_ptr->Quaternion[1] * FrameED_ptr->Quaternion[1] +
                    FrameOP_ptr->Quaternion[2] * FrameED_ptr->Quaternion[2] +
                    FrameOP_ptr->Quaternion[3] * FrameED_ptr->Quaternion[3] ;
            if(DotProduct > 1) DotProduct = 1;
            if(DotProduct <-1) DotProduct =-1;
            float Angle = acos(DotProduct);
            float p1 = (Angle ==0) ? 0 : sin((1-InterpolationT) *Angle) / sin(Angle);
            float p2 = (Angle ==0) ? 1 : sin(InterpolationT *Angle) / sin(Angle);

            this->set_BoneQuaternionParam(
                        fBone->nID,
                        p1 *FrameOP_ptr->Quaternion[0] + p2 *FrameED_ptr->Quaternion[0],
                    p1 *FrameOP_ptr->Quaternion[1] + p2 *FrameED_ptr->Quaternion[1],
                    p1 *FrameOP_ptr->Quaternion[2] + p2 *FrameED_ptr->Quaternion[2],
                    p1 *FrameOP_ptr->Quaternion[3] + p2 *FrameED_ptr->Quaternion[3],
                    ((1-InterpolationT) *FrameOP_ptr->Translate[0] + InterpolationT *FrameED_ptr->Translate[0]) * Spirit->GeometryCtx.ScaleRate,
                    ((1-InterpolationT) *FrameOP_ptr->Translate[1] + InterpolationT *FrameED_ptr->Translate[1]) * Spirit->GeometryCtx.ScaleRate,
                    ((1-InterpolationT) *FrameOP_ptr->Translate[2] + InterpolationT *FrameED_ptr->Translate[2]) * Spirit->GeometryCtx.ScaleRate,
                    PostureMode_Replace);
        }
    }
    // posture
    update_SkeletonTransformMatrix(NULL);
    update_SkeletonSurfaceVertex(&MasterSkeletonCtx.ChainRoot);
    update_InverseKinematics();
    if(MaterializeElements & MaterializeElement_Body) update_BodySurfaceVertex(&MasterSkeletonCtx.ChainRoot);

    gettimeofday(&pEnd, NULL);
    static float AvgCounter =0;
    static float AvgTime =0;
    float uTime = (pEnd.tv_sec -pStart.tv_sec)*1000000.0 +(pEnd.tv_usec -pStart.tv_usec);
    uTime /=1000.0f;
    AvgTime += uTime;
    AvgCounter++;
    printf("Posture time : %.4lf ms , Average : %.4lfms \n", uTime, AvgTime /AvgCounter );
    //printf("--------------------------------------------------------------------\n");
}
/* ---------------------------------------------------------------------------------
 * Assign the motion data into avatar
 */
int VisualizationAvatar::
motion(VocaloidMotionDataContext *inMotionCtx, float presentTimeStamp)
{
    int bi;
    int LastTimeStamp = (CurrentMotionCtx_ptr != NULL) ? CurrentMotionCtx_ptr->MotionStamp : 0 ;  // the timestamp in last motion

    this->CurrentMotionCtx_ptr = inMotionCtx;
    this->TimeStamp_Last = this->TimeStamp_CurrentPresent +LastTimeStamp;   //switch the time stamp
    this->TimeStamp_CurrentPresent = presentTimeStamp;

    //reset the bone motion register pointer
    for(bi = 0 ; bi < MasterSkeletonCtx.ChainNodeCount ; bi++) {
        struct SkeletonChainContext *fBone = MasterSkeletonCtx.ChainNodeSet +bi;
        fBone->Motion_ptr = NULL;
        fBone->Frame1_ptr = fBone->Frame2_ptr;  //keep the last motion for smooth the poseture between motions.
        fBone->Frame2_ptr = NULL;
        fBone->FrameInMotion_Index =0;
    }
}

/* ---------------------------------------------------------------------------------
 * Create the Skeleton and Sureface data from PMD data context
 */
int VisualizationAvatar::
materialize(VisualizationMemoryContext *vMemoryCtx, unsigned int mElements)
{
    struct timeval pStart, pEnd;

    printf("Model Materialize\n");
    if(vMemoryCtx == NULL) return 0;    //param & elements check
    if(!((mElements & MaterializeElement_Body) && (vMemoryCtx->MaterializeElementBitFiled & MemoryElement_Body)))
        mElements &= ~MaterializeElement_Body;
    if(!((mElements & MaterializeElement_Skeleton) && (vMemoryCtx->MaterializeElementBitFiled & MemoryElement_Bone)))
        mElements &= ~MaterializeElement_Skeleton;

    Spirit = vMemoryCtx;
    MaterializeElements = mElements;

    gettimeofday(&pStart, NULL);
    if(MaterializeElements & MaterializeElement_Body)
        generate_BodySurface();

    if(MaterializeElements & MaterializeElement_Skeleton) {
        generate_SkeletonSurface();
        generate_SkeletonArchitecture();
    }
    AvatarStatus |= AvatarState_Materilize;
    gettimeofday(&pEnd, NULL);
    float uTime = (pEnd.tv_sec -pStart.tv_sec) *1000000.0f +(pEnd.tv_usec -pStart.tv_usec);
    printf("Materialize time : %lf ms\n", uTime /1000.0f);
    printf("--------------------------------------------------------------------\n");
}
// ---------------------------------------------------------------------------------
#ifdef USEING_QT
VisualizationAvatar::
VisualizationAvatar(QGLContext *GLContext)
{
    create();
    initializeGLFunctions(GLContext);
    printf("initializeGLFunctions\n");
}
#else
VisualizationAvatar::
VisualizationAvatar()
{
    create();
}
#endif
// ---------------------------------------------------------------------------------
void VisualizationAvatar::create()
{
    Location[0] = 0;
    Location[1] = 0;
    Location[2] = 0;
    MaterializeElements =MaterializeElement_Ghost;
    Spirit = NULL;
    CurrentMotionCtx_ptr = NULL;
    AvatarStatus = 0;
    TimeStamp_CurrentPresent = -1;
    TimeStamp_Last = -1;
    BodyRederingCtx.GroupCtxCount =0;
    uID = 0;
    memset(&BodyRederingCtx, 0, sizeof(GLSurfaceRenderingContext));
    memset(&SkeletonRenderingCtx, 0, sizeof(GLSkeletonRenderingContext));
}
// ---------------------------------------------------------------------------------
void VisualizationAvatar::release()
{

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
    this->CurrentMotionCtx_ptr = NULL;  //reset the time and data to initial the avatar state for the new universe
    this->TimeStamp_CurrentPresent = -1;
    this->TimeStamp_Last = -1;
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
GLSurfaceRenderingContext *VisualizationAvatar::get_VertexArrayCtx()
{
    if(MaterializeElements & MaterializeElement_Body) return &BodyRederingCtx;
    return NULL;
}
GLSkeletonRenderingContext *VisualizationAvatar::get_SkeletonRenderingCtx()
{
    if(MaterializeElements & MaterializeElement_Skeleton) return &SkeletonRenderingCtx;
    return NULL;
}
struct VisualizationMemoryContext *VisualizationAvatar::get_Spirit()
{
    return Spirit;
}

