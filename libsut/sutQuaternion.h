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
    void asConjugated();
    void asRotation(float RotationVector_x , float RotationVector_y ,float RotationVector_z ,float RotationAngle);
    void setQuaternion(float ax ,float by ,float cz, float w);
    void setTranslate(float Tx ,float Ty ,float Tz);
    float *toMatrix();
    float *getMatrix();
    void getTranslate(float *dstAry);
    void getQuaternion(float *dstAry);
    struct sutQuaternionContext *getQuaternionCtx();
    void mul(sutQuaternionOperator *Multiplier);
    void add(sutQuaternionOperator *Multiplier);

private:
    int isIdentity;
    struct sutQuaternionContext QuaternionCtx ;
    float MatrixCtx[4][4];
};
#endif // GLsutQuaternionOperator_H_INCLUDED
