/*
 *  Sampler.cpp
 *  openFrameworks
 *
 *  Created by Marek Bereza on 18/01/2009.
 *  Copyright 2009 Royal College of Art. All rights reserved.
 *
 */

#include "Sampler.h"

Sampler::Sampler() {

	noteJustReleased = -1;
	currentFrame = 0;
	recorder = NULL;
	volume = 0;
	recordPos = 0;
	triggerLevel = 0.02f;
	autoTrigger = false;
	recordMode = true;
	recording = false;
	frameSize = VIDEO_WIDTH*VIDEO_HEIGHT*3;
	frame = new unsigned char[frameSize];
	maxFrames = MAX_RECORD_TIME*FRAME_RATE/44100.f;
	videoBuffer = new unsigned char*[maxFrames];
	for(int i = 0; i < maxFrames; i++) {
		videoBuffer[i] = new unsigned char[frameSize];
	}
	lastPlayed = NULL;
	for(int i = 0; i < MAX_SLOTS; i++) slots[i] = NULL;

}

void Sampler::videoReceived(unsigned char *_frame) {
	int videoLength = VIDEO_WIDTH * VIDEO_HEIGHT * 3;
	for(int i = 0; i < videoLength; i++) {
		frame[i] = _frame[i];
	}
	
	if(recording) {
		if(currentFrame<maxFrames) {
			// write the frame to our videoBuffer
			memcpy(videoBuffer[currentFrame], _frame, frameSize*sizeof(unsigned char));
		}
		currentFrame++;
	}
	
	if(noteJustReleased>-1) {
		samples[noteJustReleased] = new Sample(recordBuffer, recordPos, videoBuffer, currentFrame); 
		recordPos = 0;
		recording = false;
		currentFrame = 0;
		noteJustReleased = -1;
	}
}
bool first;
unsigned char *Sampler::videoRequested() {
	int videoLength = VIDEO_WIDTH * VIDEO_HEIGHT * 3;
	for(int i = 0; i < videoLength; i++) {
		frame[i] = 0;
	}
	
	
	map<int,Sample*>::iterator it;
	first = true;
	if(Sample::mode==LAST||Sample::mode==HOLD) {
		if(lastPlayed!=NULL) {
			if(lastPlayed->videoPlaying) {
				lastPlayed->getFrame(frame);
			}
		}
	} else {
		for ( it=samples.begin() ; it != samples.end(); it++ ) {
			sample = (*it).second;
			if(sample!=NULL && sample->playing) {
				
				if(first) { first = !sample->getFrame(frame); }
				else sample->addFrame(frame);
			}
		}
	}
	// update slots
	for(int i = 0; i < MAX_SLOTS; i++) {
		if(slots[i]!=NULL && !slots[i]->videoPlaying) {
			slots[i] = NULL;
		}
	}
	return frame;
}


/**
 * AUDIO OUT
 */
void Sampler::audioRequested(float * output, int bufferSize, int nChannels) {
	
		for(int i = 0; i < bufferSize*nChannels; i++) {
			output[i] = 0.f;
		}
		
		map<int,Sample*>::iterator it;
		for ( it=samples.begin() ; it != samples.end(); it++ ) {
			sample = (*it).second;
			
			if(sample!=NULL && sample->playing) {
				//if(it==samples.begin()) {
				//	sample->getSamples(output, bufferSize);
				//} else {
					sample->addSamples(output, bufferSize);
				//} 
			}
		}
	
}

/**
 * AUDIO IN
 */
void Sampler::audioReceived(float * input, int bufferSize, int nChannels) {
	if(&detector!=NULL && input!=NULL) {
	
		volume = detector.process(input, bufferSize);
		if(autoTrigger && recordMode) {
				if(!recording && volume>triggerLevel) {
				recording = true;
					
			} else if(recording && volume<triggerLevel) {
				if(recordPos>1024) {
					samples[nextNote] = new Sample(recordBuffer, recordPos, videoBuffer, currentFrame); 
				}
				recordPos = 0;
				recording = false;
				currentFrame = 0;
			}
		}
		
		if(recording) {
			for(int i = recordPos; i < recordPos + bufferSize && i < MAX_RECORD_TIME; i++) {
				recordBuffer[i] = input[i-recordPos];
			}
			recordPos += bufferSize;
		}
	}
}



/**
 * This should be cleverer, but for now, if you receive a note with a non-zero
 * velocity it's a note on, and if it's a zero velocity then it's a note-off.
 */
void Sampler::midiReceived(int note, int velocity) {

	
	//printf("Note: %d %d\n", note, velocity);
	if(note<97) {
		if(velocity>0) notePressed(note, velocity);
		else noteReleased(note);
	} else {
		Sample::mode = (note - 97) % MODE_COUNT;
	}
}

void Sampler::stopRecording() {
	recordPos = 0;
	recording = false;
	currentFrame = 0;
}
/**
 * Happens when a note is pressed by either midi or the computer keyboard.
 */
void Sampler::notePressed(int note, int velocity) {
	if(recorder!=NULL) {
		recorder->record(note, velocity);
	}
	//printf("note pressed\n");
	if(recordMode==false) {
		// play sounds
		// show content:
		map<int,Sample*>::iterator it;

		for ( it=samples.begin() ; it != samples.end(); it++ ) {
			if((*it).first==note) {
				
				(*it).second->trigger(((float)velocity)/127.f);

				lastPlayed = (*it).second;
				
				bool slotFound = false;
				
				// default
				int numberOfSlots = 4;
				
				if(Sample::mode==NINE_UP) numberOfSlots = 9;
				if(Sample::mode==THREE_SLITS_VERT || Sample::mode==THREE_SLITS_HORIZ) numberOfSlots = 3;
				
				// check it's not already in a slot
				for(int i = 0; i < numberOfSlots; i++) {
					if(slots[i]==(*it).second) slots[i] = NULL;
				}
				unsigned long oldestTime = 100000000;//(*it).second->lastTimePlayed;
				int oldestSlot = -1;
				
				// put the sound in a slot
				for(int i = 0; i < numberOfSlots; i++) {
					if(slots[i]==NULL || !slots[i]->videoPlaying || !slots[i]->playing) {
						//printf("Found empty slot at %d\n", i);
						slots[i] = (*it).second;
						slotFound = true;
						break;
					} else {
						//printf("slot[%d] is full\n", i);
						if(slots[i]->lastTimePlayed<=oldestTime) {
							oldestSlot = i;
							oldestTime = slots[i]->lastTimePlayed;
						}
						//printf("Oldest time: %d    -    %d\n", oldestTime, slots[i]->lastTimePlayed);						
					}
				}
				//printf("Oldest slot: %d\n", oldestSlot);
				//if(slots[1]!=NULL) printf("slot[1] is full\n");
				if(!slotFound) {

					if(oldestSlot!=-1 &&oldestSlot<numberOfSlots) {
						//printf("Replacing empty slot at %d\n", oldestSlot);
						slots[oldestSlot] = (*it).second;
					} else {
						printf("Oldest slot is broken!!\n");
						slots[0] = (*it).second;
					}

				} 
				
				
				
				for(int i = 0; i < numberOfSlots; i++) {
					if(slots[i]!=NULL) {
						slots[i]->slot = i;
						//printf("Slot %d\n", i);
					}
				}
				return;
			} else {
				//(*it).second->stop();
			}
		}
	} else {
		if(autoTrigger) {
			nextNote = note;
		} else {
			recording = true;
		}
	}
}

void Sampler::noteReleased(int note) {
	if(recording && !autoTrigger) {
		noteJustReleased = note;
	}
	
}

