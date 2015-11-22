//
// Created by Julian  on 17/11/15.
//

#include "../Headers/Graphics.h"
//------------------------------------------------------------------------------------
// Name: LoadSurface
// Desc:
//------------------------------------------------------------------------------------
Graphics::Graphics(int windowWidth, int windowHeight, std::string appName)
{
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        return;
    }

    // Create window
    this->window = SDL_CreateWindow(appName.c_str(), 100, 100, windowWidth, windowHeight, SDL_WINDOW_SHOWN);

    if ( this->window == nullptr) {
        SDL_Quit();
        return;
    }

    // Create renderer
    this->renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    if (this->renderer == nullptr) {
        SDL_DestroyWindow(window);
        SDL_Quit();
        return;
    }

    Uint32 rmask, gmask, bmask, amask;

    /* SDL interprets each pixel as a 32-bit number, so our masks must depend
       on the endianness (byte order) of the machine */
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
    rmask = 0xff000000;
    gmask = 0x00ff0000;
    bmask = 0x0000ff00;
    amask = 0x000000ff;
#else
    rmask = 0x000000ff;
    gmask = 0x0000ff00;
    bmask = 0x00ff0000;
    amask = 0xff000000;
#endif


    // Create surface
    this->sdlSurface = SDL_CreateRGBSurface(0, 20, 20, 32, rmask, gmask, bmask, amask);

    if (sdlSurface == nullptr) {

        SDL_DestroyWindow(window);
        SDL_DestroyRenderer(renderer);
        SDL_Quit();

        return;
    }

    std::cout << sdlSurface->format->Amask << std::endl;

    // Create texture
    SDL_Texture* sdlTexture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STATIC, 20, 20);

    if (sdlTexture == nullptr) {
        SDL_DestroyWindow(window);
        SDL_DestroyRenderer(renderer);
        SDL_Quit();

        return;
    }

    int imgFlags = IMG_INIT_PNG;
    if ( !(IMG_Init(imgFlags) & imgFlags) ) {
        SDL_DestroyWindow(window);
        SDL_DestroyRenderer(renderer);
        SDL_Quit();

        return;
    }
}
//------------------------------------------------------------------------------------
// Name: LoadSurface
// Desc:
//------------------------------------------------------------------------------------
Graphics::~Graphics()
{
    SDL_DestroyWindow(this->window);
    SDL_FreeSurface(this->sdlSurface);
    SDL_DestroyRenderer(this->renderer);
    SDL_Quit();
}
//------------------------------------------------------------------------------------
// Name: LoadSurface
// Desc:
//------------------------------------------------------------------------------------
SDL_Surface* Graphics::LoadSurface(std::string filename, SDL_PixelFormat* format)
{
    //The final optimized image
    SDL_Surface* optimizedSurface = NULL;

    //Load image at specified path
    SDL_Surface* loadedSurface = IMG_Load( filename.c_str() );
    if( loadedSurface == NULL )
    {
        printf( "Unable to load image %s! SDL_image Error: %s\n", filename.c_str(), IMG_GetError() );
    }
    else
    {
        //Convert surface to screen format
        optimizedSurface = SDL_ConvertSurface( loadedSurface, format, NULL );
        if( optimizedSurface == NULL )
        {
            printf( "Unable to optimize image %s! SDL Error: %s\n", filename.c_str(), SDL_GetError() );
        }

        //Get rid of old loaded surface
        SDL_FreeSurface( loadedSurface );
    }
    return optimizedSurface;
}
//------------------------------------------------------------------------------------
// Name: AddGraphicsResource
// Desc:
//------------------------------------------------------------------------------------
void Graphics::AddGraphicsResource(int resourceId, IGraphicsResource* graphicsResource)
{
    this->graphicsResourceMap.insert(std::pair<int, IGraphicsResource*>(resourceId, graphicsResource));
}
//------------------------------------------------------------------------------------
// Name: UpdateGraphics
// Desc:
//------------------------------------------------------------------------------------
void Graphics::UpdateGraphics(std::vector<GraphicsComponent> graphicsComponents, std::vector<TransformComponent> transformComponents)
{
    int w, h;
    SDL_GetWindowSize(this->window, &w, &h);

    SDL_RenderClear(this->renderer);

    for (GraphicsComponent& graphicsComponent : graphicsComponents) {
        TransformComponent transformComponent = transformComponents[graphicsComponent.transformId];

        IGraphicsResource* graphicsResource;
        graphicsResource = this->graphicsResourceMap[graphicsComponent.resourceId];
        graphicsResource->Render(this->renderer, &transformComponent);
    }
    SDL_RenderPresent(this->renderer);
}