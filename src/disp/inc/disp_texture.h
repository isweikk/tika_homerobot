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

    UINT32 OpenWindow();
    void CloseWindow();

    //Loads image at specified path
    UINT32 CreateTextureFromFile(std::string path);

    //Creates image from font string
    UINT32 CreateTextureFromRenderText(std::string textureText, SDL_Color textColor);

    //Creates blank texture
    UINT32 CreateTexture(INT32 width, INT32 height, SDL_TextureAccess access);

    //Deallocates texture
    void FreeTexture();

    //Clear window
    void ClearScreen();

    //update screen
    void UpdateScreen();

    //Set color modulation
    void SetColor(UINT8 red, UINT8 green, UINT8 blue);

    //Set blending
    void SetBlendMode(SDL_BlendMode blending);

    //Set alpha modulation
    void SetAlpha(UINT8 alpha);

    //Renders texture at given point
    void Render(INT32 x, INT32 y, SDL_Rect* clip = NULL, double angle = 0.0, SDL_Point* center = NULL, SDL_RendererFlip flip = SDL_FLIP_NONE);

    //Set self as render target
    void SetAsRenderTarget();

    // set pen color
    void SetDrawColor(UINT8 r, UINT8 g, UINT8 b, UINT8 a);
    INT32 DrawPoint(const SDL_Point *points, INT32 count);

    //Gets image dimensions
    INT32 GetWidth();
    INT32 GetHeight();

    //Pixel manipulators
    UINT32 LockTexture();
    UINT32 UnlockTexture();
    void* GetPixels();
    void CopyPixels(void* pixels);
    INT32 GetPitch();
    INT32 GetPixel32(INT32 x, INT32 y);

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

