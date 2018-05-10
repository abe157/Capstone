//This is a sloppy way to use these pre-processor directives. Requirees them to match up with main.cpp
//Need to incorporate them into VS, or I need to switch over to global variables for these values.
#define SAMPLE_DURATION 32 // Number of times each channel is measured in a calculation interval. Roughly 60 seconds = 120 
#define MAX_LEN 128 // The number of 'sample_duration' intervals taken.
#define C_LEN 14 // number of channels... hard coded.
#define CAL_FREQ 5 // The number of 'sample_duration' intervals taken before the TBR Threshold value is updated again

#include <iostream>
#include <iomanip>
#include "Stimulation.h"
#include <windows.h>

Stimulator::Stimulator(void)
{}

/**
* getBand
* Purpose: Return the power of a specific brain waveband
* Input:  Alpha, High_Beta, Low_Beta, Beta, Gamma, TBR, Theta, Threshold
* Output: Power value on success. Returns less than 0 on error.
*/
double Stimulator::getBand(const Brain_Band band)
{
	const int i = this->Theta_Beta_Counter;
	switch (band) {
	case Alpha:   return this->alpha_history[i];
	case High_Beta: return this->high_beta_history[i];
	case Low_Beta:  return this->low_beta_history[i];
	case Gamma:		return this->gamma_history[i];
	case TBR:	return this->Theta_Beta_Ratio;
	case Theta:	return this->theta_history[i];
	case Threshold: return this->TBR_THRESHOLD;
	default:	return -1;
	}

}

/**
* setBand
* Purpose: Sets the measured power of a specific brain waveband or value with a specific value
* Input:  Alpha, High_Beta, Low_Beta, Beta, Gamma, TBR, Theta, Threshold ; Power value
* Output: Power value on success. Returns less than 0 on error.
* Note: Use with extreme caution. Erroneous use will cause logical errors and desynchronization.
*		The only regular use of this function should be with the input Threshold
*/
int Stimulator::setBand(const Brain_Band band, const double value)
{
	const int i = this->Theta_Beta_Counter;
	const int j = this->Threshold_Counter;
	switch (band) {
	case Alpha:   alpha_history[i] = value; break;
	case High_Beta: high_beta_history[i] = value; break;
	case Low_Beta:  low_beta_history[i] = value; break;
	case Gamma:		gamma_history[i] = value; break;
	case TBR:	Theta_Beta_Ratio = value; Theta_Beta_History[i] = value; break;
	case Theta:	theta_history[i] = value; break;
	case Threshold: TBR_THRESHOLD = value; Threshold_History[j] = value; Threshold_Counter++; break;
	default:	return -1;
	}
	return 0;
}

/**
* setTBR
* Purpose: This is used to set all the brain wavebands with a value measured from the EPOC
* Input:  The five wavebands the EMOTIV measures along with the exteneral counter.
* Output: none
*/
void Stimulator::setTBR(const double Alpha, const double hBeta, const double lBeta, const double Gamma, const double Theta, const int counter)
{
	const int i = this->Theta_Beta_Counter;
	const double Beta = hBeta + lBeta;
	if (counter != i) //This can be deleted after we've tested with the EPOC
	{
		std::cout << "The history counter is: " << counter << std::endl;
		std::cout << "The internal AK counter is: " << i << std::endl;
		std::cout << "LOGICAL ERROR REQUIRES INVESTIGATION" << std::endl;
		Sleep(10000);
	}

	this->alpha_history[i] = Alpha / SAMPLE_DURATION;
	this->high_beta_history[i] = hBeta / SAMPLE_DURATION;
	this->low_beta_history[i] = lBeta / SAMPLE_DURATION;
	this->gamma_history[i] = Gamma / SAMPLE_DURATION;
	this->theta_history[i] = Theta / SAMPLE_DURATION;
	this->beta_history[i] = Beta / SAMPLE_DURATION;
	this->Theta_Beta_Ratio = this->theta_history[i] / this->beta_history[i];
	this->Theta_Beta_History[i] = this->Theta_Beta_Ratio;

	this->Theta_Beta_Counter++;
}

/**
* getFreq
* Purpose: Returns the desired binaural beat frequency
* Input:   none
* Output: Frequency
*/
int Stimulator::getFreq(void)
{
	return this->Freq;
}


/**
* setTBR
* Purpose: Sets the desired binaural beat frequency
* Input:  Frequency
* Output: none
*/
void Stimulator::setFreq(const int value)
{
	this->Freq = value;
}

/**
* playFreq
* Purpose: Play the binaural beat.
* Note: Going to use the OS to play the audio. Must ensure that playing the audio does not cause the program to hold here.
		If we can't get around that, we're going to need to spawn another thread or process when playing the audio.
		We might want to explore playing the audio continously until the next time sample has been examined.
* returns zero on success. If non-zero, may need to program terminate.
* https://msdn.microsoft.com/en-us/library/windows/desktop/ms679277(v=vs.85).aspx
* The beep function holds (which isn't desirable); That would require spawning a thread. Also isn't clear how to specify which headphone.
*  The message beep (https://msdn.microsoft.com/en-us/library/windows/desktop/ms680356.aspx) doesn't hold, but it isn't clear how it is used.
*/
int Stimulator::playFreq(const int seconds)
{
	this->BeatPlaying = true;

	this->BeatPlaying = false; //becomes obselete if we go with a continous method
	return 0;
}



/**
* Cal_Announcment
* Purpose: This is used to handle the initial calibration session
* Input: Start , End
* Output: SUCCESS_START, FAIL_START, SUCCESS_END, FAIL_END, INVALID
* Note:
*/

Stimulator::Announcment_Success Stimulator::Cal_Announcment(const Annoucment_Phase phase)
{
	if (phase == Start)
	{
		//Play audio informing user that calibration is taking place and instructions to relax (TBD).
		//Visual showing a loading screen. Count down timer won't work! 
		return SUCCESS_START;
	}

	else if (phase == End)
	{
		//Audio notifying user that calibration is done, and they can return to normal activity
		//Remove the visual from the screen.
		return SUCCESS_END;
	}

	else
		return INVALID;
}

