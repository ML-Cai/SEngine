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
#include <vaMemory.h>
#include <va.h>
#include <ParallelUniverse.h>
#include <unistd.h>
#include <sys/time.h>
/* -------------------------------------------------------------------- */
#include <pthread.h>

pthread_t thread_elapse;
void *elapse(void *ptr)
{
    VisualizationSystem::VisualizationAvatar *MikuAvatar = (VisualizationSystem::VisualizationAvatar *)ptr;
    MMDCppDev::VMD_Profile VMD;
    MMDCppDev::VocaloidMotionDataContext *VMDCtx;
    VMDCtx = VMD.genContext(0);
    //VMD.load(VMDCtx, "./VMD/dance_1/", "dance_1.vmd");
    VMD.load(VMDCtx, "./VMD/", "electric_angel motion.vmd");
   // VMD.load(VMDCtx, "./VMD/walk_1/", "walk_front.vmd");

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
        //WorldLine.elapse(10);
    }
}

/* -------------------------------------------------------------------- */
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;

    w.showNormal();

    /** --------------------------------
     *  PMX file input
     * ---------------------------------
    **/
    MMDCppDev::PMX_profile PMX;
    MMDCppDev::PolygonModeleXtendedContext *PMXCtx;
    VisualizationSystem::vaMemory VM;
    VisualizationSystem::vaMemoryContext *VMCtx_Miku;
    VisualizationSystem::VisualizationAvatar MikuAvatar(w.get_GLView()->context()) ;

    PMXCtx = PMX.genContext(0, 1000, 1000, 1000);
    //PMX.load(PMXCtx, "./PMX/", "miku.pmx", NULL);
    //PMX.load(PMXCtx, "./PMX/jintsu/", "jintsu.pmx", NULL);
    //PMX.load(PMXCtx, "./PMX/sendai/", "sendai.pmx", NULL);
    PMX.load(PMXCtx, "./PMX/haruna/", "榛名改二(艤装なし).pmx", NULL);
    //PMX.load(PMXCtx, "./PMX/haruna/", "榛名改二.pmx", NULL);
    //PMX.load(PMXCtx, "./PMX/碧彩式 涼風/", "碧彩式_涼風_準.pmx", NULL);
    //PMX.load(PMXCtx, "./PMX/筑摩改二_ver1.25/", "筑摩改二_ver.1.25.pmx", NULL);
    VMCtx_Miku = VM.create(PMXCtx, VisualizationSystem::MemoryElement_FULL);
    PMX.release(PMXCtx);

    /** --------------------------------
     *  Create visualization avatar
     * ---------------------------------
    **/
    MikuAvatar.create();
    MikuAvatar.materialize(VMCtx_Miku, VisualizationSystem::MaterializeElement_FULL);
    w.get_GLView()->bind_Avatar(&MikuAvatar);

    /** --------------------------------
     *  create world line
     * ---------------------------------
    **/
    pthread_create(&thread_elapse, NULL, elapse, &MikuAvatar);

    return a.exec();
}
