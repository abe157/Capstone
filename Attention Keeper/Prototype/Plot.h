#ifndef PLOT_H
#define PLOT_H

#include "SDL.h"
#include "SDLApp.h"
#include "SDL_ttf.h"
#include <string>

class Plot {

public:
	SDL_Renderer * myRenderer; /*this is a pointer to the renderer the plot class will interface with
							   it will connect the plot class to the SDLApp class */
	SDLApp *myApp;

	int *points; // pointer to the points, may change this to private later
	bool LineGraph; //whether or not the plot is functioning as a Line Graph


	Plot();
	void addPoint(int value, int time); //changes the current value of the plot and moves it forward in time
	void shiftPoints();
	void plotPoints(double points[], int size, int tot_width, int tot_height, SDL_Color color);
	void textplotPoints(double points[], std::string label, int size, int tot_width, int tot_height, int offset, SDL_Color color);
	//shift current points to display a function of time
	void changeApp(SDLApp *reference); // will reference the app class to easily draw lines etc.
	void drawTitle(std::string title, SDL_Color color);
	void update();

};

#endif
