//Using SDL, standard IO, and strings
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <iostream>
#include <unistd.h>
#include <curl/curl.h>
#include <regex>
#include <list>
#include <iterator>
#include <dirent.h>

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
std::string get_meme();

std::string pick_source();

// Loads media
bool load_media();

// curl something
static size_t write_callback(void *contents, size_t size, size_t nmemb, void *userp);

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
SDL_Surface* load_surface( std::string path );

// The window we'll be rendering to
SDL_Window* gWindow = NULL;
	
// The surface contained by the window
SDL_Surface* gScreenSurface = NULL;

// Current displayed image
SDL_Surface* Surface = NULL;

bool init() {
	//Initialization flag
    bool success = true;

    //Initialize SDL
    if( SDL_Init( SDL_INIT_VIDEO ) < 0 ) {
        printf( "SDL could not initialize! SDL Error: %s\n", SDL_GetError() );
        success = false;
    } else {
        //Create window
        gWindow = SDL_CreateWindow( "Memeware by Vinci", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, screenWidth, screenHeight, SDL_WINDOW_SHOWN );
        if( gWindow == NULL )
        {
            cout << "Renderer could not be created! SDL_Error:\n" << SDL_GetError() << endl;
            success = false;
        }
        else {
            //Initialize PNG loading
            int imgFlags = IMG_INIT_PNG;
            if( !( IMG_Init( imgFlags ) & imgFlags ) ) {
                cout << "SDL_image could not initialize! SDL_image Error: " << IMG_GetError() << endl;
                success = false;
            } else {
                //Get window surface
                gScreenSurface = SDL_GetWindowSurface( gWindow );
            }
        }
    }
    return success;
}


std::string get_meme() {
    std::list<std::string> path;

    struct dirent *d;
    DIR *dr;
    dr = opendir("memes");
    if(dr!=NULL)
    {
        // cout<<"List of Files & Folders:-\n";
        int i = 0;
        for(d=readdir(dr); d!=NULL; d=readdir(dr))
        {
			// cout << i << endl;
            // cout<<d->d_name<<endl;
            i++;
            std::string path_n = d->d_name;
            std::list<std::string>::iterator it = path.begin();
            std::advance(it, i);
            path.insert(it,path_n);
        }
        closedir(dr);
    }

    int size = path.size();
	srand(time(NULL));
    int ran_num = rand() % size + 0 ;
	// cout << ran_num << endl;

    std::list<std::string>::iterator itr = path.begin();

    std::advance(itr, ran_num);

    std::string itr1 = "memes/" +  *itr;
	
	return itr1;
}

static size_t write_callback(void *contents, size_t size, size_t nmemb, void *userp) {
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
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
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

bool load_media() {
	//Loading success flag
	bool success = true;

	//Load stretching surface
	Surface = load_surface( pick_source() );
	if( Surface == NULL ) {
		cout << "Failed to load image!" << endl;
		success = false;
	}

	return success;
}

void close() {
    //Free loaded image
	SDL_FreeSurface( Surface );
	Surface = NULL;

	//Destroy window
	SDL_DestroyWindow( gWindow );
	gWindow = NULL;
}

void quit() {
	//Quit SDL subsystems
	SDL_Quit();
}

SDL_Surface* load_surface( std::string path )
{
    //The final optimized image
    SDL_Surface* optimizedSurface = NULL;

    //Load image at specified path
    SDL_Surface* loadedSurface = IMG_Load( path.c_str() );
    if( loadedSurface == NULL ) {
        printf( "Unable to load image %s! SDL_image Error: %s\n", path.c_str(), IMG_GetError() );
    } else {
        //Convert surface to screen format
        optimizedSurface = SDL_ConvertSurface( loadedSurface, gScreenSurface->format, 0 );
        if( optimizedSurface == NULL )
        {
            printf( "Unable to optimize image %s! SDL Error: %s\n", path.c_str(), SDL_GetError() );
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
		if( !load_media() ) {
			cout << "Failed to load media!" << endl;
		} else {	

            // Apply the image
			
			SDL_BlitScaled( Surface, NULL, gScreenSurface, NULL );
			
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

std::string pick_source() {
	srand(time(NULL));
	std::string online = "meme.jpg";
	std::string local = get_meme();
	std::string source = rand() % 2 > 0 ? online : local;
	return source;
}

int main( int argc, char* args[] ) {
    while(1) {
		std::string curl_command = "curl " + meme_url_curl() + " --output meme.jpg";
    	system(curl_command.c_str());
	    display();
        cout << "Meme displayed!" << endl;
        sleep(memeDelay);
    }
	return 0;
}