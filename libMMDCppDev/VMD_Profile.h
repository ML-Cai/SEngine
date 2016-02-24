#ifndef VMD_PROFILE_H
#define VMD_PROFILE_H
// --------------------------------------------------------------------
#include <stdio.h>
// --------------------------------------------------------------------
namespace MMDCppDev {
// --------------------------------------------------------------------
struct KeyFrameUnit {
    int FrameID;
    float Translate[3];
    float Quaternion[4]; //store as : x y z w
    char Interpolation[64];
};

struct BoneMotionUnit {
    unsigned int    nameKey_UTF8_DKDR;   //The key of bone name
    unsigned int    nameKey_UTF8_djb2;
    int KeyFrameCount;
    struct KeyFrameUnit *KeyFrame;
};

struct VocaloidMotionDataContext {
    int ObjID;
    int MotionStamp;    // total stamps in this motion
    int effectiveBoneCount;
    struct BoneMotionUnit *effectiveBoneSet;
};

// --------------------------------------------------------------------
class VMD_Profile {
public:
    VMD_Profile();
    struct VocaloidMotionDataContext *genContext(int ID);
    int load(struct VocaloidMotionDataContext *dstCtx ,const char *fPath, const char *fName);
    void release(struct VocaloidMotionDataContext *relCtx);

private:
    FILE * fMotion;
    struct VocaloidMotionDataContext *ProfiledCtx_ptr;
    int parse_VocaloidMotionKeyFrameUnit_ShiftJIS();
};

}
// --------------------------------------------------------------------
#endif // VMD_PROFILE_H
