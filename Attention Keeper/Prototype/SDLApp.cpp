#include <iostream>
#include "SDLApp.h"

using namespace std;
//Globally used font
TTF_Font *gFont = NULL;


SDLApp::SDLApp()
{
	TTF_Init();
	SDL_Init(SDL_INIT_VIDEO);

	//Initialize the parameters for the text
	mTexture = NULL;
	mWidth = 0;
	mHeight = 0;

	//initialize the font
	gFont = TTF_OpenFont("arial.ttf", 12);
	if (gFont != NULL) {
		std::cout << "The global font has been loaded\n";
	}
}

//drawing a rectangle
void SDLApp::drawRectangle(SDL_Renderer *renderer, int x, int y, int w, int h, int r, int g, int b)
{
	setColor(renderer, r, g, b);
	SDL_Rect rect = { x, y, w, h };//theta bar 
	SDL_RenderFillRect(renderer, &rect);
}

//drawing a line
void SDLApp::drawLine(SDL_Renderer *renderer, int x1, int y1, int x2, int y2, SDL_Color color)
{
	setColor(renderer, color.r, color.g, color.b);
	SDL_RenderDrawLine(renderer, x1, y1, x2, y2);
}

//setting a color
void SDLApp::setColor(SDL_Renderer *renderer, int r, int g, int b)
{
	SDL_SetRenderDrawColor(renderer, r, g, b, 255); // Red color
}

//sets the renderer
void SDLApp::setRenderer(SDL_Renderer *renderer)
{
	myRenderer = renderer;
}

//sets the window
void SDLApp::setWindow(SDL_Window *window)
{
	myWindow = window;
}

//sets the background color
void SDLApp::setBackColor(SDL_Renderer *renderer, int r, int g, int b)
{
	SDL_SetRenderDrawColor(renderer, r, g, b, 255);
	drawRectangle(renderer, 0, 0, 640, 480, 0, 0, 0);
	clear(renderer);
}

//updates the renderer
void SDLApp::update(SDL_Renderer *renderer)
{
	SDL_RenderPresent(renderer);
}

//clear
void SDLApp::clear(SDL_Renderer *renderer)
{
	SDL_RenderClear(renderer);
}

//ending the app
void SDLApp::appEnd(SDL_Renderer *renderer, SDL_Window *window)
{
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
}

//free
void SDLApp::free()
{
	//Free texture if it exists
	if (mTexture != NULL)
	{
		SDL_DestroyTexture(mTexture);
		mTexture = NULL;
		mWidth = 0;
		mHeight = 0;
	}
}

//draw text
bool SDLApp::drawText(std::string textureText, SDL_Color textColor, int x, int y)
{
	//Get rid of preexisting texture
	free();
	if (gFont == NULL) {
		printf("issue with font");
		exit;
	}
	//Render text surface
	SDL_Surface* textSurface;
	if (!(textSurface = TTF_RenderText_Solid(gFont, textureText.c_str(), textColor))) {
		std::cout << "error creating text surface" << endl;
	}

	if (textSurface == NULL)
	{
		std::cout << "Unable to render text surface! SDL_ttf Error:\n" << TTF_GetError() << endl;
	}
	else
	{
		//Create texture from surface pixels
		mTexture = SDL_CreateTextureFromSurface(myRenderer, textSurface);
		if (mTexture == NULL)
		{
			printf("Unable to create texture from rendered text! SDL Error: %s\n", SDL_GetError());
		}
		else
		{
			//Get image dimensions
			mWidth = textSurface->w;
			mHeight = textSurface->h;
		}
		const SDL_Rect textlocation = { 0,0,mWidth,mHeight };
		const SDL_Rect screenlocation = { x,y,mWidth,mHeight };
		const SDL_Rect *textloc = &textlocation;
		const SDL_Rect *screenloc = &screenlocation;

		//SDL_BlitSurface(textSurface, NULL, mySurface, NULL);
		SDL_RenderCopy(myRenderer, mTexture, textloc, screenloc);

		//perhaps we can reuse it, but I assume not for simplicity.
		//Get rid of old surface
		SDL_FreeSurface(textSurface);
	}

	//Return success
	return mTexture != NULL;
}