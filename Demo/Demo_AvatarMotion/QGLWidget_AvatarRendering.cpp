#include <math.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/time.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glext.h>
#include "sutAlgebraMath.h"
#include "QGLWidget_AvatarRendering.h"
#include "VMD_Profile.h"
// ----------------------------------------------------------------------------------
using namespace MMDCppDev;
using namespace VisualizationSystem;
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
// ----------------------------------------------------------------------------------
static const float PI = 3.14159f / 180.0f;
using namespace MMDCppDev;

sutQuaternionOperator QuaternionRotation;
// ----------------------------------------------------------------------------------
QGLWidget_AvatarRendering::QGLWidget_AvatarRendering(const QGLFormat& format, QWidget* parent, const QGLWidget* shareWidget, Qt::WindowFlags f)
    :QGLWidget(format, parent, shareWidget, f)
{
    src_Avatar = NULL;
    flush_timer = new QTimer;
    connect(flush_timer, SIGNAL(timeout()),  this , SLOT(Display()));
    flush_timer->start(30);
}
// ----------------------------------------------------------------------------------
QGLWidget_AvatarRendering::~QGLWidget_AvatarRendering()
{
}
// ----------------------------------------------------------------------------------
int QGLWidget_AvatarRendering::LoadShader_Default()
{
    int isSuccess;
    const char *VertexShader_Avatar = VertexShaderContext_Default;
    const char *FragmentShader_Avatar = FragmentShaderContext_Default;

    printf("Load Default Shader\n");
    VertexShader_Default = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(VertexShader_Default, 1, &VertexShader_Avatar, NULL);
    glCompileShader(VertexShader_Default);
    glGetShaderiv(VertexShader_Default, GL_COMPILE_STATUS, &isSuccess);
    if (isSuccess != GL_TRUE)
        printf("Fatal : Failed to compile vertex shader.\n");
    else
        printf("Info : Compile vertex shader success.\n");

    FragmentShader_Default = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(FragmentShader_Default, 1, &FragmentShader_Avatar, NULL);
    glCompileShader(FragmentShader_Default);
    glGetShaderiv(FragmentShader_Default, GL_COMPILE_STATUS, &isSuccess);
    if (isSuccess != GL_TRUE)
        printf("Fatal : Failed to compile Fragment shader.\n");
    else
        printf("Info : Compile Fragment shader success.\n");

    ShaderProgram_Default = glCreateProgram();
    glAttachShader(ShaderProgram_Default, VertexShader_Default);
    glAttachShader(ShaderProgram_Default, FragmentShader_Default);
    glLinkProgram(ShaderProgram_Default);
    printf("%d %d %d\n", VertexShader_Default, FragmentShader_Default , ShaderProgram_Default);
}
// ----------------------------------------------------------------------------------
void QGLWidget_AvatarRendering::initializeGL()
{
    this->makeCurrent();
    initializeGLFunctions(this->context());
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_NORMALIZE);
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_BLEND);
    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    //glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    view_eye.length = 1000;
    view_eye.angle_1 = 10;
    view_eye.angle_2 = -90;

    LoadShader_Default();
}
// ----------------------------------------------------------------------------------
void QGLWidget_AvatarRendering::resizeGL(int width, int height)
{
    ViewHeight = height;
    ViewWidth = width;
}
// ----------------------------------------------------------------------------------
void QGLWidget_AvatarRendering::bind_Avatar(VisualizationAvatar * src_Avatar)
{
    this->src_Avatar = src_Avatar;
}
// ----------------------------------------------------------------------------------
//http://people.freedesktop.org/~idr/glu3/glu3_8h.html#a8ea2939d5a59ad5b6ccb3259141f4858
void QGLWidget_AvatarRendering::get_PorjectMatrix(float FovAngle, float ViewHeight , float ViewWidth,
                                                GLdouble zNear, GLdouble zFar, float *dstMatrix)
{
    float f = 1.0f / tan(FovAngle* 0.5f * 3.1415929535f/180.0f);
    float a = ViewWidth/ ViewHeight;
    dstMatrix[0] = f/a;
    dstMatrix[1] = 0.0f;
    dstMatrix[2] = 0.0f;
    dstMatrix[3] = 0.0f;
    dstMatrix[4] = 0.0f;
    dstMatrix[5] = f;
    dstMatrix[6] = 0.0f;
    dstMatrix[7] = 0.0f;
    dstMatrix[8] = 0;
    dstMatrix[9] = 0;
    dstMatrix[10] = -(zFar +zNear) /(zFar -zNear);
    dstMatrix[11] = -1.0f;
    dstMatrix[12] = 0.0f;
    dstMatrix[13] = 0.0f;
    dstMatrix[14] = -(2.0f *zFar *zNear) /(zFar -zNear);
    dstMatrix[15] = 0.0f;
}
// ----------------------------------------------------------------------------------
/* Right Hand Law
 *
 * Default camera posite in (0, 0, -1)
 * http://people.freedesktop.org/~idr/glu3/glu3_8h.html#a8ea2939d5a59ad5b6ccb3259141f4858
*/
void QGLWidget_AvatarRendering::
get_ModelViewMatrix(float eyeX, float eyeY, float eyeZ,
                    float targetX, float targetY, float targetZ,
                    float upX, float upY, float upZ,
                    float *dstMatrix)
{
    float TranslateM[] ={1.0f, 0.0f, 0.0f, -eyeX,
                         0.0f, 1.0f, 0.0f, -eyeY,
                         0.0f, 0.0f, 1.0f, -eyeZ,
                         0.0f, 0.0f, 0.0f, 1.0f};
    float UpVector[] = {upX, upY, upZ};
    float EyeAxis_X[3];
    float EyeAxis_Y[3];
    float EyeAxis_Z[3] = {targetX -eyeX, targetY -eyeY, targetZ -eyeZ};  //the new z Vector of camera
    //float EyeAxis_Z[3] = {eyeX -targetX, eyeY -targetY, eyeZ -targetZ};
    sutNormalizeVector3(EyeAxis_Z);
    sutNormalizeVector3(UpVector);
    sutCrossProduct_3x3(EyeAxis_Z, UpVector, EyeAxis_X);
    sutNormalizeVector3(EyeAxis_X);
    sutCrossProduct_3x3(EyeAxis_X, EyeAxis_Z,EyeAxis_Y);

    /*printf("----------------------------------------------------\n");
    printf("EyeAxis_X %f %f %f\n", EyeAxis_X[0], EyeAxis_X[1], EyeAxis_X[2]);
    printf("EyeAxis_Y %f %f %f\n", EyeAxis_Y[0], EyeAxis_Y[1], EyeAxis_Y[2]);
    printf("EyeAxis_Z %f %f %f\n", EyeAxis_Z[0], EyeAxis_Z[1], EyeAxis_Z[2]);*/

    float EyeVector[3] = {EyeAxis_X[0] +EyeAxis_Y[0] +EyeAxis_Z[0],
                          EyeAxis_X[1] +EyeAxis_Y[1] +EyeAxis_Z[1],
                          EyeAxis_X[2] +EyeAxis_Y[2] +EyeAxis_Z[2] };

    sutNormalizeVector3(EyeVector);

    float LocalVector[] = {1, 1, 1};
    sutNormalizeVector3(LocalVector);

    float RotationAxis[3];
    sutCrossProduct_3x3(EyeVector, LocalVector, RotationAxis);

    float msutDotProduct_3x3 = sutDotProduct_3x3(EyeVector, LocalVector);
    float RotationAngle = acos(msutDotProduct_3x3) *180.0f /3.1415926 ;

    sutQuaternionOperator CameraPositionQuaternion;
    float QuaternionMatrix_ptr[16];
    float matrix[] = {EyeAxis_X[0], EyeAxis_X[1], EyeAxis_X[2],0,
                      EyeAxis_Y[0], EyeAxis_Y[1], EyeAxis_Y[2],0,
                      -EyeAxis_Z[0], -EyeAxis_Z[1], -EyeAxis_Z[2],0,
                      0, 0, 0, 1};
    sutMatrixOperator_Mul_4x4M_4x4M(matrix, TranslateM, QuaternionMatrix_ptr);

    dstMatrix[0] = QuaternionMatrix_ptr[0];
    dstMatrix[1] = QuaternionMatrix_ptr[4];
    dstMatrix[2] = QuaternionMatrix_ptr[8];
    dstMatrix[3] = QuaternionMatrix_ptr[12];
    dstMatrix[4] = QuaternionMatrix_ptr[1];
    dstMatrix[5] = QuaternionMatrix_ptr[5];
    dstMatrix[6] = QuaternionMatrix_ptr[9];
    dstMatrix[7] = QuaternionMatrix_ptr[13];
    dstMatrix[8] = QuaternionMatrix_ptr[2];
    dstMatrix[9] = QuaternionMatrix_ptr[6];
    dstMatrix[10] = QuaternionMatrix_ptr[10];
    dstMatrix[11] = QuaternionMatrix_ptr[14];
    dstMatrix[12] = QuaternionMatrix_ptr[3];
    dstMatrix[13] = QuaternionMatrix_ptr[7];
    dstMatrix[14] = QuaternionMatrix_ptr[11];
    dstMatrix[15] = QuaternionMatrix_ptr[15];
}
// ----------------------------------------------------------------------------------
void QGLWidget_AvatarRendering::Rendering_GridFloor(float *ProjectionMatrix, float *ModelViewMatrix, float FiledSize)
{
    const int GridLine =20;
    const float GridSize = FiledSize / (float)GridLine;
    float VertexSet[20 * 4 *3] ;
    float ColorSet[20 * 4 *4] ;
    unsigned short IndexSet[20 * 4] ;
    int pProjectionMatrix = glGetUniformLocation(ShaderProgram_Default, "inProjectionMatrix");
    int pModleViewMatrix = glGetUniformLocation(ShaderProgram_Default, "inModelViewMatrix");
    int pinVertex = glGetAttribLocation(ShaderProgram_Default, "inVertex");
    int pinColor  = glGetAttribLocation(ShaderProgram_Default, "inColor");

    for(int i = 0 ; i < GridLine *4 ; i ++) {
        int index = i *4;
        IndexSet[i] = i;
        ColorSet[index +0] = 0.6f;
        ColorSet[index +1] = 0.6f;
        ColorSet[index +2] = 0.6f;
        ColorSet[index +3] = 0.6f;
    }
    for(int Row = 0 ; Row < GridLine ; Row ++) {
        int index = Row *6;
        VertexSet[index +0] = (Row - GridLine/2) *GridSize *2;
        VertexSet[index +1] = 0.0f;
        VertexSet[index +2] = FiledSize;
        VertexSet[index +3] = (Row - GridLine/2) *GridSize *2;
        VertexSet[index +4] = 0.0f;
        VertexSet[index +5] = -FiledSize;
    }
    for(int Column = 0 ; Column < GridLine ; Column ++) {
        int index = Column *6 + GridLine *6;
        VertexSet[index +0] = FiledSize;
        VertexSet[index +1] = 0.0f;
        VertexSet[index +2] = (Column - GridLine/2) *GridSize *2;
        VertexSet[index +3] = -FiledSize;
        VertexSet[index +4] = 0.0f;
        VertexSet[index +5] = (Column - GridLine/2) *GridSize *2;
    }
    /* Rendering Axis */
    glUseProgram(ShaderProgram_Default);
    glEnableVertexAttribArray(pinVertex);
    glEnableVertexAttribArray(pinColor);

    glUniformMatrix4fv(pProjectionMatrix, 1, false, ProjectionMatrix);
    glUniformMatrix4fv(pModleViewMatrix, 1, false, ModelViewMatrix);

    glVertexAttribPointer(pinVertex, 3, GL_FLOAT, false, 0, VertexSet);
    glVertexAttribPointer(pinColor, 4, GL_FLOAT, false, 0, ColorSet);
    glDrawElements(GL_LINES, 80, GL_UNSIGNED_SHORT, IndexSet);

    glDisableVertexAttribArray(pinVertex);
    glDisableVertexAttribArray(pinColor);
}
// ----------------------------------------------------------------------------------
void QGLWidget_AvatarRendering::Rendering_Axis(float *ProjectionMatrix, float *ModelViewMatrix, float Size)
{
    float VertexSet[] = {-Size, 0.0f, 0.0f, Size, 0.0f, 0.0f,
                         0.0f, -Size, 0.0f, 0.0f, Size, 0.0f,
                         0.0f, 0.0f, -Size, 0.0f, 0.0f, Size};
    float ColorSet[] = {1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
                        0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f,
                        0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f};
    unsigned short IndexSet[] = {0, 1, 2, 3, 4, 5};
    int pProjectionMatrix = glGetUniformLocation(ShaderProgram_Default, "inProjectionMatrix");
    int pModleViewMatrix = glGetUniformLocation(ShaderProgram_Default, "inModelViewMatrix");
    int pinVertex = glGetAttribLocation(ShaderProgram_Default, "inVertex");
    int pinColor  = glGetAttribLocation(ShaderProgram_Default, "inColor");

    glUseProgram(ShaderProgram_Default);
    glEnableVertexAttribArray(pinVertex);
    glEnableVertexAttribArray(pinColor);
    glUniformMatrix4fv(pProjectionMatrix, 1, false, ProjectionMatrix);
    glUniformMatrix4fv(pModleViewMatrix, 1, false, ModelViewMatrix);
    glVertexAttribPointer(pinVertex, 3, GL_FLOAT, false, 0, VertexSet);
    glVertexAttribPointer(pinColor, 4, GL_FLOAT, false, 0, ColorSet);
    glDrawElements(GL_LINES, 6, GL_UNSIGNED_SHORT, IndexSet);
    glDisableVertexAttribArray(pinVertex);
    glDisableVertexAttribArray(pinColor);
}
// ----------------------------------------------------------------------------------
void QGLWidget_AvatarRendering::paintGL()
{
    float ProjectionMatrix[16];
    float ModelViewMatrix[16];
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glViewport(0, 0, ViewWidth, ViewHeight);

    get_PorjectMatrix(50, float (ViewHeight), float(ViewWidth), 1.0f, 6000.0f, ProjectionMatrix);

    float angle_1_cos =cos(PI* view_eye.angle_1);
    float eyeX = view_eye.length * angle_1_cos * cos(PI * view_eye.angle_2) ;
    float eyeY = view_eye.length * sin(PI * view_eye.angle_1);
    float eyeZ = view_eye.length * angle_1_cos * sin(PI * view_eye.angle_2);
    float upY = angle_1_cos;

    get_ModelViewMatrix(eyeX, eyeY, eyeZ,
                        0.0f, 0.0f, 0.0f,
                        0.0f, upY, 0.0f, ModelViewMatrix);
    Rendering_Axis(ProjectionMatrix, ModelViewMatrix, 2000);
    Rendering_GridFloor(ProjectionMatrix, ModelViewMatrix, 2000);

    if(src_Avatar != NULL) {
        src_Avatar->setCameraLocation(eyeX, eyeY, eyeZ);
        src_Avatar->visualize(ProjectionMatrix, ModelViewMatrix, VA_RENDERING_BODY);
        //src_Avatar->visualize(ProjectionMatrix, ModelViewMatrix, VA_RENDERING_BODY | VA_RENDERING_SKELETON);
        //src_Avatar->visualize(ProjectionMatrix, ModelViewMatrix, VA_RENDERING_SKELETON);
    }
}
// ----------------------------------------------------------------------------------
/* slots */
void QGLWidget_AvatarRendering::Display()
{
    paintGL();
    updateGL();
}
// ----------------------------------------------------------------------------------
void QGLWidget_AvatarRendering::mouseMoveEvent(QMouseEvent *event)
{
    if(event->buttons() & Qt::RightButton) {
        int cx =event->pos().x();
        int cy =event->pos().y();
        view_eye.angle_2 += (cx - view_eye.mouse_x) ;
        view_eye.angle_1 += (cy - view_eye.mouse_y)  ;
        view_eye.mouse_x = cx;
        view_eye.mouse_y = cy;
    }
}
// ----------------------------------------------------------------------------------
void QGLWidget_AvatarRendering::mousePressEvent(QMouseEvent *event)
{
    if(event->buttons() & Qt::RightButton) {
        int cx =event->pos().x();
        int cy =event->pos().y();
        view_eye.mouse_x = cx;
        view_eye.mouse_y = cy;
        //printf("press %d %d , ", event->pos().x(), event->pos().y());
    }
}
// ----------------------------------------------------------------------------------
void QGLWidget_AvatarRendering::wheelEvent(QWheelEvent *event)
{
    int delta = event->delta() ;
    if(delta >0) {
        view_eye.length += 50;
    }
    else {
        view_eye.length -= 50;
    }
}
