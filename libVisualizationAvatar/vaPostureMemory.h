#ifndef vaPostureMemory_H
#define vaPostureMemory_H
/* ------------------------------------------------------------------ */
namespace VisualizationSystem {

struct vaPostureMemoryUnit {
    int isEnable;
    unsigned int nameKey_UTF8_DKDR;
    unsigned int nameKey_UTF8_djb2;
    void *targetBone;
    float translate[3];
    float quaternion[4]; //store as : x y z w
   /* unsigned int previousKeyFrameID;
    float previousKeyFrameGlobalTimeStamp;
    float previousKeyFrameTranslate[3];
    float previousKeyFrameQuaternion[4];*/
};

struct vaPostureMemoryContext {
    int poseUnitCount;
    struct vaPostureMemoryUnit *poseUnit;
};

}
/* ------------------------------------------------------------------ */
#endif // vaPostureMemory_H

