#version 330 core

in vec2 chTexCoord;
out vec4 outCol;

uniform sampler2D uTexture;
uniform sampler2D uInteriorMask;
uniform sampler2D uFrontMask;

uniform bool uDarkOverlay;

uniform bool uIsDrinkingCat;
uniform sampler2D uBowlTexture;
uniform vec4 uCatBowlRect;
uniform float uCatHeadDrop;

uniform bool uIsBowl;
uniform float uFill;

uniform bool uRotateMilk;
uniform bool uIsStream;
uniform float uPourTime;

uniform bool uIsSolidColor;
uniform vec4 uSolidColor;

// Преобразование координат струи в координаты картинки миски
uniform vec4 uStreamBowlRect;

void main(){

    if (uIsSolidColor){ 
        outCol = uSolidColor;
        return;
    }

    if(uDarkOverlay){
        outCol = vec4(0.02,0.03,0.05,0.65);
        return;
    }

   if (uIsStream){
    float down = 1.0 - chTexCoord.y;

    // Небольшое движение струи сверху вниз.
    float center = 0.5 + 0.08 * down * sin(down * 16.0 - uPourTime * 18.0);
    float opacity = 1.0 - smoothstep(0.22, 0.48, abs(chTexCoord.x - center));

    vec2 bowlUV = uStreamBowlRect.xy + chTexCoord * uStreamBowlRect.zw;

    // Передняя стенка миски должна перекрывать струю.
    if (bowlUV.x >= 0.0 && bowlUV.x <= 1.0 && bowlUV.y >= 0.0 && bowlUV.y <= 1.0){
        vec2 frontUV = bowlUV * vec2(1.0, 1.087) + vec2(0.0, -0.142);

        float front = smoothstep(  0.05, 0.95, texture(uFrontMask, frontUV).a);
        opacity *= 1.0 - front;
    }

    outCol = vec4(0.96, 0.94, 0.87, opacity);
    return;
}

    vec2 textureUV = chTexCoord;

    if (uRotateMilk) textureUV = vec2(chTexCoord.y, 1.0 - chTexCoord.x);

    if (uIsDrinkingCat)
{
    // Правая верхняя часть картинки содержит голову.
    // Область лап остаётся неподвижной.
    float headWeight =
        smoothstep(0.50, 0.68, chTexCoord.x) *
        smoothstep(0.11, 0.16, chTexCoord.y);

    textureUV.y += uCatHeadDrop * headWeight;
}

vec4 base = texture(uTexture, textureUV);

if (uIsDrinkingCat)
{
    vec2 bowlUV =
        uCatBowlRect.xy + chTexCoord * uCatBowlRect.zw;

    if (bowlUV.x >= 0.0 && bowlUV.x <= 1.0 &&
        bowlUV.y >= 0.0 && bowlUV.y <= 1.0)
    {
        vec2 frontUV =
            bowlUV * vec2(1.0, 1.087) + vec2(0.0, -0.142);

        float front = smoothstep(
            0.05, 0.95, texture(uFrontMask, frontUV).a
        );

        float bowlAlpha = texture(uBowlTexture, bowlUV).a;
        base.a *= 1.0 - front * bowlAlpha;
    }

    outCol = base;
    return;
}

    if(!uIsBowl || uFill <= 0.0) {
        outCol = base;
        return;
    }

    //совмещаем координаты текстуры с маской интерьера миски
    vec2 interiorUV = chTexCoord*vec2(0.959, 1.15) + vec2(0.009, -0.099);
    vec2 frontUV = chTexCoord*vec2(1.0, 1.087) + vec2(0.0, -0.142);

    float inside = smoothstep(0.05,0.95, texture(uInteriorMask, interiorUV).a);
    float front = smoothstep(0.05,0.95, texture(uFrontMask, frontUV).a);

    float level = mix(0.45,0.72,clamp(uFill,0.0,1.0));
    float belowLevel = 1.0 - smoothstep(level,level+0.004,chTexCoord.y);
    float milkAmount = inside * (1.0 - front) * belowLevel;
    vec3 milkColor = vec3(0.96, 0.94, 0.87);
    outCol = vec4(mix(base.rgb,milkColor,milkAmount),base.a);


}
