#include <iostream>
#include <iostream>
#include <string>
#include "Plot.h"
#include "SDLApp.h"

using namespace std;

Plot::Plot() {
	points = NULL;
}

void Plot::addPoint(int value, int time) {
}

void Plot::shiftPoints() {
}

//the parameters tot_width and tot_height are the width and height of the module/app respectively
void Plot::plotPoints(double points[], int size, int tot_width, int tot_height,SDL_Color color) {

	if (myApp == NULL) {
		cout << "Error,Plot::update>: Trying to plot when there is no app target" << endl;
	}
	else {
		for (int i = 0; i < (size - 1); i++) {
			int x1, x2, y1, y2;
			x1 = (tot_width / size)*i;
			x2 = (tot_width / size)*(i + 1);
			y1 = -1 * (points[i]) + (tot_height / 2);
			y2 = -1 * (points[i + 1]) + (tot_height / 2);
			myApp->drawLine(myApp->myRenderer, x1, y1, x2, y2, color);
		}
	}
}

//the parameters tot_width and tot_height are the width and height of the module/app respectively
void Plot::textplotPoints(double points[],std::string label, int size, int tot_width, int tot_height,int offset, SDL_Color color) {

	if (myApp == NULL) {
		cout << "Error,Plot::update>: Trying to plot when there is no app target" << endl;
	}
	else {
		for (int i = 0; i < (size - 1); i++) {
			int x1, x2, y1, y2;
			x1 = (tot_width / size)*i;
			x2 = (tot_width / size)*(i + 1);
			y1 = -1 * (points[i]) + (tot_height / 2);
			y2 = -1 * (points[i + 1]) + (tot_height / 2);
			myApp->drawText(label + std::to_string(i), color, x1+offset, y1);
		}
	}
}

void Plot::changeApp(SDLApp *reference) {
	myApp = reference;
}

void Plot::drawTitle(std::string title,SDL_Color color) {
	if (myApp == NULL) {
		cout << "Error,Plot::update>: Trying to plot when there is no app target" << endl;
	}
	else {
		myApp->drawText(title, color, myApp->screenWidth / 2, 10); ///just using ten as a value that is at the top but not 0
		//cout << myApp << endl;
	}
}

void Plot::update() {
	if (myApp == NULL) {
		cout << "Error,Plot::update>: Trying to plot when there is no app target" << endl;

	}else{
		if (LineGraph) {
			int h, w;
			h = myApp->screenHeight;
			w = myApp->screenWidth;
			SDL_Color grey = { 215,215,215 };
			myApp->drawLine(myApp->myRenderer, 0, h / 2, w, h / 2, grey);
			myApp->drawLine(myApp->myRenderer, w / 2, 0, w / 2, h, grey);
		}
	}
}
