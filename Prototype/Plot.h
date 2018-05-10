/*
The plot.cpp and plot.h look good. But I think
we should avoid having extensive comments in the header file.
let's stick standard comments in the source file.
I've been using Doxygen style (https://www.stack.nl/~dimitri/doxygen/manual/docblocks.html)
and I think we should stay consistent.
*/
#ifndef PLOT_H
#define PLOT_H

#include "SDL.h"
#include "SDLApp.h"
#include "SDL_ttf.h"
#include "Stimulation.h"
#include <string>
#include <vector>

enum menuStages { MAIN, MONITOR, VISUALS, TESTCONTROL, TEST, HELP }; // 0 main menu
enum MODE { DEBUG_AK, PLOTS, TRIAL }; // MODE ENUM THAT CONTROLS WHAT THE USER IS SHOWN
int getKeyBoard();

class Plot {

public:
	SDL_Renderer * myRenderer; /*this is a pointer to the renderer the plot class will interface with
							   it will connect the plot class to the SDLApp class */
	SDLApp *myApp;

	int *points; // pointer to the points, may change this to private later
	bool LineGraph; //whether or not the plot is functioning as a Line Graph


	Plot();
	Plot(SDLApp* app);

	void addPoint(int value, int time); //changes the current value of the plot and moves it forward in time
	void shiftPoints();
	void plotPoints(double points[], int size, int tot_width, int tot_height, SDL_Color color);
	void plotVectorPoints(std::vector<double> points, int tot_width, int tot_height, SDL_Color color);

	void TBRplot(Stimulator* data, int tot_width, int tot_height);
	void textplotPoints(double points[], std::string label, int size, int tot_width, int tot_height, int offset, SDL_Color color);
	//shift current points to display a function of time
	void changeApp(SDLApp *reference); // will reference the app class to easily draw lines etc.
	void drawTitle(std::string title, SDL_Color color);
	void update();
	void adjustOffsets(int x1, int y1);

private:
	int xoffset;
	int yoffset;


};

#endif
