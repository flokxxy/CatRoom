
#version 330 core

layout (location = 0) in vec2 inPos; //каорды х у прямоугольника - изображения
layout (location = 1) in vec2 inTexCoord; //каорды U V изображения

out vec2 chTexCoord;
//

uniform vec2 uScale;
uniform vec2 uOffset;

uniform bool uIsHeldCat;
uniform float uCatAngle;
uniform vec2 uCatPivot;
uniform vec2 uSceneSize;

void main()
{
    vec2 position = inPos * uScale + uOffset;
    //same xb+y


    //description of the swaying image of a cat
    if (uIsHeldCat){
        vec2 pivot = vec2(2.0 * uCatPivot.x - 1.0, 1.0 -2.0*uCatPivot.y);
        vec2 relativePixels = (inPos - pivot) * uScale * uSceneSize*0.5;
        //we count the turn

        float c = cos(uCatAngle);
        float s = sin(uCatAngle);

        vec2 rotatedPixels = vec2( c * relativePixels.x - s * relativePixels.y, s * relativePixels.x + c * relativePixels.y);
        position = pivot * uScale + uOffset + rotatedPixels * 2.0 / uSceneSize;
    }
    

    gl_Position = vec4(position, 0.0, 1.0);
    chTexCoord = inTexCoord;
}
