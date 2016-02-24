uniform mat4 inProjectionMatrix;
uniform mat4 inModelViewMatrix;
attribute vec4 inVertex;
attribute vec4 inColor;
varying vec4 vColor;
void main()
{
    vColor = inColor;
    vec4 vOffset = inProjectionMatrix * vec4(5, 5, 0, 1);
    vec4 glPos = inProjectionMatrix * inModelViewMatrix * vec4(inVertex.xyz, 1.0);
    if(inVertex.w == 1) {
        glPos.x -= vOffset.x;
        glPos.y += vOffset.y;
    }
    else if(inVertex.w == 2) {
        glPos.x += vOffset.x;
        glPos.y += vOffset.y;
    }
    else if(inVertex.w == 3) {
        glPos.x -= vOffset.x;
        glPos.y -= vOffset.y;
    }\
    else if(inVertex.w == 4) {
        glPos.x += vOffset.x;
        glPos.y -= vOffset.y;
    }
    gl_Position = glPos;
}
