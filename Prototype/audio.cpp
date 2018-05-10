
/*
#include <stdio.h>
#include <iostream>
#include <string.h>
#include <windows.h>
#include <mmsystem.h>
#include <cmath> 


using namespace std;
using namespace System;
using namespace System::IO;
using namespace System::Text;

#using <System.Drawing.dll>
#using <System.dll>
#using <System.Windows.Forms.dll>

using namespace System::Collections;
using namespace System::ComponentModel;
using namespace System::Diagnostics;
using namespace System::Drawing;
using namespace System::Media;


#define PI 3.14159265358979323846

public ref class AK_Audio {

private:
	double A = 1000;
	int Dur = 1000;
	int Samples = int(44.1 * Dur);
	int Bytes = Samples * 4;
	bool playing = false;
	MemoryStream^ memStream = gcnew MemoryStream(44 + Bytes);
	BinaryWriter^ binWriter = gcnew BinaryWriter(memStream);
	SoundPlayer^ player; // = gcnew SoundPlayer();

public:
	int Left_freq;
	int Right_freq;


	AK_Audio() {
		Left_freq = 410;
		Right_freq = 400;
	}

	AK_Audio(int left, int right) {
		Left_freq = left;
		Right_freq = right;
	}

	void Play() {
		double DeltaFTL = (2 * PI * Left_freq) / 44100;
		double DeltaFTR = (2 * PI * Right_freq) / 44100;
		memStream->Seek(0, SeekOrigin::Begin);

		try {
			// Writer to write to the WAV file
			//BinaryWriter^ binWriter = gcnew BinaryWriter(memStream);
			int i = 0;

			// Wave format header
			binWriter->Write(0x46464952); // ChunkID
			binWriter->Write(36 + Bytes); // ChunkSize
			binWriter->Write(0x45564157); // Format ("Wave" in hex)
			binWriter->Write(0x20746D66); // Subchuck1ID (Contains the letters "fmt ")
			binWriter->Write(0x00000010); // Subchunk1Size
			binWriter->Write(0x00020001); // AudioFormat 
			binWriter->Write(44100);      // Sample Rate: 44100
			binWriter->Write(176400);     // ByteRate   : 176400 (SampleRate * NumChannels * BitsPerSample/8)
			binWriter->Write(0x100004);   // BlockAlign & BitsPerSample
			binWriter->Write(0x61746164); // Data start of the file 
			binWriter->Write(Bytes);

			for (i = 0; i < (Samples - 1); i++) {
				short SampleL = short(A * sin(DeltaFTL * double(i)));
				short SampleR = short(A * sin(DeltaFTR * double(i)));
				binWriter->Write(SampleL);
				binWriter->Write(SampleR);
			}

			binWriter->Flush();
			memStream->Seek(0, SeekOrigin::Begin);

			player = gcnew SoundPlayer(memStream);
			player->LoadAsync();
			if (!player->IsLoadCompleted) {
				//cout << "     Loading Audio..." << endl;
			}
			while (!player->IsLoadCompleted) {}
			//cout << "     Begining to play sound" << endl;
			player->PlayLooping();
			playing = true;
		}
		finally {
		}
	}

	void Stop() {
		player->Stop();
	}

	~AK_Audio() {
		binWriter->Flush();
		memStream->Close();
	}

};
*/