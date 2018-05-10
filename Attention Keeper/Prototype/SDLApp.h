#ifndef SDL_H
#define SDL_H

#include "SDL.h"
#include "SDL_ttf.h"

class SDLApp {

public:
	SDL_Window *myWindow;
	SDL_Renderer *myRenderer;
	SDL_Event *myEvent;
	SDL_Surface *mySurface;
	int screenHeight = 600;
	int screenWidth = 800;

	SDLApp();
	void drawLine(SDL_Renderer * renderer, int x1, int y1, int x2, int y2, SDL_Color color);
	void setColor(SDL_Renderer *renderer, int R, int G, int B);

	// Draw points with random colors and positions
	void drawRectangle(SDL_Renderer *renderer, int x, int y, int width, int height, int r, int g, int b);
	void appEnd(SDL_Renderer *renderer, SDL_Window *window);
	void setRenderer(SDL_Renderer *renderer);
	void setWindow(SDL_Window *window);
	void setBackColor(SDL_Renderer *renderer, int r, int g, int b);
	void clear(SDL_Renderer *renderer);
	void update(SDL_Renderer *renderer);
	bool drawText(std::string textureText, SDL_Color textColor, int x, int y);
	void free();

private:
	SDL_Texture * mTexture;
	int mWidth;
	int mHeight;
};

#endif
