/*
 *  Playback.h
 *  openFrameworks
 *
 *  Created by Marek Bereza on 12/08/2009.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */
#pragma once
#include "Sampler.h"
#include "Recorder.h"
#include "ofxQtVideoSaver.h"
#include "WavFile.h"

class Playback {
public:
	
	// this generates the content
	Sampler *sampler;
	
	// export video and audio
	ofxQtVideoSaver encoder;
	WavFile *wav;
	
	// where to store the frame to save it
	ofImage frameBuffer;
	
	// the 'score'
	Frame frames[MAX_FRAMES];
	
	// position in frames[]
	int framesPos;
	
	// position in FPS frames
	int frame;
	
	Playback() {
		wav = NULL;
		fstream file;
		file.open(ofToDataPath("../record_cache", true).c_str(), fstream::in | fstream::binary);
		if ( (file.rdstate() & fstream::failbit ) != 0 ) {
			printf("Failed to load %s\n", ofToDataPath("../record_cache", true).c_str());
			return;
		}
		file.read((char*) frames, sizeof(Frame)*MAX_FRAMES);
		file.close();
		for(int i = 0; i < 100; i++) {
			printf("%d      %d      %d\n", frames[i].time, frames[i].note, frames[i].velocity);
		}
	}
	
	
	
	void setup(Sampler *s) {
		
		
		frame = 0;
		sampler = s;
		framesPos = 0;
		frameBuffer.setUseTexture(false);
		frameBuffer.allocate(VIDEO_WIDTH, VIDEO_HEIGHT, OF_IMAGE_COLOR);
		encoder.setup	(VIDEO_WIDTH, VIDEO_HEIGHT , ofToDataPath("../output.mov", true) );
		encoder.setCodecQualityLevel(OF_QT_SAVER_CODEC_QUALITY_LOSSLESS);
	}
	
#define FPS 30.0
	void update() {
		if(frames[framesPos].note!=0) {
			unsigned long time = (double) frame *1000.0 / FPS;
			// find all the frames that haven't been played that should have been played
			// starting with frames[framesPos]
			while(frames[framesPos].time<=time  && frames[framesPos].note!=0) {
				// trigger any notes
				sampler->midiReceived(frames[framesPos].note, frames[framesPos].velocity);
				framesPos++;
			}
			// write out the frame
			encoder.addFrame(sampler->videoRequested(), 1.0f / FPS); 
			
			// - this below does it as an image
			//frameBuffer.setFromPixels(sampler->videoRequested(), VIDEO_WIDTH, VIDEO_HEIGHT, true);
			//frameBuffer.saveImage("../output/"+ofToString(frame)+".bmp");
			frame++;
		}
	}
	
	float audioPercent;
	float videoPercent;
	
	void exportMovie();
};