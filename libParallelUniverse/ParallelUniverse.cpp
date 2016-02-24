#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <assert.h>
#include <sys/time.h>
#include "ParallelUniverse.h"
#include "va.h"
#include "vaPostureMemory.h"
#include "VMD_Profile.h"
// --------------------------------------------------------------------
using namespace MMDCppDev;
using namespace UniverseSpace;
using namespace VisualizationSystem;
static struct KeyFrameUnit IdentityFrame = {
    0,
    0.0f, 0.0f, 0.0f,
    0.0f, 0.0f, 0.0f, 1.0f,
};
// --------------------------------------------------------------------
static inline float interpolationFunction(float timeStamp_current, float timeStamp_OP, float timeStamp_ED)
{
    if(timeStamp_ED == timeStamp_OP) return 0;
    return (float)(timeStamp_current -timeStamp_OP) / (float)abs(timeStamp_ED - timeStamp_OP);
}

// --------------------------------------------------------------------
/*
 *  |-------View of Visualization avtar--------|
 *  |                                          |
 *
 *   VA bone data <----  vaPostureMemoryContext <---- wlBoneMotionMapUnit <---- VocaloidMotionDataContext
 *                             (targetBone)         (targetPostureMemory)          (BoneMotionUnit)
 *
 *                       |                                                                                |
 *                       |--------------------View of parallel universe system----------------------------|
*/
static void generateBoneMotionMap(struct AvatarMotionStepContext *srcMotionCtx,
                                  struct VocaloidMotionDataContext *srcMotion)
{
    printf("generateBoneMotionMap\n");
    srcMotionCtx->boneMotionMap = (wlBoneMotionMapUnit *)malloc(sizeof(wlBoneMotionMapUnit) * srcMotion->effectiveBoneCount);
    memset(srcMotionCtx->boneMotionMap ,0 , sizeof(wlBoneMotionMapUnit) * srcMotion->effectiveBoneCount);
}

static void mapping_BoneMotionMapToPostureMemoryUnit(struct AvatarMotionStepContext *srcCtx,
                                                     struct VocaloidMotionDataContext *srcMotion,
                                                     struct vaPostureMemoryContext *dstMem)
{
    int i, bi;
    for(bi = 0 ; bi < srcCtx->Motion_ptr->effectiveBoneCount ; bi++) {
        //search the fit BoneMotion of each bone.
        struct wlBoneMotionMapUnit *boneMotionMap = srcCtx->boneMotionMap + bi;
        struct BoneMotionUnit *boneUnit = srcMotion->effectiveBoneSet + bi;
        if(boneMotionMap->targetPostureMemory == NULL) {
            for(i = 0 ; i < dstMem->poseUnitCount ; i++) {
                struct vaPostureMemoryUnit *pPoseUnit = dstMem->poseUnit +i;
                if((pPoseUnit->nameKey_UTF8_djb2 == boneUnit->nameKey_UTF8_djb2) &&
                   (pPoseUnit->nameKey_UTF8_DKDR == boneUnit->nameKey_UTF8_DKDR)) {
                    boneMotionMap->targetPostureMemory = pPoseUnit;
                    break;
                }
            }
        }
    }
}

vaPostureMemoryContext *getElapseMotion(struct AvatarMotionStepContext *srcCtx,
                                        struct VocaloidMotionDataContext *srcMotion,
                                        float globalTimeStamp,
                                        float presentTimeStamp)
{
    int bi, fi;
    const float localTimeStamp = globalTimeStamp - presentTimeStamp;

    // check all eccevtive bones in the motion contex.
    for(bi = 0 ; bi < srcMotion->effectiveBoneCount ; bi++) {
        struct wlBoneMotionMapUnit *boneMotionMap = srcCtx->boneMotionMap + bi;
        struct BoneMotionUnit *effectiveBoneSet = srcMotion->effectiveBoneSet + bi;
        struct vaPostureMemoryUnit *poseUnit = (struct vaPostureMemoryUnit *)boneMotionMap->targetPostureMemory ;
        struct KeyFrameUnit *pFrameOP = NULL;
        struct KeyFrameUnit *pFrameED = NULL;

        if(poseUnit == NULL) continue;

        // search the present key frame.
        if(boneMotionMap->keyFrameTimeStampLimit <= globalTimeStamp) {
            for(fi = boneMotionMap->keyFrameID ; fi < effectiveBoneSet->KeyFrameCount ; fi++) {
                struct KeyFrameUnit *pFrame =(effectiveBoneSet->KeyFrame +fi);
                if(pFrame->FrameID <= localTimeStamp) pFrameOP = pFrame;
                if(pFrame->FrameID >= localTimeStamp) {  //any bone have end frame will be displied
                    pFrameED = pFrame;
                    boneMotionMap->keyFrameID = fi;
                    boneMotionMap->keyFrameTimeStampLimit = presentTimeStamp + pFrameED->FrameID;
                    break;
                }
            }

            // If the time stamp of last key frame less then the current time stamp. keep the motion of last pFrame.
            if(pFrameED == NULL) {
                pFrameED = pFrameOP;
            }
            memcpy(boneMotionMap->quaternionED, pFrameED->Quaternion, sizeof(float) *4);
            memcpy(boneMotionMap->translateED, pFrameED->Translate, sizeof(float) *3);

            /* Assign the quaternion & translater into previousKeyFrame.
                 * But, if the time stamp is less than the first key frame of current motion context, the frame op wil keep as the last key frame,
                */
            if(pFrameOP != NULL) {
                memcpy(boneMotionMap->quaternion, pFrameOP->Quaternion, sizeof(float) *4);
                memcpy(boneMotionMap->translate, pFrameOP->Translate, sizeof(float) *3);
                 boneMotionMap->globalTimeStamp = presentTimeStamp + pFrameOP->FrameID;
            }
        }

        // elapse the quaternion between current and previous frame.
        float InterpolationT = interpolationFunction(globalTimeStamp,
                                                     boneMotionMap->globalTimeStamp,
                                                     boneMotionMap->keyFrameTimeStampLimit);
        float DotProduct = boneMotionMap->quaternion[0] * boneMotionMap->quaternionED[0] +
                           boneMotionMap->quaternion[1] * boneMotionMap->quaternionED[1] +
                           boneMotionMap->quaternion[2] * boneMotionMap->quaternionED[2] +
                           boneMotionMap->quaternion[3] * boneMotionMap->quaternionED[3] ;
        float param = 1.0f;
        if (DotProduct < 0.0f) param = -1.0f ;
        DotProduct *= param;
        if(DotProduct < -1.0f) DotProduct = -1.0f;
        if(DotProduct > 1.0f) DotProduct = 1.0f;

        float Angle = acos(DotProduct);
        float slerpP1 = (Angle ==0) ? 0 : sin((1-InterpolationT) *Angle) / sin(Angle);
        float slerpP2 = (Angle ==0) ? 1 : sin(InterpolationT *Angle) / sin(Angle);
        float linearP1 = (1-InterpolationT);
        float linearP2 = InterpolationT;

        poseUnit->quaternion[0] = slerpP1 *boneMotionMap->quaternion[0] + slerpP2 *boneMotionMap->quaternionED[0] *param;
        poseUnit->quaternion[1] = slerpP1 *boneMotionMap->quaternion[1] + slerpP2 *boneMotionMap->quaternionED[1] *param;
        poseUnit->quaternion[2] = slerpP1 *boneMotionMap->quaternion[2] + slerpP2 *boneMotionMap->quaternionED[2] *param;
        poseUnit->quaternion[3] = slerpP1 *boneMotionMap->quaternion[3] + slerpP2 *boneMotionMap->quaternionED[3] *param;
        poseUnit->translate[0] = linearP1 *boneMotionMap->translate[0] + linearP2 *boneMotionMap->translateED[0];
        poseUnit->translate[1] = linearP1 *boneMotionMap->translate[1] + linearP2 *boneMotionMap->translateED[1];
        poseUnit->translate[2] = linearP1 *boneMotionMap->translate[2] + linearP2 *boneMotionMap->translateED[2];
    }
}
// --------------------------------------------------------------------
void WorldLineContext::elapseMotion(float TimeStamp)
{
    if(MotionChain_Head != NULL) {
        struct AvatarMotionStepContext *cMotion = this->MotionChain_Head;
        while((cMotion != NULL) && (cMotion->SetupTime <= TimeStamp)) {   //Check the setup time
            if((cMotion->PresentTime + cMotion->StampSize) > TimeStamp) { //Check TimeStamp is large than present region or not.
                VisualizationAvatar *pAvatar = Avatar[cMotion->AvatarID -1];
                if(pAvatar!=NULL) {
                    mapping_BoneMotionMapToPostureMemoryUnit(cMotion, cMotion->Motion_ptr, pAvatar->getPostureMemoryCtx(0));
                    currentMotionStep = cMotion;
                }
            }
            // Recycle motion unit, The motion unit is different between location and music.
            //    The motion only setup once for avatar, so it could recycle immediately.
            MotionChain_Head = cMotion->next;
            if(MotionChain_Head == NULL) MotionChain_Tail = NULL;
            cMotion->next = RecycleMotionPool;
            RecycleMotionPool = cMotion;
            cMotion = MotionChain_Head;
        }
    }

    // Pass the time stamp to avatar for posture
    if(currentMotionStep != NULL) {
        for(int ai = 0 ; ai < MAX_Avatar ; ai++) {
            if(Avatar[ai] !=NULL) {
                struct vaPostureMemoryContext *vaMem = Avatar[ai]->getPostureMemoryCtx(0);
                getElapseMotion(currentMotionStep, currentMotionStep->Motion_ptr, TimeStamp, currentMotionStep->PresentTime);
                Avatar[ai]->posture(vaMem);
            }
        }
    }
}
// --------------------------------------------------------------------
void WorldLineContext::elapseLocation(float TimeStamp)
{
    if(LocationChain_Head != NULL) {
        struct AvatarLocationStepContext *cLocation = this->LocationChain_Head;

        while((cLocation != NULL) && (cLocation->SetupTime <= TimeStamp)) {   //Check the setup time
            if((cLocation->PresentTime + cLocation->StampSize) > TimeStamp) { //Check TimeStamp largr than present region or not.
                if(Avatar[cLocation->AvatarID -1] !=NULL) {

                    // linear translate only now 2015.11.15
                    float T = (TimeStamp -cLocation->SetupTime) / cLocation->StampSize ;
                    float nagT = 1.0f - T;
                    float SmmothT[3] = {nagT * cLocation->Translate_Start[0] - T * cLocation->Translate_End[0],
                                       nagT * cLocation->Translate_Start[1] - T * cLocation->Translate_End[1],
                                       nagT * cLocation->Translate_Start[2] - T * cLocation->Translate_End[2]};
                    Avatar[cLocation->AvatarID -1]->location(SmmothT[0], SmmothT[1], SmmothT[2]);
                }
            }
            else {
                // recycle Location unit
                LocationChain_Head = cLocation->next;
                if(LocationChain_Head == NULL) LocationChain_Tail = NULL;
                cLocation->next = RecycleLocationPool;
                RecycleLocationPool = cLocation;
                cLocation = LocationChain_Head;
            }
            if(cLocation != NULL) cLocation = cLocation->next;
        }
    }
}
// --------------------------------------------------------------------
void WorldLineContext::elapseAudio(float TimeStamp)
{
    if(AudioChain_Head != NULL) {
        struct AvatarAudioStepContext *cAudio = this->AudioChain_Head;
        while((cAudio != NULL) && (cAudio->SetupTime <= TimeStamp)) {   //Check the setup time
            if((cAudio->PresentTime + cAudio->StampSize) > TimeStamp) { //Check TimeStamp largr than present region or not.
                cAudio->State = AudioState_Playing;
            }
            else {
                // recycle Audio unit
                cAudio->State = AudioState_Stop;
                AudioChain_Head = cAudio->next;
                if(AudioChain_Head == NULL) AudioChain_Tail = NULL;
                cAudio->next = RecycleAudioPool;
                RecycleAudioPool = cAudio;
                cAudio = AudioChain_Head;
            }
            if(cAudio != NULL) cAudio = cAudio->next;
        }
    }
}

/** --------------------------------------------------------------------
 * Elapse the world by into timestamp
 *
 *      @param TimeStamp : The timestamp elapse in, Note. the world line couldn't backtracking.
 */
void WorldLineContext::elapse(float TimeStamp)
{
    /*struct timeval pStart, pEnd;
    this->WorldTimeStamp = TimeStamp;
    gettimeofday(&pStart, NULL);*/

    elapseLocation(TimeStamp);
    elapseMotion(TimeStamp);
    elapseAudio(TimeStamp);

   /* gettimeofday(&pEnd, NULL);
    float uTime = (pEnd.tv_sec -pStart.tv_sec) *1000000.0f +(pEnd.tv_usec -pStart.tv_usec);
    printf("elapse time : %lf ms\n", uTime /1000.0f);*/
}
// --------------------------------------------------------------------
int WorldLineContext::reincarnate(VisualizationSystem::VisualizationAvatar *theAvatar)
{
    int i;
    if(this->AvatarCount >= MAX_Avatar) return 0;
    for(i = 0 ; i < MAX_Avatar ; i++) {
        if(this->Avatar[i] == NULL) {
            this->Avatar[i] = theAvatar;
            theAvatar->setUniverseID(i +1);
            return (i +1);  //The ID of avatar is start from 1.
        }
    }
}

// --------------------------------------------------------------------
int WorldLineContext::addAudio(int AudioID, float setupTime, float presentTime, float presentRegion)
{
    struct AvatarAudioStepContext *AudioCtx ;
    if(setupTime > presentTime) {
        printf("Fatal : The Audio couldn't setup after present\n");
        return -1;
    }
    if(RecycleAudioPool != NULL) {    //check the recycle pool
        AudioCtx = RecycleAudioPool;
        RecycleAudioPool = RecycleAudioPool->next;
    }
    else {
        AudioCtx = (struct AvatarAudioStepContext *)malloc(sizeof(struct AvatarAudioStepContext));
        if(AudioCtx == NULL) return -1;
    }
    AudioCtx->next = NULL;
    AudioCtx->State = AudioState_Freezing;
    AudioCtx->AudioID =AudioID;
    AudioCtx->SetupTime = setupTime;
    AudioCtx->PresentTime = presentTime;
    AudioCtx->StampSize = presentRegion;

    //add new Audio into Audio chain (sort by time stamp).
    if(this->AudioChain_Head == NULL) {
        this->AudioChain_Head = AudioCtx;
        this->AudioChain_Tail = AudioCtx;
    }
    else {
        if(presentTime < AudioChain_Head->PresentTime) {
            AudioCtx->next = AudioChain_Head;
            AudioChain_Head = AudioCtx;
        }
        else if(presentTime > AudioChain_Tail->PresentTime) {
            AudioChain_Tail->next = AudioCtx;
            AudioCtx->next = NULL;
            AudioChain_Tail = AudioCtx;
        }
        else {
            struct AvatarAudioStepContext *cAudio = this->AudioChain_Head;
            struct AvatarAudioStepContext *lastAudio = cAudio;
            while((cAudio != NULL) && cAudio->PresentTime < presentTime) {
                lastAudio = cAudio;
                cAudio = cAudio->next;
            }
            AudioCtx= lastAudio->next;
            lastAudio->next = AudioCtx;
        }
    }
    return presentTime + presentRegion;
}

int WorldLineContext::getAudioPlayStatus(int AudioID)
{
    struct AvatarAudioStepContext *cAudio = this->AudioChain_Head;
    while(cAudio != NULL) {
        if(cAudio->AudioID == AudioID) return cAudio->State;
        cAudio = cAudio->next;
    }
    return AudioState_Chaos;
}

/** --------------------------------------------------------------------
 * Add the absolute location into world line.
 *
 *  @param AvatarID         : The associate avatar.
 *  @param setupTime        : The setup time.
 *  @param presentTime      : The time to presnet this location,
 *  @param presentRegion    : The time region of this locaiotn step , the location will be linearly interpolated in this region.
 *  @param StartX,Y,Z       : The start location.
 *  @param EndX,Y,Z         : The end location.
 *
 *  @return : -1 when error occure, or return the timestamp after this motion.
*/
int WorldLineContext::addLocation(int AvatarID, float setupTime, float presentTime, float presentRegion, float StartX, float StartY, float StartZ, float EndX, float EndY, float EndZ)
{
    struct AvatarLocationStepContext *LocationCtx ;
    if(setupTime > presentTime) {
        printf("Fatal : The location couldn't setup after present\n");
        return -1;
    }
    if(RecycleLocationPool != NULL) {    //check the recycle pool
        LocationCtx = RecycleLocationPool;
        RecycleLocationPool = RecycleLocationPool->next;
    }
    else {
        LocationCtx = (struct AvatarLocationStepContext *)malloc(sizeof(struct AvatarLocationStepContext));
        if(LocationCtx == NULL) return -1;
    }
    LocationCtx->next = NULL;
    LocationCtx->AvatarID = AvatarID;
    LocationCtx->Translate_Start[0] = StartX;
    LocationCtx->Translate_Start[1] = StartY;
    LocationCtx->Translate_Start[2] = StartZ;
    LocationCtx->Translate_End[0] = EndX;
    LocationCtx->Translate_End[1] = EndY;
    LocationCtx->Translate_End[2] = EndZ;
    LocationCtx->SetupTime = setupTime;
    LocationCtx->PresentTime = presentTime;
    LocationCtx->StampSize = presentRegion;

    //add new Location into Location chain (sort by time stamp).
    if(this->LocationChain_Head == NULL) {
        this->LocationChain_Head = LocationCtx;
        this->LocationChain_Tail = LocationCtx;
    }
    else {
        if(presentTime < LocationChain_Head->PresentTime) {
            LocationCtx->next = LocationChain_Head;
            LocationChain_Head = LocationCtx;
        }
        else if(presentTime > LocationChain_Tail->PresentTime) {
            LocationChain_Tail->next = LocationCtx;
            LocationCtx->next = NULL;
            LocationChain_Tail = LocationCtx;
        }
        else {
            struct AvatarLocationStepContext *cLocation = this->LocationChain_Head;
            struct AvatarLocationStepContext *lastLocation = cLocation;
            while((cLocation != NULL) && cLocation->PresentTime < presentTime) {
                lastLocation = cLocation;
                cLocation = cLocation->next;
            }
            LocationCtx= lastLocation->next;
            lastLocation->next = LocationCtx;
        }
    }
    return presentTime + presentRegion;
}
/** --------------------------------------------------------------------
 * Add the motion into world line.
 *
 *  @param AvatarID : The associate avatar.
 *  @param srcMotion : The source motion data.
 *
 *  @return : -1 when error occure, or return the timestamp after this motion.
*/
int WorldLineContext::addMotion(int AvatarID, float setupTime, float presentTime, MMDCppDev::VocaloidMotionDataContext *srcMotion)
{
    struct AvatarMotionStepContext *MotionCtx ;

    if(setupTime > presentTime) {
        printf("Fatal : The motion couldn't setup after present\n");
        return -1;
    }
    if(RecycleMotionPool != NULL) {    //check the recycle pool
        MotionCtx = RecycleMotionPool;
        RecycleMotionPool = RecycleMotionPool->next;
    }
    else {
        MotionCtx = (struct AvatarMotionStepContext *)malloc(sizeof(struct AvatarMotionStepContext));
        if(MotionCtx == NULL) return -1;
    }
    memset(MotionCtx, 0, sizeof(struct AvatarMotionStepContext));
    MotionCtx->next = NULL;
    MotionCtx->AvatarID = AvatarID;
    MotionCtx->Motion_ptr = srcMotion;
    MotionCtx->SetupTime = setupTime;
    MotionCtx->PresentTime = presentTime;
    MotionCtx->StampSize = srcMotion->MotionStamp;

    generateBoneMotionMap(MotionCtx, srcMotion);

    //Add new motion into motion chain (sort by time stamp).
    if(this->MotionChain_Head == NULL) {
        this->MotionChain_Head = MotionCtx;
        this->MotionChain_Tail = MotionCtx;
    }
    else {
        if(presentTime < MotionChain_Head->PresentTime) {
            MotionCtx->next = MotionChain_Head;
            MotionChain_Head = MotionCtx;
        }
        else if(presentTime > MotionChain_Tail->PresentTime) {
            MotionChain_Tail->next = MotionCtx;
            MotionCtx->next = NULL;
            MotionChain_Tail = MotionCtx;
        }
        else {
            struct AvatarMotionStepContext *cMotion = this->MotionChain_Head;
            struct AvatarMotionStepContext *lastMotion = cMotion;
            while((cMotion != NULL) && cMotion->PresentTime < presentTime) {
                lastMotion = cMotion;
                cMotion = cMotion->next;
            }
            MotionCtx= lastMotion->next;
            lastMotion->next = MotionCtx;
        }
    }
    return presentTime + (float)srcMotion->MotionStamp;
}
// --------------------------------------------------------------------
void WorldLineContext::create()
{
    ID = 0;
    WorldTimeStamp = 0;
    AvatarCount = 0;
    memset(Avatar, 0, sizeof(VisualizationSystem::VisualizationAvatar *) * MAX_Avatar);
    MotionChain_Head = NULL;
    MotionChain_Tail = NULL;
    RecycleMotionPool = NULL;

    LocationChain_Head = NULL;
    LocationChain_Tail = NULL;
    RecycleLocationPool = NULL;

    AudioChain_Head = NULL;
    AudioChain_Tail = NULL;
    RecycleAudioPool = NULL;
}
/** --------------------------------------------------------------------
 * Check the unit count in world line
*/
int WorldLineContext::isEmpty()
{
    return (MotionChain_Head == NULL && LocationChain_Head == NULL && AudioChain_Head == NULL);
}

// --------------------------------------------------------------------
void WorldLineContext::reset()
{
    // reset all chain in to recycle.
    struct AvatarAudioStepContext *cAudio = this->AudioChain_Head;
    if(cAudio != NULL) {
        cAudio->next = RecycleAudioPool;
        RecycleAudioPool = cAudio;
        AudioChain_Head = NULL;
        AudioChain_Tail = NULL;
    }
    struct AvatarMotionStepContext *cMotion = this->MotionChain_Head;
    if(cMotion != NULL) {
        cMotion->next = RecycleMotionPool;
        RecycleMotionPool = cMotion;
        MotionChain_Head = NULL;
        MotionChain_Tail = NULL;
    }

    struct AvatarLocationStepContext *cAvatar = this->LocationChain_Head;
    if(cAvatar != NULL) {
        cAvatar->next = RecycleLocationPool;
        RecycleLocationPool = cAvatar;
        LocationChain_Head = NULL;
        LocationChain_Tail = NULL;
    }

    WorldTimeStamp = 0;
    AvatarCount = 0;
    memset(Avatar, 0, sizeof(VisualizationSystem::VisualizationAvatar *) * MAX_Avatar);
}

// --------------------------------------------------------------------
void WorldLineContext::dumpInfo()
{
    int Counter = 0;
    struct AvatarMotionStepContext *mTmp = MotionChain_Head;
    while(mTmp != NULL) {
        Counter++;
        mTmp = mTmp->next;
    }
    printf("The Data in Motion Chain %d\n", Counter);
    Counter = 0;
    mTmp = RecycleMotionPool;
    while(mTmp != NULL) {
        Counter++;
        mTmp = mTmp->next;
    }

    printf("The Data in Motion Recycle Chain %d\n", Counter);


    Counter = 0;
    struct AvatarLocationStepContext *lTmp = LocationChain_Head;
    while(lTmp != NULL) {
        Counter++;
        lTmp = lTmp->next;
    }
    printf("The Data in Location Chain %d\n", Counter);
    Counter = 0;
    lTmp = RecycleLocationPool;
    while(lTmp != NULL) {
        Counter++;
        lTmp = lTmp->next;
    }

    printf("The Data in Location Recycle Chain %d\n", Counter);

    Counter = 0;
    struct AvatarAudioStepContext *aTmp = AudioChain_Head;
    while(aTmp != NULL) {
        Counter++;
        aTmp = aTmp->next;
    }
    printf("The Data in Audio Chain %d\n", Counter);
    Counter = 0;
    aTmp = RecycleAudioPool;
    while(aTmp != NULL) {
        Counter++;
        aTmp = aTmp->next;
    }

    printf("The Data in Audio Recycle Chain %d\n", Counter);

}
