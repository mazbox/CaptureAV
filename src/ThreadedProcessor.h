/*
 *  ThreadedProcessor.h
 *  openFrameworks
 *
 *  Created by Marek Bereza on 03/01/2010.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#pragma once

#include "ofMain.h"
#include "CaptureAV.h"

// this is not a very exciting example yet
// but ofThread provides the basis for ofNetwork and other
// operations that require threading.
//
// please be careful - threading problems are notoriously hard
// to debug and working with threads can be quite difficult



class ThreadedProcessor : public ofThread {
	
public:
	
	
	// and unlock in order to write to that data
	// otherwise it's possible to get crashes.
	//
	// also no opengl specific stuff will work in a thread...
	// threads can't create textures, or draw stuff on the screen
	// since opengl is single thread safe
	
	//--------------------------

	void (*func)();
	
	ThreadedProcessor() {
		func = NULL;
	}
	

		
	
		
	void start(void(*_func)()){
		if(func==NULL) {
			func = _func;
			startThread(false, false);   // blocking, verbose
			printf("Starting some thread\n");
		} else {
			printf("Error: threaded loading/saving/exporting function still hasn't come back\n");
		}
	}
		
	
	
	//--------------------------
	void threadedFunction(){
		
		func();
		func = NULL;
		stopThread();
		printf("Finished some thread\n");
	}
	
	
	
	
};

