#include "SDLApp.h"

// GLOBALLY USED FONT
TTF_Font *gFont = NULL;
int fontwidth = 12;
int fontheight = 12;

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
		std::cout << "The global font has been loaded" << std::endl;
	}
}

/*
drawTextVector:
This function draws a rectangle
*/
void SDLApp::drawRectangle(SDL_Renderer *renderer, int x, int y, int w, int h, int r, int g, int b)
{
	setColor(renderer, r, g, b);
	SDL_Rect rect = { x, y, w, h };//theta bar 
	SDL_RenderFillRect(renderer, &rect);
}

/*
drawTextVector:
This function draws a line
*/
void SDLApp::drawLine(SDL_Renderer *renderer, int x1, int y1, int x2, int y2, SDL_Color color)
{
	setColor(renderer, color.r, color.g, color.b);
	SDL_RenderDrawLine(renderer, x1, y1, x2, y2);
}

/*
drawTextVector:
This function sets the default color to be used in rendering
*/
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

/*
setBackColor:
This function sets the background color of the screen
*/
void SDLApp::setBackColor(SDL_Renderer *renderer, int r, int g, int b)
{
	SDL_SetRenderDrawColor(renderer, r, g, b, 255);
	drawRectangle(renderer, 0, 0, 640, 480, r, g, b);
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

/*
appEnd:
This functions destroys the renderer and window of the app
*/
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

/*
drawLoadCircle:
This function draws a loading bar to the screen that rotates based on integer input "ang"
*/
void SDLApp::drawLoadCircle(SDL_Renderer *renderer, int x, int y, int ang, int rad, int r, int g, int b)
{
	int x1, y1, x2, y2;
	setColor(renderer, r, g, b);
	x1 = int(x + (rad * cos(ang * PI / 180)));
	y1 = int(y + (rad * sin(ang * PI / 180)));
	x2 = int(x + (rad * cos((ang + 30) * PI / 180)));
	y2 = int(y + (rad * sin((ang + 30) * PI / 180)));
	SDL_RenderDrawLine(renderer, x1, y1, x2, y2);
	SDL_RenderDrawLine(renderer, x1, y1, x, y);
	SDL_RenderDrawLine(renderer, x2, y2, x, y);
}

/*
drawText:
This function draws text to the screen in a given color
*/
bool SDLApp::drawText(std::string textureText, SDL_Color textColor, int x, int y)
{
	//Get rid of preexisting texture
	free();
	if (gFont == NULL) {
		printf("issue with font");
		exit(EXIT_FAILURE); // Is this expected? [QUESTION] 
	}
	//Render text surface
	SDL_Surface* textSurface;
	if (!(textSurface = TTF_RenderText_Solid(gFont, textureText.c_str(), textColor))) {
		std::cout << "error creating text surface" << std::endl;
	}

	if (textSurface == NULL)
	{
		std::cout << "Unable to render text surface! SDL_ttf Error:\n" << TTF_GetError() << std::endl;
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

/*
drawImage:
Input: image file name and location, width and height of image
This function draws and image
*/
bool SDLApp::drawImage(std::string img_file, int x, int y, int mWidth, int mHeight) {
	int result = 1;

	SDL_Surface* optimizedSurface = NULL;
	SDL_Surface* loadedSurface = IMG_Load(img_file.c_str());
	if (loadedSurface == NULL)
	{
		std::cout << "Unable to load image: " << img_file << std::endl;
		result = 0;
	}
	else
	{
		//Convert surface to screen format
		optimizedSurface = SDL_ConvertSurface(loadedSurface, SDL_GetWindowSurface(this->myWindow)->format, NULL);
		if (optimizedSurface == NULL)
		{
			std::cout << "error optimizing image" << std::endl;
		}
		SDL_FreeSurface(loadedSurface);// get rid of old loaded surface
	}

	if (result == 1) {
		//Create texture from surface pixels
		mTexture = SDL_CreateTextureFromSurface(myRenderer, optimizedSurface);
		if (mTexture == NULL)
		{
			printf("Unable to create texture from rendered text! SDL Error: %s\n", SDL_GetError());
		}
		else
		{
			//Get image dimensions
			mWidth = optimizedSurface->w;
			mHeight = optimizedSurface->h;
		}
		const SDL_Rect imglocation = { 0,0,mWidth,mHeight };
		const SDL_Rect screenlocation = { x,y,mWidth,mHeight };
		const SDL_Rect *textloc = &imglocation;
		const SDL_Rect *screenloc = &screenlocation;

		//SDL_BlitSurface(textSurface, NULL, mySurface, NULL);
		SDL_RenderCopy(myRenderer, mTexture, textloc, screenloc);

		//perhaps we can reuse it, but I assume not for simplicity.
		//Get rid of old surface
		SDL_FreeSurface(optimizedSurface);
	}

	return result;
}

/*
drawTextArray:
This function draws the contents of an array from top to bottom
*/
void SDLApp::drawTextArray(std::string textureText, SDL_Color textColor, double array[], int size, int x, int y)
{
	for (int i = 0; i < (size - 1); i++) {
		int y1;
		y1 = y + fontheight * i;
		drawText(textureText + std::to_string(array[i]), textColor, x, y1);
	}
}

/*
drawTextVector:
This function draws the contents of a vector from top to bottom
*/
void SDLApp::drawTextVector(std::string textureText, SDL_Color textColor, std::vector<double> vector, int size, int x, int y)
{
	for (int i = 0; i < (size - 1); i++) {
		int y1;
		y1 = y + fontheight * i;
		drawText(textureText + std::to_string(vector.at(i)), textColor, x, y1);
	}
}

/*
showHelp:
This function is meant to abstract the code that is used to draw the Help screen
*/
void SDLApp::showHelp(SDL_Renderer *renderer, int width, int height) {
	std::cout << "executing" << std::endl;
	SDL_Color text_color = { 255,255,255 };
	clear(renderer);
	setBackColor(renderer, 0, 0, 0);
	drawText("HELP", text_color, 0, 0);
}

/*
showMenu:
This function is meant to abstract the code that is used to draw the main menu
*/
void SDLApp::showMainMenu(SDL_Renderer *renderer, int graphic_iter, int width, int height) {
	SDL_Color text_color = { 0, 0, 0 };

	int cycle = (int)(graphic_iter * 2 * PI / 100);

	clear(renderer);
	setBackColor(renderer, 192, 192, 192);
	int width1, width2;
	std::string str1 = "ATTENTION KEEPER";
	std::string str2 = "press enter";
	width1 = (int)(str1.length() * fontwidth);
	width2 = (int)(str2.length() * fontwidth);
	drawText("ATTENTION KEEPER", text_color, width / 2 - width1 / 4, height / 2 - 100);
	drawText("press enter", text_color, width / 2 - width2 / 4, (int)(height / 2 + 100 + cos(cycle - PI / 4) * 2));
}

/*
showMenu:
This function displays a subsidiary menu in which the user can choose items that will be highlighted
*/
void SDLApp::drawMenu(SDL_Renderer *renderer, std::vector<std::string> options, int sel, int x1, int y1) {

	SDL_Color text_color = { 0,0,0 };
	SDL_Color highlight = { 255,255,0 };

	int maxwidth, maxheight, xborder, yborder; //before drawing the menu, the max width and height of the menu system is needed
	maxwidth = 0;
	maxheight = 0;
	xborder = (int)((1 - _phi) * .5 * fontwidth); // borders used to make the text in the item boxes readable
	yborder = (int)((1 - _phi) * .5 * fontheight); // once again we use the golden ratio

	maxheight = TTF_FontHeight(gFont);

	for (unsigned int i = 0; i < options.size(); i++) {
		int curwidth;
		curwidth = (int)(options.at(i).length() * fontwidth);
		if (maxwidth < curwidth) { maxwidth = curwidth; }
	}

	for (unsigned int j = 0; j< options.size(); j++) { //drawing all the menu items
		int startx, starty; // current start x pos. and start y pos. for this menu item
		startx = x1;
		starty = y1 + (j * maxheight + yborder * 2);

		drawRectangle(renderer, startx, starty, maxwidth + (2 * xborder), maxheight + (2 * yborder), 255, 255, 255);
		if (j == sel) {
			drawRectangle(renderer, startx, starty, maxwidth + (2 * xborder), maxheight + (2 * yborder), 255, 255, 0);
		}
		drawText(options.at(j), text_color, startx + xborder, starty + yborder);
	}
}