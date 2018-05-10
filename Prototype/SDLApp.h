#ifndef SDL_H
#define SDL_H
#include "SDL.h"
#include "SDL_ttf.h"
#include "SDL_render.h"
#include "SDL_image.h"
#include <vector>
#include <string>
#include <iostream>
#include <string>
#include "math.h"




class SDLApp {

public:
	SDL_Window * myWindow;
	SDL_Renderer *myRenderer;
	SDL_Event *myEvent;
	SDL_Surface *mySurface;
	int screenHeight = 600;
	int screenWidth = 800;

	SDLApp();
	void drawLine(SDL_Renderer * renderer, int x1, int y1, int x2, int y2, SDL_Color color);
	void setColor(SDL_Renderer *renderer, int R, int G, int B);

	// draw points with random colors and positions
	void drawRectangle(SDL_Renderer *renderer, int x, int y, int width, int height, int r, int g, int b);
	void appEnd(SDL_Renderer *renderer, SDL_Window *window);
	void setRenderer(SDL_Renderer *renderer);
	void setWindow(SDL_Window *window);
	void setBackColor(SDL_Renderer *renderer, int r, int g, int b);
	void clear(SDL_Renderer *renderer);
	void update(SDL_Renderer *renderer);
	bool drawText(std::string textureText, SDL_Color textColor, int x, int y);
	void drawTextArray(std::string textureText, SDL_Color textColor, double array[], int size, int x, int y);
	void drawTextVector(std::string textureText, SDL_Color textColor, std::vector<double> vector, int size, int x, int y);
	void free();
	//drawing a circle
	void drawLoadCircle(SDL_Renderer *renderer, int x, int y, int ang, int rad, int r, int g, int b);

	//functions for drawing entire menu screens
	void showMainMenu(SDL_Renderer *renderer, int graphic_iter, int width, int height);
	void showHelp(SDL_Renderer *renderer, int width, int height);

	//functions for drawing subsidiary menus
	void drawMenu(SDL_Renderer *renderer, std::vector<std::string> options, int sel, int x1, int y1);

	//drawing an image
	bool drawImage(std::string img_file, int x, int y, int mWidth, int mHeight);

private:
	SDL_Texture * mTexture;
	int mWidth;
	int mHeight;
};

#endif
