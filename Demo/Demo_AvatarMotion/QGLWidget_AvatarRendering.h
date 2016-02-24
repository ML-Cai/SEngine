#ifndef GLWIDGET_H
#define GLWIDGET_H
/* -------------------------------------------------------- */
#include <QGLWidget>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QGLFunctions>
#include <QTimer>
#include "PMD_Profile.h"
#include "va.h"
#include "sutQuaternion.h"

/* -------------------------------------------------------- */
struct GL_EyeAxis {
    float length;
    float angle_1;
    float angle_2;
    int mouse_x;
    int mouse_y;
};
/* -------------------------------------------------------- */
class QGLWidget_AvatarRendering : public QGLWidget , protected QGLFunctions
{
    Q_OBJECT
public:
    explicit QGLWidget_AvatarRendering(const QGLFormat& format, QWidget* parent=0, const QGLWidget* shareWidget = 0, Qt::WindowFlags f=0);
    ~QGLWidget_AvatarRendering();
    void bind_Avatar(VisualizationSystem::VisualizationAvatar * src_Avatar);

protected :
    void initializeGL();
    void paintGL();
    void resizeGL(int width, int height);
    void mouseMoveEvent(QMouseEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void wheelEvent(QWheelEvent *event);

private:
    //Shader parameter
    GLuint VertexShader_Default;
    GLuint FragmentShader_Default;
    GLuint ShaderProgram_Default;
    int LoadToonShader();
    int LoadShader_Default();
    int LoadToonTextureShader();
    int LoadTexture();

    //Rendering function
    void get_ModelViewMatrix(float eyeX, float eyeY, float eyeZ,
                             float centerX, float centerY, float centerZ,
                             float upX, float upY, float upZ,
                             float *dstMatrix);
    void get_PorjectMatrix(float FovAngle, float ViewHeight , float ViewWidth, GLdouble zNear, GLdouble zFar, float *dstMatrix);
    void Rendering_Axis(float *ProjectionMatrix, float *ModelViewMatrix, float FiledSize);
    void Rendering_GridFloor(float *ProjectionMatrix, float *ModelViewMatrix, float FiledSize);
    void Rendering_Skeleton();

    VisualizationSystem::VisualizationAvatar *src_Avatar;
    GL_EyeAxis view_eye;
    int ViewHeight;
    int ViewWidth;
    QTimer* flush_timer;

public slots:
    void Display();
};
/* -------------------------------------------------------- */

#endif // GLWIDGET_H
