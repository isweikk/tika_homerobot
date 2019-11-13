//Using SDL, SDL Threads, SDL_image, standard IO, and, strings
#include <SDL.h>
#include <SDL_thread.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <stdio.h>
#include <string>

#include "disp/inc/disp_texture.h"

//The protective mutex
SDL_mutex* gBufferLock = NULL;

//The conditions
SDL_cond* gCanProduce = NULL;
SDL_cond* gCanConsume = NULL;

//The "data buffer"
int gData = -1;

void produce()
{
    //Lock
    SDL_LockMutex( gBufferLock );
    
    //If the buffer is full
    if( gData != -1 )
    {
        //Wait for buffer to be cleared
        printf( "\nProducer encountered full buffer, waiting for consumer to empty buffer...\n" );
        SDL_CondWait( gCanProduce, gBufferLock );
    }

    //Fill and show buffer
    gData = rand() % 255;
    printf( "\nProduced %d\n", gData );
    
    //Unlock
    SDL_UnlockMutex( gBufferLock );
    
    //Signal consumer
    SDL_CondSignal( gCanConsume );
}

void consume()
{
    //Lock
    SDL_LockMutex( gBufferLock );
    
    //If the buffer is empty
    if( gData == -1 )
    {
        //Wait for buffer to be filled
        printf( "\nConsumer encountered empty buffer, waiting for producer to fill buffer...\n" );
        SDL_CondWait( gCanConsume, gBufferLock );
    }

    //Show and empty buffer
    printf( "\nConsumed %d\n", gData );
    gData = -1;
    
    //Unlock
    SDL_UnlockMutex( gBufferLock );
    
    //Signal producer
    SDL_CondSignal( gCanProduce );
}

int producer( void *data )
{
    printf( "\nProducer started...\n" );

    //Seed thread random
    srand( SDL_GetTicks() );
    
    //Produce
    for( int i = 0; i < 5; ++i )
    {
        //Wait
        SDL_Delay( rand() % 1000 );
        
        //Produce
        produce();
    }

    printf( "\nProducer finished!\n" );
    
    return 0;

}

int consumer( void *data )
{
    printf( "\nConsumer started...\n" );

    //Seed thread random
    srand( SDL_GetTicks() );

    for( int i = 0; i < 5; ++i )
    {
        //Wait
        SDL_Delay( rand() % 1000 );
        
        //Consume
        consume();
    }
    
    printf( "\nConsumer finished!\n" );

    return 0;
}

void init()
{
    //Create the mutex
    gBufferLock = SDL_CreateMutex();
            
    //Create conditions
    gCanProduce = SDL_CreateCond();
    gCanConsume = SDL_CreateCond();

    return;
}

void deinit()
{
    //Destroy the mutex
    SDL_DestroyMutex(gBufferLock);
    gBufferLock = NULL;
            
    //Destroy conditions
    SDL_DestroyCond(gCanProduce);
    SDL_DestroyCond(gCanConsume);
    gCanProduce = NULL;
    gCanConsume = NULL;

    return;
}


int main( int argc, char* args[] )
{
    
    DispTexture dispTexture;
    
    //Start up SDL and create window
    if (dispTexture.openWindow() != OS_OK) {
        printf( "Failed to initialize!\n" );
        dispTexture.closeWindow();
    }
    
    //Load media
    if (dispTexture.createTextureFromFile("./res/img/background.png") != OS_OK) {
        printf("Failed to load media!\n");
        dispTexture.closeWindow();
        return 0;
    }

    //Main loop flag
    bool quit = false;

    //Event handler
    SDL_Event e;

    init();
    //Run the threads
    SDL_Thread* producerThread = SDL_CreateThread( producer, "Producer", NULL );
    SDL_Thread* consumerThread = SDL_CreateThread( consumer, "Consumer", NULL );
    
    //While application is running
    while( !quit )
    {
        //Handle events on queue
        while( SDL_PollEvent( &e ) != 0 )
        {
            //User requests quit
            if( e.type == SDL_QUIT )
            {
                quit = true;
            }
        }

        //Clear screen
        dispTexture.clearScreen();

        //Render splash
        dispTexture.render( 0, 0 );

        //Update screen
        dispTexture.updateScreen();
        SDL_Delay(25);
    }

    //Wait for producer and consumer to finish
    SDL_WaitThread( consumerThread, NULL );
    SDL_WaitThread( producerThread, NULL );
    deinit();
    
    //Free resources and close SDL
    dispTexture.freeTexture();
    dispTexture.closeWindow();

    return 0;
}
