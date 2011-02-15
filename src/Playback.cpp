/*
 *  Playback.cpp
 *  openFrameworks
 *
 *  Created by Marek Bereza on 04/02/2010.
 *  Copyright 2010 Apple Inc. All rights reserved.
 *
 */


#include "Playback.h"
#include "CaptureAV.h"


void Playback::exportMovie() {
	audioPercent = 0;
	videoPercent = 0;
	CaptureAV::busyString = "Exporting video";
	int fp = 1;
	while(frames[fp].note!=0) fp++;
	float duration = 2 + frames[fp-1].time/1000; // add 2 seconds to it.

	ofSoundStreamStop();
	// WRITE VIDEO
	printf("exporting movie for %f secs\n", duration);
	framesPos = 0;
	frame = 0;
	float frameCount = duration*FPS;
	for(int i = 0; i < frameCount; i++) {
		videoPercent = (float)i/frameCount;
		update();
		CaptureAV::progress = videoPercent*0.5f;
	}


	printf("exporting audio\n");


	// WRITE AUDIO!!
	CaptureAV::busyString = "Exporting video to 'output.mov' and 'output.wav'";
	if(wav!=NULL) delete wav;
	wav = new WavFile();
	int totalSamples = round(44100*duration);
	wav->open(ofToDataPath("../output.wav", true).c_str(), WAVFILE_WRITE, totalSamples);
	int zeroOffset = 4410;
	float zeroes[zeroOffset]; // 10th of a second of silence because of the weird quicktime offset
	bzero(zeroes, zeroOffset*sizeof(float));
	wav->write(zeroes, zeroOffset);
	
	// loop through every bufferSize
	int bufferSize = 4;
	float buffer[bufferSize];
	double msIncrement = (double)bufferSize/44.1;
	framesPos = 0;
	int lastFrame = -1;
	//FILE *fp = fopen("/wav.raw", "wb");
	int numWrittenSamples = 0;
	for(double ms = 0; ms < duration*1000.f; ms += msIncrement) {
		audioPercent = (ms/1000.f)/duration;
		CaptureAV::progress = 0.5 + audioPercent*0.5;
		unsigned long uLongMS = lround(ms);
		if(frames[framesPos].note!=0) {
			
			while(frames[framesPos].time<=uLongMS && frames[framesPos].note!=0) {
				// trigger any notes
				sampler->midiReceived(frames[framesPos].note, frames[framesPos].velocity);
				framesPos++;
			}
		}
		// BIG WARNING!!!! CaptureAV was MONO last time I checked!!!!!!
		sampler->audioRequested(buffer, bufferSize, 1);
		totalSamples += bufferSize;
		// write captured data to audio file
		
		if(!wav->write(buffer, bufferSize)) {
			printf("Write failed!!\n");
		}
		//fwrite(buffer, bufferSize, sizeof(float), fp);
		frame = floor(FPS*ms/1000.f);
		if(frame!=lastFrame) {
			// pull through the video, even though we don't need it.
			sampler->videoRequested();
		}
		lastFrame = frame;
	}
	//fclose(fp);
	while(numWrittenSamples<totalSamples) {
		int toWrite = MIN(bufferSize, totalSamples - numWrittenSamples);
		bzero(buffer, bufferSize*sizeof(float));
		wav->write(buffer, toWrite);
		numWrittenSamples += toWrite;
	}
	wav->close();
	
	encoder.addAudioTrack(ofToDataPath("../output.wav", true));
	encoder.finishMovie();
	//unlink(ofToDataPath("output.wav", true).c_str());
	ofSoundStreamStart();

	audioPercent = 0;
	videoPercent = 0;
}