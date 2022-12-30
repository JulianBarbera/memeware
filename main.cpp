//Using SDL, standard IO, and strings
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <iostream>
#include <unistd.h>
#include <curl/curl.h>
#include <regex> 

using std::cout;
using std::endl;

//Screen dimension constants
const int screenWidth = 640;
const int screenHeight = 480;

// Time the meme remains on the screen
const int onScreen = 5;

// Time between memes
const int memeDelay = 5;

int width;
int height;

// Starts up SDL and creates window
bool init();

// Gets the meme
std::string getMeme();

// Loads media
bool loadMedia();

// curl something
static size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp);

// Fetches meme url
std::string meme_url_curl();

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
    return "meme.bmp";
}

static size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp) {
    ((std::string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}

std::string meme_url_curl() {
    
    CURL *curl;
    CURLcode res;
    std::string readBuffer;

    curl = curl_easy_init();
    if(curl) {
        curl_easy_setopt(curl, CURLOPT_URL, "https://meme-api.com/gimme/wholesomememes");
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
        res = curl_easy_perform(curl);
        curl_easy_cleanup(curl);
    }

    std::smatch m; 
    // std::regex regexp("(.*?)");
    std::regex regexp("(https:\\/\\/i.redd.it\\/)([\\w_-]+(?:(?:\\.[\\w_-]+)+))([\\w.,@?^=%&:\\/~+#-]*[\\w@?^=%&\\/~+#-])");
    std::regex_search(readBuffer, m, regexp);
    cout << m.str() << endl;
    return m.str();

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

void download_image(std::string url) {
    CURL *curl;
    CURLcode res;
    std::string readBuffer;

    curl = curl_easy_init();
    if(curl) {
        curl_easy_setopt(curl, CURLOPT_URL, url);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
        res = curl_easy_perform(curl);
        curl_easy_cleanup(curl);
    }
    cout << readBuffer << endl;
}

int main( int argc, char* args[] ) {
    download_image(meme_url_curl());
    // system("curl  --output meme.jpg");
    while(1) {
	    display();
        cout << "Meme displayed!" << endl;
        sleep(memeDelay);
    }
	return 0;
}