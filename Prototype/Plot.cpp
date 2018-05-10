#include <iostream>
#include <iostream>
#include <string>
#include "Plot.h"
#include "SDLApp.h"
#include <conio.h>  

/* getKeyboard:
funtion an integer that represents the key pressed*/
int getKeyBoard()
{
	if (_kbhit()) {
		return (int)_getch(); //this won't work outside of the console. Must use global hooks or the SDL key events.
	}
	else {
		return -1;
	}
}

Plot::Plot() {
	points = NULL;
	xoffset = 0;
	yoffset = 200;
	LineGraph = true;
}

Plot::Plot(SDLApp* app) {
	myApp = app;
	LineGraph = true;
}

void Plot::addPoint(int value, int time) {
}

void Plot::shiftPoints() {
}


/*
* plotPoints
* Purpose: plots the points in an array
* Note:
*/
//the parameters tot_width and tot_height are the width and height of the module/app respectively
void Plot::plotPoints(double points[], int size, int tot_width, int tot_height, SDL_Color color) {

	if (myApp == NULL) {
		std::cout << "Error,Plot::update>: Trying to plot when there is no app target" << std::endl;
	}
	else {
		for (int i = 0; i < (size - 1); i++) {
			int x1, x2, y1, y2;
			x1 = (tot_width / size)*i;
			x2 = (tot_width / size)*(i + 1);
			y1 = int(-1 * (points[i]) + (tot_height / 2));
			y2 = int(-1 * (points[i + 1]) + (tot_height / 2));
			myApp->drawLine(myApp->myRenderer, x1 + xoffset, y1 + yoffset, x2 + xoffset, y2 + yoffset, color);
		}
	}
}

/*
* plotVectorPoints
* Purpose: plots the points in a vector
* Note:
*/
void Plot::plotVectorPoints(std::vector<double> points, int tot_width, int tot_height, SDL_Color color) {
	if (myApp == NULL) {
		std::cout << "Error,Plot::update>: Trying to plot when there is no app target" << std::endl;
	}
	else {
		for (unsigned int i = 0; i < (points.size() - 1); i++) {
			int x1, x2, y1, y2;
			x1 = (int)((tot_width / points.size())*i);
			x2 = (int)((tot_width / points.size())*(i + 1));
			y1 = (int)(-1 * (points.at(i)) + (tot_height / 2));
			y2 = (int)(-1 * (points.at(i + 1)) + (tot_height / 2));
			myApp->drawLine(myApp->myRenderer, x1 + xoffset, y1 + yoffset, x2 + xoffset, y2 + yoffset, color);
		}
	}
}

/*
* plotVectorPoints
* Purpose: does all the theta beta plotting once a reference to the stimulator object is given
* Note:
*/
void Plot::TBRplot(Stimulator* data, int tot_width, int tot_height) {

}

//the parameters tot_width and tot_height are the width and height of the module/app respectively
/*
* textplotPoints
* Purpose: plots data using a label text as a marker instead of a line
* Note:
*/
void Plot::textplotPoints(double points[], std::string label, int size, int tot_width, int tot_height, int offset, SDL_Color color) {

	if (myApp == NULL) {
		std::cout << "Error,Plot::update>: Trying to plot when there is no app target" << std::endl;
	}
	else {
		for (int i = 0; i < (size - 1); i++) {
			int x1, x2, y1, y2;
			x1 = (tot_width / size)*i;
			x2 = (tot_width / size)*(i + 1);
			y1 = int(-1 * (points[i]) + (tot_height / 2));
			y2 = int(-1 * (points[i + 1]) + (tot_height / 2));
			myApp->drawText(label + std::to_string(i), color, x1 + offset + xoffset, y1 + yoffset);
		}
	}
}

/*
* changeApp
* Purpose: changes the rendering class that the plot class references
* Note:
*/
void Plot::changeApp(SDLApp *reference) {
	myApp = reference;
}

/*
* drawTitle
* Purpose: draws title for the plot (rarely used)
* Note:
*/
void Plot::drawTitle(std::string title, SDL_Color color) {
	if (myApp == NULL) {
		std::cout << "Error,Plot::update>: Trying to plot when there is no app target" << std::endl;
	}
	else {
		myApp->drawText(title, color, myApp->screenWidth / 2, 10); ///just using ten as a value that is at the top but not 0
																   //cout << myApp << endl;
	}
}

/*
* update
* Purpose: draws plot axis, unnecessary function
* Note:
*/
void Plot::update() {
	if (myApp == NULL) {
		std::cout << "Error,Plot::update>: Trying to plot when there is no app target" << std::endl;

	}
	else {
		if (LineGraph) {
			int h, w;
			h = myApp->screenHeight;
			w = myApp->screenWidth;
			SDL_Color grey = { 215,215,215 };
			myApp->drawLine(myApp->myRenderer, 0, h / 2 + yoffset, w, h / 2 + yoffset, grey);
			myApp->drawLine(myApp->myRenderer, w / 2 + xoffset, 0, w / 2 + xoffset, h, grey);
		}
	}
}

/*
* adjustOffsets
* Purpose: adjust the relative position of the plots
* Note:
*/
void Plot::adjustOffsets(int x1, int y1) {
	xoffset = x1;
	yoffset = y1;
	//these values well be used to shift the axis for plot functions
}
