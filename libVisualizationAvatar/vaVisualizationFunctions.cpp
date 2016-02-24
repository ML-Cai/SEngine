#include <stdio.h>
#include <stdlib.h>
#include "va.h"
// ---------------------------------------------------------------------------------
using namespace MMDCppDev;
using namespace VisualizationSystem;
/** ---------------------------------------------------------------------------------
 * Set the location of camera
 */
void VisualizationAvatar::vaVisualizationFunctions::
setCameraLocation(struct vaVisualizationFunctions::Context *visualizationCtx,
                  float cX, float cY, float cZ)
{
    visualizationCtx->cameraLocation[0] = cX;
    visualizationCtx->cameraLocation[1] = cY;
    visualizationCtx->cameraLocation[2] = cZ;
}

/** ---------------------------------------------------------------------------------
 * Visualization the avatar
 *  Note : In QT , the shader must inherentde form QGLFunctions. unmark the USEING_QT in VisualizationAvatar.h will enable that.
 *  Note : This function must called in the thread with openGL context.
 * */
void VisualizationAvatar::vaVisualizationFunctions::
visualize(struct vaMemoryContext *inMemory,
          struct vaSurfaceContext *inSurfaceCtx,
          struct vaSkeletonTreeContext *inSkeletonCtx,
          struct vaVisualizationFunctions::Context *inVisualizationCtx,
          float *inProjectionMatrix,
          float *inModelViewMatrix,
          unsigned int renderingCap)
{
    if((renderingCap ^ VA_RENDERING_NONE) & VA_RENDERING_BODY) {
        renderingBody(inMemory, inSurfaceCtx, inVisualizationCtx, inProjectionMatrix, inModelViewMatrix);
    }
    if((renderingCap ^ VA_RENDERING_NONE) & VA_RENDERING_SKELETON) {
        glDisable(GL_DEPTH_TEST);
        renderingSkeletion(inSkeletonCtx, inVisualizationCtx, inProjectionMatrix, inModelViewMatrix);
        glEnable(GL_DEPTH_TEST);
    }
}

void VisualizationAvatar::vaVisualizationFunctions::
renderingBody(struct vaMemoryContext *inMemory,
              struct vaSurfaceContext *inSurfaceCtx,
              struct vaVisualizationFunctions::Context *inVisualizationCtx,
              float *inProjectionMatrix, float *inModelViewMatrix)
{
    int i;
    const GLfloat LightPosition[] = {2000.0f, 2000.0f, -2000.0f};
    struct GLshaderProgram *renderingShader = &inVisualizationCtx->bodySurfaceShader;

    for(i = 0 ; i < inSurfaceCtx->GroupCtxCount ; i++) {
        struct vaSurfaceGroupContext *Group_ptr = inSurfaceCtx->GroupCtx +i ;
        struct MaterialUnitContext *pMaterial = inMemory->MaterialMemory.MaterialUnit +i;
        unsigned int textureID = inVisualizationCtx->emptyTexture;

        if(pMaterial->TextureIndex != 255) textureID = *(inVisualizationCtx->MaterialTexture + pMaterial->TextureIndex);

        // assign shader
        glUseProgram(renderingShader->Program);

        // transform matrix
        glUniformMatrix4fv(renderingShader->L_ProjectionMatrix, 1, false, inProjectionMatrix);
        glUniformMatrix4fv(renderingShader->L_ModelViewMatrix, 1, false, inModelViewMatrix);

        // vertex parts
        glEnableVertexAttribArray(renderingShader->L_TexCoord);
        glEnableVertexAttribArray(renderingShader->L_Vertex);
        glEnableVertexAttribArray(renderingShader->L_Normal);
        glVertexAttribPointer(renderingShader->L_TexCoord, 2, GL_FLOAT, false, 0, inSurfaceCtx->Texture);
        glVertexAttribPointer(renderingShader->L_Normal, 3, GL_FLOAT, false, 0, inSurfaceCtx->Normal);
        glVertexAttribPointer(renderingShader->L_Vertex, 3, GL_FLOAT, false, 0, inSurfaceCtx->Vertex);

        // lighting/material parts
        glUniform3fv(renderingShader->L_CameraLocation, 1, inVisualizationCtx->cameraLocation);
        glUniform3fv(renderingShader->L_inLightPosition, 1, LightPosition);
        glUniform4fv(renderingShader->L_AmbientColor, 1, Group_ptr->Ambient_RGBA);
        glUniform4fv(renderingShader->L_DiffuseColor, 1, Group_ptr->Diffuse_RGBA);

        // texture parts
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, textureID);
        glUniform1i(renderingShader->L_uTexture, 0);

        switch(inMemory->IndexMemory.IndexUnitTypeSize) {
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
        if(pMaterial->TextureIndex != 255) glBindTexture(GL_TEXTURE_2D, 0);
    }
}

void VisualizationAvatar::vaVisualizationFunctions::renderingSkeletion(struct vaSkeletonTreeContext *inSkeletonCtx,
              struct vaVisualizationFunctions::Context *inVisualizationCtx,
              float *inProjectionMatrix, float *inModelViewMatrix)
{
    struct GLshaderProgram *renderingShader = &inVisualizationCtx->skeletonShader;

    // assign shader
    glUseProgram(renderingShader->Program);

    // transform matrix
    glUniformMatrix4fv(renderingShader->L_ProjectionMatrix, 1, false, inProjectionMatrix);
    glUniformMatrix4fv(renderingShader->L_ModelViewMatrix, 1, false, inModelViewMatrix);

    // vertex parts
    glEnableVertexAttribArray(renderingShader->L_Vertex);
    glEnableVertexAttribArray(renderingShader->L_Color);
    glVertexAttribPointer(renderingShader->L_Vertex, SKELETON_RENDERING_VERTEX_ELEMENTS, GL_FLOAT, false, 0, inSkeletonCtx->renderingVertex);
    glVertexAttribPointer(renderingShader->L_Color, 4, GL_FLOAT, false, 0, inSkeletonCtx->renderingColor);
    glDrawElements(GL_LINES, inSkeletonCtx->renderingCount_Bone *4, GL_UNSIGNED_SHORT, inSkeletonCtx->renderingIndex_Bone);
    glDrawElements(GL_QUADS, inSkeletonCtx->renderingCount_Joint *4, GL_UNSIGNED_SHORT, inSkeletonCtx->renderingIndex_Joint );
}

// ---------------------------------------------------------------------------------
int VisualizationAvatar::vaVisualizationFunctions::
reincarnate(vaMemoryContext *vaMemory, Context *visualizationCtx)
{
    int ret = 1;
    int i, Tcount = vaMemory->TextureMemory.TextureCount ;
    TextureContext *Image_ptr;

    //prepare shader
    ret &= loadShaderFromFileSystem(&visualizationCtx->skeletonShader, (char *)"./GLSL_Shader/skeleton.vert", (char *)"./GLSL_Shader/skeleton.frag");
    ret &= loadShaderFromFileSystem(&visualizationCtx->bodySurfaceShader, (char *)"./GLSL_Shader/toon.vert", (char *)"./GLSL_Shader/toon.frag");
    if(ret ==0) exit(0);

    //load empty texture
    unsigned char emptyImage[] = {255, 255, 255, 255};
    glGenTextures(1, &visualizationCtx->emptyTexture);
    glBindTexture(GL_TEXTURE_2D, visualizationCtx->emptyTexture);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, emptyImage);
    glBindTexture(GL_TEXTURE_2D, 0);

    //load mterial texture/ toon texture
    visualizationCtx->MaterialTexture = (GLuint *)malloc(sizeof(GLuint) * Tcount);
    glGenTextures(Tcount, visualizationCtx->MaterialTexture);
    for(i = 0 ; i < Tcount ; i++) {
        Image_ptr = *(vaMemory->TextureMemory.TextureImg +i) ;
        glBindTexture(GL_TEXTURE_2D, *(visualizationCtx->MaterialTexture +i));
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, Image_ptr->Width, Image_ptr->Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, Image_ptr->RGBA8888);
        glBindTexture(GL_TEXTURE_2D, 0);
        printf("Create Texture[%d] in openGL ... result : %d\n", i, *(visualizationCtx->MaterialTexture +i));
    }

    // get the location value in the shader.
    struct GLshaderProgram *shader = &visualizationCtx->bodySurfaceShader;
    shader->L_ProjectionMatrix  = glGetUniformLocation(shader->Program, "inProjectionMatrix");
    shader->L_ModelViewMatrix   = glGetUniformLocation(shader->Program, "inModelViewMatrix");
    shader->L_AmbientColor      = glGetUniformLocation(shader->Program, "vAmbientColor");
    shader->L_DiffuseColor      = glGetUniformLocation(shader->Program, "vDiffuseColor");
    shader->L_Vertex            = glGetAttribLocation(shader->Program, "inVertex");
    shader->L_Normal            = glGetAttribLocation(shader->Program, "inNormal");
    shader->L_TexCoord          = glGetAttribLocation(shader->Program, "inTexCoord");
    shader->L_uTexture          = glGetUniformLocation(shader->Program, "uTexture_1");
    shader->L_inLightPosition   = glGetUniformLocation(shader->Program, "inLightPosition");
    shader->L_CameraLocation    = glGetUniformLocation(shader->Program, "inCameraLocation");
    if((shader->L_ProjectionMatrix == -1) || (shader->L_ModelViewMatrix == -1) || (shader->L_AmbientColor == -1) || (shader->L_DiffuseColor == -1) ||
       (shader->L_Vertex == -1) || (shader->L_Normal == -1) || (shader->L_TexCoord == -1) || (shader->L_uTexture == -1) ||
       (shader->L_inLightPosition == -1) || (shader->L_CameraLocation == -1))
        return 0;

    shader = &visualizationCtx->skeletonShader;
    shader->L_ProjectionMatrix  = glGetUniformLocation(shader->Program, "inProjectionMatrix");
    shader->L_ModelViewMatrix   = glGetUniformLocation(shader->Program, "inModelViewMatrix");
    shader->L_Vertex            = glGetAttribLocation(shader->Program, "inVertex");
    shader->L_Color             = glGetAttribLocation(shader->Program, "inColor");
    if((shader->L_ProjectionMatrix == -1) || (shader->L_ModelViewMatrix == -1) || (shader->L_Vertex == -1) ||(shader->L_Color == -1))
        return 0;
    return 1;
}
//---------------------------------------------------------------------------------
int VisualizationAvatar::vaVisualizationFunctions::
loadShaderFromFileSystem(struct GLshaderProgram *dstShader, char *vertexShaderPath, char *fragmentShaderPath)
{
    int i, fSize;
    char *fPathSet[2] = {vertexShaderPath, fragmentShaderPath};
    char *shaderContent[2];

    // read shader content from files.
    for(i = 0 ; i < 2 ; i++) {
        FILE *fShader = fopen(fPathSet[i], "r");
        if(fShader != NULL) {
            fseek(fShader, 0L, SEEK_END);
            fSize = ftell(fShader);
            shaderContent[i] = (char *)malloc(fSize +1);
            if(shaderContent[i] == NULL) goto LOAD_ERROR;
            fseek(fShader, 0L, SEEK_SET);
            fread(shaderContent[i], fSize, 1, fShader);
            shaderContent[i][fSize] ='\0';
            fclose(fShader);
        }
    }
    if(0 == genShaderProgram(dstShader, (const char *)shaderContent[0], (const char *)shaderContent[1]))
        goto LOAD_ERROR;

    free(shaderContent[0]);
    free(shaderContent[1]);
    return 1;

LOAD_ERROR:
    return 0;
}

//---------------------------------------------------------------------------------
int VisualizationAvatar::vaVisualizationFunctions::
loadShader(const char *srcContent, int Type)
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
                printf("Fatal :  Compilation error in shader  %s\n", LogStr);
                glDeleteShader(allocShader);
                free(LogStr);
                return 0;
            }
            printf("Fatal : No memory space to list compilation error message\n");
            return 0;
        }
        printf("Info : Compile vertex shader success.\n");
    }
    return allocShader;
}
//---------------------------------------------------------------------------------
int VisualizationAvatar::vaVisualizationFunctions::
genShaderProgram(GLshaderProgram *dstShader, const char *srcVertexShder, const char *srcFragmentShader)
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
#ifdef USEING_QT
VisualizationAvatar::vaVisualizationFunctions::
vaVisualizationFunctions(QGLContext *GLContext)
{
    initializeGLFunctions(GLContext);
    printf("initializeGLFunctions\n");
}
#else
VisualizationAvatar::vaVisualizationFunctions::
vaVisualizationFunctions()
{
}
#endif
