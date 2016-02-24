#include "mainwindow.h"
/* --------------------------------------------------------------------------------- */
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    //Init UI
    this->resize(800,600);
    centralWidget = new QWidget(this);
    centralWidget->setObjectName(QStringLiteral("centralWidget"));
    centralLayout = new QGridLayout(centralWidget);
    centralLayout->setObjectName(QStringLiteral("centralLayout"));
    this->setCentralWidget(centralWidget);

    // Init visualization avatar GL widget
    QGLFormat GLFmt = QGLFormat(QGL::SampleBuffers);
    GLView = new QGLWidget_AvatarRendering(GLFmt, this->centralWidget);
    this->centralLayout->addWidget(GLView, 0, 0, 1, 1);
}
/* --------------------------------------------------------------------------------- */
MainWindow::~MainWindow()
{

}
/* --------------------------------------------------------------------------------- */
QGLWidget_AvatarRendering *MainWindow::get_GLView()
{
    return GLView;
}
