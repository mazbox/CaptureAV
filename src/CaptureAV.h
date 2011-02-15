/*
 *  CaptureAV.h
 *  openFrameworks
 *
 *  Created by Marek Bereza on 03/01/2010.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */
#pragma once
#include "ofMain.h"
#include "Playback.h"
#include "ThreadedProcessor.h"

void loadCallback();
void saveCallback();
void exportCallback();

class CaptureAV {
public:
	
	static float progress;
	static bool busy;
	static CaptureAV *instance;
	static string busyString;
	ThreadedProcessor thread;
	
	Playback *playback;
	void enablePlayback() {
		if(playback!=NULL) delete playback;
		playback = new Playback();
		playback->setup(&sampler);
	}
	
	void disablePlayback() {
		printf("Disable playback called\n");
		if(playback!=NULL) {
			playback->exportMovie();
			delete playback;
			playback = NULL;
		}
	}
	
	void exportMovie() {
		thread.start(&exportCallback);
	}
	void enableRecording() {
		sampler.enableRecording();
	}
	
	void disableRecording() {
		sampler.disableRecording();
	}
	
	bool isRecording() {
		return sampler.isRecording();
	}
	
	float getRecordingTime() {
		return sampler.recorder->getRecordingTime();
	}
	
	void save() {
		
		thread.start(&saveCallback);
	}
	
	CaptureAV() {
		// singleton
		instance = this;
	}
	
	void setup() {
		video.initGrabber(VIDEO_WIDTH, VIDEO_HEIGHT, true);
		display.allocate(VIDEO_WIDTH, VIDEO_HEIGHT, GL_RGB);
		
		ofSetFrameRate(FRAME_RATE);
		
		thread.start(&loadCallback);
	}
	
	
	void update() {
		if(!busy) {
			if(playback==NULL) {
				
				
				if (sampler.getRecordMode()) {
					video.grabFrame();
					if(video.isFrameNew()){
						sampler.videoReceived(video.getPixels());
						display.loadData(sampler.videoRequested(), VIDEO_WIDTH, VIDEO_HEIGHT, GL_RGB);
					}
				}
				
				display.loadData(sampler.videoRequested(), VIDEO_WIDTH, VIDEO_HEIGHT, GL_RGB);
			} else {
				playback->update();
			}
		}
	}
	
	
	void draw() {
		if(playback==NULL) {
			if(sampler.getRecordMode()) {
				if(sampler.getRecording()) {
					ofBackground(200, 0, 0);
				} else {
					ofBackground(100, 0, 0);
				}
			} else {
				ofBackground(0, 0, 0);
			}
			
			ofSetColor(255, 255, 255);
			display.draw(0,0, VIDEO_WIDTH*ofGetHeight()/VIDEO_HEIGHT, ofGetHeight());
			
			
			if(sampler.getRecordMode()) {
				video.draw(0,0);
				string modeType = "  - normal mode - press a key to record some video";
				if(sampler.getAutoTrigger()) {
					modeType = "  - auto trigger mode";
				}
				if(sampler.getRecording()) {
					ofSetColor(255, 0, 0);
					ofDrawBitmapString("RECORDING!!!" + modeType, 10, 20);
				} else {
					ofSetColor(200, 0, 0);
					ofDrawBitmapString("RECORD MODE" + modeType, 10, 20);
				}
				ofSetColor(255, 255, 255);
				
			}  else {//if(drawMenu) {
				ofSetColor(255, 255, 255);
				ofDrawBitmapString("PLAYBACK MODE", 10, 20);
			}
			
		}
	}
	
	
	float decimator[2048];
	void audioRequested(float * output, int bufferSize, int nChannels) {
		sampler.audioRequested(decimator, bufferSize, nChannels);
		for(int i = 0; i < bufferSize; i++) {
			output[i*2] = decimator[i];
			output[i*2 + 1] = decimator[i];
		}
	}
	void audioReceived(float * input, int bufferSize, int nChannels) {
		sampler.audioReceived(input, bufferSize, nChannels);
	}
	
	int mapQwertyToMidi(char qwertyIn) {
		return (qwertyIn - 36)%500;
	}
	ofVideoGrabber video;
	ofTexture display;
	Sampler sampler;
	
	
	
};