/*
	@Meshach, for the plots we aren't interested in the individual channels. We are interested in the average of those channels
			  for that reason, I don't think you need your own set of variables. You can use the variables I have.
*/

//If any of these defines are changed, they need to be updated in Stimulation.cpp
#define SAMPLE_DURATION 32 // Number of times each channel is measured in a calculation interval. Roughly 60 seconds = 120 
#define MAX_LEN 128 // The number of 'sample_duration' intervals taken.
#define C_LEN 14 // number of channels... hard coded.
#define CAL_FREQ 5 // The number of 'sample_duration' intervals taken before the TBR Threshold value is updated again

#include <cerrno> //maybe we'll use this later
#include <conio.h>
#include <windows.h>
#include <iostream>
#include <string>
#include <fstream>

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "math.h"

#include "SDL.h"
#include "SDLApp.h"
#include "Plot.h"
#include "Stimulation.h"

#include "IEmoStateDLL.h"
#include "Iedk.h"
#include "IedkErrorCode.h"


//constant colors and screen width
static const int width = 800;
static const int height = 600;
SDL_Color WHITE = { 255,255,255 };
SDL_Color BLACK = { 0,0,0 };
SDL_Color RED = { 255,0,0 };
SDL_Color GREEN = { 0, 255,0 };
SDL_Color BLUE = { 0,0,255 };
int counter = 0;


int main(int argc, char *argv[])
{
	//creating the app and initializing it
	SDLApp app;
	std::cout << "red: theta | blue: beta" << std::endl;
	app.setWindow(SDL_CreateWindow("TBR Display Red=Theta Band Blue=Beta Band Green=Theta-Beta ratio", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, SDL_WINDOW_OPENGL));
	app.setRenderer(SDL_CreateRenderer(app.myWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC));
	app.setBackColor(app.myRenderer, 0, 0, 0);
	app.update(app.myRenderer);

	bool running = true;
	SDL_Event event;

	//creating the plot class
	Plot plot;
	plot.changeApp(&app);
	plot.LineGraph = true;
	std::cout << "created the plot class" << std::endl;


	//Beginning Emotiv Engine Code
	EmoEngineEventHandle eEvent = IEE_EmoEngineEventCreate();
	EmoStateHandle eState = IEE_EmoStateCreate();
	unsigned int userID = 0;
	bool ready = false;
	int state = 0;

	IEE_DataChannel_t channelList[] = { IED_AF3, IED_F7, IED_F3, IED_FC5, IED_T7, IED_P7, IED_O1,IED_O2,  IED_P8,  IED_T8, IED_FC6, IED_F4, IED_F8, IED_AF4 }; // EEG and system data channel description
	const char header[] = "Theta, Alpha, Low_beta, High_beta, Gamma";

	if (IEE_EngineConnect() != EDK_OK) {
		std::cout << "Emotiv Driver start up failed.";
		return -1;
	}


	int SamplesTaken = 0, history_counter = 0;
	const int& ah = history_counter; //This is a read only way to access history counter
	double alpha_history[MAX_LEN] = {}, low_beta_history[MAX_LEN] = {}, high_beta_history[MAX_LEN] = {}, gamma_history[MAX_LEN] = {}, theta_history[MAX_LEN] = {}, beta_history[MAX_LEN] = {};

	double theta_channels[14] = {}, beta_channels[14] = {};//variables [Meshach] added to keep track of the channels various values as they come in


	Stimulator AK; //Later we can experiment to see if a dynamic allocation would give us better performance. But the entire class is static.
	if (AK.Cal_Announcment(Stimulator::Start) != Stimulator::SUCCESS_START)
	{
		std::cout << "There was a critical error." << std::endl;
		return -1; 		//Error handling protocol can be implemented later
	}


	double alpha_int[SAMPLE_DURATION] = {}, low_beta_int[SAMPLE_DURATION] = {}, high_beta_int[SAMPLE_DURATION] = {}, gamma_int[SAMPLE_DURATION] = {}, theta_int[SAMPLE_DURATION] = {}, beta_int[SAMPLE_DURATION] = {};

	while (!_kbhit()) //Checks for key press
	{
		state = IEE_EngineGetNextEvent(eEvent); // Retrieve the next EmoEngine event. EDK_OK or EDK_NO_EVENT

		if (state == EDK_OK)
		{
			IEE_Event_t eventType = IEE_EmoEngineEventGetType(eEvent);
			IEE_EmoEngineEventGetUserId(eEvent, &userID);

			if (eventType == IEE_UserAdded) { //List of events: https://goo.gl/1irCQV
				std::cout << "User added" << std::endl; // A headset is connected.
				IEE_FFTSetWindowingType(userID, IEE_HAMMING); //List of Windowing Types: https://goo.gl/64EFbj (we can ask LaBerge to suggest a window function)
				ready = true;
			}
			//			double alpha_int[SAMPLE_DURATION] = {}, low_beta_int[SAMPLE_DURATION] = {}, high_beta_int[SAMPLE_DURATION] = {}, gamma_int[SAMPLE_DURATION] = {}, theta_int[SAMPLE_DURATION] = {}, beta_int[SAMPLE_DURATION] = {};

			if (ready)
			{
				double alpha = 0, low_beta = 0, high_beta = 0, gamma = 0, theta = 0;
				double alpha_sum = 0, low_beta_sum = 0, high_beta_sum = 0, gamma_sum = 0, theta_sum = 0, beta_sum = 0;

				for (int i = 0; i < sizeof(channelList) / sizeof(channelList[0]); ++i)
				{
					std::cout << "history counter" << ah << std::endl; //Removing unnecc references to history_counter. ah is a safer way to read only.
					std::cout << "samples taken" << SamplesTaken << std::endl;
					std::cout << "channel " << i << std::endl;

					int result = IEE_GetAverageBandPowers(userID, channelList[i], &theta, &alpha, // Import function to understand: https://goo.gl/NBQ8Mm
						&low_beta, &high_beta, &gamma);

					if (result == EDK_OK) {
						//updating the brainwave values and sums
						alpha_sum += alpha;
						low_beta_sum += low_beta;
						high_beta_sum += high_beta;
						gamma_sum += gamma;
						theta_sum += theta;
						beta_sum += (low_beta + high_beta);
						//
						theta_channels[i] = theta; //realtime values for the theta and beta values
						beta_channels[i] = low_beta + high_beta; //realtime values for the theta and beta values

						if (i == (C_LEN - 1)) //Processed all the channels
						{
							alpha_int[SamplesTaken] = alpha_sum;
							low_beta_int[SamplesTaken] = low_beta_sum;
							high_beta_int[SamplesTaken] = high_beta_sum;
							gamma_int[SamplesTaken] = gamma_sum;
							theta_int[SamplesTaken] = theta_sum;
							beta_int[SamplesTaken] = beta_sum;
							SamplesTaken++;
						}

						if (SamplesTaken == SAMPLE_DURATION) //Sample Duration may be off be 1 for all them
						{
							std::cout << "updating history" << std::endl;

							alpha = 0; low_beta = 0; high_beta = 0;  gamma = 0; theta = 0; beta_sum = 0;


							for (int j = 0; j < SAMPLE_DURATION; j++)
							{
								alpha += alpha_int[j];
								high_beta += high_beta_int[j];
								low_beta += low_beta_int[j];
								theta += theta_int[j];
								gamma += gamma_int[j];
							}

							memset(alpha_int, 0, sizeof(alpha_int));
							memset(low_beta_int, 0, sizeof(low_beta_int));
							memset(high_beta_int, 0, sizeof(high_beta_int));
							memset(theta_int, 0, sizeof(theta_int));
							memset(gamma_int, 0, sizeof(gamma_int));
							SamplesTaken = 0;

							if (AK.initCalibration == false)
							{
								const double beta = low_beta + high_beta;
								const double init_threshold = (theta / SAMPLE_DURATION) / (beta / SAMPLE_DURATION);
								AK.setBand(Stimulator::Threshold, init_threshold);
								AK.initCalibration = true;
								if (AK.Cal_Announcment(Stimulator::End) != Stimulator::SUCCESS_END)
								{
									std::cout << "There was a critical error." << std::endl;
									return -1; 		//Error handling protocol can be implemented later
								}
								continue;
							}


							AK.setTBR(alpha, high_beta, low_beta, theta, gamma, ah);
							history_counter++;

							if (ah == MAX_LEN)
							{
								/* //Commeneted out this block as I don't think it serves any purpose.
									history_counter = 0; // Why would we reset this value if we're going to break?
									//resetting the history values
									for (int i = 0; i < MAX_LEN; i++) { //What does this accomplish?
										alpha_history[ah] = 0;
										low_beta_history[ah] = 0;
										high_beta_history[ah] = 0;
										theta_history[ah] = 0;
										gamma_history[ah] = 0;
										beta_history[ah] = 0;
									}
								*/
								break; //This seems like the only pertinent line
							}
						}

						app.clear(app.myRenderer);
						app.setBackColor(app.myRenderer, 0, 0, 0);
						//labeling test

						//app.drawText(app.mySurface, "test", 15, 50, 50, WHITE, BLACK);
						plot.drawTitle("theta-beta ratio plot", WHITE);
						plot.plotPoints(theta_channels, 14, width, height, RED);
						plot.plotPoints(beta_channels, 14, width, height, BLUE);
						plot.textplotPoints(theta_history, "t", MAX_LEN, width, height, 0, WHITE);
						plot.textplotPoints(low_beta_history, "bl", MAX_LEN, width, height, 16, WHITE);
						plot.textplotPoints(high_beta_history, "bh", MAX_LEN, width, height, 32, WHITE);
						plot.update();

						app.update(app.myRenderer);

					}
				}
			}

			// running the SDL code for displaying content outside of the EMOTIV loop
			if (running)// originally a while statement
			{

				// Process events
				if (SDL_PollEvent(&event))// originally a while statement
				{
					if (event.type == SDL_QUIT) { running = false; }
				}

			}

			//
		}
		else if (running) {
			//THE FOLLOWING STATEMENTS WILL BE EXECUTED IF THE PROGRAM DOES NOT CONNECT WITH THE EMOTIV

			/* TESTS !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
			debug test to check if the plotting functionality works
			int testarray[50] = {};
			for (int i = 0; i < 50; i++) {
			testarray[i] = (5)*exp(i / 5)*pow(-1, i)*cos(counter);
			counter += 1;
			cout << cos(i) << endl;
			}
			app.clear(app.myRenderer);
			app.setBackColor(app.myRenderer, 0, 0, 0);
			plot.plotPoints(testarray, 50, width, height,RED);
			app.update(app.myRenderer);
			!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
		}
		//outside of the emotiv loop following code will run regardless of whether emotiv is running


		//app.update(app.myRenderer);

	}// No longer capturing data



	IEE_EngineDisconnect();
	IEE_EmoStateFree(eState);
	IEE_EmoEngineEventFree(eEvent);


	for (int k = 0; k < history_counter; k++)
	{
		std::cout << "Program terminating..." << std::endl;
		std::cout << "Interval: " << k << "\tTheta:\t" << theta_history[k] << "\tBeta:\t" << beta_history[k] << std::endl;
	}

	app.appEnd(app.myRenderer, app.myWindow);

	std::cout << "Sleeping for 30 seconds while you read the data" << std::endl;
	Sleep(30000);

	return 0;
}
