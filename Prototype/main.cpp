//is decibel diviion the same????

#define RESPONSE_TEST 1
#include "Plot.h"
#include "Stimulation.h"
#include "IEmoStateDLL.h"
#include "Iedk.h"
#include "IedkErrorCode.h"
#include "SDL_keyboard.h"
#include "ResponseTest.h" 

/*
@Meshach, don't use standard words like 'DEBUG'. They are typically already used. 
It can cause big problems that are hard to identify.
In the previous versions of the code, 'DEBUG' was defaulting to a macro outside of your enumeration 'MODE'.
*/
static const int width = 800, height = 600; //constant colors and screen width
SDL_Color WHITE = { 255,255,255 }, BLACK = { 0,0,0 }, RED = { 255,0,0 }, GREEN = { 0, 255,0 }, BLUE = { 0,0,255 };
int counter = 0;


int main(int argc, char *argv[])
{
	if (RESPONSE_TEST)
	{
	if (response_verification() != 0 )
	{
	std::cerr << "Verification task for response was a failure. Termininating early.\n";
	return -1;
	}
	std::cout << "Verification task for response was a success. See Response_Test_Output.txt" << std::endl;
	}

	return 0;
	
	AK_Audio^ audio_obj = gcnew AK_Audio;
	Stimulator AK;

	SDLApp app;

	Uint32 flags = SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_SHOWN; // 

	app.setWindow(SDL_CreateWindow("AK", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, flags));
	//app.setWindow(SDL_CreateWindow("AK", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, SDL_WINDOW_OPENGL));
	app.setRenderer(SDL_CreateRenderer(app.myWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC));
	app.setBackColor(app.myRenderer, 0, 0, 0);
	app.update(app.myRenderer);

	Plot plot(&app); //creating the plot class

	int SamplesTaken = 0;
	int history_counter = 0;
	const int& ah = history_counter; //This is a read only way to access history counter
	int graphic_iter = 0;
	double theta_channels[C_LEN] = {}, beta_channels[C_LEN] = {}; //variables [Meshach] added to keep track of the channels various values as they come in
	double alpha_history[MAX_LEN] = {}, low_beta_history[MAX_LEN] = {}, high_beta_history[MAX_LEN] = {}, gamma_history[MAX_LEN] = {}, theta_history[MAX_LEN] = {}, beta_history[MAX_LEN] = {};
	double alpha_int[SAMPLE_DURATION] = {}, low_beta_int[SAMPLE_DURATION] = {}, high_beta_int[SAMPLE_DURATION] = {}, gamma_int[SAMPLE_DURATION] = {}, theta_int[SAMPLE_DURATION] = {}, beta_int[SAMPLE_DURATION] = {};


	EmoEngineEventHandle eEvent = IEE_EmoEngineEventCreate();
	EmoStateHandle eState = IEE_EmoStateCreate();
	unsigned int userID = 0;
	bool ready = false;
	int state = 0;
	IEE_DataChannel_t channelList[] = { IED_AF3, IED_F7, IED_F3, IED_FC5, IED_T7, IED_P7, IED_O1,IED_O2,  IED_P8,  IED_T8, IED_FC6, IED_F4, IED_F8, IED_AF4 }; // EEG and system data channel description
	const char header[] = "Theta, Alpha, Low_beta, High_beta, Gamma";

	// functions for the game 
	bool appRunning = true;
	bool running = true;

	if (IEE_EngineConnect() != EDK_OK) {
		std::cerr << "Emotiv Driver start up failed. See Emotiv Documentation." << std::endl;
		return -1;
	}

	if (AK.Cal_Announcment(Stimulator::Start) != Stimulator::SUCCESS_START) //Cal_Announcment function still needs to be completedThe audio is still missing. [IMPORTANT]
	{
		std::cerr << "There was a critical error.\n";
		return -1; 		//Error handling protocol can be implemented later
	}



	menuStages menuStage = MAIN;
	int usersel1 = 0, usersel2 = 0; //integer representing the user's first and second selection in a stage
	int timeDisconnected = 0;
	SDL_Keycode keyPressed;
	MODE RUNMODE = MODE::DEBUG_AK; // CHANGE THIS LINE FOR TESTING PURPOSES

	SDL_Event localEvent;

	while (appRunning == true) // Critically important
	{
		keyPressed = NULL;
		(graphic_iter > 100) ? graphic_iter = 0 : graphic_iter++;

		SDL_PollEvent(&localEvent);

		if (localEvent.type == SDL_KEYDOWN && localEvent.key.repeat == 0) // key press detected
		{
			keyPressed = localEvent.key.keysym.sym;
		}


		if (keyPressed == SDLK_ESCAPE) { appRunning = false; }

		if (menuStage == MAIN) {
			std::string choice0 = "monitor";
			std::string choice1 = "visuals";
			std::string choice2 = "test control";
			std::string choice3 = "test";
			std::string choice4 = "help";
			std::vector<std::string> mainoptions; //string vector for the options in this stage
			mainoptions.push_back(choice0);
			mainoptions.push_back(choice1);
			mainoptions.push_back(choice2);
			mainoptions.push_back(choice3);
			mainoptions.push_back(choice4);

			int cycle = (int)(graphic_iter * 2 * PI / 100);
			app.showMainMenu(app.myRenderer, graphic_iter, width, height);
			app.drawMenu(app.myRenderer, mainoptions, usersel1, (int)(width * .5 * (1 - _phi)), (int)(height * _phi));
			app.drawImage("AKlogo.PNG", width / 2 - 40, height / 8, 50, 50);

			// if enter is pressed, we move to another menu stage
			if (SDLK_KP_ENTER == K_ENTER) //case statement based on the user's
			{
				menuStage = (menuStages)(usersel1 + 1); //plus 1 is because we disregard the main option
														//directly casting back to a menu stage option to avoid using a case statement
			}
			else if (keyPressed == SDLK_RIGHT) { menuStage = menuStages::MONITOR; }
			else if (keyPressed == SDLK_UP) {
				usersel1--;
				if (usersel1 < 0) { usersel1 = 0; }
			}
			else if (keyPressed == SDLK_DOWN) {
				usersel1++;
				if (usersel1 > 4) { usersel1 = 4; }
			}

		}
		else if (menuStage == menuStages::HELP) {
			app.showHelp(app.myRenderer, width, height);

			if (keyPressed == SDLK_LEFT) { menuStage = MAIN; }

		}
		else if (menuStage == menuStages::TESTCONTROL) {
			app.clear(app.myRenderer);
			app.setBackColor(app.myRenderer, 0, 0, 0);

		}
		else if (menuStage == menuStages::TEST or menuStage == menuStages::MONITOR or menuStage == menuStages::VISUALS)
		{

			// change parameters here based on whether this is a test, monitoring or visuals

			state = IEE_EngineGetNextEvent(eEvent); // Retrieve the next EmoEngine event. EDK_OK or EDK_NO_EVENT

			if (state == EDK_OK)
			{
				timeDisconnected = 0;
				/* only clear screen the emotiv is connected or has been disconnected for a long time*/
				app.clear(app.myRenderer);
				app.setBackColor(app.myRenderer, 0, 0, 0);

				IEE_Event_t eventType = IEE_EmoEngineEventGetType(eEvent);
				IEE_EmoEngineEventGetUserId(eEvent, &userID);

				if (eventType == IEE_UserAdded) { //List of events: https://goo.gl/1irCQV
					std::cout << "User added" << std::endl; // A headset is connected.
					IEE_FFTSetWindowingType(userID, IEE_HAMMING); //List of Windowing Types: https://goo.gl/64EFbj (we can ask LaBerge to suggest a window function)
					ready = true;
				}


				if (ready)
				{
					double alpha = 0, low_beta = 0, high_beta = 0, gamma = 0, theta = 0;
					double alpha_sum = 0, low_beta_sum = 0, high_beta_sum = 0, gamma_sum = 0, theta_sum = 0, beta_sum = 0;

					for (int i = 0; i < sizeof(channelList) / sizeof(channelList[0]); ++i)
					{
						int result = IEE_GetAverageBandPowers(userID, channelList[i], &theta, &alpha,
							&low_beta, &high_beta, &gamma);

						if (result == EDK_OK) {	//updating the brainwave values and sums
							alpha_sum += alpha;
							low_beta_sum += low_beta;
							high_beta_sum += high_beta;
							gamma_sum += gamma;
							theta_sum += theta;
							beta_sum += (low_beta + high_beta);

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

							if (SamplesTaken == SAMPLE_DURATION) //Sample Duration may be off by 1 for all them
							{
								std::cout << "Samples taken matches sample duration. Updating TBR." << std::endl;
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

								if (AK.initCalibration == false) //In calibration mode
								{
									const double beta = low_beta + high_beta;

									if (CAL_CYCLES == 1) //Direct questions to @Shawn
									{
										AK.setBand(Stimulator::Threshold, (theta / beta) / C_LEN);
										if (AK.Cal_Announcment(Stimulator::End) != Stimulator::SUCCESS_END) // sets this->initCalibration = true;
										{
											std::cerr << "There was a critical error in calibration." << std::endl;
											return -1; 		//Error handling protocol can be implemented later
										}
										continue;
									}

									else //Direct questions to @Meshach
									{
										const double cur_threshold = AK.getBand(Stimulator::Threshold); //0???
										const double init_threshold = (theta / SAMPLE_DURATION) / (beta / SAMPLE_DURATION);
										AK.setBand(Stimulator::Threshold, cur_threshold + (init_threshold / (CAL_CYCLES)));
										// THE THRESHOLD WILL BE AN AVERAGE OF THE VALUES COLLECTED DURING CALIBRATION PERIODS
										std::cout << "threshold update" << std::endl;
										std::cout << "current threshold: " << std::to_string(cur_threshold) << std::endl;
										std::cout << "new threshold component: " << std::to_string(init_threshold / CAL_CYCLES) << std::endl;
										if ((AK.CalPeriods) >= (CAL_CYCLES - 1))
										{
											if (AK.Cal_Announcment(Stimulator::End) != Stimulator::SUCCESS_END)
											{
												std::cout << "There was a critical error." << std::endl;
												return -1; 		//Error handling protocol can be implemented later
											}
											AK.CalPeriods++;
										}
									}
								}

								else if (AK.initCalibration == true) // Without this you have a bug when CAL_CYCLES > 1
								{
									AK.setTBR(alpha, high_beta, low_beta, gamma, theta, ah); //normal update
									//void setTBR(const double Alpha, const double hBeta, const double lBeta, const double Gamma, const double Theta, const int counter);
									history_counter++;
									AK.evaluate(audio_obj); /* Important is an understatement*/
								}

								if (ah == MAX_LEN) { break; }
							}

							if (AK.initCalibration == false) {
								app.drawText("Running Calibration", WHITE, width / 2 - 50, height / 2 - 100);
								app.drawLoadCircle(app.myRenderer, int(width / 2), int(height / 2), int(graphic_iter*3.6), 10, 255, 255, 255);
								app.drawRectangle(app.myRenderer, 10, 10, width - 20, 15, 30, 30, 30);
								app.drawRectangle(app.myRenderer, 10, 10, ((width - 20) / CAL_CYCLES)*AK.CalPeriods, 15, 0, 255, 0);
							}

							else { // IF NOT CALIBRATING ->
								if (RUNMODE == MODE::DEBUG_AK) {
									app.drawTextVector("TBR_hist", WHITE, AK.Theta_Beta_plot_data, (int)AK.Theta_Beta_plot_data.size(), 10, 10);

									// theta beta ratio based on sampled values
									app.drawText("Threshold (Calibration):" + std::to_string(AK.getBand(Stimulator::Threshold)), WHITE, 430, 500);
									app.drawText("Calibration Periods:" + std::to_string(AK.CalPeriods), WHITE, 330, 530);
									plot.plotPoints(theta_channels, C_LEN, width, height, RED);
									plot.plotPoints(beta_channels, C_LEN, width, height, BLUE);
									plot.update();
									// drawing information related to Stimulator
									app.drawText("Calibration Complete" + std::to_string(AK.initCalibration), WHITE, 40, 560);
									//show whether or not the audio should be playing
									app.drawText("Audio Playing:" + std::to_string(AK.BeatPlaying), WHITE, 200, 560);
								}
								else if (RUNMODE == MODE::PLOTS) {
									plot.plotVectorPoints(AK.Theta_Beta_plot_data, width, height, GREEN);
									plot.plotVectorPoints(AK.beta_plot_data, width, height, BLUE);
									plot.plotVectorPoints(AK.theta_plot_data, width, height, RED);
								}
								else if (RUNMODE == MODE::TRIAL) {
									;//
								}
								// UPDATES SCREEN REGARDLESS OF RUNMODE
								app.update(app.myRenderer); //Why is this line here? 
							}
						}
					}
				}

				app.update(app.myRenderer);
			}
			else if (running) { // Why are you using 'running'? Why aren't you using 'ready'? 'running' is always true.
				timeDisconnected++; // keeps track of of how long the Emotiv has been disconnected
				if (timeDisconnected > 100) {
					app.clear(app.myRenderer);
					app.setBackColor(app.myRenderer, 0, 0, 0);
					app.drawText("Not connected with the EMOTIV", WHITE, width / 2 - 100, height / 2);
				}
			} // any code below will run regardless of whether emotiv is running

			std::cout << "reached this point" << keyPressed << std::endl;

			if (keyPressed == K_LEFT) { menuStage = menuStages::MAIN; }

			// * THIS MAY BE A GOOD PLACE TO PUT CODE FOR THE TEST (IF NECESSARY)  !!!!!!

		} // updating of the screen regardless of mode, should only be done here at the end of the loop
		app.update(app.myRenderer);
		Sleep(1);
	}

	IEE_EngineDisconnect(); //ENDING THE APPLICATION
	IEE_EmoStateFree(eState);
	IEE_EmoEngineEventFree(eEvent);

	app.appEnd(app.myRenderer, app.myWindow);

	return 0;
}