#include "disp_texture.h"

//Using SDL, SDL Threads, SDL_image, standard IO, and, strings
#include <SDL.h>
#include <SDL_thread.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <stdio.h>
#include <string>

//Screen dimension constants
const UINT32 SCREEN_WIDTH = 640;
const UINT32 SCREEN_HEIGHT = 480;
const UINT32 SCREEN_FPS = 60;

DispTexture::DispTexture()
{
    //Initialize
    window_ = NULL_PTR;
    renderer_ = NULL_PTR;
    texture_ = NULL_PTR;
    width_ = 0;
    height_ = 0;
    pixels_ = NULL_PTR;
    pitch_ = 0;
}

DispTexture::~DispTexture()
{
    //Deallocate
    FreeTexture();
    CloseWindow();
}

UINT32 DispTexture::OpenWindow()
{
    //Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("SDL could not initialize! SDL Error: %s\n", SDL_GetError());
        return OS_ERROR;
    }
    //Set texture filtering to linear
    if (!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1")) {
        printf("Warning: Linear texture filtering not enabled!");
    }

    //Create window
    window_ = SDL_CreateWindow("Tika", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
        SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_FULLSCREEN);
    if (window_ == NULL_PTR) {
        CloseWindow();
        printf("Window could not be created! SDL Error: %s\n", SDL_GetError());
        return OS_ERROR;
    }

    //Create renderer for window
    renderer_ = SDL_CreateRenderer(window_, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (renderer_ == NULL_PTR) {
        CloseWindow();
        printf("Renderer could not be created! SDL Error: %s\n", SDL_GetError());
        return OS_ERROR;
    }

    //Initialize renderer color
    SDL_SetRenderDrawColor(renderer_, 0xFF, 0xFF, 0xFF, 0xFF);

    //Initialize PNG loading
    int32_t imgFlags = IMG_INIT_PNG;
    if (!(IMG_Init(imgFlags) & imgFlags)) {
        CloseWindow();
        printf("SDL_image could not initialize! %s\n", IMG_GetError());
        return OS_ERROR;
    }

    return OS_OK;
}

void DispTexture::CloseWindow()
{
    //Destroy window
    if (renderer_) {
        SDL_DestroyRenderer(renderer_);
        renderer_ = NULL_PTR;
    }
    if (window_) {
        SDL_DestroyWindow(window_);
        window_ = NULL_PTR;
    }

    //Quit SDL subsystems
    IMG_Quit();
    SDL_Quit();
}

UINT32 DispTexture::CreateTextureFromFile(std::string path)
{
    //Get rid of preexisting texture
    FreeTexture();

    //The final texture
    SDL_Texture* newTexture = NULL_PTR;

    //Load image at specified path
    SDL_Surface* loadedSurface = IMG_Load(path.c_str());
    if (loadedSurface == NULL_PTR) {
        printf("Unable to load image %s! SDL_image Error: %s\n", path.c_str(), IMG_GetError());
        return OS_ERROR;
    }

    //Convert surface to display format
    SDL_Surface* formattedSurface = SDL_ConvertSurfaceFormat(loadedSurface, SDL_PIXELFORMAT_RGBA8888, 0);
    if (formattedSurface == NULL_PTR) {
        SDL_FreeSurface(loadedSurface);
        printf("Unable to convert loaded surface to display format! %s\n", SDL_GetError());
        return OS_ERROR;
    }

    //Create blank streamable texture
    newTexture = SDL_CreateTexture(renderer_, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, formattedSurface->w, formattedSurface->h);
    if (newTexture == NULL_PTR) {
        SDL_FreeSurface(formattedSurface);
        SDL_FreeSurface(loadedSurface);
        printf("Unable to create blank texture! SDL Error: %s\n", SDL_GetError());
        return OS_ERROR;
    }

    //Enable blending on texture
    SDL_SetTextureBlendMode(newTexture, SDL_BLENDMODE_BLEND);

    //Lock texture for manipulation
    SDL_LockTexture(newTexture, &formattedSurface->clip_rect, &pixels_, &pitch_);

    //Copy loaded/formatted surface pixels
    memcpy(pixels_, formattedSurface->pixels, formattedSurface->pitch * formattedSurface->h);

    //Get image dimensions
    width_ = formattedSurface->w;
    height_ = formattedSurface->h;

    //Get pixel data in editable format
    UINT32 *pixels = (UINT32 *)pixels_;
    INT32 pixelCount = (pitch_ / 4) * height_;

    //Map colors
    UINT32 colorKey = SDL_MapRGB(formattedSurface->format, 0, 0xFF, 0xFF);
    UINT32 transparent = SDL_MapRGBA(formattedSurface->format, 0x00, 0xFF, 0xFF, 0x00);

    //Color key pixels
    for (INT32 i = 0; i < pixelCount; ++i) {
        if (pixels[i] == colorKey) {
            pixels[i] = transparent;
        }
    }

    //Unlock texture to update
    SDL_UnlockTexture(newTexture);
    pixels_ = NULL_PTR;

    //Get rid of old surface
    SDL_FreeSurface(formattedSurface);
    SDL_FreeSurface(loadedSurface);

    texture_ = newTexture;
    return OS_OK;
}

UINT32 DispTexture::CreateTextureFromRenderText(std::string textureText, SDL_Color textColor)
{
#if defined(SDL_TTF_H_) || defined(SDL_TTF_H)
    //Get rid of preexisting texture
    FreeTexture();

    //Render text surface
    SDL_Surface* textSurface = TTF_RenderText_Solid(font_, textureText.c_str(), textColor);
    if (textSurface != NULL_PTR) {
        //Create texture from surface pixels
        texture_ = SDL_CreateTextureFromSurface(renderer_, textSurface);
        if (texture_ == NULL_PTR) {
            printf("Unable to create texture from rendered text! SDL Error: %s\n", SDL_GetError());
        } else {
            //Get image dimensions
            width_ = textSurface->w;
            height_ = textSurface->h;
        }

        //Get rid of old surface
        SDL_FreeSurface(textSurface);
    } else {
        printf("Unable to render text surface! SDL_ttf Error: %s\n", TTF_GetError());
    }
#endif

    //Return ret
    return ((texture_ != NULL_PTR) ? OS_OK : OS_ERROR);
}
        
UINT32 DispTexture::CreateTexture(INT32 width, INT32 height, SDL_TextureAccess access)
{
    //Create uninitialized texture
    texture_ = SDL_CreateTexture(renderer_, SDL_PIXELFORMAT_RGBA8888, access, width, height);
    if (texture_ == NULL_PTR) {
        printf("Unable to create blank texture! SDL Error: %s\n", SDL_GetError());
        return OS_ERROR;
    }

    width_ = width;
    height_ = height;

    return OS_OK;
}

void DispTexture::FreeTexture()
{
    //Free texture if it exists
    if (texture_ != NULL_PTR) {
        SDL_DestroyTexture(texture_);
        texture_ = NULL_PTR;
        width_ = 0;
        height_ = 0;
        pixels_ = NULL_PTR;
        pitch_ = 0;
    }
}

void DispTexture::ClearScreen()
{
    //Clear screen
    SDL_SetRenderDrawColor(renderer_, 0xFF, 0xFF, 0xFF, 0xFF );
    SDL_RenderClear(renderer_);
}

void DispTexture::UpdateScreen()
{
    SDL_RenderPresent(renderer_);
}

void DispTexture::SetColor(UINT8 red, UINT8 green, UINT8 blue)
{
    //Modulate texture rgb
    SDL_SetTextureColorMod(texture_, red, green, blue);
}

void DispTexture::SetBlendMode(SDL_BlendMode blending)
{
    //Set blending function
    SDL_SetTextureBlendMode(texture_, blending);
}
        
void DispTexture::SetAlpha(UINT8 alpha)
{
    //Modulate texture alpha
    SDL_SetTextureAlphaMod(texture_, alpha);
}

void DispTexture::Render(INT32 x, INT32 y, SDL_Rect* clip, double angle, SDL_Point* center, SDL_RendererFlip flip)
{
    //Set rendering space and render to screen
    SDL_Rect renderQuad = {x, y, width_, height_};

    //Set clip rendering dimensions
    if (clip != NULL_PTR) {
        renderQuad.w = clip->w;
        renderQuad.h = clip->h;
    }

    //Render to screen
    SDL_RenderCopyEx(renderer_, texture_, clip, &renderQuad, angle, center, flip);
}

void DispTexture::SetAsRenderTarget()
{
    //Make self render target
    SDL_SetRenderTarget(renderer_, texture_);
}

void DispTexture::SetDrawColor(UINT8 r, UINT8 g, UINT8 b, UINT8 a)
{
    SDL_SetRenderDrawColor(renderer_, r, g, b, a);
}

INT32 DispTexture::DrawPoint(const SDL_Point *points, INT32 count)
{
    return SDL_RenderDrawPoints(renderer_, points, count);
}

INT32 DispTexture::GetWidth()
{
    return width_;
}

INT32 DispTexture::GetHeight()
{
    return height_;
}

UINT32 DispTexture::LockTexture()
{
    bool ret = true;

    //Texture is already locked
    if (pixels_ != NULL_PTR) {
        printf("Texture is already locked!\n");
        return OS_ERROR;
    }

    //Lock texture
    if (SDL_LockTexture(texture_, NULL_PTR, &pixels_, &pitch_) != 0) {
        printf("Unable to lock texture! %s\n", SDL_GetError());
        return OS_ERROR;
    }

    return OS_OK;
}

UINT32 DispTexture::UnlockTexture()
{
    //Texture is not locked
    if (pixels_ == NULL_PTR) {
        printf("Texture is not locked!\n");
        return OS_ERROR;
    }

    //Unlock texture
    SDL_UnlockTexture(texture_);
    pixels_ = NULL_PTR;
    pitch_ = 0;

    return OS_OK;
}

void* DispTexture::GetPixels()
{
    return pixels_;
}

void DispTexture::CopyPixels(void* pixels)
{
    //Texture is locked
    if (pixels_ != NULL_PTR) {
        //Copy to locked pixels
        memcpy(pixels_, pixels, pitch_ * height_);
    }
}

INT32 DispTexture::GetPitch()
{
    return pitch_;
}

INT32 DispTexture::GetPixel32(INT32 x, INT32 y)
{
    //Convert the pixels to 32 bit
    INT32 *pixels = (INT32*)pixels_;

    //Get the pixel requested
    return pixels[(y * (pitch_ / 4)) + x];
}


