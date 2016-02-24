uniform mat4 inProjectionMatrix;
uniform mat4 inModelViewMatrix;
uniform vec3 inLightPosition;
uniform vec3 inCameraLocation;
attribute vec2 inTexCoord;
attribute vec3 inVertex;
attribute vec3 inNormal;
varying vec2 vTexCoord;
varying vec3 vLightVector_0;
varying vec3 vNormal;
varying vec3 vVertex;
varying vec3 vCameraVector;

void main() 
{
    vec4 mVertex        = inModelViewMatrix * vec4(inVertex.xyz, 1.0);
	vVertex             = inVertex;
    vNormal             = normalize(inNormal);
    vLightVector_0      = normalize(inLightPosition - vVertex);
    vCameraVector       = normalize(inCameraLocation - vVertex);
    vTexCoord           = inTexCoord;
    gl_Position         = inProjectionMatrix * mVertex;
}
