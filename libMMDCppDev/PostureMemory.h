#ifndef POSTURE_MEMORY_H
#define POSTURE_MEMORY_H
// --------------------------------------------------------------------
namespace MMDCppDev {
//---------------------------------------------------------------------
struct PostureMemoryUnit {
    unsigned int Key_ShiftJIS_DKDR;
    unsigned int Key_ShiftJIS_djb2;
    float Translate[3];
    float Quaternion[4]; //store as : x y z w
    int InterpolstionID;
};

struct PostureMemoryContext {
    int PoseUnitCount;
    struct PostureMemoryUnit *PoseUnit;
    unsigned char *InterpolationData;
};
//---------------------------------------------------------------------
class PostureMemoryManager {
public:
    struct PostureMemoryContext *generate_Context();
    void release(struct PostureMemoryContext *relCtx);
    int load(struct PostureMemoryContext *inCtx, const char *Path);

private :
    FILE *ProfileFile_ptr;
    struct PostureMemoryContext *ProfileCtx_ptr;
    int parse_PostureMemoryUnit();
    int parse_PostureMemoryUnit_ShiftJIS();
};


// --------------------------------------------------------------------
}
// --------------------------------------------------------------------
#endif // VPD_PROFILE_H
