/*
 *  Sampler.h
 *  openFrameworks
 *
 *  Created by Marek Bereza on 18/01/2009.
 *  Copyright 2009 Royal College of Art. All rights reserved.
 *
 */
#pragma once
// maximum sample length in samples
#define MAX_RECORD_TIME 882000

#include "Detector.h"
#include <map>
#include <vector>
#include "Midi.h"
#include "Sample.h"

#define MAX_SLOTS 9

// portaudio stuff
#define SAMPLERATE 44100
#define BUFFER_SIZE 256



static int gNumNoInputs = 0;


#include "Recorder.h"



class Sampler: public MidiIn {
public:
	
	Recorder *recorder;

	void enableRecording() {
		if(recorder!=NULL) disableRecording();
		recorder = new Recorder();
	}
	
	void disableRecording() {
		if(recorder!=NULL) {
			recorder->save();
			delete recorder;
			recorder = NULL;
		}
	}
	bool isRecording() {
		return recorder!=NULL;
	}
	
	Sampler();
	~Sampler() { samples.clear(); };
		void audioRequested(float * output, int bufferSize, int nChannels);
		void audioReceived(float * input, int bufferSize, int nChannels);
	
		void videoReceived(unsigned char *_frame);
		unsigned char *videoRequested();
		void notePressed(int note, int velocity);
		void noteReleased(int note);

		void setRecordMode(bool _recordMode) {
			recordMode = _recordMode;
		};	
	
		bool getRecordMode() {
			return recordMode;
		};
		void toggleRecordMode() {
			recordMode ^= true;
			if(!recordMode) stopRecording();
		};

		void midiReceived(int note, int velocity);
	
		/** See SamplerFileIO.cpp for implementation */
		bool save(string name);
		bool load(string name);
		vector<string> getPresets();
		unsigned char *frame;
	float getVolume() {
		return volume;
	};
	void setAutoTrigger(bool _t) {
		autoTrigger = _t;
	};
	
	bool getAutoTrigger() {
		return autoTrigger;
	};
	void toggleAutoTrigger() {
		autoTrigger ^= true;
	};
	
	void stopRecording();
	
	void setTriggerLevel(float _l) {
		triggerLevel = _l;
	};
	
	float getTriggerLevel() {
		return triggerLevel;
	};
	bool getRecording() {
		return recording;
	};
		
	private:
		bool saveGig(string file);
		bool loadGig(string file);
		bool saveDLS(string file);
		bool loadDLS(string file);
	
		bool loadVideo(string name);
		bool saveVideo(string name);
	
		Sample *lastPlayed;
	
		bool recordMode;
		bool recording;
	int frameSize;
		Detector detector;
		Sample *sample;
		float volume;
		float recordBuffer[MAX_RECORD_TIME];
	unsigned char **videoBuffer;
		int recordPos;
		map<int, Sample*> samples;
	Sample *slots[MAX_SLOTS];

	
	int nextNote; // the note to save to when autotriggering
	bool autoTrigger;
	float triggerLevel;
	int maxFrames;
	
	int currentFrame;
	int noteJustReleased;
	
	
	
};
                                                  

