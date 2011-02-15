/*
 *  Recorder.h
 *  openFrameworks
 *
 *  Created by Marek Bereza on 12/08/2009.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */
#pragma once
#include "ofMain.h"
#include <vector>
#include <fstream>
#define MAX_FRAMES 1000000

struct Frame {
	int time;
	int note;
	int velocity;
	
};
class Recorder {
public:
	Frame frames[MAX_FRAMES];
	
	Recorder() {
		pos = 0;
		memset(frames, 0, sizeof(Frame)*MAX_FRAMES);
		birthday = ofGetElapsedTimeMillis();
	}
	
	unsigned long birthday;
	unsigned long pos;
	
	float getRecordingTime() {
		return (ofGetElapsedTimeMillis()-birthday)/1000.f;
	}
	void record(int note, int velocity) {
		if(pos<MAX_FRAMES) {
			frames[pos].time = ofGetElapsedTimeMillis() - birthday;
			frames[pos].note = note;
			frames[pos].velocity = velocity;
			pos++;
		}
	}
	void save() {
		fstream file;
		file.open(ofToDataPath("../record_cache", true).c_str(), fstream::out | fstream::trunc | fstream::binary);
		if ( (file.rdstate() & fstream::failbit ) != 0 ) {
			printf("Failed to save\n");
			return;
		}
		
		file.write((char*) frames, sizeof(Frame)*MAX_FRAMES);
		file.close();
	}
};