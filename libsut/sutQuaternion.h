#ifndef SUT_QUATERNION_H_INCLUDED
#define SUT_QUATERNION_H_INCLUDED
/* -------------------------------------------------------------------------------------
 * http://openhome.cc/Gossip/ComputerGraphics/QuaternionsRotate.htm
 */
//--------------------------------------------------------------------------------------
struct sutQuaternionContext
{
    float w ;   /* Quaternion = w+ a*i + b*j + c*k */
    float a ;
    float b ;
    float c ;
    float Translate[3];
};
//--------------------------------------------------------------------------------------
class sutQuaternionOperator
{
public:
    sutQuaternionOperator() ;
    void init();
    void normalize();
    void truncate(float uperRoll, float lowerRoll, float uperPitch, float lowerPitch, float uperYaw, float lowerYaw);
    void asConjugated();
    void asInverse();
    void asRotation(float RotationVector_x , float RotationVector_y ,float RotationVector_z ,float RotationAngle);
    void setQuaternion(float ax ,float by ,float cz, float w);
    void fromRollPitchYaw(float radiusRoll , float radiusPitch , float radiusYaw);
    void setTranslate(float Tx ,float Ty ,float Tz);
    float *getMatrix();
    void getTranslate(float *dstAry);
    void getQuaternion(float *dstAry);
    struct sutQuaternionContext *getQuaternionCtx();
    void mul(sutQuaternionOperator *Multiplier, unsigned int inMode);
    void add(sutQuaternionOperator *Multiplier);
    int isIdentity();
    void dump();

    static const unsigned int INPUT_AS_LEFT     = 0;
    static const unsigned int INPUT_AS_RIGHT    = 1;
private:
    void toMatrix();
    int stateIdentity;
    int isMatrixchange;
    struct sutQuaternionContext QuaternionCtx ;
    float MatrixCtx[4][4];
};
#endif // GLsutQuaternionOperator_H_INCLUDED
