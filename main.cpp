//Using SDL, standard IO, and strings
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <iostream>
#include <unistd.h>

using std::cout;
using std::endl;

//Screen dimension constants
const int screenWidth = 640;
const int screenHeight = 480;

// Time the meme remains on the screen
const int onScreen = 5;

// Time between memes
const int memeDelay = 15;

int width;
int height;


// Starts up SDL and creates window
bool init();

// Gets the meme
std::string getMeme();

// Loads media
bool loadMedia();

// Displays the meme
void display();

// Closes the window
void close();

// Frees media and shuts down SDL
void quit();

unsigned sleep(unsigned seconds);

// Loads individual image
SDL_Surface* loadSurface( std::string path );

// The window we'll be rendering to
SDL_Window* gWindow = NULL;
	
// The surface contained by the window
SDL_Surface* gScreenSurface = NULL;

// Current displayed image
SDL_Surface* gStretchedSurface = NULL;

bool init() {
	//Initialization flag
	bool success = true;

	//Initialize SDL
	if( SDL_Init( SDL_INIT_VIDEO ) < 0 ) {
		cout << "SDL could not initialize! SDL_Error:\n" << SDL_GetError() << endl;
		success = false;
	} else {
		//Create window
		gWindow = SDL_CreateWindow( "Memeware by Vinci", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, screenWidth, screenHeight, SDL_WINDOW_SHOWN );
		if( gWindow == NULL ) {
			cout << "Renderer could not be created! SDL_Error:\n" << SDL_GetError() << endl;
			success = false;
		} else {
			//Get window surface
			gScreenSurface = SDL_GetWindowSurface( gWindow );
		}
	}

	return success;
}

std::string getMeme() {
    return "zero-two/zero-two-a.bmp";
}

bool loadMedia() {
	//Loading success flag
	bool success = true;

	//Load stretching surface
	gStretchedSurface = loadSurface( getMeme() );
	if( gStretchedSurface == NULL ) {
		cout << "Failed to load image!" << endl;
		success = false;
	}

	return success;
}

void close() {
    //Free loaded image
	SDL_FreeSurface( gStretchedSurface );
	gStretchedSurface = NULL;

	//Destroy window
	SDL_DestroyWindow( gWindow );
	gWindow = NULL;
}

void quit() {
	//Quit SDL subsystems
	SDL_Quit();
}

SDL_Surface* loadSurface( std::string path ) {
	//The final optimized image
	SDL_Surface* optimizedSurface = NULL;

	//Load image at specified path
	SDL_Surface* loadedSurface = SDL_LoadBMP( path.c_str() );
	if( loadedSurface == NULL ) {
		cout << "Unable to load image!\n" << path.c_str() << endl << SDL_GetError() << endl;
	} else {
		//Convert surface to screen format
		optimizedSurface = SDL_ConvertSurface( loadedSurface, gScreenSurface->format, 0 );
		if( optimizedSurface == NULL )
		{
			cout << "Unable to load image!\n" << path.c_str() << endl << SDL_GetError() << endl;
		}

		//Get rid of old loaded surface
		SDL_FreeSurface( loadedSurface );
	}

	return optimizedSurface;
}

void display() {
    //Start up SDL and create window
	if( !init() )
	{
		cout << "Failed to initialize!" << endl;
	} else {
		//Load media
		if( !loadMedia() ) {
			cout << "Failed to load media!" << endl;
		} else {	

            //Apply the image stretched
			SDL_Rect stretchRect;
			stretchRect.x = 0;
			stretchRect.y = 0;
			stretchRect.w = screenWidth;
			stretchRect.h = screenHeight;
			SDL_BlitScaled( gStretchedSurface, NULL, gScreenSurface, &stretchRect );
			
			//Update the surface
			SDL_UpdateWindowSurface( gWindow );

            sleep(onScreen);
            close();
		}
	}

	//Free resources and quit SDL
    close();
	quit();
}

int main( int argc, char* args[] ) {
    while(1) {
	    display();
        cout << "Meme displayed!" << endl;
        sleep(memeDelay);
    }
	return 0;
}