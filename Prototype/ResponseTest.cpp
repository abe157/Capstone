// Comparing time here: https://stackoverflow.com/questions/19733222/how-to-compare-two-different-time-values
// good stuff here: https://stackoverflow.com/questions/8308236/performing-arithmetic-on-systemtime
// https://stackoverflow.com/questions/8308236/performing-arithmetic-on-systemtime
// // https://docs.microsoft.com/en-us/cpp/atl-mfc-shared/reference/ctime-class#ctime
#include "ResponseTest.h"

#define MIN_WAIT 10 //minimum seconds of waiting... could be zero
#define MAX_MINU_WAIT 5 // maximum number of minutes for waiting in each stage
#define EXTRA_DATA 1 // enable to see extra data
#define SIM_SLEEP 10000 //(ms) 10 seconds to simulate the time it takes the EEG to get enough data



int response_verification(void)
{
	std::ofstream myfile;
	myfile.open("Response_Test_Output.txt", std::ios::out | std::ios::trunc);
	if (!myfile.is_open()) // https://stackoverflow.com/questions/12482725/ofstream-doesnt-create-file-in-c
	{
		std::cerr << "Failed to open file : " << SYSERROR() << std::endl;
		std::cerr << "Unable to create file required for this Response Test.\n";
		std::cerr << "Termininating Response Test.\n";
		return -1;
	}
	std::cout << "Log is stored in Response_Test_Output.txt" << std::endl;
	myfile << "===\tResponse verification task\t===\n";
	myfile << "===\tNote: Times are presented on a 24-hour clock\t===\n";

	unsigned int tdataPoints = 0, tfailPoints = 0, tsuccPoints = 0; //Number of 'average values generated for EEG'
	unsigned int dataPoints = 0, succPoints = 0, failPoints = 0; //Number of 'average values generated for EEG'
	double alphaT = 0, low_betaT = 0, high_betaT = 0, gammaT = 0, thetaT = 0;
	int history_counterT = 0;
	const int& ahT = history_counterT;
	AK_Audio^ audio_test = gcnew AK_Audio;
	Stimulator stimT; //check the BeatPlaying

					  //generate a random double
	double low = 1.0;
	double max = 20;
	std::uniform_real_distribution<double> unif(low, max);
	std::default_random_engine re;
	stimT.setBand(Stimulator::Threshold, unif(re));
	stimT.initCalibration = true;
	myfile << "===\tThreshold value will be: ";
	myfile << std::fixed << std::setprecision(PREC) << stimT.getBand(Stimulator::Threshold) << "\t===\n";
	std::uniform_real_distribution<double> unifTBR(low - 0.1, stimT.getBand(Stimulator::Threshold) - 0.1);
	std::default_random_engine reTBR; //dummy TBR values for no audio stage

	std::random_device rd;   // non-deterministic generator  
	std::mt19937 gen(rd());  // to seed mersenne twister.  
	std::uniform_int_distribution<> minWait(1, MAX_MINU_WAIT); // Minutes delay
	std::uniform_int_distribution<> secWait(MIN_WAIT, 59); //Seconds delay

	CTime curTime = CTime::GetCurrentTime();
	int nMin = minWait(gen);
	int nSec = secWait(gen);
	if (nSec > 59 || nSec < MIN_WAIT) { nSec = 59; } //prevent overflow
	if (nMin > MAX_MINU_WAIT || nMin < 0) { nMin = (MAX_MINU_WAIT / 2); } // prevent overflow
	myfile << "===\tThere should be no audio for the next " << nMin << " minutes and " << nSec, " seconds\t===\n";
	std::cout << "===\tThere should be no audio during the upcoming " << nMin << " minutes and " << nSec << " seconds\t===" << std::endl;
	CString cs = curTime.Format(_T("%H hours, %M minutes, and %S seconds, of %A, %B %d, %Y\t===\n"));
	std::string timeMesg = (LPCSTR)cs;
	myfile << "\n===\tFirst portion of this test starting at:\t";
	myfile << timeMesg;


	time_t start_time, end_time;
	double runTime = nMin * 60 + nSec;
	time(&start_time);  /* get current time; same as: now = time(NULL)  */


	do { //Audio shouldn't generate in this loop, TBR should be less than threshold
		double newTBR = unifTBR(reTBR);

		if (EEG_SCALE)
		{
			thetaT = 2 * SAMPLE_DURATION * newTBR;
			high_betaT = 1 * SAMPLE_DURATION;
			low_betaT = SAMPLE_DURATION * 1;
		}
		else
		{
			thetaT = 2 * newTBR;
			high_betaT = 1;
			low_betaT = 1;
		}


		stimT.setTBR(0, high_betaT, low_betaT, 0, thetaT, ahT); //normal update
		history_counterT++;
		dataPoints++;

		stimT.evaluate(audio_test);

		if (!stimT.BeatPlaying) { succPoints++; }
		else {
			failPoints++;
			myfile << "===\tWarning audio detected during data sample " << (dataPoints - 1) << ". Error count incremented.\t===\n";
			if (isDefinitelyGreaterThan<double>(stimT.getBand(Stimulator::TBR), stimT.getBand(Stimulator::Threshold)))
			{
				myfile << "===\tFalse positive. During data sample" << (dataPoints - 1) << "this test erred in generating a TBR greater than the Threshold. " <<
				 "Error count decremented.\t===\n";
				failPoints--;

			}
		}

		if (EXTRA_DATA)
		{
			myfile << std::fixed << std::setprecision(PREC) << "TBR is: " << stimT.getBand(Stimulator::TBR) << std::endl;
			myfile << std::fixed << std::setprecision(PREC) << "Threshold is: " << stimT.getBand(Stimulator::Threshold) << std::endl;
			if (stimT.BeatPlaying)
				myfile  << "===\tAudio Playing? [YES][TRUE]\t===\n";
			else
				myfile << "===\tAudio Playing? [NO][FALSE]\t===\n";
		}

		if (ahT == MAX_LEN)
		{
			myfile << "===\tInsuff. memory. Now erasing recorded values to continue\t===\n";
			stimT.resetTBRHistory(history_counterT);
		}

		Sleep(SIM_SLEEP);
		time(&end_time);
	} while (difftime(end_time, start_time) < runTime);

	curTime = CTime::GetCurrentTime();
	cs = curTime.Format(_T("%H hours, %M minutes, and %S seconds, of %A, %B %d, %Y\t===\n"));
	timeMesg = (LPCSTR)cs;
	myfile << "\n===\tTest 1 finished at: \t";
	myfile << timeMesg;
	myfile << "===\tTest 1 generated " << dataPoints << " sample points.\t===\n";
	myfile << "===\tTest 1 [SUCCESSFUL] points: " << succPoints << std::endl;
	myfile << "===\tTest 1 [FAILURE] points: " << failPoints << std::endl;

	tdataPoints = dataPoints;
	tfailPoints = failPoints;
	tsuccPoints = succPoints;

	dataPoints = 0, succPoints = 0, failPoints = 0; //Number of 'average values generated for EEG'
	alphaT = 0, low_betaT = 0, high_betaT = 0, gammaT = 0, thetaT = 0;
	history_counterT = 0;
	stimT.resetTBRHistory(history_counterT);
	myfile << "===\tCleaning up for second test:\t===\n";
	myfile << "===\tThreshold value is still: ";
	myfile << std::fixed << std::setprecision(PREC) << stimT.getBand(Stimulator::Threshold) << "\t===\n";


	std::uniform_real_distribution<double> unifTBR_new(stimT.getBand(Stimulator::Threshold) + 0.1, max + 1);
	std::default_random_engine reTBR_new; //dummy TBR values for audio stage

	nMin = minWait(gen);
	nSec = secWait(gen);
	curTime = CTime::GetCurrentTime();

	if (nSec > 59 || nSec < MIN_WAIT) { nSec = 59; } //prevent overflow
	if (nMin > MAX_MINU_WAIT || nMin < 0) { nMin = (MAX_MINU_WAIT / 2); } // prevent overflow

	myfile << "===\tThere should be audio for the next " << nMin << " minutes and " << nSec, " seconds\t===\n";
	std::cout << "===\tThere should be audio during the upcoming " << nMin << " minutes and " << nSec << " seconds\t===" << std::endl;
	cs = curTime.Format(_T("%H hours, %M minutes, and %S seconds, of %A, %B %d, %Y\t===\n"));
	timeMesg = (LPCSTR)cs;
	myfile << "\n===\tSecond portion of this test starting at:\t";
	myfile << timeMesg;


	runTime = nMin * 60 + nSec;
	time(&start_time);  /* get current time; same as: now = time(NULL)  */

	do { //Audio should be generated, TBR should be greater than threshold
		double newTBR = unifTBR_new(reTBR_new);

		if (EEG_SCALE)
		{
			thetaT = 2 * SAMPLE_DURATION * newTBR;
			high_betaT = 1 * SAMPLE_DURATION;
			low_betaT = SAMPLE_DURATION * 1;
		}
		else
		{
			thetaT = 2 * newTBR;
			high_betaT = 1;
			low_betaT = 1;
		}


		stimT.setTBR(0, high_betaT, low_betaT, 0, thetaT, ahT); //normal update
		history_counterT++;
		dataPoints++;

		stimT.evaluate(audio_test);

		if (stimT.BeatPlaying) { succPoints++; }
		else 
		{
			failPoints++;
			myfile << "===\tWarning no audio detected during data sample " << (dataPoints - 1) << ". Error count incremented.\t===\n";
			if (isDefinitelyGreaterThan<double>(stimT.getBand(Stimulator::Threshold), stimT.getBand(Stimulator::TBR)))
			{
				myfile << "===\tFalse positive. During data sample" << (dataPoints - 1) << "this test erred in generating a TBR less than the Threshold. " <<
					 "Error count decremented.\t===\n";
				failPoints--;

			}
		}

		if (EXTRA_DATA)
		{
			myfile << std::fixed << std::setprecision(PREC) << "TBR is: " << stimT.getBand(Stimulator::TBR) << std::endl;
			myfile << std::fixed << std::setprecision(PREC) << "Threshold is: " << stimT.getBand(Stimulator::Threshold) << std::endl;
			if (stimT.BeatPlaying)
				myfile << "===\tAudio Playing? [YES][TRUE]\t===\n";
			else
				myfile  << "===\tAudio Playing? [NO][FALSE]\t===\n";
		}


		if (ahT == MAX_LEN)
		{
			myfile << "===\tInsuff. memory. Now erasing recorded values to continue\t===\n";
			stimT.resetTBRHistory(history_counterT);
		}

		Sleep(SIM_SLEEP);
		time(&end_time);
	} while (difftime(end_time, start_time) < runTime);

	stimT.resetTBRHistory(history_counterT);
	stimT.evaluate(audio_test);

	curTime = CTime::GetCurrentTime();
	cs = curTime.Format(_T("%H hours, %M minutes, and %S seconds, of %A, %B %d, %Y\t===\n"));
	timeMesg = (LPCSTR)cs;
	myfile << "\n===\tTest 2 finished at: \t";
	myfile << timeMesg;
	myfile << "===\tTest 2 generated " << dataPoints << " sample points.\t===\n";
	myfile << "===\tTest 2 [SUCCESSFUL] points: " << succPoints << std::endl;
	myfile << "===\tTest 2 [FAILURE] points: " << failPoints << std::endl;

	tdataPoints += dataPoints;
	tfailPoints += failPoints;
	tsuccPoints += succPoints;

	myfile << "\n===\tAll tests complete.\t===\n";
	myfile << "===\tTests generated " << tdataPoints << " sample points.\t===\n";
	myfile << "===\tTests [SUCCESSFUL] points: " << tsuccPoints << std::endl;
	myfile << "===\tTests [FAILURE] points: " << tfailPoints << std::endl;

	curTime = CTime::GetCurrentTime();
	cs = curTime.Format(_T("%H hours, %M minutes, and %S seconds, of %A, %B %d, %Y\t===\n"));
	timeMesg = (LPCSTR)cs;
	myfile << "\n===\tTerminiating program at: : \t";
	myfile << timeMesg;

	myfile.close();
	return 0; 
}


