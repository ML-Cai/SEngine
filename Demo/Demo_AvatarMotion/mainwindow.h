#ifndef MAINWINDOW_H
#define MAINWINDOW_H
/* -------------------------------------------------------------------- */
#include <QMainWindow>
#include <qgridlayout.h>
#include "QGLWidget_AvatarRendering.h"
/* -------------------------------------------------------------------- */
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();
    QGLWidget_AvatarRendering *get_GLView();

private:
    QWidget *centralWidget;
    QGridLayout *centralLayout;
    QGLWidget_AvatarRendering *GLView;
};
/* -------------------------------------------------------------------- */
#endif // MAINWINDOW_H

