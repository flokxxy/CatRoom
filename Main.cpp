
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <ft2build.h>
#include FT_FREETYPE_H

#include <iostream>
#include "Util.h"
#include <chrono>
#include <thread>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <map>
#include <string>
#include <vector>


struct FontGlyph {
    unsigned int texture = 0;
    int width = 0;
    int height = 0;
    int bearingX = 0; //межстрочный интервал
    int bearingY = 0; //межстрочный интервал
    float advance = 0.0f; //межбуквенный интервал
};
std::map<wchar_t, FontGlyph> fontGlyphs;
float fontLineHeight = 30.0f;
std::wstring currentHint;
float hintVisibleCharacters = 0.0f;
const float hintCharactersPerSecond = 25.0f;

const std::wstring angryCatHintText = L"Try petting the cat.\nMaybe he won't bite.";
const std::wstring mouseHintText = L"Did you see the mouse?";

float angryHintTimeLeft = 5.0f;
float mouseHintTimeLeft = 0.0f;
bool mouseHintTriggered = false;

const int WINDOW_WIDTH = 1280;
const int WINDOW_HEIGHT = 720;

float grabOffsetX = 0.0f;
float grabOffsetY = 0.0f;

GLFWcursor* cursorOpen = NULL;
GLFWcursor* cursorClosed = NULL;

bool roomLightOn = false;
const float lampX = 525.0f;
const float lampY = 65.0f;
const float lampWidth = 100.0f;
const float lampHeight = 70.0f;

const float lampSpriteX = 501.0f;
const float lampSpriteY = 55.0f;
const float lampSpriteWidth = 146.0f;
const float lampSpriteHeight = 88.0f;

bool catOnTable = false;
const float tableLeftX = 160.0f;
const float tableRightX = 190.0f;
const float tableY = 470.0f;
const float tableDropTolerance = 35.0f;
const float catFloorDropLeftX = 145.0f;
int furniturePixelWidth = 0;
int furniturePixelHeight = 0;
std::vector<unsigned char> furniturePixels;

const float bowlX = 420.0f;
const float bowlY = 590.0f;
const float bowlWidth = 165.0f;
const float bowlHeight = 108.0f;

// fregidr on background
const float fridgeX = 1012.0f;
const float fridgeY = 145.0f;   
const float fridgeWidth = 176.0f;
const float fridgeHeight = 450.0f;

const float closedScaleX = fridgeWidth / 540.0f;
const float openScaleX = fridgeWidth / 523.0f;
const float fridgeScaleY = fridgeHeight / 1527.0f;
const float closedSpriteX = fridgeX - 244.0f * closedScaleX;
const float openSpriteX = fridgeX - 231.0f * openScaleX;
const float fridgeSpriteY = fridgeY - 5.0f * fridgeScaleY;
const float closedSpriteWidth = 1025.0f * closedScaleX;
const float openSpriteWidth = 1024.0f * openScaleX;
const float closedSpriteHeight = 1535.0f * fridgeScaleY;
const float openSpriteHeight = 1536.0f * fridgeScaleY;

// milk
const float shelfMilkX = fridgeX + (358.0f - 231.0f) * openScaleX;
const float shelfMilkY = fridgeY + (375.0f - 5.0f) * fridgeScaleY;
const float milkSizeMultiplier = 1.2f;
const float milkWidth = 116.0f * openScaleX * milkSizeMultiplier;
const float milkHeight = 185.0f * fridgeScaleY * milkSizeMultiplier;
float milkX = shelfMilkX;
float milkY = shelfMilkY;

bool fridgeOpen = false;        
bool milkTaken = false;         
bool draggingMilk = false;      

bool pouringMilk = false; 
float bowlFill = 0.0f;   
float pourElapsed = 0.0f; 
const float streamFallTime = 0.2f; 

const float milkSpoutU = 0.20f;
const float milkSpoutV = 0.18f;

enum class MouseState {
    Hidden,
    Emerginig,
    Waching,
    Hiding
};
MouseState mouseState = MouseState::Hidden;
const float mouseWidth = 56.0f;
const float mouseHeight = 56.0f;

const float mouseEdgeX = fridgeX + fridgeWidth;
const float mouseHiddenX = mouseEdgeX - mouseWidth - 12.0f;
const float mousePeekX = mouseEdgeX - mouseWidth * 0.35f;
const float mouseSpriteY = fridgeY + fridgeHeight - mouseHeight - 8.0f;
const float mouseAppearSpeed = 110.0f;
const float mouseHideSpeed = 200.0f;
const float mouseWatchDuration = 5.8f;

float mouseSpriteX = mouseHiddenX;
float mouseTimer = 0.0f;


enum class CatState{
    WalkingAround, 
    WalkingToBowl,
    WaitingForMilk, 
	Drinking,
	Happy,
    Slepping
};
CatState catState = CatState::Slepping;
CatState catStateBeforeSleep = CatState::WalkingAround;

enum class CatReaction {
    None,
    Looking,
    Hissing,
    Biting,
    Petting
};
CatReaction catReaction = CatReaction::None;

const float catWidth = 310.0f;
const float catHeight = 310.0f;
const float catWalkSpeed = 100.0f;
const float catFastWalkSpeed = 200.f;
const float catWalkLeftX = 70.0f;
const float catWalkRightX = 700.0f;
const float catGroundY = 680.0f;
const float catIdleGroundV = 0.936204f;

bool draggingCat = false;
const float catHeldGrabU = 0.503987f;
const float catHeldGrabV = 0.453748f;
const float catHeldFeetV = 0.9f; //нижняя точка лап кота для соприкосновения с поверхностью
const float counterLeftX = 562.0f;
const float counterRightX = 600.0f;
const float counterY = 385.0f;
const float counterDropTolerance = 35.0f;

//шатания кота
float catSwayAngle = 0.0f;
float catSwayVelocity = 0.0f; //скорость изменения наклона
float catSwayPreviousX = 0.0f;

const float catSwaySensitivity = 0.0005f;
const float catSwayMaxAngle = 0.25f; //наклон
const float catSwaySpring = 36.0f;
const float catSwayDamping = 8.0f;


// Height adjustments due to different transparency fields in the cat's poses
const float catWalkRightOffsetY = (catIdleGroundV - 0.921053f) * catHeight;
const float catWalkLeftOffsetY = (catIdleGroundV - 0.925837f) * catHeight;
const float catHappyOffsetY = (catIdleGroundV - 0.937002f) * catHeight;
const float catStartX = 95.0f;
const float catStartY = catGroundY - catHeight * catIdleGroundV;

float catX = catStartX;
float catY = catStartY;
bool catFacingLeft = false;

const float catDrinkDuration = 3.5f; // time for drinking milk 
const float catDrinkGroundV = 0.919458f;
const float catDrinkMouthU = 0.750399f;
const float catDrinkMouthV = 0.737640f;
const float fullMilkSurfaceY = bowlY + bowlHeight * (1.0f - 0.72f);


//connect paws with the floor and the tip of the tongue with the surface of the milk
const float catDrinkHeight = (catGroundY - fullMilkSurfaceY) / (catDrinkGroundV - catDrinkMouthV);
const float catDrinkWidth = catDrinkHeight;
const float catDrinkX = bowlX + bowlWidth * 0.5f - catDrinkWidth * catDrinkMouthU;
const float catDrinkY = catGroundY - catDrinkHeight * catDrinkGroundV;

float catReactionTime = 0.0f;
const float catHissDuration = 0.6f; // time for hissing sound
const float catBiteDuration = 0.4f; // time for bite
const float catPetDuration = 1.0f; // time for love of cat
const float catLookDuration = 3.5f;

float catAutoReactionTimer = 0.0f;
bool catBiteAtherHiss = false;
const float catLookRightOffsetY = (catIdleGroundV - 0.936204f) * catHeight;
const float catLookLeftOffsetY = (catIdleGroundV - 0.937002f) * catHeight;

float catSleepTimer = 0.0f;
bool catSleepEffectVisible = true;
const float catSleepEffectInterval = 1.5f;
const float catSleepGroundV = 1207.0f / 1254.0f;
const float catSleepScale = 0.85f;
const float catSleepWidth = catWidth * catSleepScale;
const float catSleepHeight = catHeight * catSleepScale;
const float catSleepOffsetX = (catWidth - catSleepWidth) * 0.5f;
const float catSleepOffsetY = catHeight * catIdleGroundV - catSleepHeight * catSleepGroundV;



float getRandomCatDelay() {
    return 10.0f + static_cast<float>(std::rand() % 11);
}
float getRandomMouseDelay() {
    return 15.0f + static_cast<float>(std::rand() % 11);
}

int endProgram(const char* message)
{
    std::cout << message << std::endl;
    glfwTerminate();
    return -1;
}

// check cursor (left corner)
bool mouseInside(double mouseX, double mouseY, float x, float y, float width, float height){
    return mouseX >= x && mouseX <= x + width && mouseY >= y && mouseY <= y + height;
}

bool getMouseInScene(GLFWwindow* window, double& x, double& y){
    int width, height;
    glfwGetWindowSize(window, &width, &height);

    if (width <= 0 || height <= 0)
        return false;

    glfwGetCursorPos(window, &x, &y);

    x = x * WINDOW_WIDTH / width;
    y = y * WINDOW_HEIGHT / height;

    return true;
}

//fun for not copacity
void loadFurnitireMask(unsigned int texture) {
    glActiveTexture(GL_TEXTURE0);   
    glBindTexture(GL_TEXTURE_2D, texture);
    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &furniturePixelWidth);
    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &furniturePixelHeight);
    if (furniturePixelWidth <= 0 || furniturePixelHeight <= 0) return;

    furniturePixels.resize(static_cast<size_t>(furniturePixelWidth)*furniturePixelHeight*4);

    GLint previousAlignment;
    //выравнивание строк
    glGetIntegerv(GL_PACK_ALIGNMENT, &previousAlignment);
    glPixelStorei(GL_PACK_ALIGNMENT, 1);

    glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, furniturePixels.data());
    glPixelStorei(GL_PACK_ALIGNMENT, previousAlignment);
}

//check is an opaque piece of furniture
bool furnitureCoverPoint(double x, double y) {
    if (furniturePixels.empty() || x < 0.0 || y < 0.0 || x >= WINDOW_WIDTH || y >= WINDOW_HEIGHT) return false;

    int pixelX = static_cast<int>(x*furniturePixelWidth/WINDOW_WIDTH);
    int pixelY = furniturePixelHeight - 1 - static_cast<int>(y * furniturePixelHeight / WINDOW_HEIGHT);
    size_t alphaIndex = (static_cast<size_t>(pixelY)*furniturePixelWidth+pixelX) * 4 + 3;
    return furniturePixels[alphaIndex] > 128;
}

void resetMouse() {
    mouseState = MouseState::Hidden;
    mouseSpriteX = mouseHiddenX;
    mouseTimer = getRandomMouseDelay();
}

void updateMouse(GLFWwindow* window, float deltaTime) {
    if (fridgeOpen) return;
    if (mouseState == MouseState::Emerginig || mouseState == MouseState::Waching) {
        double cursorX, cursorY;

        if (getMouseInScene(window, cursorX, cursorY)) {
            float visibleLeft = mouseSpriteX;

            if (visibleLeft < mouseEdgeX) visibleLeft = mouseEdgeX;

            float visibleRight = mouseSpriteX + mouseWidth;
            float visibleWidth = visibleRight - visibleLeft;

            if (visibleWidth > 0.0f && mouseInside(cursorX, cursorY, visibleLeft, mouseSpriteY, visibleWidth, mouseHeight)) {
                mouseState = MouseState::Hiding;
            }
        }
    }
    switch (mouseState) {
    case MouseState::Hidden:
        mouseTimer -= deltaTime;
        if (mouseTimer <= 0.0f) mouseState = MouseState::Emerginig;
        break;

    case MouseState::Emerginig:
        mouseSpriteX += mouseAppearSpeed * deltaTime;
        if (mouseSpriteX >= mousePeekX) {
            mouseSpriteX = mousePeekX;
            mouseState = MouseState::Waching;
            mouseTimer = mouseWatchDuration;
        }
        break;

    case MouseState::Waching:
        mouseTimer -= deltaTime;
        if (mouseTimer <= 0.0f) mouseState = MouseState::Hiding;
        break;

    case MouseState::Hiding:
        mouseSpriteX -= mouseHideSpeed * deltaTime;
        if (mouseSpriteX <= mouseHiddenX) { 
            if (roomLightOn && !mouseHintTriggered) {
                mouseHintTimeLeft = 5.0f;
                mouseHintTriggered = true;
            }
            resetMouse(); 
        }
        break;

    }
}


void moveMilkToMouse(double mouseX, double mouseY){
    milkX = static_cast<float>(mouseX) - grabOffsetX;
    milkY = static_cast<float>(mouseY) - grabOffsetY;

    if (milkX < 0.0f) milkX = 0.0f;
    if (milkY < 0.0f) milkY = 0.0f;

    if (milkX > WINDOW_WIDTH - milkWidth)
        milkX = WINDOW_WIDTH - milkWidth;

    if (milkY > WINDOW_HEIGHT - milkHeight)
        milkY = WINDOW_HEIGHT - milkHeight;
}

void moveCatToMouse(double mouseX, double mouseY) {

    catX = static_cast<float>(mouseX) - catHeldGrabU * catWidth;
    catY = static_cast<float>(mouseY) - catHeldGrabV * catHeight;

    if (catX < 0.0f) catX = 0.0f;
    if (catY < 0.0f) catY = 0.0f;
    if (catX > WINDOW_WIDTH - catWidth) catX = WINDOW_WIDTH - catWidth;
    if (catY > WINDOW_HEIGHT - catHeight) catY = WINDOW_HEIGHT - catHeight;
}



void updateCatSway(float deltaTime){
    if (!draggingCat)
    {
        catSwayAngle = 0.0f;
        catSwayVelocity = 0.0f;
        catSwayPreviousX = catX;
        return;
    }

    if (deltaTime <= 0.0f)
    {
        catSwayPreviousX = catX;
        return;
    }

    float speedX = (catX - catSwayPreviousX) / deltaTime;
    catSwayPreviousX = catX;

    float targetAngle = -speedX * catSwaySensitivity;
    if (targetAngle > catSwayMaxAngle) targetAngle = catSwayMaxAngle;
    if (targetAngle < -catSwayMaxAngle) targetAngle = -catSwayMaxAngle;

    //Limit the animation step
    float simulationTime = deltaTime;
    if (simulationTime > 0.05f) simulationTime = 0.05f;

    float acceleration = (targetAngle - catSwayAngle) * catSwaySpring - catSwayVelocity * catSwayDamping;
    catSwayVelocity += acceleration * simulationTime;
    catSwayAngle += catSwayVelocity * simulationTime;

    if (catSwayAngle > catSwayMaxAngle){
        catSwayAngle = catSwayMaxAngle;
        catSwayVelocity = 0.0f;
    }
    else if (catSwayAngle < -catSwayMaxAngle){
        catSwayAngle = -catSwayMaxAngle;
        catSwayVelocity = 0.0f;
    }
}

void setRoomLight(GLFWwindow* window, bool lightOn) {
    if (roomLightOn == lightOn)
        return;

    if (!lightOn){
        catStateBeforeSleep = catState;
        catState = CatState::Slepping;

        catSleepTimer = 0.0f;
        catSleepEffectVisible = true;
    }
    else{
        catState = catStateBeforeSleep;
    }

    roomLightOn = lightOn;
}

void mouseCallback(GLFWwindow* window, int button, int action, int mods){
    if (button != GLFW_MOUSE_BUTTON_LEFT)
        return;

    if (action == GLFW_PRESS){
        glfwSetCursor(window, cursorClosed);

        double mouseX, mouseY;
        if (!getMouseInScene(window, mouseX, mouseY))
            return;
        if (mouseInside(mouseX, mouseY, lampX, lampY, lampWidth, lampHeight)) {
            setRoomLight(window, !roomLightOn);
            return;
        }
        if (!roomLightOn) return;
        if (draggingCat) return;

        bool catVisibleAtCursor = catOnTable || !furnitureCoverPoint(mouseX, mouseY);		
        bool milkOverBowl = milkTaken && mouseInside(
			milkX + milkWidth / 2.0f,
			milkY + milkHeight / 2.0f,
			bowlX, bowlY, bowlWidth, bowlHeight
        );

       
		if (catState != CatState::Drinking && catState != CatState::Happy && !pouringMilk 
            && !draggingMilk && milkOverBowl && bowlFill < 1.0f
            && mouseInside(mouseX, mouseY, bowlX, bowlY, bowlWidth, bowlHeight)){

			pouringMilk = true;
			pourElapsed = 0.0f;

            float spoutX = bowlX + bowlWidth / 2.0f;
            float spoutY = bowlY - 45.0f;

            milkX = spoutX - milkSpoutV * milkHeight;
            milkY = spoutY - (1.0f - milkSpoutU)*milkWidth;
			return;
		}

           
        // 
        if (!pouringMilk && milkTaken && mouseInside(mouseX, mouseY,
            milkX, milkY, milkWidth, milkHeight))
        {
            draggingMilk = true;

            grabOffsetX = static_cast<float>(mouseX) - milkX;
            grabOffsetY = static_cast<float>(mouseY) - milkY;
        }
        //take the bag from the shelf
        else if (fridgeOpen && !milkTaken && mouseInside(mouseX, mouseY, shelfMilkX, shelfMilkY, milkWidth, milkHeight))
        {
            milkTaken = true;
            draggingMilk = true;

            milkX = shelfMilkX;
            milkY = shelfMilkY;

            grabOffsetX = static_cast<float>(mouseX) - milkX;
            grabOffsetY = static_cast<float>(mouseY) - milkY;
        }
       
        else if (catState == CatState::Happy && !draggingMilk && catVisibleAtCursor && mouseInside(mouseX, mouseY,
            catX + catWidth * 0.48f - 16.0f, catY + catHappyOffsetY + catHeight * 0.53f - 16.0f, 32.0f, 32.0f)) {

            draggingCat = true;
            catReaction = CatReaction::None;
            catReactionTime = 0.0;

            moveCatToMouse(mouseX, mouseY);
            catSwayAngle = 0.0f;
            catSwayVelocity = 0.0f;
            catSwayPreviousX = catX;

            return;
        }
        else if ((catState != CatState::Drinking) && catVisibleAtCursor &&
            mouseInside(mouseX, mouseY, catX+catWidth*0.20f, catY+catHeight*0.12f, catWidth*0.60f, catHeight*0.86f) &&
            !mouseInside(mouseX,mouseY,bowlX,bowlY,bowlWidth,bowlHeight)) {
            if (catReaction != CatReaction::None && catReaction != CatReaction::Looking) return;
            if (catState != CatState::Happy) {
                catBiteAtherHiss = true;
				catReaction = CatReaction::Hissing;
				catReactionTime = catHissDuration;
            }
            else {
                catBiteAtherHiss = false;
                catReaction = CatReaction::Petting;
				catReactionTime = catPetDuration;
            }
            
            return;
        }

        // open or close frige.
        else if ( mouseInside(mouseX, mouseY, fridgeX, fridgeY, fridgeWidth, fridgeHeight) ||
            (fridgeOpen && mouseInside(mouseX, mouseY, fridgeX + fridgeWidth, fridgeY, 90.0f, 247.0f))){

            fridgeOpen = !fridgeOpen;
            if (fridgeOpen) resetMouse();
        }
    }
    else if (action == GLFW_RELEASE)
    {
        glfwSetCursor(window, cursorOpen);
        if (draggingCat) {
            double mouseX, mouseY;
            if (getMouseInScene(window, mouseX, mouseY)) moveCatToMouse(mouseX, mouseY);      
                        
            float pivotX = catX + catHeldGrabU * catWidth;
            float pivotY = catY + catHeldGrabV * catHeight;

            float dx = catWidth * (0.5f - catHeldGrabU);
            float dy = catHeight * (catHeldFeetV - catHeldGrabV);

            float c = std::cos(catSwayAngle);
            float s = std::sin(catSwayAngle);

            float supportX = pivotX + c * dx + s * dy;
            float feetY = pivotY - s * dx + c * dy;

            bool nearCounter = supportX >= counterLeftX - counterDropTolerance &&
                supportX <= counterRightX + counterDropTolerance &&
                feetY >= counterY - counterDropTolerance &&
                feetY <= counterY + counterDropTolerance;

            bool nearTable = supportX >= tableLeftX - tableDropTolerance &&
                supportX <= tableRightX + tableDropTolerance &&
                feetY >= tableY - tableDropTolerance &&
                feetY <= tableY + tableDropTolerance;

            if (nearTable)
            {
                if (supportX < tableLeftX) supportX = tableLeftX;
                if (supportX > tableRightX) supportX = tableRightX;

                catX = supportX - catWidth * 0.5f;
                catY = tableY - catHeight * catIdleGroundV;

                catOnTable = true;
            }
            else if (nearCounter)
            {
                if (supportX < counterLeftX) supportX = counterLeftX;
                if (supportX > counterRightX) supportX = counterRightX;
                catX = supportX - catWidth * 0.5f;
                catY = counterY - catHeight * catIdleGroundV;

                catOnTable = false;
            }
            else
            {
                catX = supportX - catWidth * 0.5f;
                catY = catStartY;

                if (catX < catFloorDropLeftX) catX = catFloorDropLeftX;
                if (catX > catWalkRightX) catX = catWalkRightX;

                catOnTable = false;
            }

            draggingCat = false;
            return;

        }
        if (!draggingMilk) return;    
        draggingMilk = false;

        double mouseX, mouseY;
        if (!getMouseInScene(window, mouseX, mouseY)) return;
        

        // exact position for take milk
        moveMilkToMouse(mouseX, mouseY);

        bool milkInsideFridge = mouseInside(
            milkX + milkWidth / 2.0f,
            milkY + milkHeight / 2.0f,
            fridgeX, fridgeY, fridgeWidth, fridgeHeight);

        if (milkInsideFridge)
        {
            milkTaken = false;
            milkX = shelfMilkX;
            milkY = shelfMilkY;
        }
    }
}

// Draw the sprite: its position and size are specified in pixels.
// Use the same VAO, VBO, and shader for all objects.
void drawSprite(unsigned int texture, float x, float y, float width, float height, int scaleLocation, int offsetLocation){
    float scaleX = width / WINDOW_WIDTH;
    float scaleY = height / WINDOW_HEIGHT;

    float offsetX = (2.0f * x + width) / WINDOW_WIDTH - 1.0f;
    float offsetY = 1.0f - (2.0f * y + height) / WINDOW_HEIGHT;

    glUniform2f(scaleLocation, scaleX, scaleY);
    glUniform2f(offsetLocation, offsetX, offsetY);
    glBindTexture(GL_TEXTURE_2D, texture);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
}

void freeFont() {
    for (const auto& entry : fontGlyphs) {
        if (entry.second.texture != 0) {
            glDeleteTextures(1, &entry.second.texture);
        }        
    }
    fontGlyphs.clear();
}
bool loadFont(const char* path, unsigned int pixelSize) {
    FT_Library library;
    if(FT_Init_FreeType(&library))
        return false;

    FT_Face face;
    if (FT_New_Face(library, path, 0, &face)) {
        FT_Done_FreeType(library);
        return false;
    }
    if (FT_Select_Charmap(face, FT_ENCODING_UNICODE) || FT_Set_Pixel_Sizes(face, 0, pixelSize)) {
        FT_Done_Face(face);
        FT_Done_FreeType(library);
        return false;
    }

    fontLineHeight = static_cast<float>(face->size->metrics.height) / 64.0f;
    GLint previousAlignment;
    glGetIntegerv(GL_UNPACK_ALIGNMENT, &previousAlignment);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    glActiveTexture(GL_TEXTURE0);

    for (FT_ULong code = 32; code <= 0x017F; ++code)
    {
        if (FT_Get_Char_Index(face, code) == 0)
            continue;

        if (FT_Load_Char(face, code, FT_LOAD_RENDER))
            continue;

        const FT_Bitmap& bitmap = face->glyph->bitmap;

        FontGlyph glyph;

        glyph.width = static_cast<int>(bitmap.width);
        glyph.height = static_cast<int>(bitmap.rows);

        glyph.bearingX = face->glyph->bitmap_left;
        glyph.bearingY = face->glyph->bitmap_top;

        glyph.advance = static_cast<float>(face->glyph->advance.x) / 64.0f;

        if (glyph.width > 0 && glyph.height > 0){
            if (bitmap.pixel_mode != FT_PIXEL_MODE_GRAY)
                continue;

            std::vector<unsigned char> pixels(
                static_cast<std::size_t>(glyph.width)
                * glyph.height * 4
            );

            for (int y = 0; y < glyph.height; ++y){
                int sourceOffset = bitmap.pitch >= 0 ? y * bitmap.pitch : (glyph.height - 1 - y) * (-bitmap.pitch);
                const unsigned char* row = bitmap.buffer + sourceOffset;

                for (int x = 0; x < glyph.width; ++x){
                    int index = ((glyph.height - 1 - y) * glyph.width + x) * 4;

                    pixels[index] = 245;
                    pixels[index + 1] = 235;
                    pixels[index + 2] = 210;
                    pixels[index + 3] = row[x];
                }
            }

            glGenTextures(1, &glyph.texture);
            glBindTexture(GL_TEXTURE_2D, glyph.texture);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, glyph.width, glyph.height, 0, GL_RGBA, GL_UNSIGNED_BYTE,pixels.data());

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        }

        fontGlyphs[static_cast<wchar_t>(code)] = glyph;
    }

    glPixelStorei(GL_UNPACK_ALIGNMENT, previousAlignment);

    FT_Done_Face(face);
    FT_Done_FreeType(library);

    return fontGlyphs.count(L'?') != 0;
}

void drawText(
    const std::wstring& text,
    float x,
    float baselineY,
    float scale,
    int scaleLocation,
    int offsetLocation)
{
    float penX = x;
    float penY = baselineY;

    glActiveTexture(GL_TEXTURE0);

    for (wchar_t character : text)
    {
        if (character == L'\n')
        {
            penX = x;
            penY += fontLineHeight * scale;
            continue;
        }

        auto it = fontGlyphs.find(character);

        if (it == fontGlyphs.end())
            it = fontGlyphs.find(L'?');

        if (it == fontGlyphs.end())
            continue;

        const FontGlyph& glyph = it->second;

        if (glyph.texture != 0)
        {
            drawSprite(
                glyph.texture,
                penX + glyph.bearingX * scale,
                penY - glyph.bearingY * scale,
                glyph.width * scale,
                glyph.height * scale,
                scaleLocation,
                offsetLocation
            );
        }

        penX += glyph.advance * scale;
    }
}

std::wstring getHintText()
{
    if (!roomLightOn)
        return L"Try to wake up the cat.";
    if (draggingCat)
        return L"Put the cat down first.";
    if (milkTaken && !draggingMilk && !pouringMilk)
        return L"Put the milk back!!";
    if (catState == CatState::Drinking)
        return L"The cat is drinking.";
    if (pouringMilk)
        return L"Milk is pouring - cool!";    
    if (mouseHintTimeLeft > 0.0f && !draggingMilk)
        return mouseHintText;
    if (catState == CatState::Happy)
        return L"Pet the cat!!!!";
    if (bowlFill >= 1.0f)
        return L"Filled the bowl?\nWhere's the cat?!";

    bool milkOverBowl = milkTaken && mouseInside(
        milkX + milkWidth * 0.5f,
        milkY + milkHeight * 0.5f,
        bowlX, bowlY, bowlWidth, bowlHeight
    );

    if (milkTaken && milkOverBowl)
        return L"Pour the milk!\nPour the milk!";

    if (milkTaken)
        return L"Try pouring some milk.";

    if (fridgeOpen)
        return L"Take the milk carton.";

    if (angryHintTimeLeft > 0.0f)
        return angryCatHintText;

    return L"Try opening the fridge!";
}

void updateHint(float deltaTime)
{
    std::wstring nextHint = getHintText();

    if (nextHint != currentHint)
    {
        currentHint = nextHint;
        hintVisibleCharacters = 0.0f;
    }

    hintVisibleCharacters += hintCharactersPerSecond * deltaTime;

    float totalCharacters = static_cast<float>(currentHint.size());

    if (hintVisibleCharacters > totalCharacters)
        hintVisibleCharacters = totalCharacters;

    if (roomLightOn) {
        if (currentHint == angryCatHintText && angryHintTimeLeft > 0.0f) {
            angryHintTimeLeft -= deltaTime;
        }
    }
    if (currentHint == mouseHintText && mouseHintTimeLeft > 0.0f) {
        mouseHintTimeLeft -= deltaTime;
    }
}


void updatePouringMilk(float deltaTime)
{
    if (!roomLightOn || !pouringMilk)
        return;

    //how long has the milk been pouring directly into the bowl
    float previousFillTime = pourElapsed - streamFallTime;
    if (previousFillTime < 0.0f)
        previousFillTime = 0.0f;

    pourElapsed += deltaTime;

    float currentFillTime = pourElapsed - streamFallTime;
    if (currentFillTime < 0.0f)
        currentFillTime = 0.0f;

    bowlFill += (currentFillTime - previousFillTime) / 3.0f;

    if (bowlFill >= 1.0f)
    {
        bowlFill = 1.0f;
        pouringMilk = false;

        //after pouring, place the bag next to the bowl again
        milkX = bowlX + bowlWidth + 10.0f;
        milkY = bowlY + bowlHeight - milkHeight;
    }
}

void updateCat(float deltaTime)
{

    if (catState == CatState::Slepping) {
        catSleepTimer += deltaTime;
        while (catSleepTimer >= catSleepEffectInterval) {
            catSleepTimer -= catSleepEffectInterval;
            catSleepEffectVisible = !catSleepEffectVisible;
        }
        return;
    }
	if (catReaction == CatReaction::Hissing || catReaction == CatReaction::Biting || catReaction == CatReaction::Looking) return;

    float currentCatSpeed = catWalkSpeed;
    if (bowlFill >= 1.0f && !pouringMilk)
    {
        currentCatSpeed = catFastWalkSpeed;
    }

    float step = currentCatSpeed * deltaTime;
    if (catState == CatState::WalkingAround) {
        if (!catFacingLeft) {
			catX += step;
            if (catX >= catWalkRightX) {
                catX = catWalkRightX;
				catFacingLeft = true; // chenge the direction of the cat
            }
        }
        else {
            catX -= step;
            if (catX <= catWalkLeftX) {
                catX = catWalkLeftX;
				catFacingLeft = false; 
                catState = CatState::WalkingToBowl;
            }
        }
        return;
    }
    if (catState == CatState::WalkingToBowl) {
        float targetX = bowlX - catWidth * 0.6f;
        
        if (catX < targetX)
        {
            catFacingLeft = false;
            catX += step;
            if (catX > targetX) catX = targetX;
        }
        else if (catX > targetX)
        {
            catFacingLeft = true;
            catX -= step;
            if (catX < targetX) catX = targetX;
        }

        if (catX == targetX) {
            catFacingLeft = false;
            if (bowlFill >= 1.0f && !pouringMilk) {
				catState = CatState::Drinking;
            }
            else {
				catState = CatState::WaitingForMilk;
            }

            return;
        }
    }

    if (catState == CatState::WaitingForMilk) {
        if (bowlFill >= 1.0f && !pouringMilk) {
			catState = CatState::Drinking;
        }
        return;
    }

    if (catState == CatState::Drinking) {
		bowlFill -= deltaTime / catDrinkDuration;
		if (bowlFill <= 0.0f) {
			bowlFill = 0.0f;
			catState = CatState::Happy;
		}
    }
}

void updateCatReaction(float deltaTime)
{
    if (catState == CatState::Slepping) return;


    if (catState == CatState::Drinking) {
        catReaction = CatReaction::None;
        catReactionTime = 0.0f; 
        catBiteAtherHiss = false;
        return;
    }

    if (catReaction == CatReaction::None) {
        
        if (catState == CatState::Happy) return;
        catAutoReactionTimer -= deltaTime;
        if (catAutoReactionTimer > 0.0f) return;
        catBiteAtherHiss = false;
        if (std::rand() % 2 == 0) {
            catReaction = CatReaction::Looking;
            catReactionTime = catLookDuration;
        }
        else {
            catReaction = CatReaction::Hissing;
            catReactionTime = catHissDuration;
        }
        return;
    }
    
    catReactionTime -= deltaTime;
    if (catReactionTime > 0.0f) return;

    if (catReaction == CatReaction::Hissing && catBiteAtherHiss) {
		catReaction = CatReaction::Biting;
        catReactionTime += catBiteDuration;
    }
    if (catReactionTime <= 0.0f) {
		catReaction = CatReaction::None;
		catReactionTime = 0.0f;
        catBiteAtherHiss = false;
        catAutoReactionTimer = getRandomCatDelay();
    }
}

//handling window resizing
void framebufferSizeCallback(GLFWwindow* window, int width, int height){
    glViewport(0, 0, width, height);
}


int main()
{
    //create windiow    
    if (!glfwInit()) return endProgram("GLFW init failed.");

    std::srand(static_cast<unsigned int>(std::time(nullptr)));
    catAutoReactionTimer = getRandomCatDelay();
    resetMouse();

    //фулл экран 
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
   

	//get the main monitor and its video mode to create a window with the correct resolution
	GLFWmonitor* monitor = glfwGetPrimaryMonitor();
   
	if (monitor == NULL) return endProgram("Cannot find monitor.");
	//current monitor resolution
	const GLFWvidmode* mode = glfwGetVideoMode(monitor);
    if (mode == NULL) return endProgram("Cannot get video mode.");
    

    //окно
    /*
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
    glfwWindowHint(GLFW_MAXIMIZED, GLFW_TRUE);

    GLFWwindow* window = glfwCreateWindow(
        WINDOW_WIDTH, WINDOW_HEIGHT,
        "Cat Room",
        NULL, NULL
    );*/
    

	//create a full-size window with the monitor's resolution (for full-size)
    GLFWwindow* window = glfwCreateWindow(mode->width, mode->height, "Cat Room", monitor, NULL);
    if (window == NULL) return endProgram("Cannot create window.");
    glfwMakeContextCurrent(window);

    //glfwMakeContextCurrent(window);
	//disable vertical sync to avoid limiting FPS
	glfwSwapInterval(0); // vsync

    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) return endProgram("GLEW init failed.");

    //frame dimensions
	int framebufferWidth, framebufferHeight;
	glfwGetFramebufferSize(window, &framebufferWidth, &framebufferHeight);
    glViewport(0, 0, framebufferWidth, framebufferHeight); //the area in which the image will be displayed

    //connecting a callback to a window
    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);

    // allowing PNG transparency
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    //shejders
    unsigned int basicShader = createShader("basic.vert", "basic.frag");

    
    float vertices[] = {
        // X      Y       U      V
        -1.0f, -1.0f,  0.0f, 0.0f,
         1.0f, -1.0f,  1.0f, 0.0f,
         1.0f,  1.0f,  1.0f, 1.0f,
        -1.0f,  1.0f,  0.0f, 1.0f
    };

    unsigned int VAO, VBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER,sizeof(vertices),vertices,GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE,4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer( 1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float),(void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    unsigned int backgroundTexture = loadImageToTexture("res/kitchen_background_clean.png");

	unsigned int lampOnTexture = loadImageToTexture("res/lamp/lamp_on.png");
	unsigned int lampOffTexture = loadImageToTexture("res/lamp/lamp_off.png");
    unsigned int tableChairTexture = loadImageToTexture("res/items/table_chair_foreground.png");
   
    unsigned int catTexture = loadImageToTexture("res/cat/set_v2/cat_idle.png");
    unsigned int catWalkRightTexture = loadImageToTexture("res/cat/set_v2/cat_walk_right.png");
    unsigned int catWalkLeftTexture = loadImageToTexture("res/cat/set_v2/cat_walk_left.png");
    unsigned int catDrinkTexture = loadImageToTexture("res/cat/set_v2/cat_drink.png");
    unsigned int catHappyTexture = loadImageToTexture("res/cat/set_v2/cat_happy.png");
    unsigned int catHissTexture = loadImageToTexture("res/cat/set_v2/cat_hiss.png");
    unsigned int catBiteTexture = loadImageToTexture("res/cat/set_v2/cat_bite.png");
    unsigned int catHeldTexture = loadImageToTexture("res/cat/set_v2/cat_held_scruff.png");
    unsigned int catLookRightTexture = loadImageToTexture("res/cat/set_v2/cat_look_right.png");
    unsigned int catLookLeftTexture = loadImageToTexture("res/cat/set_v2/cat_look_left.png");

	unsigned int heartTexture = loadImageToTexture("res/effects/effect_heart.png");
    unsigned int angerTexture =loadImageToTexture("res/effects/effect_anger.png");


    unsigned int bowlTexture = loadImageToTexture("res/bowl/bowl_empty.png");
    unsigned int bowlHalfTexture = loadImageToTexture("res/bowl/bowl_half.png");
    unsigned int bowlFullTexture = loadImageToTexture("res/bowl/bowl_full.png");

    unsigned int bowlInteriorMaskTexture = loadImageToTexture("res/bowl/bowl_interior_mask.png");
    unsigned int bowlFrontMaskTexture = loadImageToTexture("res/bowl/bowl_front_mask.png");

    unsigned int fridgeClosedTexture = loadImageToTexture("res/fridge/fridge_closed.png");
    unsigned int fridgeOpenTexture = loadImageToTexture("res/fridge/fridge_open_matched_empty.png");
    unsigned int milkTexture = loadImageToTexture("res/items/milk_pack.png");
    unsigned int mouseTexture = loadImageToTexture("res/items/mouse.png");
    unsigned int catSleepTexture = loadImageToTexture("res/cat/set_v2/cat_sleep.png");
    unsigned int sleepEffectTexture = loadImageToTexture("res/effects/effect_sleep.png");

    if ( backgroundTexture == 0 || lampOnTexture == 0 || lampOffTexture == 0 || tableChairTexture == 0 || mouseTexture == 0 || catSleepTexture == 0||sleepEffectTexture == 0
        || catTexture == 0 || catWalkRightTexture == 0 || catWalkLeftTexture == 0
        || catDrinkTexture == 0 || catHappyTexture == 0 || catHissTexture == 0
        || catBiteTexture == 0 || catHeldTexture == 0 || catLookRightTexture == 0 || catLookLeftTexture == 0
        || angerTexture == 0 || heartTexture == 0 || bowlTexture == 0 || bowlHalfTexture == 0 || bowlFullTexture == 0 || fridgeClosedTexture == 0
        || fridgeOpenTexture == 0 || milkTexture == 0 || bowlInteriorMaskTexture == 0 || bowlFrontMaskTexture == 0){

        if (backgroundTexture) glDeleteTextures(1, &backgroundTexture);
        if (lampOnTexture) glDeleteTextures(1, &lampOnTexture);
        if (lampOffTexture) glDeleteTextures(1, &lampOffTexture);
        if (tableChairTexture) glDeleteTextures(1, &tableChairTexture);
        if (mouseTexture) glDeleteTextures(1, &mouseTexture);
        if (catTexture) glDeleteTextures(1, &catTexture);
        if (catWalkRightTexture) glDeleteTextures(1, &catWalkRightTexture);
        if (catWalkLeftTexture) glDeleteTextures(1, &catWalkLeftTexture);
        if (catDrinkTexture) glDeleteTextures(1, &catDrinkTexture);
        if (catHappyTexture) glDeleteTextures(1, &catHappyTexture);
        if (catHissTexture) glDeleteTextures(1, &catHissTexture);
        if (catBiteTexture) glDeleteTextures(1, &catBiteTexture);
        if (catHeldTexture) glDeleteTextures(1, &catHeldTexture);
        if (catLookRightTexture) glDeleteTextures(1, &catLookRightTexture);
        if (catLookLeftTexture) glDeleteTextures(1, &catLookLeftTexture);
        if (heartTexture) glDeleteTextures(1, &heartTexture);
        if (angerTexture) glDeleteTextures(1, &angerTexture);
        if (bowlTexture) glDeleteTextures(1, &bowlTexture);
        if (bowlHalfTexture) glDeleteTextures(1, &bowlHalfTexture);
        if (bowlFullTexture) glDeleteTextures(1, &bowlFullTexture);
        if (bowlInteriorMaskTexture) glDeleteTextures(1, &bowlInteriorMaskTexture);
        if (bowlFrontMaskTexture) glDeleteTextures(1, &bowlFrontMaskTexture);
        if (fridgeClosedTexture) glDeleteTextures(1, &fridgeClosedTexture);
        if (fridgeOpenTexture) glDeleteTextures(1, &fridgeOpenTexture);
        if (milkTexture) glDeleteTextures(1, &milkTexture);
        if (catSleepTexture) glDeleteTextures(1, &catSleepTexture);
        if (sleepEffectTexture) glDeleteTextures(1, &sleepEffectTexture);

        glDeleteBuffers(1, &VBO);
        glDeleteVertexArrays(1, &VAO);
        glDeleteProgram(basicShader);

        return endProgram("Cannot load textures.");
    }

    loadFurnitireMask(tableChairTexture);

    cursorOpen = loadImageToCursor("res/cursors/hand_open_48.png");
    cursorClosed = loadImageToCursor("res/cursors/hand_closed_48.png");

	if (cursorOpen == NULL || cursorClosed == NULL) return endProgram("Cannot load cursor textures.");
    glfwSetCursor(window, cursorOpen);

	glfwSetMouseButtonCallback(window, mouseCallback);

    

    unsigned int textures[] = {
        backgroundTexture, 
        lampOnTexture,
		lampOffTexture,
        tableChairTexture,
        catTexture,
        catWalkRightTexture,
        catWalkLeftTexture,
        catDrinkTexture,
        catHappyTexture,
        catHissTexture,
        catBiteTexture,
        catHeldTexture,
        catLookRightTexture,
        catLookLeftTexture,
        angerTexture,
        heartTexture,
        bowlTexture, 
        fridgeClosedTexture, 
        fridgeOpenTexture, 
        milkTexture,
        mouseTexture,
        bowlHalfTexture,
        bowlFullTexture,
        bowlInteriorMaskTexture,
        bowlFrontMaskTexture,
        catSleepTexture,
        sleepEffectTexture,
    };



    for (unsigned int texture : textures)
    {
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    }

    if (!loadFont("res/fonts/courbd.ttf", 24)) {
        freeFont();
        return endProgram("cannot load font");
    }
    // get uniform-variables
    glUseProgram(basicShader);

    glUniform1i(glGetUniformLocation(basicShader, "uTexture"), 0);
    int scaleLocation = glGetUniformLocation(basicShader, "uScale");
    int offsetLocation = glGetUniformLocation(basicShader, "uOffset");

    int heldCatLocation = glGetUniformLocation(basicShader, "uIsHeldCat");
    int catAngleLocation = glGetUniformLocation(basicShader, "uCatAngle");
    int catPivotLocation = glGetUniformLocation(basicShader, "uCatPivot");
    int sceneSizeLocation = glGetUniformLocation(basicShader, "uSceneSize");
    glUniform1i(heldCatLocation, 0);
    glUniform1f(catAngleLocation, 0.0f);
    glUniform2f(catPivotLocation, catHeldGrabU, catHeldGrabV);
    glUniform2f(sceneSizeLocation,static_cast<float>(WINDOW_WIDTH), static_cast<float>(WINDOW_HEIGHT)); //за что это отвечает 


    int solidModeLocation = glGetUniformLocation(basicShader, "uIsSolidColor");
    int solidColorLocation = glGetUniformLocation(basicShader, "uSolidColor");
    glUniform1i(solidModeLocation, 0);

    int darkOverlayLocation = glGetUniformLocation(basicShader, "uDarkOverlay");
    glUniform1i(darkOverlayLocation, 0);

	glUniform1i(glGetUniformLocation(basicShader, "uInteriorMask"), 1);
	glUniform1i(glGetUniformLocation(basicShader, "uFrontMask"), 2);

	int bowlModeLocation = glGetUniformLocation(basicShader, "uIsBowl");
	int bowlFillLocation = glGetUniformLocation(basicShader, "uFill");
	glUniform1i(bowlModeLocation, 0);

    int rotateMilkLocation = glGetUniformLocation(basicShader, "uRotateMilk");
    int streamModeLocation = glGetUniformLocation(basicShader, "uIsStream");
    int pourTimeLocation = glGetUniformLocation(basicShader, "uPourTime");
    int streamBowlRectLocation = glGetUniformLocation(basicShader, "uStreamBowlRect");

	int drinkingCatLocation = glGetUniformLocation(basicShader, "uIsDrinkingCat");
	int catBowlRectLocation = glGetUniformLocation(basicShader, "uCatBowlRect");
	int catHeadDropLocation = glGetUniformLocation(basicShader, "uCatHeadDrop");
	glUniform1i(glGetUniformLocation(basicShader, "uBowlTexture"), 3);
	glUniform1i(drinkingCatLocation, 0);

    glUniform1i(rotateMilkLocation, 0);
    glUniform1i(streamModeLocation, 0);
    glUniform1i(drinkingCatLocation, 0);
    

    //stationary empty bowl 
    float grabOffsetX = 0.0f;
    float grabOffsetY = 0.0f;

    const float catSpeed = 250.0f;
    double previousTime = glfwGetTime();
    const double targetFPS = 75.0;
	const double targetFrameTime = 1.0 / targetFPS;

   

    while (!glfwWindowShouldClose(window))
    {
		double frameStartTime = glfwGetTime();
        glfwPollEvents();

        double currentTime = glfwGetTime();
        float deltaTime = static_cast<float>(currentTime - previousTime);

        previousTime = currentTime;

        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) glfwSetWindowShouldClose(window, GLFW_TRUE);

            
        if (draggingMilk) {
            double mouseX, mouseY;

            if (getMouseInScene(window, mouseX, mouseY))
                moveMilkToMouse(mouseX, mouseY);
        }
        else if (draggingCat) {
            double mouseX, mouseY;
            if (getMouseInScene(window, mouseX, mouseY)) moveCatToMouse(mouseX, mouseY);
        }


        
        // R
        if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS)
        {
            roomLightOn = false;

            fridgeOpen = false;
            resetMouse();

            milkTaken = false;
            draggingMilk = false;
            milkX = shelfMilkX;
            milkY = shelfMilkY;

            bowlFill = 0.0f;
            pourElapsed = 0.0f;
            pouringMilk = false;

            catState = CatState::Slepping;
            catStateBeforeSleep = CatState::WalkingAround;

            catX = catStartX;
            catY = catStartY;
            catFacingLeft = false;
            draggingCat = false;
            catOnTable = false;

            catReaction = CatReaction::None;
            catReactionTime = 0.0f;
            catBiteAtherHiss = false;
            catAutoReactionTimer = getRandomCatDelay();

            catSleepTimer = 0.0f;
            catSleepEffectVisible = true;

            catSwayAngle = 0.0f;
            catSwayVelocity = 0.0f;
            catSwayPreviousX = catX;

            currentHint.clear();
            hintVisibleCharacters = 0.0f;
            angryHintTimeLeft = 5.0f;
            mouseHintTimeLeft = 0.0f;
            mouseHintTriggered = false;

            glfwSetCursor(window, cursorOpen);
        }
        
		updatePouringMilk(deltaTime);
		updateCat(deltaTime);
		updateCatReaction(deltaTime);
        updateCatSway(deltaTime);
        updateMouse(window, deltaTime);
        updateHint(deltaTime);


        glClearColor(0.85f, 0.80f, 0.73f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(basicShader);
        glUniform1i(darkOverlayLocation, 0);
        glUniform1i(bowlModeLocation, 0);
        glActiveTexture(GL_TEXTURE0);
        glBindVertexArray(VAO);

        // draw the background
        glUniform2f(scaleLocation, 1.0f, 1.0f);
        glUniform2f(offsetLocation, 0.0f, 0.0f);

        glBindTexture(GL_TEXTURE_2D, backgroundTexture);
        glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

		unsigned int lampTexture = roomLightOn ? lampOnTexture : lampOffTexture;
		drawSprite(lampTexture, lampSpriteX, lampSpriteY, lampSpriteWidth, lampSpriteHeight, scaleLocation, offsetLocation);

        if (catOnTable) {
            glActiveTexture(GL_TEXTURE0);
            drawSprite(tableChairTexture, 0.0f, 0.0f, WINDOW_WIDTH, WINDOW_HEIGHT, scaleLocation, offsetLocation);
        }

        if (catState != CatState::Drinking && !draggingCat) {
            unsigned int currentCatTexture = catTexture;

            float currentCatX = catX;
            float currentCatY = catY;
            float currentCatWidth = catWidth;
            float currentCatHeight = catHeight;

            if (catState == CatState::WalkingAround || catState == CatState::WalkingToBowl){
                   if (catFacingLeft){
                        currentCatTexture = catWalkLeftTexture;
                        currentCatY += catWalkLeftOffsetY;
                   }
                   else{
                        currentCatTexture = catWalkRightTexture;
                        currentCatY += catWalkRightOffsetY;
                   }
            }
            else if (catState == CatState::Happy){
                currentCatTexture = catHappyTexture;
                currentCatY += catHappyOffsetY;
            }

            if (catReaction == CatReaction::Looking){
                if (catReactionTime > catLookDuration * 0.5f){
                    currentCatTexture = catLookRightTexture;
                    currentCatY = catY + catLookRightOffsetY;
                }
                else{
                    currentCatTexture = catLookLeftTexture;
                    currentCatY = catY + catLookLeftOffsetY;
                }
            }
            else if (catReaction == CatReaction::Hissing){
                currentCatTexture = catHissTexture;
                currentCatY = catY;
            }
            else if (catReaction == CatReaction::Biting){
                currentCatTexture = catBiteTexture;
                currentCatY = catY;
            }

            if (catState == CatState::Slepping) {
                currentCatTexture = catSleepTexture;
                currentCatY = catY + catSleepOffsetY;
                currentCatX = catX + catSleepOffsetX;
                currentCatWidth = catSleepWidth;
                currentCatHeight = catSleepHeight;
            }
            drawSprite(currentCatTexture, currentCatX, currentCatY, currentCatWidth, currentCatHeight, scaleLocation, offsetLocation);
        }

        if (catState == CatState::Slepping && catSleepEffectVisible)
        {
            const float sleepEffectSize = 72.0f;

            float sleepEffectX = catX + catSleepOffsetX+ catSleepWidth * 0.30f;
            float sleepEffectY = catY + catSleepOffsetY + catSleepHeight * 0.40f - sleepEffectSize;
            glActiveTexture(GL_TEXTURE0);

            drawSprite(
                sleepEffectTexture,
                sleepEffectX,
                sleepEffectY,
                sleepEffectSize,
                sleepEffectSize,
                scaleLocation,
                offsetLocation
            );
        }


        if (!catOnTable) {
            glActiveTexture(GL_TEXTURE0);
            drawSprite(tableChairTexture, 0.0f, 0.0f, WINDOW_WIDTH, WINDOW_HEIGHT, scaleLocation, offsetLocation);
        }


		if (!draggingCat && catState == CatState::Happy && catReaction == CatReaction::Petting) {

			float progress = 1.0f - catReactionTime / catPetDuration;
			const float heartSize = 52.0f;
			float heartX = catX + catWidth * 0.5f - heartSize * 0.5f;
			float heartY = catY - heartSize * 0.25f - progress * 20.0f; 
			drawSprite(heartTexture, heartX, heartY, heartSize, heartSize, scaleLocation, offsetLocation);
		}

        if (catState !=CatState::Slepping && (catReaction == CatReaction::Hissing || catReaction == CatReaction::Biting)) {
            const float angerSize = 52.0f;
            float angerX = catX + catWidth * 0.5f - angerSize * 0.5f;
            float angerY = catY + catHeight * 0.1f - angerSize;
            drawSprite(angerTexture, angerX, angerY, angerSize, angerSize,scaleLocation, offsetLocation);
        }
        
        
        //bowl is on the floor and is drawn in front of the cat when the sprites overlap
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, bowlInteriorMaskTexture);

        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, bowlFrontMaskTexture);

       
        glActiveTexture(GL_TEXTURE0);

        glUniform1i(bowlModeLocation, 1);
        glUniform1f(bowlFillLocation, bowlFill);

        // drawSprite binds the main image to the active block
        drawSprite(bowlTexture, bowlX, bowlY, bowlWidth, bowlHeight, scaleLocation, offsetLocation);        
        glUniform1i(bowlModeLocation, 0);



        if (catState == CatState::Drinking)
        {
            //original bowl is needed to check its transparent fields.
            glActiveTexture(GL_TEXTURE3);
            glBindTexture(GL_TEXTURE_2D, bowlTexture);
            glActiveTexture(GL_TEXTURE0);

            glUniform1i(drinkingCatLocation, 1);

            glUniform4f(
                catBowlRectLocation,
                (catDrinkX - bowlX) / bowlWidth,
                (bowlY + bowlHeight - catDrinkY - catDrinkHeight) / bowlHeight,
                catDrinkWidth / bowlWidth,
                catDrinkHeight / bowlHeight
            );

            float headDropPixels = (1.0f - bowlFill) * (0.72f - 0.45f) * bowlHeight;
            glUniform1f( catHeadDropLocation, headDropPixels / catDrinkHeight);
            drawSprite(catDrinkTexture, catDrinkX, catDrinkY, catDrinkWidth, catDrinkHeight, scaleLocation, offsetLocation);
            glUniform1i(drinkingCatLocation, 0);
        }

        //mouse is frond of fridge
        if (!fridgeOpen && mouseState != MouseState::Hidden) {
            glActiveTexture(GL_TEXTURE0);
            drawSprite(mouseTexture, mouseSpriteX + mouseWidth, mouseSpriteY, -mouseWidth, mouseHeight, scaleLocation, offsetLocation);
        }

        // Рисуем холодильник
        if (fridgeOpen){
            drawSprite(fridgeOpenTexture, openSpriteX, fridgeSpriteY, openSpriteWidth, openSpriteHeight,scaleLocation, offsetLocation);
        }
        else{
            drawSprite(fridgeClosedTexture,  closedSpriteX, fridgeSpriteY, closedSpriteWidth, closedSpriteHeight, scaleLocation, offsetLocation);
        }

		//draw a stream of milk if the bag is above the bowl and the pouring has begun
        if (pouringMilk && roomLightOn)
        {
            //the spout of a turned package
            float spoutX = milkX + milkSpoutV * milkHeight;
            float spoutY = milkY + (1.0f - milkSpoutU) * milkWidth;

            //the same surface height as used in basic.frag
            float level = 0.45f + (0.72f - 0.45f) * bowlFill;
            float surfaceY = bowlY + (1.0f - level) * bowlHeight;

            float progress = pourElapsed / streamFallTime;
            if (progress > 1.0f)
                progress = 1.0f;

            float streamWidth = 4.0f;
            float streamHeight = (surfaceY - spoutY) * progress;
            float streamX = spoutX - streamWidth / 2.0f;

            if (streamHeight > 0.0f)
            {
                glUniform1i(streamModeLocation, 1);
                glUniform1f(pourTimeLocation, pourElapsed);

                glUniform4f(
                    streamBowlRectLocation,
                    (streamX - bowlX) / bowlWidth,
                    (bowlY + bowlHeight - spoutY - streamHeight) / bowlHeight,
                    streamWidth / bowlWidth,
                    streamHeight / bowlHeight
                );

                // milk sprites
                drawSprite(milkTexture, streamX, spoutY, streamWidth, streamHeight, scaleLocation, offsetLocation);
                glUniform1i(streamModeLocation, 0);
            }
        }

        //milk is visible on the shelf of an open refrigerator or after removal
        if (fridgeOpen || milkTaken)
        {
            glUniform1i(rotateMilkLocation, pouringMilk ? 1 : 0);

            drawSprite( milkTexture, milkX, milkY, pouringMilk ? milkHeight : milkWidth,pouringMilk ? milkWidth : milkHeight,scaleLocation, offsetLocation);
            glUniform1i(rotateMilkLocation, 0);
        }


        if (draggingCat) {
            glActiveTexture(GL_TEXTURE0);

            glUniform1i(heldCatLocation, 1);
            glUniform1f(catAngleLocation,catSwayAngle);

            drawSprite(catHeldTexture,catX, catY, catWidth, catHeight, scaleLocation, offsetLocation);
            glUniform1i(heldCatLocation, 0);
        }

        
		// darck overlay
        if (!roomLightOn) {
			glUniform1i(darkOverlayLocation, 1);
			
            glUniform2f(scaleLocation,1.0f,1.0f);
			glUniform2f(offsetLocation, 0.0f, 0.0f);

            glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
            glUniform1i(darkOverlayLocation, 0);
        }
               


        float infoX = 20.0f;
        float infoY = 20.0f;
        float infoWidth = 360.0f;
        float infoHeight = 90.0f;

        glUniform1i(solidModeLocation, 1);

        // darck 60%.
        glUniform4f(solidColorLocation, 0.1f, 0.1f, 0.1f, 0.6f);
        glUniform2f(scaleLocation, infoWidth / WINDOW_WIDTH,infoHeight / WINDOW_HEIGHT);

        glUniform2f(offsetLocation, (2.0f * infoX + infoWidth) / WINDOW_WIDTH - 1.0f, 1.0f - (2.0f * infoY + infoHeight) / WINDOW_HEIGHT);
        glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

        glUniform1i(solidModeLocation, 0);

        //background for hints
        glUniform1i(solidModeLocation, 1);
        glUniform4f(solidColorLocation, 0.08f, 0.07f, 0.06f, 0.75f);
        drawSprite(backgroundTexture, 760.0f, 20.0f, 500.0f, 90.0f, scaleLocation,offsetLocation);
        glUniform1i(solidModeLocation, 0);

        std::wstring visibleHint = currentHint.substr(0, static_cast<std::size_t>(hintVisibleCharacters));
        
        drawText(visibleHint, 780.0f, 50.0f, 1.0f, scaleLocation,offsetLocation);
        drawText(
            L"Viktorija Avanesova\nINDEX: SV 87/2024",
            infoX + 14.0f,
            infoY + 30.0f,
            0.85f,
            scaleLocation, offsetLocation
        );

        glfwSwapBuffers(window);

		double frameTime = glfwGetTime() - frameStartTime;
		double remainingTime = targetFrameTime - frameTime;
       
        if (remainingTime > 0.0) {
			std::this_thread::sleep_for(std::chrono::duration<double>(remainingTime));
        }
    }

    freeFont();
	glDeleteTextures(1, &lampOnTexture);
	glDeleteTextures(1, &lampOffTexture);
    glDeleteTextures(1, &tableChairTexture);
    glDeleteTextures(1, &catTexture);
    glDeleteTextures(1, &catWalkRightTexture);
    glDeleteTextures(1, &catWalkLeftTexture);
    glDeleteTextures(1, &catDrinkTexture);
    glDeleteTextures(1, &catHappyTexture);
    glDeleteTextures(1, &catHissTexture);
    glDeleteTextures(1, &catBiteTexture);
    glDeleteTextures(1, &catHeldTexture);
    glDeleteTextures(1, &bowlTexture);
    glDeleteTextures(1, &backgroundTexture);
    glDeleteTextures(1, &fridgeClosedTexture);
    glDeleteTextures(1, &fridgeOpenTexture);
    glDeleteTextures(1, &milkTexture);
    glDeleteTextures(1, &bowlHalfTexture);
    glDeleteTextures(1, &bowlFullTexture);
    glDeleteTextures(1, &bowlInteriorMaskTexture);
    glDeleteTextures(1, &bowlFrontMaskTexture);
	glDeleteTextures(1, &heartTexture);
    glDeleteTextures(1, &catLookLeftTexture);
    glDeleteTextures(1, &catLookRightTexture);
    glDeleteTextures(1, &angerTexture);
    glDeleteTextures(1, &mouseTexture);
    glDeleteTextures(1, &catSleepTexture);
    glDeleteTextures(1, &sleepEffectTexture);

    glDeleteBuffers(1, &VBO);
    glDeleteVertexArrays(1, &VAO);
    glDeleteProgram(basicShader);

    glfwDestroyCursor(cursorOpen);
    glfwDestroyCursor(cursorClosed);


    glfwTerminate();
    return 0;
}
