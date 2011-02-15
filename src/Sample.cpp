/*
 *  Sample.cpp
 *  openFrameworks
 *
 *  Created by Marek Bereza on 17/01/2009.
 *  Copyright 2009 Royal College of Art. All rights reserved.
 *
 */

#include "Sample.h"
#include "ofUtils.h"

Sample::Sample(float* _data, int _length, unsigned char **frames, int numFrames) {
	pos = 0;
	slot = 0;
	currentFrame = 0;
	playing = false;
	length = _length;
	volume = 0.05;
	lastTimePlayed = ofGetElapsedTimeMillis();
	VIDEOSIZE = VIDEO_WIDTH*VIDEO_HEIGHT*3*sizeof(unsigned char);
	VIDEOPIXELS = VIDEO_WIDTH*VIDEO_HEIGHT*3;
	data = new float[length];
	for(int i = 0; i < length; i++) {
		data[i] = _data[i];
	}
	printf("%d video frames were recorded\n", numFrames);
	setFrames(frames, numFrames);
	videoPlaying = false;
}

void Sample::setFrames(unsigned char **frames, int numFrames) {
	//frameCount = numFrames - 1;
	frameCount = numFrames;
	if(frames!=NULL && frameCount>0) {
		videoData = new unsigned char *[frameCount];
		for(int i = 0; i < frameCount; i++) {
			videoData[i] = new unsigned char[VIDEO_WIDTH*VIDEO_HEIGHT*3];
			memcpy(videoData[i], frames[i], VIDEO_WIDTH*VIDEO_HEIGHT*3*sizeof(unsigned char));
		}
	}
}

void Sample::trigger(float _volume) {
	pos = 0;
	currentFrame = 0;
	playing = true;
	volume = _volume;
	videoPlaying = true;
	lastTimePlayed = playCounter++;
	printf("starting sample\n");
}
int Sample::mode = HOLD;
int Sample::frameNum = 0;
int Sample::playCounter = 0;
void Sample::getSamples(float* out, int _length) {
	int count = 0;
	for(int i = pos; i < _length+pos && i < length; i++) {
		if(i<3) {
			out[i-pos] = ((float)(i+1.f)/3.f)*volume*data[i];
		} else if(i+3>length) {
			out[i-pos] = ((float)(length - i)/3.f)*volume*data[i];
		} else {
			out[i-pos] = volume*data[i];
		}
		count++;
	}
	pos += count;
	// if we've reached the end
	if(pos>=length-1) {
		playing = false;
		for(int i = count; i < _length; i++) {
			out[i] = 0.f;
		}
	}
}

void Sample::addSamples(float* out, int _length) {
	int count = 0;

	for(int i = pos; i < _length+pos && i < length; i++) {
		if(i<3) {
			out[i-pos] += ((float)(i+1.f)/3.f)*volume*data[i];
		} else if(i+3>length) {
			out[i-pos] += ((float)(length - i)/3.f)*volume*data[i];
		} else {
			out[i-pos] += volume*data[i];
		}
		count++;
	}
	pos += count;
	if(pos>=length-1) {
		playing = false;
	}
}

void Sample::stop() {
	playing = false;
	pos = 0;
	currentFrame = 0;
}


