#include "testApp.h"

#define RECORD_BUTTON 1
#define PAUSE_BUTTON 2
#define EXPORT_BUTTON 3
testApp *app;
//--------------------------------------------------------------
void testApp::setup(){
	ofSetVerticalSync(true);
	ofSoundStreamSetup(2, 1, this, 44100, 256, 4);
	capture.setup();
	app = this;
	drawMenu = true;
//	enablePlayback();
	recButton.load(20, 460, "record", RECORD_BUTTON);
	recButton.setEnabled(true);
	pauseButton.load(20, 490, "stop", PAUSE_BUTTON);
	pauseButton.setEnabled(false);
	isHidingGui = true;
	exportButton.load(20, 520, "export", EXPORT_BUTTON);
	exportButton.setEnabled(false);
	statusMessage = "";
}

void buttonPressed(int id) {
	app->buttonPressed(id);

}


void testApp::buttonPressed(int id) {
	if(id==RECORD_BUTTON) {
		if(!capture.isRecording()) {
			capture.enableRecording();
			recButton.setEnabled(false);
			pauseButton.setEnabled(true);
			exportButton.setEnabled(false);
		}
		
	}
	if(id==PAUSE_BUTTON) {
		if(capture.isRecording()) {
			capture.disableRecording();
			recButton.setEnabled(true);
			pauseButton.setEnabled(false);
			exportButton.setEnabled(true);
		}
	}
	if(id==EXPORT_BUTTON) {
		printf("Export\n");
		capture.exportMovie();
	}
}

//--------------------------------------------------------------
void testApp::update(){
	if(CaptureAV::busy && !isHidingGui) {
		isHidingGui = true;
		recButton.disableAllEvents();
		pauseButton.disableAllEvents();
		exportButton.disableAllEvents();
	} else if(!CaptureAV::busy && isHidingGui) {
		isHidingGui = false;
		recButton.enableAllEvents();
		pauseButton.enableAllEvents();
		exportButton.enableAllEvents();

	}
	
	if(capture.isRecording()) {
		log("recording: "+ofToString(capture.getRecordingTime(), 2));
	}
	capture.update();
}

//--------------------------------------------------------------
void testApp::draw(){
	if(CaptureAV::busy) {
		ofBackground(80, 40, 40);
		ofSetColor(0, 0, 0);
		ofRect(0, 0, ofGetWidth()*CaptureAV::progress, ofGetHeight()*CaptureAV::progress);
		ofSetColor(255, 255, 255);
		ofDrawBitmapString(CaptureAV::busyString, 20, 20);
		ofDrawBitmapString(ofToString(CaptureAV::progress*100.f, 1)+"%", ofGetWidth()*CaptureAV::progress, ofGetHeight()*CaptureAV::progress);
		if(recButton.visible) {
			recButton.setVisible(false);
			pauseButton.setVisible(false);
			exportButton.setVisible(false);
		}
	} else {
		
		if(!recButton.visible && drawMenu) {
			recButton.setVisible(true);
			pauseButton.setVisible(true);
			exportButton.setVisible(true);
		} else if(recButton.visible && !drawMenu) {
			recButton.setVisible(false);
			pauseButton.setVisible(false);
			exportButton.setVisible(false);
		}
			
		capture.draw();
		
		ofSetColor(255, 255, 255);
		if(drawMenu) {
			
			ofDrawBitmapString("KEYS\n----\n\n[   ] (space bar) toggle record/play mode\n[ ` ] show/hide this menu\n[ ; ] decrement display mode\n[ ' ] increment display mode\n[ / ] save samples\n[ - ] toggle auto trigger mode\n[esc] quit\nAll other keys are recordable", 20, 50);
			
			
			int modeOffset = 240;
			ofDrawBitmapString("MODES\n-----", 20, modeOffset-30);
			for(int i = 0; i < MODE_COUNT; i++) {
				string str = "- ";
				if(Sample::mode==i) {
					str = "* ";
					ofSetColor(255, 255, 255);
				} else {
					ofSetColor(150, 150, 150);
				}
				str += 	Sample::getModeString(i);
				ofDrawBitmapString(str, 20, modeOffset+i*20);
			}
			if(statusMessage!="") {
				ofSetColor(0);
				ofRect(0, ofGetHeight()-15, ofGetWidth(), 15);
				ofSetHexColor(0xFFFFFF);
				ofDrawBitmapString(statusMessage, 10, ofGetHeight()-3);
			}
		}
		ofEnableAlphaBlending();
		ofSetColor(50, 50, 100, 120);
		ofRect(14, 438, 74, 110);
		ofSetHexColor(0xFFFFFF);
		ofDrawBitmapString("Exporter", 20, 453);
	}
	
	
}




//--------------------------------------------------------------
void testApp::keyPressed(int key){
	if(key==' ') capture.sampler.toggleRecordMode();
	else if(key==';') {
		Sample::mode--;
		if(Sample::mode<0) Sample::mode = MODE_COUNT - 1;
	}
	else if(key=='\'') {
		Sample::mode++;
		if(Sample::mode>MODE_COUNT-1) Sample::mode = 0;
	}
	else if(key=='/') capture.save();
	else if(key=='-') capture.sampler.toggleAutoTrigger();

	else if(key=='=') {if(capture.sampler.recorder!=NULL) {capture.sampler.recorder->save();}}
	else if(key=='`') drawMenu ^= 1;
	else {
		capture.sampler.notePressed(capture.mapQwertyToMidi(key), 100);
	}
	
}

//--------------------------------------------------------------
void testApp::keyReleased(int key){
	if(key!=' ' && key!='/' && key!= '\'' && key!=';' && key!= '\\' && key!='=') capture.sampler.noteReleased(capture.mapQwertyToMidi(key));
}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h){

}

void testApp::audioRequested(float * output, int bufferSize, int nChannels) {
	capture.audioRequested(output, bufferSize, nChannels);
}

void testApp::audioReceived(float * input, int bufferSize, int nChannels) {
	capture.audioReceived(input, bufferSize, nChannels);
}


