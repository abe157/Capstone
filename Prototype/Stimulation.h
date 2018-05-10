#ifndef STIMULATOR_H
#define STIMULATOR_H
#include <iostream>
#include <windows.h>
#include <iomanip>
#include <vector>
#include "Float_Compare.h"
#include "AK_Audio.cpp"

class Stimulator
{
private:
	double Theta_Beta_Ratio = 0;
	double TBR_THRESHOLD = 0;
	double alpha_history[MAX_LEN] = { 0 };
	double beta_history[MAX_LEN] = { 0 };
	double gamma_history[MAX_LEN] = { 0 };
	double high_beta_history[MAX_LEN] = { 0 };
	double low_beta_history[MAX_LEN] = { 0 };
	double Theta_Beta_History[MAX_LEN] = { 0 };
	double theta_history[MAX_LEN] = { 0 };
	double Threshold_History[(MAX_LEN / CAL_FREQ) + 1] = { 0 }; // Why is this the only private one???

	int Threshold_Counter = 0; //Indexing of Threshold_History
	int Theta_Beta_Counter = 0; // Indexing of Theta_Beta_History
	int Freq = 0;

public:
	enum Announcment_Phase { Start, End }; //Also used for audio
	enum Announcment_Success { SUCCESS_START, FAIL_START, SUCCESS_END, FAIL_END, INVALID };
	enum Brain_Band { Alpha, Beta, High_Beta, Low_Beta, Gamma, TBR, Theta, Threshold, Frequency };
	enum Evaluation_Result {
		CRIT_EVAL_ERR, FALSE_INSUFF_DATA, FALSE_OFF, FALSE_IN_PROGRESS, TRUE_STARTED, TRUE_IN_PROGRESS, FALSE_TURNING_OFF
	};
	enum State { Monitor, Play, Calibrate };

	std::vector<double> alpha_plot_data;
	std::vector<double> beta_plot_data;
	std::vector<double> high_beta_plot_data;
	std::vector<double> low_beta_plot_data;
	std::vector<double> gamma_plot_data;
	std::vector<double> Theta_Beta_plot_data;
	std::vector<double> theta_plot_data;
	std::vector<double> Threshold_plot_data;

	bool initCalibration = false;
	bool BeatPlaying = false;
	int CalPeriods = 0;
	State AK_State = Calibrate;

	Announcment_Success Cal_Announcment(const Announcment_Phase phase);
	Evaluation_Result evaluate(AK_Audio^% audio_obj);
	double getBand(const Brain_Band band);
	int getBandList(const Brain_Band band, std::vector<double>& list);
	int setBand(const Brain_Band band, const double value);
	void resetTBRHistory(int &counter);
	void setTBR(const double Alpha, const double hBeta, const double lBeta, const double Gamma, const double Theta, const int counter);
	Stimulator(void);
	void resetTBRCounter(void);
};
#endif

/*Perhaps a 2d array that includes values of frequencies and a rating of how significant they were on the user.*/
//enum User_State {Focused,Unfocused}; //may add this in later

/*
//#define SAMPLE_DURATION 10 // Number of times each channel is measured in a calculation interval. Roughly 60 seconds = 120
//#define MAX_LEN 32 // The number of 'sample_duration' intervals taken.
//#define C_LEN 14 // number of channels... hard coded.
//#define CAL_FREQ 5 // The number of 'sample_duration' intervals taken before the TBR Threshold value is updated again
//#define UPDATE_FREQ 5 // The minimum number of sample intervals before prediction is made on attention
//#define CAL_CYCLES 5
*/
//#include <math.h>
//#include <string>
//#include <iomanip>