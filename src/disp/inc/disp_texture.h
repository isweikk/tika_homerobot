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
        UINT32 createTextureFromFile(std::string path );
        
        //Creates image from font string
        UINT32 createTextureFromRenderText(std::string textureText, SDL_Color textColor );

        //Creates blank texture
        UINT32 createTexture(int width, int height, SDL_TextureAccess access);

        //Deallocates texture
        void freeTexture();

        //Set color modulation
        void setColor( Uint8 red, Uint8 green, Uint8 blue );

        //Set blending
        void setBlendMode( SDL_BlendMode blending );

        //Set alpha modulation
        void setAlpha( Uint8 alpha );
        
        //Renders texture at given point
        void render( int x, int y, SDL_Rect* clip = NULL, double angle = 0.0, SDL_Point* center = NULL, SDL_RendererFlip flip = SDL_FLIP_NONE );

        //Set self as render target
        void setAsRenderTarget();

        //Gets image dimensions
        int getWidth();
        int getHeight();

        //Pixel manipulators
        bool lockTexture();
        bool unlockTexture();
        void* getPixels();
        void copyPixels( void* pixels );
        int getPitch();
        Uint32 getPixel32( unsigned int x, unsigned int y );

    //private:
        //The actual hardware texture
        SDL_Window* window_;
        SDL_Renderer* renderer_;
        SDL_Texture* texture_;
        TTF_Font* font_;
        
        void* pixels_;
        int pitch_;

        //Image dimensions
        int width_;
        int height_;
};


#endif

