//Using SDL, SDL_image, standard IO, and strings
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_ttf.h>
#include <stdio.h>
#include <string>
#include <cmath>
#include <iostream>
#include <fstream>
#include <list>
#include <vector>
#include <iterator>

using namespace std;

//Screen dimension constants
const int SCREEN_WIDTH = 1200;
const int SCREEN_HEIGHT = 640;
const int ANIMATION_FRAMES = 16;
vector<string> textualMaze;

//Texture wrapper class
class LTexture
{
	public:
		//Initializes variables
		LTexture();

		//Deallocates memory
		~LTexture();

		//Loads image at specified path
		bool loadBMPFromFile( std::string path );
		bool loadPNGFromFile( std::string path );
		
		#if defined(SDL_TTF_MAJOR_VERSION)
		//Creates image from font string
		bool loadFromRenderedText( std::string textureText, SDL_Color textColor );
		#endif

		//Deallocates texture
		void free();

		//Set color modulation
		void setColor( Uint8 red, Uint8 green, Uint8 blue );

		//Set blending
		void setBlendMode( SDL_BlendMode blending );

		//Set alpha modulation
		void setAlpha( Uint8 alpha );
		
		//Renders texture at given point
		void render( int x, int y, SDL_Rect* clip = NULL, double angle = 0.0, SDL_Point* center = NULL, SDL_RendererFlip flip = SDL_FLIP_NONE );

		//Gets image dimensions
		int getWidth();
		int getHeight();

		//The actual hardware texture
		SDL_Texture* mTexture;

		//Image dimensions
		int mWidth;
		int mHeight;
};

//The application time based timer
class LTimer
{
    public:
		//Initializes variables
		LTimer();

		//The various clock actions
		void start();
		void stop();
		void pause();
		void unpause();

		//Gets the timer's time
		Uint32 getTicks();

		//Checks the status of the timer
		bool isStarted();
		bool isPaused();

    private:
		//The clock time when the timer started
		Uint32 mStartTicks;

		//The ticks stored when the timer was paused
		Uint32 mPausedTicks;

		//The timer status
		bool mPaused;
		bool mStarted;
};

//The dot that will move around on the screen
class Dot
{
    public:

		//Maximum axis velocity of the dot
		static const int DOT_VEL = 3;
		static const int MIS_VEL = 2;

		//Initializes the variables
		Dot(int x, int y , int u , int v , int w,int h);

		//Takes key presses and adjusts the dot's velocity
		void handleEvent1( SDL_Event& e );
		void handleEvent2( SDL_Event& e );
		//Moves the dot
		void move(vector<string> list);
		//Moves the bullet
		void MoveandRenderBullet(vector<string> list,LTexture& texture);

		bool unfit();

		void getPathandRender(int x , int y,vector<string> array,LTexture& texture) ;

		//Shows the dot on the screen
		void render1(int frm );
		void render2(int frm );

		//The X and Y offsets of the dot
		int mPosX, mPosY;

		//The velocity of the dot
		int mVelX, mVelY;

		//The width and height of the dot
		int mWid,mHgt;

		//animate shape of dot
		int shape;

		//health of the dot
		int health;

		
};



//Starts up SDL and creates window
bool init();

//Loads media
bool loadMedia();

void RMBullet(list<Dot>& l);

//void RMBullet2();

bool checkCollision (Dot a ,Dot b);

bool checkWall (Dot a,vector<string> list);

//Frees media and shuts down SDL
void close();

//The window we'll be rendering to
SDL_Window* gWindow = NULL;

//The window renderer
SDL_Renderer* gRenderer = NULL;

//The music that will be played
Mix_Music *gMusic = NULL;

//The sound effects that will be used
Mix_Chunk *gScratch = NULL;
Mix_Chunk *gHigh = NULL;
Mix_Chunk *gMedium = NULL;
Mix_Chunk *gLow = NULL;


//Scene textures

SDL_Rect gPacSpriteClips[ ANIMATION_FRAMES ];
SDL_Rect gMonSpriteClips[ ANIMATION_FRAMES ];
LTexture gPacmanSpriteSheetTexture;
LTexture gMonsterSpriteSheetTexture;
LTexture gBulletTexture;
LTexture gFoodTexture;
LTexture gFinishTexture;
LTexture gFTextTexture;
LTexture gBackgroundTexture;
LTexture gScore1Texture;
LTexture gScore2Texture;
LTexture gWin1Texture;
LTexture gWin2Texture;
LTexture gH1Texture;
LTexture gH2Texture;
LTexture gHealthBarTexture;
LTexture gHKitTexture;
LTexture gMissile1Texture;
LTexture gMissile2Texture;
LTexture gFPSTexture;
LTexture gInsTexture;
LTexture gPosTexture;
TTF_Font *gFont = NULL;
list <Dot> Bullets1;
list <Dot> Bullets2;
list <Dot> Gems;
list <Dot> Missiles1;
list <Dot> Missiles2;
list <Dot> HKit;
int score1 = 0;
int score2 = 0;
int Tscore ;

LTexture::LTexture()
{
	//Initialize
	mTexture = NULL;
	mWidth = 0;
	mHeight = 0;
}

LTexture::~LTexture()
{
	//Deallocate
	free();
}

bool LTexture::loadPNGFromFile( std::string path )
{
	//Get rid of preexisting texture
	free();

	//The final texture
	SDL_Texture* newTexture = NULL;

	//Load image at specified path
	SDL_Surface* loadedSurface = IMG_Load( path.c_str() );
	if( loadedSurface == NULL )
	{
		printf( "Unable to load image %s! SDL_image Error: %s\n", path.c_str(), IMG_GetError() );
	}
	else
	{
		//Color key image
		SDL_SetColorKey( loadedSurface, SDL_TRUE, SDL_MapRGB( loadedSurface->format, 0, 0, 0 ) );

		//Create texture from surface pixels
        newTexture = SDL_CreateTextureFromSurface( gRenderer, loadedSurface );
		if( newTexture == NULL )
		{
			printf( "Unable to create texture from %s! SDL Error: %s\n", path.c_str(), SDL_GetError() );
		}
		else
		{
			//Get image dimensions
			mWidth = loadedSurface->w;
			mHeight = loadedSurface->h;
		}

		//Get rid of old loaded surface
		SDL_FreeSurface( loadedSurface );
	}

	//Return success
	mTexture = newTexture;
	return mTexture != NULL;
}
bool LTexture::loadBMPFromFile( std::string path )
{
	//Get rid of preexisting texture
	free();

	//The final texture
	SDL_Texture* newTexture = NULL;

	//Load image at specified path
	SDL_Surface* loadedSurface = SDL_LoadBMP( path.c_str() );
	if( loadedSurface == NULL )
	{
		printf( "Unable to load image %s! SDL_image Error: %s\n", path.c_str(), IMG_GetError() );
	}
	else
	{
		//Color key image
		SDL_SetColorKey( loadedSurface, SDL_TRUE, SDL_MapRGB( loadedSurface->format, 0, 0, 0) );

		//Create texture from surface pixels
        newTexture = SDL_CreateTextureFromSurface( gRenderer, loadedSurface );
		if( newTexture == NULL )
		{
			printf( "Unable to create texture from %s! SDL Error: %s\n", path.c_str(), SDL_GetError() );
		}
		else
		{
			//Get image dimensions
			mWidth = loadedSurface->w;
			mHeight = loadedSurface->h;
		}

		//Get rid of old loaded surface
		SDL_FreeSurface( loadedSurface );
	}

	//Return success
	mTexture = newTexture;
	return mTexture != NULL;
}



void LTexture::free()
{
	//Free texture if it exists
	if( mTexture != NULL )
	{
		SDL_DestroyTexture( mTexture );
		mTexture = NULL;
		mWidth = 0;
		mHeight = 0;
	}
}

void LTexture::setColor( Uint8 red, Uint8 green, Uint8 blue )
{
	//Modulate texture rgb
	SDL_SetTextureColorMod( mTexture, red, green, blue );
}

void LTexture::setBlendMode( SDL_BlendMode blending )
{
	//Set blending function
	SDL_SetTextureBlendMode( mTexture, blending );
}
		
void LTexture::setAlpha( Uint8 alpha )
{
	//Modulate texture alpha
	SDL_SetTextureAlphaMod( mTexture, alpha );
}

bool LTexture::loadFromRenderedText( std::string textureText, SDL_Color textColor )
{
	//Get rid of preexisting texture
	free();

	//Render text surface
	SDL_Surface* textSurface = TTF_RenderText_Solid( gFont, textureText.c_str(), textColor );
	if( textSurface == NULL )
	{
		printf( "Unable to render text surface! SDL_ttf Error: %s\n", TTF_GetError() );
	}
	else
	{
		//Create texture from surface pixels
        mTexture = SDL_CreateTextureFromSurface( gRenderer, textSurface );
		if( mTexture == NULL )
		{
			printf( "Unable to create texture from rendered text! SDL Error: %s\n", SDL_GetError() );
		}
		else
		{
			//Get image dimensions
			mWidth = textSurface->w;
			mHeight = textSurface->h;
		}

		//Get rid of old surface
		SDL_FreeSurface( textSurface );
	}
	
	//Return success
	return mTexture != NULL;
}

void LTexture::render( int x, int y, SDL_Rect* clip, double angle, SDL_Point* center, SDL_RendererFlip flip )
{
	//Set rendering space and render to screen
	SDL_Rect renderQuad = { x, y, mWidth, mHeight };

	//Set clip rendering dimensions
	if( clip != NULL )
	{
		renderQuad.w = clip->w;
		renderQuad.h = clip->h;
	}

	//Render to screen
	SDL_RenderCopyEx( gRenderer, mTexture, clip, &renderQuad, angle, center, flip );
}

int LTexture::getWidth()
{
	return mWidth;
}

int LTexture::getHeight()
{
	return mHeight;
}

LTimer::LTimer()
{
    //Initialize the variables
    mStartTicks = 0;
    mPausedTicks = 0;

    mPaused = false;
    mStarted = false;
}

void LTimer::start()
{
    //Start the timer
    mStarted = true;

    //Unpause the timer
    mPaused = false;

    //Get the current clock time
    mStartTicks = SDL_GetTicks();
	mPausedTicks = 0;
}

void LTimer::stop()
{
    //Stop the timer
    mStarted = false;

    //Unpause the timer
    mPaused = false;

	//Clear tick variables
	mStartTicks = 0;
	mPausedTicks = 0;
}

void LTimer::pause()
{
    //If the timer is running and isn't already paused
    if( mStarted && !mPaused )
    {
        //Pause the timer
        mPaused = true;

        //Calculate the paused ticks
        mPausedTicks = SDL_GetTicks() - mStartTicks;
		mStartTicks = 0;
    }
}

void LTimer::unpause()
{
    //If the timer is running and paused
    if( mStarted && mPaused )
    {
        //Unpause the timer
        mPaused = false;

        //Reset the starting ticks
        mStartTicks = SDL_GetTicks() - mPausedTicks;

        //Reset the paused ticks
        mPausedTicks = 0;
    }
}

Uint32 LTimer::getTicks()
{
	//The actual timer time
	Uint32 time = 0;

    //If the timer is running
    if( mStarted )
    {
        //If the timer is paused
        if( mPaused )
        {
            //Return the number of ticks when the timer was paused
            time = mPausedTicks;
        }
        else
        {
            //Return the current time minus the start time
            time = SDL_GetTicks() - mStartTicks;
        }
    }

    return time;
}

bool LTimer::isStarted()
{
	//Timer is running and paused or unpaused
    return mStarted;
}

bool LTimer::isPaused()
{
	//Timer is running and paused
    return mPaused && mStarted;
}

Dot::Dot(int x , int y , int u, int v , int w , int h)
{
	//Initialize shape
	shape = 0;

	//Intialize health 
	health = 10;

	mWid = w;
	mHgt = h;

    //Initialize the offsets
    mPosX = x;
    mPosY = y;

    //Initialize the velocity
    mVelX = u;
    mVelY = v;
}

void Dot::handleEvent1( SDL_Event& e )
{
    //If a key was pressed
	if( e.type == SDL_KEYDOWN && e.key.repeat == 0 )
    {
        //Adjust the velocity
        switch( e.key.keysym.sym )
        {
            case SDLK_UP: mVelY -= DOT_VEL;shape = 6; Mix_PlayChannel( -1, gScratch, 0 ); break;
            case SDLK_DOWN: mVelY += DOT_VEL;shape = 2; Mix_PlayChannel( -1, gScratch, 0 ); break;
            case SDLK_LEFT: mVelX -= DOT_VEL; shape = 0; Mix_PlayChannel( -1, gScratch, 0 ); break;
            case SDLK_RIGHT: mVelX += DOT_VEL; shape = 4; Mix_PlayChannel( -1, gScratch, 0 ); break;
			case SDLK_RETURN:
			if(score1>0){
			score1 = score1 - 1;
			int u ;
			int v ;
			 if(shape == 0 or shape == 1){ u = -5; v = 0;}
			 else if (shape == 2 or shape == 3){ u = 0 ; v = 5;}
			 else if (shape == 4 or shape == 5){ u = 5 ; v  = 0;}
			 else { u = 0 ; v = -5; };
			 Dot bullet(mPosX,mPosY,u,v,17,17);
			Bullets1.push_back(bullet);} break;
			case SDLK_RSHIFT:
			if(score1>19){
			score1 = score1 - 20;
			/*int u ;
			int v ;
			 if(shape == 0 or shape == 1){ u = -3; v = 0;}
			 else if (shape == 2 or shape == 3){ u = 0 ; v = 3;}
			 else if (shape == 4 or shape == 5){ u = 3 ; v  = 0;}
			 else { u = 0 ; v = -3; };*/
			 Dot missile(mPosX,mPosY,0,0,17,17);
			Missiles1.push_back(missile);} break;
			//default:mVelX = 0; mVelY = 0;break;
        }
    }
    //If a key was released
    else if( e.type == SDL_KEYUP && e.key.repeat == 0 )
    {
        //Adjust the velocity
        switch( e.key.keysym.sym )
        {
            case SDLK_UP: mVelY += DOT_VEL; Mix_PlayChannel( -1, gMedium, 0 );break;
            case SDLK_DOWN: mVelY -= DOT_VEL;Mix_PlayChannel( -1, gMedium, 0 ); break;
            case SDLK_LEFT: mVelX += DOT_VEL; Mix_PlayChannel( -1, gMedium, 0 );break;
            case SDLK_RIGHT: mVelX -= DOT_VEL;Mix_PlayChannel( -1, gMedium, 0 ); break;
			//default: mVelX = 0; mVelY = 0;break;
        }
    }
}

void Dot::handleEvent2( SDL_Event& e )
{
    //If a key was pressed
	if( e.type == SDL_KEYDOWN && e.key.repeat == 0 )
    {
        //Adjust the velocity
        switch( e.key.keysym.sym )
        {
            case SDLK_w: mVelY -= DOT_VEL;shape = 0; Mix_PlayChannel( -1, gHigh, 0 ); break;
            case SDLK_s: mVelY += DOT_VEL;shape = 2; Mix_PlayChannel( -1, gHigh, 0 ); break;
            case SDLK_a: mVelX -= DOT_VEL; shape = 6; Mix_PlayChannel( -1, gHigh, 0 ); break;
            case SDLK_d: mVelX += DOT_VEL; shape = 4; Mix_PlayChannel( -1, gHigh, 0 ); break;
			case SDLK_q:
			if (score2>0){
			score2 = score2 - 1;
			int u ;
			int v ;
			 if(shape == 0 or shape == 1){ u = 0; v = -5;}
			 else if (shape == 2 or shape == 3){ u = 0 ; v = 5;}
			 else if (shape == 4 or shape == 5){ u = 5 ; v  = 0;}
			 else { u = -5 ; v = 0; };
			 Dot bullet(mPosX,mPosY,u,v,17,17);
			Bullets2.push_back(bullet);} break;
			case SDLK_e:
			if (score2>19){
			score2 = score2 - 20;
			/*int u ;
			int v ;
			 if(shape == 0 or shape == 1){ u = 0; v = -3;}
			 else if (shape == 2 or shape == 3){ u = 0 ; v = 3;}
			 else if (shape == 4 or shape == 5){ u = 3 ; v  = 0;}
			 else { u = -3 ; v = 0; };*/
			 Dot missile(mPosX,mPosY,0,0,17,17);
			Missiles2.push_back(missile);} break;


			//default : mVelX = 0; mVelY = 0;break;
        }
    }
    //If a key was released
    else if( e.type == SDL_KEYUP && e.key.repeat == 0 )
    {
        //Adjust the velocity
        switch( e.key.keysym.sym )
        {
            case SDLK_w: mVelY += DOT_VEL; Mix_PlayChannel( -1, gMedium, 0 ); break;
            case SDLK_s: mVelY -= DOT_VEL; Mix_PlayChannel( -1, gMedium, 0 ); break;
            case SDLK_a: mVelX += DOT_VEL; Mix_PlayChannel( -1, gMedium, 0 ); break;
            case SDLK_d: mVelX -= DOT_VEL; Mix_PlayChannel( -1, gMedium, 0 );break;
			//default: mVelX = 0; mVelY = 0;break;
        }
    }
}

void Dot::move(vector<string> list)
{
    //Move the dot left or right
    mPosX += mVelX;

	if(checkWall(*this,list)){
		mPosX -= mVelX;
	}
    //Move the dot up or down
    mPosY += mVelY;

	if(checkWall(*this,list)){
		mPosY -= mVelY;
	}

}

void Dot::MoveandRenderBullet(vector<string> list,LTexture& texture)
{
    //Move the dot left or right
    mPosX += mVelX;
	//Move the dot up or down
    mPosY += mVelY;
	
	if(checkWall(*this,list)){
		health = -1;
 	}else if( ( mPosX < 0 ) || ( mPosX + mWid > SCREEN_WIDTH ) )
    {
        mPosX = 1500;
    }
    //If the dot went too far up or down
    else if( ( mPosY < 0 ) || ( mPosY + mHgt > SCREEN_HEIGHT ) )
    {
        mPosY = 700;
	}else{
		texture.render( mPosX, mPosY );
	}
}

void Dot::getPathandRender(int x ,int y , vector<string> array ,LTexture& texture)
{
	int tX = x/40;
	int tY = y/20;
	int pX = mPosX/40;
	int pY = mPosY/20;

	SDL_Rect button_pos;
	button_pos.w = 30;//(*it).mWid;
	button_pos.h = 20;//(*it).mHgt;

	if((tX == pX)&&(pY == tY)){
		if(mPosX>x+15){
			mVelX = -MIS_VEL;
			mVelY = 0;
			mPosX = mPosX + mVelX;
			button_pos.x = mPosX;
			button_pos.y = mPosY;
			SDL_RenderCopyEx(gRenderer,texture.mTexture,NULL,&button_pos,180.0f,NULL,SDL_FLIP_NONE);
			return;
		}else if (mPosX+mWid-2<x){
			mVelX = +MIS_VEL;
			mVelY = 0;
			mPosX = mPosX + mVelX;
			button_pos.x = mPosX;
			button_pos.y = mPosY;
			SDL_RenderCopyEx(gRenderer,texture.mTexture,NULL,&button_pos,0.0f,NULL,SDL_FLIP_NONE);
			return;
		}else {
			return;
		}
	}
	list<Dot> queue;
	vector <vector<int>>  tm(32,vector<int> (28,0));
	for(int i=0;i<32;i++){
    	for(int j=0;j<28;j++){
    		if((array[i][j])=='|'){
				tm[i][j] = -1;
    		}else{
				tm[i][j] = 0;			
			}
    	}
	}
	tm[pY][pX] = 1;
	if(tm[pY-1][pX]!=-1){
		Dot temp(pX,pY-1,0,0,0,1);
		tm[pY-1][pX] = 1;
		queue.push_back(temp);
	}
	if(tm[pY+1][pX]!=-1){
		Dot temp(pX,pY+1,0,0,0,3);
		tm[pY+1][pX] = 3;
		queue.push_back(temp);
	}
	if(tm[pY][pX-1]!=-1){
		Dot temp(pX-1,pY,0,0,0,4);
		tm[pY][pX-1] = 4;
		queue.push_back(temp);
	}
	if(tm[pY][pX+1]!=-1){
		Dot temp(pX+1,pY,0,0,0,2);
		tm[pY][pX+1] = 2;
		queue.push_back(temp);
	}
	//1 is top , 2 is right ,3 is bottom and 4 is left.
	while(tm[tY][tX] == 0){
		Dot s = queue.front();
		queue.pop_front();

		int dX = s.mPosX;
		int dY = s.mPosY;
		int h = s.mHgt;
		if(tm[dY-1][dX]==0){
			Dot temp(dX,dY-1,0,0,0,h);
			tm[dY-1][dX] = h;
			queue.push_back(temp);
		}
		if(tm[dY+1][dX]==0){
			Dot temp(dX,dY+1,0,0,0,h);
			tm[dY+1][dX] = h;
			queue.push_back(temp);
		}
		if(tm[dY][dX-1]==0){
			Dot temp(dX-1,dY,0,0,0,h);
			tm[dY][dX-1] = h;
			queue.push_back(temp);
		}
		if(tm[dY][dX+1]==0){
			Dot temp(dX+1,dY,0,0,0,h);
			tm[dY][dX+1] = h;
			queue.push_back(temp);
		}
		
	}

	if(tm[tY][tX]==1){
		mVelX = 0;
		mVelY = -MIS_VEL;
		//cout<<"up"<<endl;
		mPosY = mPosY + mVelY;
		if(checkWall(*this,array)){
			mPosX = mPosX - MIS_VEL;
			mPosY = mPosY - mVelY;
		}
		button_pos.x = mPosX;
		button_pos.y = mPosY;
		SDL_RenderCopyEx(gRenderer,texture.mTexture,NULL,&button_pos,270.0f,NULL,SDL_FLIP_NONE);
	}
	 if(tm[tY][tX]==2){
		mVelX = MIS_VEL;
		mVelY = 0;
		//cout<<"right"<<endl;
		mPosX = mPosX + mVelX;
		if(checkWall(*this,array)){
			mPosX = mPosX - mVelX;
			mPosY = mPosY - MIS_VEL;
		}
		button_pos.x = mPosX;
		button_pos.y = mPosY;
		SDL_RenderCopyEx(gRenderer,texture.mTexture,NULL,&button_pos,0.0f,NULL,SDL_FLIP_NONE);

	}
	if(tm[tY][tX]==3){
		mVelX = 0;
		mVelY = MIS_VEL;
		//cout<<"down"<<endl;
		mPosY = mPosY + mVelY;
		if(checkWall(*this,array)){
			mPosY = mPosY - mVelY;
			mPosX = mPosX - MIS_VEL;
		}
		button_pos.x = mPosX;
		button_pos.y = mPosY;
		SDL_RenderCopyEx(gRenderer,texture.mTexture,NULL,&button_pos,90.0f,NULL,SDL_FLIP_NONE);
	}
	if(tm[tY][tX]==4){
		mVelX = -MIS_VEL;
		mVelY = 0;
		//cout<<"left"<<endl;
		mPosX = mPosX + mVelX;
		if(checkWall(*this,array)){
			mPosX = mPosX - mVelX;
			mPosY = mPosY - MIS_VEL;
		}
		button_pos.x = mPosX;
		button_pos.y = mPosY;
		SDL_RenderCopyEx(gRenderer,texture.mTexture,NULL,&button_pos,180.0f,NULL,SDL_FLIP_NONE);
	}
	return;
}

bool Dot::unfit()
{
	if( ( mPosX < 0 ) || ( mPosX + mWid > SCREEN_WIDTH ) )
    {
        return true;
    }
    //If the dot went too far up or down
    else if( ( mPosY < 0 ) || ( mPosY + mHgt > SCREEN_HEIGHT ) )
    {
        return true;
    }else if (health <= 0){
		return true;
	}else{
		return false;
	}
}


void Dot::render1(int frm )
{
    //Show the dot
	SDL_Rect* currentClip = &gPacSpriteClips[ shape+frm ];
	SDL_Rect button_pos;
	button_pos.w = 20;
	button_pos.h = 20;
	button_pos.x = mPosX;
	button_pos.y = mPosY;
	SDL_RenderCopy(gRenderer,gPacmanSpriteSheetTexture.mTexture,currentClip,&button_pos );
}

void Dot::render2(int frm )
{
    //Show the dot
	SDL_Rect* currentClip = &gMonSpriteClips[ shape+frm ];
	SDL_Rect button_pos;
	button_pos.w = 20;
	button_pos.h = 20;
	button_pos.x = mPosX;
	button_pos.y = mPosY;
	SDL_RenderCopy(gRenderer,gMonsterSpriteSheetTexture.mTexture, currentClip,&button_pos );
}

bool init()
{
	//Initialization flag
	bool success = true;

	//Initialize SDL
	if( SDL_Init( SDL_INIT_VIDEO ) < 0 )
	{
		printf( "SDL could not initialize! SDL Error: %s\n", SDL_GetError() );
		success = false;
	}
	else
	{
		//Set texture filtering to linear
		if( !SDL_SetHint( SDL_HINT_RENDER_SCALE_QUALITY, "1" ) )
		{
			printf( "Warning: Linear texture filtering not enabled!" );
		}

		//Create window
		gWindow = SDL_CreateWindow( "SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN );
		if( gWindow == NULL )
		{
			printf( "Window could not be created! SDL Error: %s\n", SDL_GetError() );
			success = false;
		}
		else
		{
			//Create vsynced renderer for window
			gRenderer = SDL_CreateRenderer( gWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC );
			if( gRenderer == NULL )
			{
				printf( "Renderer could not be created! SDL Error: %s\n", SDL_GetError() );
				success = false;
			}
			else
			{
				//Initialize renderer color
				SDL_SetRenderDrawColor( gRenderer, 0xFF, 0xFF, 0xFF, 0xFF );

				//Initialize PNG loading
				int imgFlags = IMG_INIT_PNG;
				if( !( IMG_Init( imgFlags ) & imgFlags ) )
				{
					printf( "SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError() );
					success = false;
				}

				 //Initialize SDL_ttf
				if( TTF_Init() == -1 )
				{
					printf( "SDL_ttf could not initialize! SDL_ttf Error: %s\n", TTF_GetError() );
					success = false;
				}

				 //Initialize SDL_mixer
				if( Mix_OpenAudio( 44100, MIX_DEFAULT_FORMAT, 2, 2048 ) < 0 )
				{
					printf( "SDL_mixer could not initialize! SDL_mixer Error: %s\n", Mix_GetError() );
					success = false;
				}
			}
		}
	}

	return success;
}

bool loadMedia()
{
	//Loading success flag
	bool success = true;

	//Load dot texture

	if( !gPacmanSpriteSheetTexture.loadBMPFromFile( "PacMan.bmp" ) )
	{
		printf( "Failed to load walking animation texture!\n" );
		success = false;
	}
	else
	{
		//Set sprite clips
		gPacSpriteClips[ 0 ].x =   0;
		gPacSpriteClips[ 0 ].y =   0;
		gPacSpriteClips[ 0 ].w =  25;
		gPacSpriteClips[ 0 ].h =  25;

		gPacSpriteClips[ 1 ].x =  25;
		gPacSpriteClips[ 1 ].y =   0;
		gPacSpriteClips[ 1 ].w =  25;
		gPacSpriteClips[ 1 ].h =  25;
		
		gPacSpriteClips[ 2 ].x = 50;
		gPacSpriteClips[ 2 ].y =   0;
		gPacSpriteClips[ 2 ].w =  25;
		gPacSpriteClips[ 2 ].h = 25;

		gPacSpriteClips[ 3 ].x = 75;
		gPacSpriteClips[ 3 ].y =   0;
		gPacSpriteClips[ 3 ].w =  25;
		gPacSpriteClips[ 3 ].h = 25;

		gPacSpriteClips[ 4 ].x =   0;
		gPacSpriteClips[ 4 ].y =   25;
		gPacSpriteClips[ 4 ].w =  25;
		gPacSpriteClips[ 4 ].h = 25;

		gPacSpriteClips[ 5 ].x =  25;
		gPacSpriteClips[ 5 ].y =   25;
		gPacSpriteClips[ 5 ].w =  25;
		gPacSpriteClips[ 5 ].h = 25;
		
		gPacSpriteClips[ 6 ].x = 50;
		gPacSpriteClips[ 6 ].y =   25;
		gPacSpriteClips[ 6 ].w =  24;
		gPacSpriteClips[ 6 ].h = 25;

		gPacSpriteClips[ 7 ].x = 75;
		gPacSpriteClips[ 7 ].y =   25;
		gPacSpriteClips[ 7 ].w =  25;
		gPacSpriteClips[ 7 ].h = 25;

		gPacSpriteClips[ 8 ].x =   0;
		gPacSpriteClips[ 8 ].y =   50;
		gPacSpriteClips[ 8 ].w =  25;
		gPacSpriteClips[ 8 ].h = 25;

		gPacSpriteClips[ 9 ].x =  25;
		gPacSpriteClips[ 9 ].y =   50;
		gPacSpriteClips[ 9 ].w =  25;
		gPacSpriteClips[ 9 ].h = 25;
		
		gPacSpriteClips[ 10 ].x = 50;
		gPacSpriteClips[ 10 ].y =   50;
		gPacSpriteClips[ 10 ].w =  25;
		gPacSpriteClips[ 10 ].h = 25;

		gPacSpriteClips[ 11 ].x = 75;
		gPacSpriteClips[ 11 ].y =   50;
		gPacSpriteClips[ 11 ].w =  25;
		gPacSpriteClips[ 11 ].h = 25;

		gPacSpriteClips[ 12 ].x =   0;
		gPacSpriteClips[ 12 ].y =   75;
		gPacSpriteClips[ 12 ].w =  25;
		gPacSpriteClips[ 12 ].h = 25;

		gPacSpriteClips[ 13 ].x =  25;
		gPacSpriteClips[ 13 ].y =   75;
		gPacSpriteClips[ 13 ].w =  25;
		gPacSpriteClips[ 13 ].h = 25;
		
		gPacSpriteClips[ 14 ].x = 50;
		gPacSpriteClips[ 14 ].y =   75;
		gPacSpriteClips[ 14 ].w =  25;
		gPacSpriteClips[ 14 ].h = 25;

		gPacSpriteClips[ 15 ].x = 75;
		gPacSpriteClips[ 15 ].y =   75;
		gPacSpriteClips[ 15 ].w =  25;
		gPacSpriteClips[ 15 ].h = 25;
	}
	if( !gMonsterSpriteSheetTexture.loadBMPFromFile( "Blinky.bmp" ) )
	{
		printf( "Failed to load walking animation texture!\n" );
		success = false;
	}
	else
	{
		//Set sprite clips
		gMonSpriteClips[ 0 ].x =   0;
		gMonSpriteClips[ 0 ].y =   0;
		gMonSpriteClips[ 0 ].w =  25;
		gMonSpriteClips[ 0 ].h =  25;

		gMonSpriteClips[ 1 ].x =  25;
		gMonSpriteClips[ 1 ].y =   0;
		gMonSpriteClips[ 1 ].w =  25;
		gMonSpriteClips[ 1 ].h =  25;
		
		gMonSpriteClips[ 2 ].x = 50;
		gMonSpriteClips[ 2 ].y =   0;
		gMonSpriteClips[ 2 ].w =  25;
		gMonSpriteClips[ 2 ].h = 25;

		gMonSpriteClips[ 3 ].x = 75;
		gMonSpriteClips[ 3 ].y =  0;
		gMonSpriteClips[ 3 ].w =  25;
		gMonSpriteClips[ 3 ].h = 25;

		gMonSpriteClips[ 4 ].x =   0;
		gMonSpriteClips[ 4 ].y =   25;
		gMonSpriteClips[ 4 ].w =  25;
		gMonSpriteClips[ 4 ].h = 25;

		gMonSpriteClips[ 5 ].x =  25;
		gMonSpriteClips[ 5 ].y =   25;
		gMonSpriteClips[ 5 ].w =  25;
		gMonSpriteClips[ 5 ].h = 25;
		
		gMonSpriteClips[ 6 ].x = 50;
		gMonSpriteClips[ 6 ].y =   25;
		gMonSpriteClips[ 6 ].w =  25;
		gMonSpriteClips[ 6 ].h = 25;

		gMonSpriteClips[ 7 ].x = 75;
		gMonSpriteClips[ 7 ].y =   25;
		gMonSpriteClips[ 7 ].w =  25;
		gMonSpriteClips[ 7 ].h = 25;

		gMonSpriteClips[ 8 ].x =   0;
		gMonSpriteClips[ 8 ].y =   50;
		gMonSpriteClips[ 8 ].w =  25;
		gMonSpriteClips[ 8 ].h = 25;

		gPacSpriteClips[ 9 ].x =  25;
		gPacSpriteClips[ 9 ].y =   50;
		gPacSpriteClips[ 9 ].w =  25;
		gPacSpriteClips[ 9 ].h = 25;
		
		gMonSpriteClips[ 10 ].x = 50;
		gMonSpriteClips[ 10 ].y =   50;
		gMonSpriteClips[ 10 ].w =  25;
		gMonSpriteClips[ 10 ].h = 25;

		gMonSpriteClips[ 11 ].x = 75;
		gMonSpriteClips[ 11 ].y =   50;
		gMonSpriteClips[ 11 ].w =  25;
		gMonSpriteClips[ 11 ].h = 25;

		gMonSpriteClips[ 12 ].x =   0;
		gMonSpriteClips[ 12 ].y =   75;
		gMonSpriteClips[ 12 ].w =  25;
		gMonSpriteClips[ 12 ].h = 25;

		gMonSpriteClips[ 13 ].x =  25;
		gMonSpriteClips[ 13 ].y =   75;
		gMonSpriteClips[ 13 ].w =  25;
		gMonSpriteClips[ 13 ].h = 25;
		
		gMonSpriteClips[ 14 ].x = 50;
		gMonSpriteClips[ 14 ].y =   75;
		gMonSpriteClips[ 14 ].w =  25;
		gMonSpriteClips[ 14 ].h = 25;

		gMonSpriteClips[ 15 ].x = 75;
		gMonSpriteClips[ 15 ].y =   75;
		gMonSpriteClips[ 15 ].w =  25;
		gMonSpriteClips[ 15 ].h = 25;
	}
	if( !gBackgroundTexture.loadBMPFromFile( "tile.bmp" ) )
	{
		printf( "Failed to load background texture image!\n" );
		success = false;
	}

	if( !gBulletTexture.loadBMPFromFile( "point.bmp" ) )
	{
		printf( "Failed to load background texture image!\n" );
		success = false;
	}

	if( !gFoodTexture.loadBMPFromFile( "Cherry.bmp" ) )
	{
		printf( "Failed to load background texture image!\n" );
		success = false;
	}

	if( !gHealthBarTexture.loadBMPFromFile( "point2.bmp" ) )
	{
		printf( "Failed to load background texture image!\n" );
		success = false;
	}

	if( !gHKitTexture.loadBMPFromFile( "player.bmp" ) )
	{
		printf( "Failed to load background texture image!\n" );
		success = false;
	}

	if( !gMissile1Texture.loadPNGFromFile( "missile.png" ) )
	{
		printf( "Failed to load background texture image!\n" );
		success = false;
	}

	if( !gMissile2Texture.loadPNGFromFile( "missile.png" ) )
	{
		printf( "Failed to load background texture image!\n" );
		success = false;
	}

	if( !gFinishTexture.loadBMPFromFile( "bullet.bmp" ) )
	{
		printf( "Failed to load background texture image!\n" );
		success = false;
	}
	
	if( !gInsTexture.loadPNGFromFile( "instructions1.png" ) )
	{
		printf( "Failed to load background texture image!\n" );
		success = false;
	}

	if( !gPosTexture.loadPNGFromFile( "pacmanposter.png" ) )
	{
		printf( "Failed to load background texture image!\n" );
		success = false;
	}

	//Load music
	gMusic = Mix_LoadMUS( "intermission.wav" );
	if( gMusic == NULL )
	{
		printf( "Failed to load beat music! SDL_mixer Error: %s\n", Mix_GetError() );
		success = false;
	}
	
	//Load sound effects
	gScratch = Mix_LoadWAV( "booster.wav" );
	if( gScratch == NULL )
	{
		printf( "Failed to load scratch sound effect! SDL_mixer Error: %s\n", Mix_GetError() );
		success = false;
	}
	
	gHigh = Mix_LoadWAV( "vuln.wav" );
	if( gHigh == NULL )
	{
		printf( "Failed to load high sound effect! SDL_mixer Error: %s\n", Mix_GetError() );
		success = false;
	}

	gMedium = Mix_LoadWAV( "munch_a.wav" );
	if( gMedium == NULL )
	{
		printf( "Failed to load medium sound effect! SDL_mixer Error: %s\n", Mix_GetError() );
		success = false;
	}

	gLow = Mix_LoadWAV( "intro.wav" );
	if( gLow == NULL )
	{
		printf( "Failed to load low sound effect! SDL_mixer Error: %s\n", Mix_GetError() );
		success = false;
	}

	gFont = TTF_OpenFont( "arial.ttf", 15);
	if( gFont == NULL )
	{
		printf( "Failed to load lazy font! SDL_ttf Error: %s\n", TTF_GetError() );
		success = false;
	}
	
	return success;
}

void RMBullet(list<Dot>& l)
{
	if (l.size()==0){
		return;
	}
	list<Dot>::iterator it;
	it  =  l.begin();
	while((it != l.end()) && (l.size() != 0)){
		if(((*it).unfit())==true ){
			it=l.erase(it); 
		}else{
			++it;
		}
	}
	return;
}

bool checkCollision (Dot a , Dot b)
{
	 //The sides of the rectangles
    int leftA, leftB;
    int rightA, rightB;
    int topA, topB;
    int bottomA, bottomB;

    //Calculate the sides of rect A
    leftA = a.mPosX;
    rightA = a.mPosX + a.mWid;
    topA = a.mPosY;
    bottomA = a.mPosY + a.mHgt;

    //Calculate the sides of rect B
    leftB = b.mPosX;
    rightB = b.mPosX + b.mWid;
    topB = b.mPosY;
    bottomB = b.mPosY + b.mHgt;

    //If any of the sides from A are outside of B
    if( bottomA <= topB )
    {
        return false;
    }

    if( topA >= bottomB )
    {
        return false;
    }

    if( rightA <= leftB )
    {
        return false;
    }

    if( leftA >= rightB )
    {
        return false;
    }

    //If none of the sides from A are outside B
    return true;
}

bool checkWall(Dot a , vector<string> array)
{
	int leftA;
    int rightA;
    int topA;
    int bottomA;

    //Calculate the sides of rect A
    leftA = a.mPosX;
    rightA = a.mPosX + a.mWid;
    topA = a.mPosY;
    bottomA = a.mPosY + a.mHgt;

	if ((array[(topA)/20][(leftA)/40])=='|'){
		return true;
	}else if((array[(bottomA)/20][(leftA)/40])=='|'){
		return true;	
	}else if((array[(topA)/20][(rightA)/40])=='|'){
		return true;
	}else if((array[(bottomA)/20][(rightA)/40])=='|'){
		return true;
	}else {
		return false;
	}
}



void close()
{


	//Free loaded images
	gPacmanSpriteSheetTexture.free();
	gMonsterSpriteSheetTexture.free();
	gFinishTexture.free();
	gFTextTexture.free();
	gFoodTexture.free();
	gWin1Texture.free();
	gWin2Texture.free();
	gScore1Texture.free();
	gScore2Texture.free();
	gBulletTexture.free();
	gMissile1Texture.free();
	gMissile2Texture.free();
	gBackgroundTexture.free();
	gFPSTexture.free();
	gH1Texture.free();
	gH2Texture.free();
	gHealthBarTexture.free();
	gInsTexture.free();
	gPosTexture.free();

	TTF_CloseFont( gFont );
	gFont = NULL;
	//Free the sound effects
	Mix_FreeChunk( gScratch );
	Mix_FreeChunk( gHigh );
	Mix_FreeChunk( gMedium );
	Mix_FreeChunk( gLow );
	gScratch = NULL;
	gHigh = NULL;
	gMedium = NULL;
	gLow = NULL;
	
	//Free the music
	Mix_FreeMusic( gMusic );
	gMusic = NULL;


	//Destroy window	
	SDL_DestroyRenderer( gRenderer );
	SDL_DestroyWindow( gWindow );
	gWindow = NULL;
	gRenderer = NULL;

	//Quit SDL subsystems
	TTF_Quit();
	Mix_Quit();
	IMG_Quit();
	SDL_Quit();
}

int main( int argc, char* args[] )
{
	//Start up SDL and create window
	if( !init() )
	{
		printf( "Failed to initialize!\n" );
	}
	else
	{
		//Load media
		if( !loadMedia() )
		{
			printf( "Failed to load media!\n" );
		}
		else
		{	
			fstream file;
			file.open("mazing.txt",ios::in);
			string str;
			while (getline(file, str))
			{
  				textualMaze.push_back(str);
			}
			file.close();



			Mix_PlayChannel( -1, gLow, 0 );
			//Main loop flag
			bool quit = false;

			//Event handler
			SDL_Event e;

			//The dot that will be moving around on the screen
			Dot pacman(40,80,0,0,17,17);
			Dot monster(40,120,0,0,17,17);

			//Current animation frame
			int frame = 0;
			SDL_Color RedColor = { 255, 0, 0 };
			SDL_Color BlueColor = { 0, 0, 255 };
			SDL_Color GreenColor = {0,255,0};

			gMissile2Texture.setColor( 0, 255, 255);

			gWin1Texture.loadFromRenderedText( "Pacman Wins!!!! and Monster Loses!!!!! ", RedColor );
			gWin2Texture.loadFromRenderedText( "Monster Wins!!!! and Pacman Loses!!!!!", BlueColor );
			gH1Texture.loadFromRenderedText( "PLAYER 1", RedColor );
			gH2Texture.loadFromRenderedText( "PLAYER 2", BlueColor );
			gFTextTexture.loadFromRenderedText( "FINISH", BlueColor);

			int geminit=0;
			Mix_PlayMusic( gMusic, -1 );

			LTimer fpstimer;

			LTimer capTimer;
			
			int frame_count = 0;
			int hkit = 0;

			//While application is running
			while( !quit )
			{
				if (start == 0){
					SDL_Rect button_pos;
					button_pos.w = SCREEN_WIDTH;
					button_pos.h = SCREEN_HEIGHT;
					button_pos.x = 0;
					button_pos.y = 0;
					SDL_RenderCopy(gRenderer,gPosTexture.mTexture,NULL,&button_pos);
					SDL_RenderPresent(gRenderer);
					SDL_Delay(2000);
					SDL_SetRenderDrawColor( gRenderer, 0, 0, 0, 0xFF);
					SDL_RenderClear(gRenderer);
					button_pos.w = 1000;
					button_pos.h = SCREEN_HEIGHT;
					button_pos.x = 0;
					button_pos.y = 0;
					SDL_RenderCopy(gRenderer,gInsTexture.mTexture,NULL,&button_pos);
					SDL_RenderPresent(gRenderer);
					start = 1;
					SDL_Delay(10000);
					fpstimer.start();
				}else{
				
				capTimer.start();

				//Handle events on queue
				while( SDL_PollEvent( &e ) != 0 )
				{
					//User requests quit
					if( e.type == SDL_QUIT )
					{
						Mix_HaltMusic();
						quit = true;
					}
					
					//Handle input for the dot
					pacman.handleEvent1( e );
					monster.handleEvent2( e );
				}
				

				//Move the dot
				pacman.move(textualMaze);
				monster.move(textualMaze);

				float fps = frame_count / ( fpstimer.getTicks() / 1000.f );

				gScore1Texture.loadFromRenderedText( to_string(score1), RedColor );
				gScore2Texture.loadFromRenderedText( to_string(score2), BlueColor );
				gFPSTexture.loadFromRenderedText("FPS "+to_string(fps),GreenColor);
				


				//Clear screen
				SDL_SetRenderDrawColor( gRenderer, 0, 0, 0, 0xFF);
				SDL_RenderClear( gRenderer );
				
				for(int i=0;i<32;i++){
    				for(int j=0;j<28;j++){
    					if((textualMaze[i][j])=='|'){
    						SDL_Rect button_pos;
							button_pos.w = 35;
							button_pos.h = 15;
							button_pos.x = j*40;
							button_pos.y = i*20;
    						//gBackgroundTexture.render(button_pos.x,button_pos.y);
							SDL_RenderCopy(gRenderer,gBackgroundTexture.mTexture,NULL,&button_pos);
    					}else{
							if(i!=0){ //&& ((i!=13) || (j!=13)) && ((i!=13) || (j!=14)) && 
							if( geminit == 0){
							int w = 15;
							int h = 15;
							int x = j*40+10;
							int y = i*20;
							if((hkit%40)==0){
								Dot kit(x,y,0,0,15,15);
								HKit.push_back(kit);
							}else{
								Dot gem(x,y,0,0,15,15);
								Gems.push_back(gem);
							}
							hkit = hkit+1;
    						
							}
							}
							//SDL_RenderCopy(gRenderer,gFoodTexture.mTexture,NULL,&button_pos);

						}
    				}
				}
				if (geminit == 0){
					Tscore = Gems.size();
					geminit = 1;
				}

				for(auto it = Gems.begin() ; it!=Gems.end(); ++it){
					if((*it).health == 10){
						if(checkCollision(*it,pacman)){
							score1 = score1 +1;
							(*it).health = -1;
						}
						if(checkCollision(*it,monster)){
							score2 = score2+1;
							(*it).health = -1;
						}
					}
				}
				
				for(auto it = HKit.begin() ; it!=HKit.end(); ++it){
					if((*it).health == 10){
						if(checkCollision(*it,pacman)){
							if(pacman.health<5){
								pacman.health = pacman.health + 5;
								(*it).health = -1;
							}else if(pacman.health<10){
								pacman.health = 10;
								(*it).health = -1;
							}else{

							}
							
						}
						if(checkCollision(*it,monster)){
							if(monster.health<5){
								monster.health = monster.health + 5;
								(*it).health = -1;
							}else if(monster.health<10){
								monster.health = 10;
								(*it).health = -1;
							}else{

							}
						}
					}
				}

				//Render objects

				for(auto it = Gems.begin() ; it!=Gems.end(); ++it){
					if((*it).health == 10){
						SDL_Rect button_pos;
						button_pos.w = (*it).mWid;
						button_pos.h = (*it).mHgt;
						button_pos.x = (*it).mPosX;
						button_pos.y = (*it).mPosY;
						SDL_RenderCopy(gRenderer,gFoodTexture.mTexture,NULL,&button_pos);
					}
				}

				for(auto it = HKit.begin() ; it!=HKit.end(); ++it){
					if((*it).health == 10){
						SDL_Rect button_pos;
						button_pos.w = (*it).mWid;
						button_pos.h = (*it).mHgt;
						button_pos.x = (*it).mPosX;
						button_pos.y = (*it).mPosY;
						SDL_RenderCopy(gRenderer,gHKitTexture.mTexture,NULL,&button_pos);
					}
				}
				
				if (frame < 4 ){
					pacman.render1(0);
					monster.render2(0);
				}else{
					pacman.render1(1);
					monster.render2(1);
				}

				gScore1Texture.render(100,0);
				gScore2Texture.render(660,0);
				gH1Texture.render(0,0);
				gH2Texture.render(560,0);
				gFPSTexture.render(1120,20);

				for (auto it = Bullets1.begin(); it != Bullets1.end(); ++it){
					for (auto it1 = Bullets2.begin(); it1 != Bullets2.end(); ++it1){
						if(checkCollision(*it,*it1)){
							(*it).health = -1;
							(*it1).health = -1;
						}
					}
				}

				for (auto it = Missiles1.begin(); it != Missiles1.end(); ++it){
					for (auto it1 = Missiles2.begin(); it1 != Missiles2.end(); ++it1){
						if(checkCollision(*it,*it1)){
							(*it).health = 0;
							(*it1).health = 0;
						}
					}
				}

				for (auto it = Missiles1.begin(); it != Missiles1.end(); ++it){
					for (auto it1 = Bullets2.begin(); it1 != Bullets2.end(); ++it1){
						if(checkCollision(*it,*it1)){
							(*it).health = (*it).health -1;
							(*it1).health = 0;
						}
					}
				}

				for (auto it = Bullets1.begin(); it != Bullets1.end(); ++it){
					for (auto it1 = Missiles2.begin(); it1 != Missiles2.end(); ++it1){
						if(checkCollision(*it,*it1)){
							(*it).health = 0;
							(*it1).health = (*it1).health -1;
						}
					}
				}
				

				for (auto it = Bullets1.begin(); it != Bullets1.end(); ++it){
					if((*it).health > 0){
					if(checkCollision(*it,monster)){
						(*it).health = -1;
						monster.health = monster.health - 1;
					}
					(*it).MoveandRenderBullet(textualMaze,gBulletTexture);
					}
				}

				for (auto it = Missiles1.begin(); it != Missiles1.end(); ++it){
					if((*it).health > 0){
					if(checkCollision(*it,monster)){
						monster.health = monster.health-(*it).health;
						(*it).health = 0;
					}
					(*it).getPathandRender(monster.mPosX,monster.mPosY,textualMaze,gMissile1Texture);
					}
				}

				for (auto it1 = Bullets2.begin(); it1 != Bullets2.end(); ++it1){
					if((*it1).health > 0){
					if(checkCollision(*it1,pacman)){
						(*it1).health = -1;
						pacman.health = pacman.health-1;
					}
					(*it1).MoveandRenderBullet(textualMaze, gBulletTexture);
					}
				}

				for (auto it = Missiles2.begin(); it != Missiles2.end(); ++it){
					if((*it).health > 0){
					if(checkCollision(*it,pacman)){
						pacman.health = pacman.health - (*it).health ;
						(*it).health = 0;
					}
					(*it).getPathandRender(pacman.mPosX,pacman.mPosY,textualMaze,gMissile2Texture);
					}
				}

				for (int i = 0; i < pacman.health ; i++){
					SDL_Rect button_pos;
					button_pos.w = 15;
					button_pos.h = 15;
					button_pos.x = 160 + i*20;
					button_pos.y = 0;
					SDL_RenderCopy(gRenderer,gHealthBarTexture.mTexture,NULL,&button_pos);
				}

				for (int i = 0; i < monster.health ; i++){
					SDL_Rect button_pos;
					button_pos.w = 15;
					button_pos.h = 15;
					button_pos.x = 720+ i*20;
					button_pos.y = 0;
					SDL_RenderCopy(gRenderer,gHealthBarTexture.mTexture,NULL,&button_pos);
				}

				if(pacman.health <= 0){
					SDL_RenderClear( gRenderer );
					gWin2Texture.render(SCREEN_WIDTH/2,SCREEN_HEIGHT/2);
					quit = true;
				}
				if(monster.health <= 0){
					SDL_RenderClear( gRenderer );
					gWin1Texture.render(SCREEN_WIDTH/2,SCREEN_HEIGHT/2);
					quit = true;
				}
				
				if(Gems.size()==0){
					SDL_Rect button_pos;
					button_pos.w = 160;
					button_pos.h = 20;
					button_pos.x = 12*40;
					button_pos.y = 15*20;
					SDL_RenderCopy(gRenderer,gFinishTexture.mTexture,NULL,&button_pos);
					gFTextTexture.render(13*40+10,15*20);
					Dot finish(12*40,15*20,0,0,160,20);
					if(checkCollision(pacman,finish)){
						SDL_RenderClear( gRenderer );
						gWin1Texture.render(SCREEN_WIDTH/2,SCREEN_HEIGHT/2);
						quit = true;
					}
					if(checkCollision(monster,finish)){
						SDL_RenderClear( gRenderer );
						gWin2Texture.render(SCREEN_WIDTH/2,SCREEN_HEIGHT/2);
						quit = true;
					}

					
				}
				
				//cout<<"789"<<endl;

				//Update screen
				SDL_RenderPresent( gRenderer );
				if(quit==true){
					fpstimer.stop();
					capTimer.stop();
					SDL_Delay(3000);
				}

				//cout<<Bullets1.size()<<endl;

				//cout<<Bullets2.size()<<endl;

				RMBullet(Bullets1);

				RMBullet(HKit);

				RMBullet(Missiles1);

				RMBullet(Missiles2);

				RMBullet(Bullets2);

				RMBullet(Gems);

				//Bullets.remove_if(unfit);

				//cout<<score1<<endl;
				//cout<<score2<<endl;

				frame = frame + 1;
				frame_count = frame_count + 1;

				//Cycle animation
				if( frame / 4 >= 2 )
				{
					frame = 0;
				}
				
				int frameTicks = capTimer.getTicks();
                if( frameTicks < 8 )
                {
                    //Wait remaining time
                    SDL_Delay( 8 - frameTicks );
                }
		}

				
			}
		}
	}

	//Free resources and close SDL
	close();

	return 0;
}
