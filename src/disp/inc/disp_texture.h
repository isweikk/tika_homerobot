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
        bool loadFromFile( std::string path );
        
        //Creates image from font string
        bool loadFromRenderedText( std::string textureText, SDL_Color textColor );

        //Creates blank texture
        bool createBlank( int width, int height, SDL_TextureAccess = SDL_TEXTUREACCESS_STREAMING );

        //Deallocates texture
        void freeBlank();

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
        SDL_Window* mWindow;
        SDL_Renderer* mRenderer;
        SDL_Texture* mTexture;
        TTF_Font* mFont;
        
        void* mPixels;
        int mPitch;

        //Image dimensions
        int mWidth;
        int mHeight;
};


#endif

