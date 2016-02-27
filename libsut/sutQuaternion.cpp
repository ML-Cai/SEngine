#include <stdio.h>
#include <math.h>
#include "sutQuaternion.h"
/* -------------------------------------------------------------------------------------
 * http://openhome.cc/Gossip/ComputerGraphics/QuaternionsRotate.htm
 * http://www.3dgep.com/understanding-quaternions/
 */
static const float PI = 3.14159 /180.0f;
//--------------------------------------------------------------------------------------
void sutQuaternionOperator::init()
{
    stateIdentity =1;
    isMatrixchange =1;
    QuaternionCtx.a = 0.0f;
    QuaternionCtx.b = 0.0f;
    QuaternionCtx.c = 0.0f;
    QuaternionCtx.w = 1.0f;
    QuaternionCtx.Translate[0] = 0.0f;
    QuaternionCtx.Translate[1] = 0.0f;
    QuaternionCtx.Translate[2] = 0.0f;
    MatrixCtx[0][0] = 1.0f;
    MatrixCtx[0][1] = 0.0f;
    MatrixCtx[0][2] = 0.0f;
    MatrixCtx[0][3] = 0.0f;
    MatrixCtx[1][0] = 0.0f;
    MatrixCtx[1][1] = 1.0f;
    MatrixCtx[1][2] = 0.0f;
    MatrixCtx[1][3] = 0.0f;
    MatrixCtx[2][0] = 0.0f;
    MatrixCtx[2][1] = 0.0f;
    MatrixCtx[2][2] = 1.0f;
    MatrixCtx[2][3] = 0.0f;
    MatrixCtx[3][0] = 0.0f;
    MatrixCtx[3][1] = 0.0f;
    MatrixCtx[3][2] = 0.0f;
    MatrixCtx[3][3] = 1.0f;
}

//-------------------------------------------------------------------------------------
sutQuaternionOperator::sutQuaternionOperator() //create a unit quaternion
{
    QuaternionCtx.w =1.0f ;
    QuaternionCtx.a =0.0f ;
    QuaternionCtx.b =0.0f ;
    QuaternionCtx.c =0.0f ;
}
//-------------------------------------------------------------------------------------
void sutQuaternionOperator::setTranslate(float Tx ,float Ty ,float Tz)
{
    QuaternionCtx.Translate[0] = Tx;
    QuaternionCtx.Translate[1] = Ty;
    QuaternionCtx.Translate[2] = Tz;
    isMatrixchange =1;
}
//-------------------------------------------------------------------------------------
void sutQuaternionOperator::fromRollPitchYaw(float radiusRoll , float radiusPitch , float radiusYaw)
{
    QuaternionCtx.w = cos(radiusRoll/2) *cos(radiusPitch/2) *cos(radiusYaw/2) +sin(radiusRoll/2) *sin(radiusPitch/2) *sin(radiusYaw/2);
    QuaternionCtx.a = sin(radiusRoll/2) *cos(radiusPitch/2) *cos(radiusYaw/2) -cos(radiusRoll/2) *sin(radiusPitch/2) *sin(radiusYaw/2);
    QuaternionCtx.b = cos(radiusRoll/2) *sin(radiusPitch/2) *cos(radiusYaw/2) +sin(radiusRoll/2) *cos(radiusPitch/2) *sin(radiusYaw/2);
    QuaternionCtx.c = cos(radiusRoll/2) *cos(radiusPitch/2) *sin(radiusYaw/2) -sin(radiusRoll/2) *sin(radiusPitch/2) *cos(radiusYaw/2);
    isMatrixchange =1;
}
//-------------------------------------------------------------------------------------
void sutQuaternionOperator::setQuaternion(float ax , float by , float cz, float w)
{
    QuaternionCtx.w = w ;
    QuaternionCtx.a = ax ;
    QuaternionCtx.b = by ;
    QuaternionCtx.c = cz;
    stateIdentity = 0;
    isMatrixchange =1;
   /* float Len = ax *ax + by *by + cz *cz + w *w;
    float invMagnitude = (Len >0) ? 1.0f / sqrtf(Len) : 1.0f;
    QuaternionCtx.w = w *invMagnitude;
    QuaternionCtx.a = ax *invMagnitude;
    QuaternionCtx.b = by *invMagnitude;
    QuaternionCtx.c = cz *invMagnitude;
    stateIdentity = 0;*/
}
//--------------------------------------------------------------------------------------
void sutQuaternionOperator::asRotation(float RotationVector_x , float RotationVector_y ,float RotationVector_z ,float RotationAngle)
{
    /* Normalizing input vector */
    float Len2 = RotationVector_x *RotationVector_x +RotationVector_y *RotationVector_y +RotationVector_z*RotationVector_z;
    float invMagnitude = (Len2 >0) ? 1.0f / sqrtf(Len2) : 1.0f;
    float tmp_sin = sin (RotationAngle *0.5f *PI);

    /* calc Quaternion unit */
    QuaternionCtx.a = RotationVector_x *invMagnitude *tmp_sin;
    QuaternionCtx.b = RotationVector_y *invMagnitude *tmp_sin;
    QuaternionCtx.c = RotationVector_z *invMagnitude *tmp_sin;
    QuaternionCtx.w = cos(RotationAngle *0.5f *PI) ;
    stateIdentity = 0;
    isMatrixchange =1;
}
//--------------------------------------------------------------------------------------
float *sutQuaternionOperator::getMatrix()
{
    if(isMatrixchange == 1) {
        this->toMatrix();
        isMatrixchange =0;
    }
    return MatrixCtx[0];
}

//--------------------------------------------------------------------------------------
void sutQuaternionOperator::toMatrix()
{
    /*MatrixCtx[0][0]= 1.0f - 2.0f *(QuaternionCtx.b *QuaternionCtx.b +QuaternionCtx.c *QuaternionCtx.c) ;
    MatrixCtx[1][0]= 2.0f * (QuaternionCtx.a *QuaternionCtx.b - QuaternionCtx.w *QuaternionCtx.c) ;
    MatrixCtx[2][0]= 2.0f * (QuaternionCtx.a *QuaternionCtx.c + QuaternionCtx.w *QuaternionCtx.b) ;
    MatrixCtx[3][0]= 0.0f ;

    MatrixCtx[0][1]= 2.0f * (QuaternionCtx.a *QuaternionCtx.b + QuaternionCtx.w *QuaternionCtx.c) ;
    MatrixCtx[1][1]= 1.0f - 2.0f *(QuaternionCtx.a *QuaternionCtx.a + QuaternionCtx.c *QuaternionCtx.c) ;
    MatrixCtx[2][1]= 2.0f * (QuaternionCtx.b *QuaternionCtx.c - QuaternionCtx.w *QuaternionCtx.a) ;
    MatrixCtx[3][1]= 0.0f;

    MatrixCtx[0][2]= 2.0f * (QuaternionCtx.a *QuaternionCtx.c - QuaternionCtx.w *QuaternionCtx.b) ;
    MatrixCtx[1][2]= 2.0f * (QuaternionCtx.b *QuaternionCtx.c +QuaternionCtx.w *QuaternionCtx.a) ;
    MatrixCtx[2][2]= 1.0f - 2.0f *(QuaternionCtx.a *QuaternionCtx.a +QuaternionCtx.b *QuaternionCtx.b) ;
    MatrixCtx[3][2]= 0.0f;
    MatrixCtx[0][3]= QuaternionCtx.Translate[0];
    MatrixCtx[1][3]= QuaternionCtx.Translate[1];
    MatrixCtx[2][3]= QuaternionCtx.Translate[2];
    MatrixCtx[3][3]= 1.0f;
    return MatrixCtx[0];*/

    // Left Hand matrix transform
    MatrixCtx[0][0]= 1.0f - 2.0f *(QuaternionCtx.b *QuaternionCtx.b + QuaternionCtx.c *QuaternionCtx.c) ;
    MatrixCtx[1][0]=        2.0f *(QuaternionCtx.a *QuaternionCtx.b + QuaternionCtx.w *QuaternionCtx.c) ;
    MatrixCtx[2][0]=        2.0f *(QuaternionCtx.a *QuaternionCtx.c - QuaternionCtx.w *QuaternionCtx.b) ;
    MatrixCtx[3][0]= 0.0f ;
    MatrixCtx[0][1]=        2.0f *(QuaternionCtx.a *QuaternionCtx.b - QuaternionCtx.w *QuaternionCtx.c) ;
    MatrixCtx[1][1]= 1.0f - 2.0f *(QuaternionCtx.a *QuaternionCtx.a + QuaternionCtx.c *QuaternionCtx.c) ;
    MatrixCtx[2][1]=        2.0f *(QuaternionCtx.b *QuaternionCtx.c + QuaternionCtx.w *QuaternionCtx.a) ;
    MatrixCtx[3][1]= 0.0f;
    MatrixCtx[0][2]=        2.0f *(QuaternionCtx.w *QuaternionCtx.b + QuaternionCtx.a *QuaternionCtx.c) ;
    MatrixCtx[1][2]=        2.0f *(QuaternionCtx.b *QuaternionCtx.c - QuaternionCtx.w *QuaternionCtx.a) ;
    MatrixCtx[2][2]= 1.0f - 2.0f *(QuaternionCtx.a *QuaternionCtx.a + QuaternionCtx.b *QuaternionCtx.b) ;
    MatrixCtx[3][2]= 0.0f;
    MatrixCtx[0][3]= QuaternionCtx.Translate[0];
    MatrixCtx[1][3]= QuaternionCtx.Translate[1];
    MatrixCtx[2][3]= QuaternionCtx.Translate[2];
    MatrixCtx[3][3]= 1.0f;
}
//--------------------------------------------------------------------------------------
void sutQuaternionOperator::getTranslate(float *dstAry)
{
    dstAry[0] = QuaternionCtx.Translate[0];
    dstAry[1] = QuaternionCtx.Translate[1];
    dstAry[2] = QuaternionCtx.Translate[2];
}
void sutQuaternionOperator::getQuaternion(float *dstAry)
{
    dstAry[0] = QuaternionCtx.a;
    dstAry[1] = QuaternionCtx.b;
    dstAry[2] = QuaternionCtx.c;
    dstAry[3] = QuaternionCtx.w;
}

/** --------------------------------------------------------------------------------------
 * CurrentQuaternion = CurrentQuaternion * Multiplier
*/
void sutQuaternionOperator::mul(sutQuaternionOperator *Multiplier, unsigned int inMode)
{
    struct sutQuaternionContext *LQuaternion, *RQuaternion;
    struct sutQuaternionContext tmp ;

    if(Multiplier->isIdentity()) {
        return;
    }
    if(stateIdentity) {
        tmp.a = Multiplier->QuaternionCtx.a;
        tmp.b = Multiplier->QuaternionCtx.b;
        tmp.c = Multiplier->QuaternionCtx.c;
        tmp.w = Multiplier->QuaternionCtx.w;
        stateIdentity = 0;
    }
    else {
        if(inMode == INPUT_AS_LEFT) {
            LQuaternion = &Multiplier->QuaternionCtx;
            RQuaternion = &QuaternionCtx;
        }
        else {
            LQuaternion = &QuaternionCtx;
            RQuaternion = &Multiplier->QuaternionCtx;
        }
        tmp.a = LQuaternion->w * RQuaternion->a +
                LQuaternion->a * RQuaternion->w +
                LQuaternion->b * RQuaternion->c -
                LQuaternion->c * RQuaternion->b ;
        tmp.b = LQuaternion->w * RQuaternion->b -
                LQuaternion->a * RQuaternion->c +
                LQuaternion->b * RQuaternion->w +
                LQuaternion->c * RQuaternion->a ;
        tmp.c = LQuaternion->w * RQuaternion->c +
                LQuaternion->a * RQuaternion->b -
                LQuaternion->b * RQuaternion->a +
                LQuaternion->c * RQuaternion->w ;
        tmp.w = LQuaternion->w * RQuaternion->w -
                LQuaternion->a * RQuaternion->a -
                LQuaternion->b * RQuaternion->b -
                LQuaternion->c * RQuaternion->c;
    }
    QuaternionCtx.a = tmp.a ;
    QuaternionCtx.b = tmp.b ;
    QuaternionCtx.c = tmp.c ;
    QuaternionCtx.w = tmp.w ;
    QuaternionCtx.Translate[0] += Multiplier->QuaternionCtx.Translate[0];
    QuaternionCtx.Translate[1] += Multiplier->QuaternionCtx.Translate[1];
    QuaternionCtx.Translate[2] += Multiplier->QuaternionCtx.Translate[2];
    isMatrixchange =1;
}

void sutQuaternionOperator::dump()
{
    printf("Quaternion x : %.5f, y : %.5f, z : %.5f, w : %.5f .... isIdentity %d\n", QuaternionCtx.a, QuaternionCtx.b, QuaternionCtx.c, QuaternionCtx.w, stateIdentity);
}

//--------------------------------------------------------------------------------------
void sutQuaternionOperator::add(sutQuaternionOperator *Multiplier)
{
    QuaternionCtx.a += Multiplier->QuaternionCtx.a;
    QuaternionCtx.b += Multiplier->QuaternionCtx.b;
    QuaternionCtx.c += Multiplier->QuaternionCtx.c;
    QuaternionCtx.w += Multiplier->QuaternionCtx.w;
    QuaternionCtx.Translate[0] += Multiplier->QuaternionCtx.Translate[0];
    QuaternionCtx.Translate[1] += Multiplier->QuaternionCtx.Translate[1];
    QuaternionCtx.Translate[2] += Multiplier->QuaternionCtx.Translate[2];
    isMatrixchange =1;
}

struct sutQuaternionContext *sutQuaternionOperator::getQuaternionCtx()
{
    if(stateIdentity) return NULL;
    return &QuaternionCtx;
}

//--------------------------------------------------------------------------------------
void sutQuaternionOperator::normalize()
{
    float len = QuaternionCtx.a *QuaternionCtx.a + QuaternionCtx.b*QuaternionCtx.b + QuaternionCtx.c*QuaternionCtx.c + QuaternionCtx.w*QuaternionCtx.w;
    float invMagnitude = (len < 0.00001) ? 1.0f : 1.0f / sqrtf(len);

    QuaternionCtx.a = QuaternionCtx.a *invMagnitude;
    QuaternionCtx.b = QuaternionCtx.b *invMagnitude;
    QuaternionCtx.c = QuaternionCtx.c *invMagnitude;
    QuaternionCtx.w = QuaternionCtx.w *invMagnitude;
    isMatrixchange =1;
}

//--------------------------------------------------------------------------------------
void sutQuaternionOperator::asConjugated()
{
    float len = QuaternionCtx.a *QuaternionCtx.a + QuaternionCtx.b*QuaternionCtx.b + QuaternionCtx.c*QuaternionCtx.c + QuaternionCtx.w*QuaternionCtx.w;
    float invMagnitude = (len < 0.00001) ? 1.0f : 1.0f / (len);

    QuaternionCtx.a = -QuaternionCtx.a *invMagnitude;
    QuaternionCtx.b = -QuaternionCtx.b *invMagnitude;
    QuaternionCtx.c = -QuaternionCtx.c *invMagnitude;
    QuaternionCtx.w = QuaternionCtx.w *invMagnitude;
    isMatrixchange =1;
}
//--------------------------------------------------------------------------------------
void sutQuaternionOperator::asInverse()
{
    float len = QuaternionCtx.a *QuaternionCtx.a + QuaternionCtx.b*QuaternionCtx.b + QuaternionCtx.c*QuaternionCtx.c + QuaternionCtx.w*QuaternionCtx.w;
    float invMagnitude = (len < 0.00001) ? 1.0f : 1.0f / (len);

    QuaternionCtx.a = -QuaternionCtx.a *invMagnitude;
    QuaternionCtx.b = -QuaternionCtx.b *invMagnitude;
    QuaternionCtx.c = -QuaternionCtx.c *invMagnitude;
    QuaternionCtx.w =  QuaternionCtx.w *invMagnitude;
    isMatrixchange =1;
}

//--------------------------------------------------------------------------------------
int sutQuaternionOperator::isIdentity()
{
    return stateIdentity;
}
//--------------------------------------------------------------------------------------
void sutQuaternionOperator::truncate(float uperRoll, float lowerRoll,
                                     float uperPitch, float lowerPitch,
                                     float uperYaw, float lowerYaw)
{
    /** Reference from :
     * https://en.wikipedia.org/wiki/Conversion_between_quaternions_and_Euler_angles
    */
    float x = QuaternionCtx.a;
    float y = QuaternionCtx.b;
    float z = QuaternionCtx.c;
    float w = QuaternionCtx.w;
    float radiusRoll  = atan2(2.0f *(w *x + y *z), 1.0f -2.0f *(x *x +y *y));
    float radiusPitch = asin (2.0f *(w *y - z *x));
    float radiusYaw   = atan2(2.0f *(w *z + x *y), 1.0f -2.0f *(y *y +z *z));

    if(radiusRoll  > uperRoll)   radiusRoll  =  uperRoll;
    else if(radiusRoll  < lowerRoll)  radiusRoll  =  lowerRoll;
    if(radiusPitch > uperPitch)  radiusPitch =  uperPitch;
    else if(radiusPitch < lowerPitch) radiusPitch =  lowerPitch;
    if(radiusYaw   > uperYaw)    radiusYaw   =  uperYaw;
    else if(radiusYaw   < lowerYaw)   radiusYaw   =  lowerYaw;

    QuaternionCtx.w = cos(radiusRoll/2) *cos(radiusPitch/2) *cos(radiusYaw/2) +sin(radiusRoll/2) *sin(radiusPitch/2) *sin(radiusYaw/2);
    QuaternionCtx.a = sin(radiusRoll/2) *cos(radiusPitch/2) *cos(radiusYaw/2) -cos(radiusRoll/2) *sin(radiusPitch/2) *sin(radiusYaw/2);
    QuaternionCtx.b = cos(radiusRoll/2) *sin(radiusPitch/2) *cos(radiusYaw/2) +sin(radiusRoll/2) *cos(radiusPitch/2) *sin(radiusYaw/2);
    QuaternionCtx.c = cos(radiusRoll/2) *cos(radiusPitch/2) *sin(radiusYaw/2) -sin(radiusRoll/2) *sin(radiusPitch/2) *cos(radiusYaw/2);
    normalize();
}
