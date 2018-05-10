#ifndef STIMULATOR_H
#define STIMULATOR_H

class Stimulator
{
private:
	double Theta_Beta_Ratio = 0;
	double TBR_THRESHOLD = 0;
	double Threshold_History[(MAX_LEN/CAL_FREQ)+1] = { 0 };
	int Threshold_Counter = 0; //Indexing of Threshold_History
	int Theta_Beta_Counter = 0; // Indexing of Theta_Beta_History
	double Theta_Beta_History[MAX_LEN] = { 0 };
	int Freq = 0; //frequency of binaural beat that should be played
	bool BeatPlaying = false;
	
	double alpha_history[MAX_LEN] = { 0 };
	double low_beta_history[MAX_LEN] = { 0 };
	double high_beta_history[MAX_LEN] = { 0 };
	double gamma_history[MAX_LEN] = { 0 };
	double theta_history[MAX_LEN] = { 0 };
	double beta_history[MAX_LEN] = { 0 };
	/*Perhaps a 2d array that includes values of frequencies and a rating of how significant they were on the user.*/

public:
	enum Annoucment_Phase { Start, End };
	enum Announcment_Success { SUCCESS_START, FAIL_START, SUCCESS_END, FAIL_END, INVALID };
	enum Brain_Band { Alpha, High_Beta, Low_Beta, Beta, Gamma, TBR, Theta, Threshold };
	bool initCalibration = false;
	Stimulator(void);
	void setTBR(const double Alpha, const double hBeta, const double lBeta, const double Gamma, const double Theta, const int counter);
	double getBand(const Brain_Band band);
	int setBand(const Brain_Band band, const double value);
	void setFreq(const int value);
	int getFreq(void);
	int playFreq(const int seconds);
	Announcment_Success Cal_Announcment(const Annoucment_Phase phase);
};




#endif