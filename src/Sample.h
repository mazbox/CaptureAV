/*
 *  Sample.h
 *  openFrameworks
 *
 *  Created by Marek Bereza on 17/01/2009.
 *  Copyright 2009 Royal College of Art. All rights reserved.
 *
 */
#include <stdio.h>
#include <string>
#ifndef SAMPLE
#define SAMPLE

#define VIDEO_WIDTH 800
#define VIDEO_HEIGHT 600

#define FRAME_RATE 30

// modes
#define LAST				0
#define HOLD				1
#define BLEND				2
#define FOUR_UP				3
#define BLEND_DOWN			4
#define NINE_UP				5
#define THREE_SLITS_VERT	6
#define THREE_SLITS_HORIZ	7
#define MODE_COUNT	8


using namespace std;
class Sample {
	
public:
	static string getModeString(int _mode) {
		switch(_mode) {
			case LAST: return "Normal";
			case HOLD: return "Hold";
			case BLEND: return "Blend";
			case FOUR_UP: return "Four Up";
			case BLEND_DOWN: return "Blend Down";
			case NINE_UP: return "Nine Up";
			case THREE_SLITS_VERT: return "Three Slits Vertical";
			case THREE_SLITS_HORIZ: return "Three Slits Horizontal";
		}
		return "MODE_NOT_FOUND";
	}
	static int mode;
	static int frameNum;
	static int playCounter;
	Sample(float* data, int length, unsigned char **frames = NULL, int numFrames = 0);
	void setFrames(unsigned char **frames, int numFrames);
	~Sample() {
		delete [] data;
	};
	void trigger(float _volume);
	void getSamples(float* out, int length);
	void addSamples(float* out, int length);
	int VIDEOSIZE;
	int VIDEOPIXELS;
	bool videoPlaying;
	bool getFrame(unsigned char *frame){
		frameNum = 0;
		
		if(mode==HOLD) {
			
			if(currentFrame>=frameCount-1 && frameCount-1>=0) {
				currentFrame = frameCount-1;
			}
			//printf("currentFrame: %d\n", currentFrame);
		}
		if(currentFrame<frameCount) {
			if(mode==FOUR_UP) {
				//printf("get slot: %d\n",slot);
				int xOffset = 0;
				int yOffset = 0;
				
				if(slot==1) {
					xOffset = VIDEO_WIDTH/2;
				}
				if(slot==2) {
					yOffset = VIDEO_HEIGHT/2;
				}
				if(slot==3) {
					xOffset = VIDEO_WIDTH/2;
					yOffset = VIDEO_HEIGHT/2;
				}
				for(int x = 0; x < VIDEO_WIDTH/2; x++) {
					for(int y = 0; y< VIDEO_HEIGHT/2; y++) {
						int offset = ((x+xOffset)+(y+yOffset)*VIDEO_WIDTH)*3;
						int miniOffset = (x*2 + (y*2*VIDEO_WIDTH)) * 3;
						frame[offset] = videoData[currentFrame][miniOffset];
						frame[offset+1] = videoData[currentFrame][miniOffset+1];
						frame[offset+2] = videoData[currentFrame][miniOffset+2];
					}
				}
				
				
			} else if(mode==NINE_UP) {
				//printf("get slot: %d\n",slot);
				int xOffset = 0;
				int yOffset = 0;
				switch(slot) {
					case 0: 
						xOffset = VIDEO_WIDTH/3;
						yOffset = VIDEO_HEIGHT/3;
						break;
					case 1: 
						xOffset = VIDEO_WIDTH/3;
						yOffset = 0;
						break;
					case 2: 
						xOffset = VIDEO_WIDTH/3;
						yOffset = VIDEO_HEIGHT*2/3;
						break;
					case 3: 
						xOffset = 0;
						yOffset = VIDEO_HEIGHT/3;
						break;
					case 4: 
						xOffset = VIDEO_WIDTH*2/3;
						yOffset = VIDEO_HEIGHT/3;
						break;
					case 5: 
						xOffset = 0;
						yOffset = 0;
						break;
					case 6: 
						xOffset = VIDEO_WIDTH*2/3;
						yOffset = VIDEO_HEIGHT*2/3;
						break;
					case 7: 
						xOffset = VIDEO_WIDTH*2/3;
						yOffset = 0;
						break;
					case 8: 
						xOffset = 0;
						yOffset = VIDEO_HEIGHT*2/3;
						break;
					
						
				}
				
				for(int x = 0; x < VIDEO_WIDTH/3; x++) {
					for(int y = 0; y< VIDEO_HEIGHT/3; y++) {
						int offset = ((x+xOffset)+(y+yOffset)*VIDEO_WIDTH)*3;
						int miniOffset = (x*3 + (y*3*VIDEO_WIDTH)) * 3;
						frame[offset] = videoData[currentFrame][miniOffset];
						frame[offset+1] = videoData[currentFrame][miniOffset+1];
						frame[offset+2] = videoData[currentFrame][miniOffset+2];
					}
				}
			} else if(mode==BLEND_DOWN) {
				float frameCountSqrt = sqrt(frameCount);
				float diffSqrt = sqrt((float)frameCount - currentFrame);
				for(int i = 0; i < VIDEOPIXELS; i++) {
					frame[i] = (float)videoData[currentFrame][i]*diffSqrt/frameCountSqrt;
				}
				
			} else if(mode==THREE_SLITS_VERT) {
				int start = slot*VIDEO_WIDTH/3;
				int end = (slot+1)*VIDEO_WIDTH/3;
				if(end>VIDEO_WIDTH) end = VIDEO_WIDTH;
				for(int x = start; x < end; x++) {
					for(int y = 0; y< VIDEO_HEIGHT; y++) {
						int offset = (x+y*VIDEO_WIDTH)*3;
						frame[offset] = videoData[currentFrame][offset];
						frame[offset+1] = videoData[currentFrame][offset+1];
						frame[offset+2] = videoData[currentFrame][offset+2];
					}
				}
				
			} else if(mode==THREE_SLITS_HORIZ) {
				int start = slot*VIDEOPIXELS/3;
				int end = (slot+1)*VIDEOPIXELS/3;
				if(end>VIDEOPIXELS) end = VIDEOPIXELS;
				for(int i = start; i < end; i++) {
					frame[i] = videoData[currentFrame][i];
				}
			} else {
				
					memcpy(frame, videoData[currentFrame], VIDEO_WIDTH*VIDEO_HEIGHT*3*sizeof(unsigned char));
				
			}
			currentFrame++;
			videoPlaying = true;
			return true;
		}
		videoPlaying = false;
		return false;
	}
	void addFrame(unsigned char *frame){
		frameNum++;
		if(currentFrame<frameCount) {
			videoPlaying = true;
			//memcpy(frame, videoData[currentFrame], VIDEO_WIDTH*VIDEO_HEIGHT*3*sizeof(unsigned char));
			if(mode==BLEND) {
				for(int i = 0; i < VIDEOPIXELS; i++) {
					frame[i] = (frame[i]/2) + (videoData[currentFrame][i]/2);
				}
			} else if(mode==LAST) {
				
					memcpy(frame, videoData[currentFrame], VIDEOSIZE);
//					frame[i] = (frame[i]/2) + (videoData[currentFrame][i]/2);
				
			} else if(mode==BLEND_DOWN) {
				float frameCountSqrt = sqrt(frameCount)*2;
				float diffSqrt = sqrt((float)frameCount - currentFrame);
				for(int i = 0; i < VIDEOPIXELS; i++) {
					frame[i] = (frame[i]/2) + (float)videoData[currentFrame][i]*diffSqrt/frameCountSqrt;
				}
			} else if(mode==NINE_UP) {
				//printf("get slot: %d\n",slot);
				int xOffset = 0;
				int yOffset = 0;
				switch(slot) {
					case 0: 
						xOffset = VIDEO_WIDTH/3;
						yOffset = VIDEO_HEIGHT/3;
						break;
					case 1: 
						xOffset = VIDEO_WIDTH/3;
						yOffset = 0;
						break;
					case 2: 
						xOffset = VIDEO_WIDTH/3;
						yOffset = VIDEO_HEIGHT*2/3;
						break;
					case 3: 
						xOffset = 0;
						yOffset = VIDEO_HEIGHT/3;
						break;
					case 4: 
						xOffset = VIDEO_WIDTH*2/3;
						yOffset = VIDEO_HEIGHT/3;
						break;
					case 5: 
						xOffset = 0;
						yOffset = 0;
						break;
					case 6: 
						xOffset = VIDEO_WIDTH*2/3;
						yOffset = VIDEO_HEIGHT*2/3;
						break;
					case 7: 
						xOffset = VIDEO_WIDTH*2/3;
						yOffset = 0;
						break;
					case 8: 
						xOffset = 0;
						yOffset = VIDEO_HEIGHT*2/3;
						break;
						
						
				}
				
				for(int x = 0; x < VIDEO_WIDTH/3; x++) {
					for(int y = 0; y< VIDEO_HEIGHT/3; y++) {
						int offset = ((x+xOffset)+(y+yOffset)*VIDEO_WIDTH)*3;
						int miniOffset = (x*3 + (y*3*VIDEO_WIDTH)) * 3;
						frame[offset] = videoData[currentFrame][miniOffset];
						frame[offset+1] = videoData[currentFrame][miniOffset+1];
						frame[offset+2] = videoData[currentFrame][miniOffset+2];
					}
				}
			} else if(mode==THREE_SLITS_HORIZ) {
				int start = slot*VIDEOPIXELS/3;
				int end = (slot+1)*VIDEOPIXELS/3;
				if(end>VIDEOPIXELS) end = VIDEOPIXELS;
				for(int i = start; i < end; i++) {
					frame[i] = videoData[currentFrame][i];
				}
			} else if(mode==THREE_SLITS_VERT) {
				int start = slot*VIDEO_WIDTH/3;
				int end = (slot+1)*VIDEO_WIDTH/3;
				if(end>VIDEO_WIDTH) end = VIDEO_WIDTH;
				for(int x = start; x < end; x++) {
					for(int y = 0; y< VIDEO_HEIGHT; y++) {
						int offset = (x+y*VIDEO_WIDTH)*3;
						frame[offset] = videoData[currentFrame][offset];
						frame[offset+1] = videoData[currentFrame][offset+1];
						frame[offset+2] = videoData[currentFrame][offset+2];
					}
				}
				
			} else if(mode==FOUR_UP) {
				//printf("add slot: %d\n", slot);
				int xOffset = 0;
				int yOffset = 0;
				
				if(slot==1) {
					xOffset = VIDEO_WIDTH/2;
				}
				if(slot==2) {
					yOffset = VIDEO_HEIGHT/2;
				}
				if(slot==3) {
					xOffset = VIDEO_WIDTH/2;
					yOffset = VIDEO_HEIGHT/2;
				}
				for(int x = 0; x < VIDEO_WIDTH/2; x++) {
					for(int y = 0; y< VIDEO_HEIGHT/2; y++) {
						int offset = ((x+xOffset)+(y+yOffset)*VIDEO_WIDTH)*3;
						int miniOffset = (x*2 + (y*2*VIDEO_WIDTH)) * 3;
						frame[offset] = videoData[currentFrame][miniOffset];
						frame[offset+1] = videoData[currentFrame][miniOffset+1];
						frame[offset+2] = videoData[currentFrame][miniOffset+2];
					}
				}
			}
		} else {
			videoPlaying = false;
		}
				currentFrame++;
	}
	
	void stop();
	bool playing;
	
	/** See SamplerFileIO.cpp for implementation */
	bool save(string path);
	int length;
	float *data;
	unsigned char **videoData;
	int frameCount;
	unsigned long lastTimePlayed;
	int slot;
private:
	int pos;
	float volume;
	int currentFrame;
};
#endif

