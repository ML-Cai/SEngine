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
}
//-------------------------------------------------------------------------------------
void sutQuaternionOperator::setRotation(float ax , float by , float cz, float w)
{
    QuaternionCtx.w = w;
    QuaternionCtx.a = ax;
    QuaternionCtx.b = by;
    QuaternionCtx.c = cz;
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
}
//--------------------------------------------------------------------------------------
float *sutQuaternionOperator::getMatrix()
{
    return MatrixCtx[0];
}

//--------------------------------------------------------------------------------------
float *sutQuaternionOperator::toMatrix()
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
    return &MatrixCtx[0][0];
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

//--------------------------------------------------------------------------------------
void sutQuaternionOperator::mul(sutQuaternionOperator *Multiplier)
{

    struct sutQuaternionContext tmp ;
    tmp.a = QuaternionCtx.w * Multiplier->QuaternionCtx.a +
            QuaternionCtx.a * Multiplier->QuaternionCtx.w +
            QuaternionCtx.b * Multiplier->QuaternionCtx.c -
            QuaternionCtx.c * Multiplier->QuaternionCtx.b ;
    tmp.b = QuaternionCtx.w * Multiplier->QuaternionCtx.b -
            QuaternionCtx.a * Multiplier->QuaternionCtx.c +
            QuaternionCtx.b * Multiplier->QuaternionCtx.w +
            QuaternionCtx.c * Multiplier->QuaternionCtx.a ;
    tmp.c = QuaternionCtx.w * Multiplier->QuaternionCtx.c +
            QuaternionCtx.a * Multiplier->QuaternionCtx.b -
            QuaternionCtx.b * Multiplier->QuaternionCtx.a +
            QuaternionCtx.c * Multiplier->QuaternionCtx.w ;
    tmp.w = QuaternionCtx.w * Multiplier->QuaternionCtx.w -
            QuaternionCtx.a * Multiplier->QuaternionCtx.a -
            QuaternionCtx.b * Multiplier->QuaternionCtx.b -
            QuaternionCtx.c * Multiplier->QuaternionCtx.c;

    /* Normalizing input vector */
    float len = tmp.a *tmp.a + tmp.b*tmp.b + tmp.c*tmp.c + tmp.w*tmp.w;
    float invMagnitude = (len < 0.00001) ? 1.0f : 1.0f / sqrtf(len);
    QuaternionCtx.a = tmp.a *invMagnitude;
    QuaternionCtx.b = tmp.b *invMagnitude;
    QuaternionCtx.c = tmp.c *invMagnitude;
    QuaternionCtx.w = tmp.w *invMagnitude;
    QuaternionCtx.Translate[0] += Multiplier->QuaternionCtx.Translate[0] ;
    QuaternionCtx.Translate[1] += Multiplier->QuaternionCtx.Translate[1] ;
    QuaternionCtx.Translate[2] += Multiplier->QuaternionCtx.Translate[2] ;
}
//--------------------------------------------------------------------------------------
void sutQuaternionOperator::add(sutQuaternionOperator *Multiplier)
{
    QuaternionCtx.a += Multiplier->QuaternionCtx.a;
    QuaternionCtx.b += Multiplier->QuaternionCtx.b;
    QuaternionCtx.c += Multiplier->QuaternionCtx.c;
    QuaternionCtx.w += Multiplier->QuaternionCtx.w;
    QuaternionCtx.Translate[0] += Multiplier->QuaternionCtx.Translate[0] ;
    QuaternionCtx.Translate[1] += Multiplier->QuaternionCtx.Translate[1] ;
    QuaternionCtx.Translate[2] += Multiplier->QuaternionCtx.Translate[2] ;
}

struct sutQuaternionContext *sutQuaternionOperator::getQuaternionCtx() {
    return &QuaternionCtx;
}
//--------------------------------------------------------------------------------------
void sutQuaternionOperator::asConjugated()
{
    float len = QuaternionCtx.a *QuaternionCtx.a + QuaternionCtx.b*QuaternionCtx.b + QuaternionCtx.c*QuaternionCtx.c + QuaternionCtx.w*QuaternionCtx.w;
    float invMagnitude = (len < 0.00001) ? 1.0f : 1.0f / sqrtf(len);

    QuaternionCtx.a = -QuaternionCtx.a *invMagnitude;
    QuaternionCtx.b = -QuaternionCtx.b *invMagnitude;
    QuaternionCtx.c = -QuaternionCtx.c *invMagnitude;
    QuaternionCtx.w = QuaternionCtx.w *invMagnitude;
}
/*//--------------------------------------------------------------------------------------
sutQuaternionOperator * quaternion_multiplication(sutQuaternionOperator *a , sutQuaternionOperator *b ) //create a unit quaternion
{
    sutQuaternionOperator* tmp= (sutQuaternionOperator*)malloc(sizeof(sutQuaternionOperator)) ;
    tmp->QuaternionCtx.a = a->QuaternionCtx.w * b->QuaternionCtx.a + a->QuaternionCtx.a * b->QuaternionCtx.w + a->QuaternionCtx.b * b->QuaternionCtx.c - a->QuaternionCtx.c * b->QuaternionCtx.b ;
    tmp->QuaternionCtx.b = a->QuaternionCtx.w * b->QuaternionCtx.b - a->QuaternionCtx.a * b->QuaternionCtx.c + a->QuaternionCtx.b * b->QuaternionCtx.w + a->QuaternionCtx.c * b->QuaternionCtx.a ;
    tmp->QuaternionCtx.c = a->QuaternionCtx.w * b->QuaternionCtx.c + a->QuaternionCtx.a * b->QuaternionCtx.b - a->QuaternionCtx.b * b->QuaternionCtx.a + a->QuaternionCtx.c * b->QuaternionCtx.w ;
    tmp->QuaternionCtx.w = a->QuaternionCtx.w * b->QuaternionCtx.w - a->QuaternionCtx.a * b->QuaternionCtx.a - a->QuaternionCtx.b * b->QuaternionCtx.b - a->QuaternionCtx.c * b->QuaternionCtx.c;
    return tmp ;
}
//--------------------------------------------------------------------------------------
sutQuaternionOperator * quaternion_conjugated(sutQuaternionOperator *a) //create a unit quaternion
{
    sutQuaternionOperator* tmp= (sutQuaternionOperator*)malloc(sizeof(sutQuaternionOperator)) ;
    tmp->QuaternionCtx.a = -a->QuaternionCtx.a ;
    tmp->QuaternionCtx.b = -a->QuaternionCtx.b ;
    tmp->QuaternionCtx.c = -a->QuaternionCtx.c ;
    tmp->QuaternionCtx.w = a->QuaternionCtx.w ;
    return tmp ;
}*/
