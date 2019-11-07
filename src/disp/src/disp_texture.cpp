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
    mWindow = NULL_PTR;
    mRenderer = NULL_PTR;
    mTexture = NULL_PTR;
    mWidth = 0;
    mHeight = 0;
    mPixels = NULL_PTR;
    mPitch = 0;
}

DispTexture::~DispTexture()
{
    //Deallocate
    freeBlank();
    closeWindow();
}

UINT32 DispTexture::openWindow()
{
    UINT32 ret = OS_OK;

    //Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("SDL could not initialize! SDL Error: %s\n", SDL_GetError());
        ret = OS_ERROR;
    } else {
        //Set texture filtering to linear
        if (!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1")) {
            printf("Warning: Linear texture filtering not enabled!");
        }

        //Create window
        mWindow = SDL_CreateWindow("SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
            SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_FULLSCREEN);
        if (mWindow == NULL) {
            printf("Window could not be created! SDL Error: %s\n", SDL_GetError());
            ret = OS_ERROR;
        } else {
            //Create renderer for window
            mRenderer = SDL_CreateRenderer(mWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
            if(mRenderer == NULL)
            {
                printf("Renderer could not be created! SDL Error: %s\n", SDL_GetError());
                ret = OS_ERROR;
            }
            else
            {
                //Initialize renderer color
                SDL_SetRenderDrawColor(mRenderer, 0xFF, 0xFF, 0xFF, 0xFF);

                //Initialize PNG loading
                int imgFlags = IMG_INIT_PNG;
                if(!(IMG_Init(imgFlags) & imgFlags))
                {
                    printf("SDL_image could not initialize! %s\n", IMG_GetError());
                    ret = OS_ERROR;
                }
            }
        }
    }

    return ret;
}

void DispTexture::closeWindow()
{
    //Destroy window
    SDL_DestroyRenderer(mRenderer);
    SDL_DestroyWindow(mWindow);
    mWindow = NULL;
    mRenderer = NULL;

    //Quit SDL subsystems
    IMG_Quit();
    SDL_Quit();
}

bool DispTexture::loadFromFile(std::string path)
{
    //Get rid of preexisting texture
    freeBlank();

    //The final texture
    SDL_Texture* newTexture = NULL;

    //Load image at specified path
    SDL_Surface* loadedSurface = IMG_Load(path.c_str());
    if (loadedSurface == NULL) {
        printf("Unable to load image %s! SDL_image Error: %s\n", path.c_str(), IMG_GetError());
    } else {
        //Convert surface to display format
        SDL_Surface* formattedSurface = SDL_ConvertSurfaceFormat(loadedSurface, SDL_PIXELFORMAT_RGBA8888, 0);
        if (formattedSurface == NULL) {
            printf("Unable to convert loaded surface to display format! %s\n", SDL_GetError());
        } else {
            //Create blank streamable texture
            newTexture = SDL_CreateTexture(mRenderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, formattedSurface->w, formattedSurface->h);
            if (newTexture == NULL) {
                printf("Unable to create blank texture! SDL Error: %s\n", SDL_GetError());
            } else {
                //Enable blending on texture
                SDL_SetTextureBlendMode(newTexture, SDL_BLENDMODE_BLEND);

                //Lock texture for manipulation
                SDL_LockTexture(newTexture, &formattedSurface->clip_rect, &mPixels, &mPitch);

                //Copy loaded/formatted surface pixels
                memcpy(mPixels, formattedSurface->pixels, formattedSurface->pitch * formattedSurface->h);

                //Get image dimensions
                mWidth = formattedSurface->w;
                mHeight = formattedSurface->h;

                //Get pixel data in editable format
                Uint32* pixels = (Uint32*)mPixels;
                int pixelCount = (mPitch / 4) * mHeight;

                //Map colors                
                Uint32 colorKey = SDL_MapRGB(formattedSurface->format, 0, 0xFF, 0xFF);
                Uint32 transparent = SDL_MapRGBA(formattedSurface->format, 0x00, 0xFF, 0xFF, 0x00);

                //Color key pixels
                for (int i = 0; i < pixelCount; ++i) {
                    if (pixels[ i ] == colorKey) {
                        pixels[ i ] = transparent;
                    }
                }

                //Unlock texture to update
                SDL_UnlockTexture(newTexture);
                mPixels = NULL;
            }

            //Get rid of old formatted surface
            SDL_FreeSurface(formattedSurface);
        }
        
        //Get rid of old loaded surface
        SDL_FreeSurface(loadedSurface);
    }

    //Return ret
    mTexture = newTexture;
    return mTexture != NULL;
}

bool DispTexture::loadFromRenderedText(std::string textureText, SDL_Color textColor)
{
#if defined(SDL_TTF_H_) || defined(SDL_TTF_H)
    //Get rid of preexisting texture
    freeBlank();

    //Render text surface
    SDL_Surface* textSurface = TTF_RenderText_Solid(mFont, textureText.c_str(), textColor);
    if (textSurface != NULL) {
        //Create texture from surface pixels
        mTexture = SDL_CreateTextureFromSurface(mRenderer, textSurface);
        if (mTexture == NULL) {
            printf("Unable to create texture from rendered text! SDL Error: %s\n", SDL_GetError());
        } else {
            //Get image dimensions
            mWidth = textSurface->w;
            mHeight = textSurface->h;
        }

        //Get rid of old surface
        SDL_FreeSurface(textSurface);
    } else {
        printf("Unable to render text surface! SDL_ttf Error: %s\n", TTF_GetError());
    }
#endif

    //Return ret
    return mTexture != NULL;
}
        
bool DispTexture::createBlank(int width, int height, SDL_TextureAccess access)
{
    //Create uninitialized texture
    mTexture = SDL_CreateTexture(mRenderer, SDL_PIXELFORMAT_RGBA8888, access, width, height);
    if (mTexture == NULL) {
        printf("Unable to create blank texture! SDL Error: %s\n", SDL_GetError());
    } else {
        mWidth = width;
        mHeight = height;
    }

    return mTexture != NULL;
}

void DispTexture::freeBlank()
{
    //Free texture if it exists
    if (mTexture != NULL) {
        SDL_DestroyTexture(mTexture);
        mTexture = NULL;
        mWidth = 0;
        mHeight = 0;
        mPixels = NULL;
        mPitch = 0;
    }
}

void DispTexture::setColor(Uint8 red, Uint8 green, Uint8 blue)
{
    //Modulate texture rgb
    SDL_SetTextureColorMod(mTexture, red, green, blue);
}

void DispTexture::setBlendMode(SDL_BlendMode blending)
{
    //Set blending function
    SDL_SetTextureBlendMode(mTexture, blending);
}
        
void DispTexture::setAlpha(Uint8 alpha)
{
    //Modulate texture alpha
    SDL_SetTextureAlphaMod(mTexture, alpha);
}

void DispTexture::render(int x, int y, SDL_Rect* clip, double angle, SDL_Point* center, SDL_RendererFlip flip)
{
    //Set rendering space and render to screen
    SDL_Rect renderQuad = {x, y, mWidth, mHeight};

    //Set clip rendering dimensions
    if (clip != NULL) {
        renderQuad.w = clip->w;
        renderQuad.h = clip->h;
    }

    //Render to screen
    SDL_RenderCopyEx(mRenderer, mTexture, clip, &renderQuad, angle, center, flip);
}

void DispTexture::setAsRenderTarget()
{
    //Make self render target
    SDL_SetRenderTarget(mRenderer, mTexture);
}

int DispTexture::getWidth()
{
    return mWidth;
}

int DispTexture::getHeight()
{
    return mHeight;
}

bool DispTexture::lockTexture()
{
    bool ret = true;

    //Texture is already locked
    if (mPixels != NULL) {
        printf("Texture is already locked!\n");
        ret = false;
    } else {
        //Lock texture
        if (SDL_LockTexture(mTexture, NULL, &mPixels, &mPitch) != 0) {
            printf("Unable to lock texture! %s\n", SDL_GetError());
            ret = false;
        }
    }

    return ret;
}

bool DispTexture::unlockTexture()
{
    bool ret = true;

    //Texture is not locked
    if (mPixels == NULL) {
        printf("Texture is not locked!\n");
        ret = false;
    } else {
        //Unlock texture
        SDL_UnlockTexture(mTexture);
        mPixels = NULL;
        mPitch = 0;
    }

    return ret;
}

void* DispTexture::getPixels()
{
    return mPixels;
}

void DispTexture::copyPixels(void* pixels)
{
    //Texture is locked
    if (mPixels != NULL) {
        //Copy to locked pixels
        memcpy(mPixels, pixels, mPitch * mHeight);
    }
}

int DispTexture::getPitch()
{
    return mPitch;
}

Uint32 DispTexture::getPixel32(unsigned int x, unsigned int y)
{
    //Convert the pixels to 32 bit
    Uint32 *pixels = (Uint32*)mPixels;

    //Get the pixel requested
    return pixels[(y * (mPitch / 4)) + x];
}


