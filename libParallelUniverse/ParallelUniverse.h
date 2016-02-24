#ifndef PARALLEL_UNIVERSE_H
#define PARALLEL_UNIVERSE_H
// --------------------------------------------------------------------
#include <pthread.h>
#include "VMD_Profile.h"
#include "va.h"
// --------------------------------------------------------------------
namespace UniverseSpace {

/** --------------------------------------------------------------------
 *  The SetupTime means the timestamp to setup the location/motion data into world line , different to the PresentTime ,
    the present time is used to present when world timestamp in.

    The prupose of SetupTime is for smooth the two different location/motion data with non-zero time offset.

    For example :
        One region TA in timestamp TA0-TA1 , and another region TB in timestamp TB0-TB1.
        In timestamp ts, due to the presentime TB0 of region TB is large than ts, the elapse() will not elapse it.
        In other world, the world line couldn't know the next location in TB0, and the location will not smooth between TA1 - TB0.
        So, the SetupTime is used to preset the location/motion data into the world, and the value is depend on your smooth reqirement.

    NOTE : Must keep the SetupTime <= PresentTime.

                                   ts
                                   |
                    TA             |                    TB
                _________          |            _________________
               |         |         |           |                 |
    -----------|---------|---------|-----------|-----------------|--------
         |    TA0       TA1   |               TB0               TB1
         |                    |
         |                    |
     TA SetupTime       TB SetupTime

*/
struct AvatarLocationStepContext {
    float SetupTime;
    float PresentTime;
    float StampSize;
    int AvatarID;
    float Translate_Start[3];
    float Translate_End[3];
    struct AvatarLocationStepContext *next;
};

enum AudioStepState {
    AudioState_Chaos        = 0x01,
    AudioState_Freezing     = 0x02,
    AudioState_Playing      = 0x04,
    AudioState_Stop         = 0x08,
};

struct AvatarAudioStepContext {
    float SetupTime;
    float PresentTime;
    float StampSize;
    unsigned int State ;
    unsigned char AudioID;
    struct AvatarAudioStepContext *next;
};


/** The motion data of world line
*/

struct wlBoneMotionMapUnit {
    void *targetPostureMemory;
    unsigned int keyFrameID;
    float keyFrameTimeStampLimit;
    float globalTimeStamp;
    float translate[3];
    float quaternion[4]; //store as : x y z w

    float timeStampED;
    float translateED[3];
    float quaternionED[4]; //store as : x y z w
};

struct AvatarMotionStepContext {
    float SetupTime;
    float PresentTime;
    float StampSize;
    int AvatarID;
    struct MMDCppDev::VocaloidMotionDataContext *Motion_ptr;
    struct wlBoneMotionMapUnit *boneMotionMap;
    int isMapping;
    struct AvatarMotionStepContext *next;
};

class WorldLineContext {
public :
    void create();
    void destroy();
    void reset();
    void dumpInfo();
    void elapse(float TimeStamp);
    int reincarnate(VisualizationSystem::VisualizationAvatar *theAvatar);
    int addMotion(int AvatarID, float setupTime, float presetTime, MMDCppDev::VocaloidMotionDataContext *srcMotion);
    int addLocation(int AvatarID, float setupTime, float presetTime, float presentRegion, float StartX, float StartY, float StartZ, float EndX, float EndY, float EndZ);
    int addAudio(int AudioID, float setupTime, float presentTime, float presentRegion);

    int getAudioPlayStatus(int AudioID);
    int isEmpty();

private:
    static const int MAX_Avatar = 8;
    int ID;
    float WorldTimeStamp;
    void elapseMotion(float TimeStamp);
    void elapseLocation(float TimeStamp);
    void elapseAudio(float TimeStamp);
    // each world line could handle the 16 avatar.
    int AvatarCount;
    VisualizationSystem::VisualizationAvatar *Avatar[8];
    //VisualizationSystem::vaPostureMemoryContext avatarPostureCtx[8];

    // motion
    struct AvatarMotionStepContext *currentMotionStep;
    struct AvatarMotionStepContext *MotionChain_Head;   //the MotionCtx will linked as list and sorted by time.
    struct AvatarMotionStepContext *MotionChain_Tail;   //The Tail pointer in motio chain
    struct AvatarMotionStepContext *RecycleMotionPool;

    // location
    struct AvatarLocationStepContext *LocationChain_Head;   //the MotionCtx will linked as list and sorted by time.
    struct AvatarLocationStepContext *LocationChain_Tail;   //The Tail pointer in motio chain
    struct AvatarLocationStepContext *RecycleLocationPool;

    // music
    struct AvatarAudioStepContext *AudioChain_Head;   //the MotionCtx will linked as list and sorted by time.
    struct AvatarAudioStepContext *AudioChain_Tail;   //The Tail pointer in motio chain
    struct AvatarAudioStepContext *RecycleAudioPool;

    // text

};

// --------------------------------------------------------------------
class ParallelUniverse {
public:
    ParallelUniverse();
    void create();

private :
    unsigned int UniverserTimeStamp;
    int WorldLineCount;
    struct WorldLineContext *WorldLine;

};

}
// --------------------------------------------------------------------
#endif // PARALLELUNIVERSE_H
