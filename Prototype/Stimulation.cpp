#include "Stimulation.h"


/**
* Cal_Announcment
* Purpose: This is used to handle the initial calibration session
* Input: Start , End
* Output: SUCCESS_START, FAIL_START, SUCCESS_END, FAIL_END, INVALID
* Note:
*/

Stimulator::Announcment_Success Stimulator::Cal_Announcment(const Announcment_Phase phase)
{
	if (phase == Start)
	{
		this->initCalibration = false;
		//Play audio informing user that calibration is taking place and instructions to relax (TBD).
		//Visual showing a loading screen. Count down timer won't work! 
		return SUCCESS_START;
	}

	else if (phase == End)
	{
		//Audio notifying user that calibration is done, and they can return to normal activity
		//Remove the visual from the screen.
		this->initCalibration = true;

		return SUCCESS_END;
	}

	else
		return INVALID;
}

/**
* evaluate
* Purpose: Controls the playing of binaural beats.
* Input:  none
* Output: CRIT_EVAL_ERR, FALSE_INSUFF_DATA, FALSE_OFF, FALSE_IN_PROGRESS, FALSE_TURNING_OFF TRUE_STARTED, TRUE_IN_PROGRESS
* Note: Currently a very basic operation
*/

Stimulator::Evaluation_Result Stimulator::evaluate(AK_Audio^% audio_obj)
{
	if (this->initCalibration == false) { return FALSE_INSUFF_DATA; }
	if (this->Threshold_Counter == 0) 
	{
		this->BeatPlaying = false;
		audio_obj->Stop();
		return FALSE_INSUFF_DATA;
	}

	if (isDefinitelyGreaterThan<double>(this->Theta_Beta_Ratio, this->TBR_THRESHOLD)) //play audio
	{
		if (!this->BeatPlaying)
		{
			audio_obj->PlayLooping();
			this->BeatPlaying = true;
			return TRUE_STARTED;
		}
		else { return TRUE_IN_PROGRESS; }
	}

	else
	{
		if (!this->BeatPlaying) { return FALSE_OFF; }
		else
		{
			audio_obj->Stop();
			this->BeatPlaying = false;
			return FALSE_TURNING_OFF;
		}
		//else { return FALSE_IN_PROGRESS; } /*Not compatible with current audio option* 4/26 12:32 AM ~ SB/
	}

	return CRIT_EVAL_ERR;
}

/**
* getBand
* Purpose: Return the power of a specific brain waveband
* Input:  Alpha, Beta, High_Beta, Low_Beta, Beta, Gamma, TBR, Theta, Threshold, Frequency
* Output: Power value on success. Returns less than 0 on error.
* Note: Gets more than just bands.
*/
double Stimulator::getBand(const Brain_Band band)
{
	const int i = this->Theta_Beta_Counter;
	switch (band) {
	case Alpha:   return alpha_plot_data.back();
	case Beta:	return beta_plot_data.back();
	case High_Beta: return high_beta_plot_data.back();
	case Low_Beta:  return low_beta_plot_data.back();
	case Gamma:		return gamma_plot_data.back();
	case TBR: return this->Theta_Beta_Ratio; 
	case Theta:	return theta_plot_data.back();
	case Threshold: return this->TBR_THRESHOLD;
	case Frequency:	return this->Freq;
	default:	return -1;
	}

	if (false)
	{
		const int i = this->Theta_Beta_Counter;
		switch (band) {
		case Alpha:   return this->alpha_history[i];
		case Beta:	return this->beta_history[i];
		case High_Beta: return this->high_beta_history[i];
		case Low_Beta:  return this->low_beta_history[i];
		case Gamma:		return this->gamma_history[i];
		case TBR:	return this->Theta_Beta_Ratio;
		case Theta:	return this->theta_history[i];
		case Threshold: return this->TBR_THRESHOLD;
		case Frequency:	return this->Freq;
		default:	return -1;
		}
	}
}



/**
* getBandList
* Purpose: Return a vector with the history of a band.
* Input:  Alpha, Beta, High_Beta, Low_Beta, Beta, Gamma, TBR, Theta, Threshold
* Output: Greater than 0 on empty list, Less than 0 on error, Zero on success.
* Note: Meshach can use this for his plotting.
*/
int Stimulator::getBandList(const Brain_Band band, std::vector<double>& list)
{
	size_t copy_length;

	if (band == Threshold)
	{
		if (this->Threshold_Counter == 0) { return 1; }
		copy_length = this->Threshold_Counter;
	}

	else
	{
		if (this->Theta_Beta_Counter == 0) { return 1; } //No values to plot
		copy_length = (size_t)(this->Theta_Beta_Counter) - list.size();
	}


	if (copy_length == 0) { return 0; } //No changes to make
	else if (copy_length < 0) { return -1; } //Probably something wrong with the list

	for (size_t i = list.size(); i < (copy_length + list.size()); i++)
	{
		switch (band) {
		case Alpha:
			list.push_back(alpha_history[i]);
			break;
		case Beta:
			list.push_back(beta_history[i]);
			break;
		case High_Beta:
			list.push_back(high_beta_history[i]);
			break;
		case Low_Beta:
			list.push_back(low_beta_history[i]);
			break;
		case Gamma:
			list.push_back(gamma_history[i]);
			break;
		case TBR:
			list.push_back(Theta_Beta_History[i]);
			break;
		case Theta:
			list.push_back(theta_history[i]);
			break;
		case Threshold:
			list.push_back(Threshold_History[i]);
		default:	return -1;
		}
	}
	return 0;
}

/**
* resetTBRHistory
* Purpose: Allows the program to 'restart' in the event the history arrays (excluding Threshold) are full
* Input:  The counter that is external to the class (ex. history counter in main)
* Output: Nothing
* Note: Use with caution. Ensure any counters used outside of this class are set to zero.
*		The only regular use of this funtion is in a Response Test.
*/
void Stimulator::resetTBRHistory(int &counter)
{
	Theta_Beta_Ratio = 0;
	Theta_Beta_Counter = 0;
	counter = 0;
	memset(alpha_history, 0, sizeof(alpha_history));
	memset(beta_history, 0, sizeof(beta_history));
	memset(gamma_history, 0, sizeof(gamma_history));
	memset(high_beta_history, 0, sizeof(high_beta_history));
	memset(low_beta_history, 0, sizeof(low_beta_history));
	memset(Theta_Beta_History, 0, sizeof(Theta_Beta_History));
	memset(theta_history, 0, sizeof(theta_history));

	alpha_plot_data.clear();
	beta_plot_data.clear();
	high_beta_plot_data.clear();
	low_beta_plot_data.clear();
	gamma_plot_data.clear();
	Theta_Beta_plot_data.clear();
	theta_plot_data.clear();
	Threshold_plot_data.clear();

	alpha_plot_data.reserve(MAX_LEN);
	beta_plot_data.reserve(MAX_LEN);
	high_beta_plot_data.reserve(MAX_LEN);
	low_beta_plot_data.reserve(MAX_LEN);
	gamma_plot_data.reserve(MAX_LEN);
	Theta_Beta_plot_data.reserve(MAX_LEN);
	theta_plot_data.reserve(MAX_LEN);
	Threshold_plot_data.reserve(MAX_LEN);
}


/**
* setBand
* Purpose: Sets the measured power of a specific brain waveband or value with a specific value
* Input:  Alpha, Beta, High_Beta, Low_Beta, Beta, Gamma, TBR, Theta, Threshold ; Frequency of Audio; Power value
* Output: Power value on success. Returns less than 0 on error.
* Note: Use with extreme caution. Erroneous use will cause logical errors and desynchronization.
*		The only regular use of this function should be with the input Threshold.
*/
int Stimulator::setBand(const Brain_Band band, const double value)
{
	const int i = this->Theta_Beta_Counter;
	const int j = this->Threshold_Counter;
	switch (band)
	{
	case Alpha:   alpha_history[i] = value; alpha_plot_data.push_back(value); break;
	case Beta:	beta_history[i] = value; beta_plot_data.push_back(value); break;
	case High_Beta: high_beta_history[i] = value; high_beta_plot_data.push_back(value); break;
	case Low_Beta:  low_beta_history[i] = value; low_beta_plot_data.push_back(value); break;
	case Gamma:		gamma_history[i] = value; gamma_plot_data.push_back(value); break;
	case TBR:	Theta_Beta_Ratio = value; Theta_Beta_History[i] = value; Theta_Beta_plot_data.push_back(value); break;
	case Theta:	theta_history[i] = value; theta_plot_data.push_back(value); break;
	case Threshold: TBR_THRESHOLD = value; Threshold_History[j] = value; Threshold_Counter++; Threshold_plot_data.push_back(value); break;
	case Frequency: this->Freq = (int)value; break;
	default:	return -1;
	}
	return 0;
}

/**
* Stimulator
* Purpose: Constructor. Starts in default state.
* Input:  none
* Output: none
*/
Stimulator::Stimulator(void)
{
	alpha_plot_data.reserve(MAX_LEN);
	beta_plot_data.reserve(MAX_LEN);
	high_beta_plot_data.reserve(MAX_LEN);
	low_beta_plot_data.reserve(MAX_LEN);
	gamma_plot_data.reserve(MAX_LEN);
	Theta_Beta_plot_data.reserve(MAX_LEN);
	theta_plot_data.reserve(MAX_LEN);
	Threshold_plot_data.reserve(MAX_LEN);
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
	if (counter != i) //This can be deleted after we've tested with the EPOC
	{
		std::cerr << "The history counter is: " << counter;
		std::cerr << "The internal AK counter is: " << i;
		std::cerr << "LOGICAL ERROR REQUIRES INVESTIGATION";
		Sleep(10000);
	}

	if (EEG_SCALE)
	{
		const double Beta = hBeta + lBeta;
		this->alpha_history[i] = Alpha / SAMPLE_DURATION;
		this->alpha_plot_data.push_back(alpha_history[i]);
		this->high_beta_history[i] = hBeta / SAMPLE_DURATION;
		this->high_beta_plot_data.push_back(high_beta_history[i]);
		this->low_beta_history[i] = lBeta / SAMPLE_DURATION;
		this->low_beta_plot_data.push_back(low_beta_history[i]);
		this->gamma_history[i] = Gamma / SAMPLE_DURATION;
		this->gamma_plot_data.push_back(gamma_history[i]);
		this->theta_history[i] = Theta / SAMPLE_DURATION;
		this->theta_plot_data.push_back(theta_history[i]);
		this->beta_history[i] = Beta / SAMPLE_DURATION;
		this->beta_plot_data.push_back(beta_history[i]);
		this->Theta_Beta_Ratio = this->theta_history[i] / this->beta_history[i];
		this->Theta_Beta_History[i] = this->Theta_Beta_Ratio;
		this->Theta_Beta_plot_data.push_back(this->Theta_Beta_Ratio);
		this->Theta_Beta_Counter++;
	}

	else
	{
		const double Beta = hBeta + lBeta;

		this->alpha_history[i] = Alpha;
		this->alpha_plot_data.push_back(Alpha);
		this->high_beta_history[i] = hBeta;
		this->high_beta_plot_data.push_back(high_beta_history[i]);
		this->low_beta_history[i] = lBeta;
		this->low_beta_plot_data.push_back(lBeta);
		this->gamma_history[i] = Gamma;
		this->gamma_plot_data.push_back(Gamma);
		this->theta_history[i] = Theta;
		this->theta_plot_data.push_back(Theta);
		this->beta_history[i] = Beta;
		this->beta_plot_data.push_back(Beta);
		this->Theta_Beta_Ratio = Theta / Beta;
		this->Theta_Beta_History[i] = this->Theta_Beta_Ratio;
		this->Theta_Beta_plot_data.push_back(this->Theta_Beta_Ratio);
		this->Theta_Beta_Counter++;
	}
}

