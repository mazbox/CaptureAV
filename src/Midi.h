
#pragma once
#ifndef NULL
#define NULL 0
#endif

#import <CoreAudio/CoreAudio.h>
#import <CoreMIDI/CoreMIDI.h>

class MidiIn {
	
public:
	static void* midiIn;
	MidiIn();
	virtual void midiReceived(int note, int velocity) = 0;
private:
	MIDIClientRef client;
	MIDIPortRef inPort;
	
	//RtMidiIn *midiin;// = new RtMidiIn();
};
