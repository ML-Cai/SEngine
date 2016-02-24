/** -------------------------------------------------------------------
 * This Demo will show the 3D model display with libVisualization.
 *
 *  The model source is from the MikuMikuDance_v926x64
 *
 *  Controll :
 *      Mouse sheel : Room in/room out the camera distance.
 *      Mouse right button + Mouse move : rotation the camera.
 *
*/
#include "mainwindow.h"
#include <QApplication>
#include <PMD_Profile.h>
#include <PMX_Profile.h>
#include <VMD_Profile.h>
#include <VisualizationMemory.h>
#include <VisualizationAvatar.h>
#include <ParallelUniverse.h>
#include <unistd.h>
#include <sys/time.h>
/* -------------------------------------------------------------------- */
#include <pthread.h>

pthread_t thread_elapse;
void *elapse(void *ptr)
{
    MMDCppDev::VisualizationAvatar *MikuAvatar = (MMDCppDev::VisualizationAvatar *)ptr;
    MMDCppDev::VMD_Profile VMD;
    MMDCppDev::VocaloidMotionDataContext *VMDCtx;
    VMDCtx = VMD.genContext(0);
    VMD.load(VMDCtx, "./VMD/dance_1/", "dance_1.vmd");

    //------------------------------------
    UniverseSpace::WorldLineContext WorldLine;
    struct timeval TimeStampStart, TimeStampEnd;

    float times =0;
    WorldLine.create();
    WorldLine.reincarnate(MikuAvatar);
    times =WorldLine.addMotion(MikuAvatar->getUniverseID(), times, times, VMDCtx);

    gettimeofday(&TimeStampStart, NULL);
    while(1) {
        usleep(10);
        gettimeofday(&TimeStampEnd, NULL);
        float StepTime = (TimeStampEnd.tv_sec -TimeStampStart.tv_sec)*1000000.0 +(TimeStampEnd.tv_usec -TimeStampStart.tv_usec);
        float ElapseTime =(StepTime /1000000.0f) *30.0f;
        WorldLine.elapse(ElapseTime);
    }
}

/* -------------------------------------------------------------------- */
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;

    w.showNormal();
    /** --------------------------------
     *  PMD file input
     * ---------------------------------
    **/
    /*MMDCppDev::PMD_profile PMD;
    MMDCppDev::PolygonModelDataContext *PMDCtx;
    MMDCppDev::VisualizationMemory VM;
    MMDCppDev::VisualizationMemoryContext *VMCtx_Miku;
    MMDCppDev::VisualizationAvatar MikuAvatar(w.get_GLView()->context()) ;

    PMDCtx = PMD.genContext(0, 1000, 1000, 1000);
    PMD.load(PMDCtx, "./PMD", "mikuVer2.pmd", NULL);
    VMCtx_Miku = VM.create(PMDCtx, MMDCppDev::MemoryElement_FULL);
    PMD.release(PMDCtx);

    MikuAvatar.create();
    MikuAvatar.materialize(VMCtx_Miku, MMDCppDev::MaterializeElement_FULL);
    w.get_GLView()->bind_Avatar(&MikuAvatar);*/

    /** --------------------------------
     *  PMX file input
     * ---------------------------------
    **/
    MMDCppDev::PMX_profile PMX;
    MMDCppDev::PolygonModeleXtendedContext *PMXCtx;
    MMDCppDev::VisualizationMemory VM;
    MMDCppDev::VisualizationMemoryContext *VMCtx_Miku;
    MMDCppDev::VisualizationAvatar MikuAvatar(w.get_GLView()->context()) ;

    PMXCtx = PMX.genContext(0, 1000, 1000, 1000);
    PMX.load(PMXCtx, "./PMX/miku.pmx", NULL);
    VMCtx_Miku = VM.create(PMXCtx, MMDCppDev::MemoryElement_FULL);
    PMX.release(PMXCtx);

    MikuAvatar.create();
    MikuAvatar.materialize(VMCtx_Miku, MMDCppDev::MaterializeElement_FULL);
    w.get_GLView()->bind_Avatar(&MikuAvatar);


    pthread_create(&thread_elapse, NULL, elapse, &MikuAvatar);

    return a.exec();
}
