#ifndef _DISP_TEXTURE_H_
#define _DISP_TEXTURE_H_

#include <SDL.h>
#include <SDL_ttf.h>
#include <string>

#include "os_type.h"

//Texture wrapper class
class DispTexture
{
public:
    //Initializes variables
    DispTexture();

    //Deallocates memory
    ~DispTexture();

    UINT32 openWindow();
    void closeWindow();

    //Loads image at specified path
    UINT32 createTextureFromFile(std::string path);

    //Creates image from font string
    UINT32 createTextureFromRenderText(std::string textureText, SDL_Color textColor);

    //Creates blank texture
    UINT32 createTexture(INT32 width, INT32 height, SDL_TextureAccess access);

    //Deallocates texture
    void freeTexture();

    //Clear window
    void clearScreen();

    //update screen
    void updateScreen();

    //Set color modulation
    void setColor(UINT8 red, UINT8 green, UINT8 blue);

    //Set blending
    void setBlendMode(SDL_BlendMode blending);

    //Set alpha modulation
    void setAlpha(UINT8 alpha);

    //Renders texture at given point
    void render(INT32 x, INT32 y, SDL_Rect* clip = NULL, double angle = 0.0, SDL_Point* center = NULL, SDL_RendererFlip flip = SDL_FLIP_NONE);

    //Set self as render target
    void setAsRenderTarget();

    //Gets image dimensions
    INT32 getWidth();
    INT32 getHeight();

    //Pixel manipulators
    UINT32 lockTexture();
    UINT32 unlockTexture();
    void* getPixels();
    void copyPixels(void* pixels);
    INT32 getPitch();
    INT32 getPixel32(INT32 x, INT32 y);

private:
    //The actual hardware texture
    SDL_Window* window_;
    SDL_Renderer* renderer_;
    SDL_Texture* texture_;
    TTF_Font* font_;

    void* pixels_;
    INT32 pitch_;

    //Image dimensions
    INT32 width_;
    INT32 height_;
};


#endif

