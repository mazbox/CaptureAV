#ifndef _TEST_APP
#define _TEST_APP


#include "ofMain.h"
#include "Playback.h"
#include "CaptureAV.h"
#include "mbButton.h"
class testApp : public ofBaseApp{

public:

	
	// interface
	CaptureAV capture;
	mbButton recButton;
	mbButton pauseButton;
	mbButton exportButton;
	void setup();
	void update();
	void draw();

	void keyPressed  (int key);
	void keyReleased(int key);
	void mouseMoved(int x, int y );
	void mouseDragged(int x, int y, int button);
	void mousePressed(int x, int y, int button);
	void mouseReleased(int x, int y, int button);
	void windowResized(int w, int h);
	void audioRequested(float * output, int bufferSize, int nChannels);
	void audioReceived(float * input, int bufferSize, int nChannels);
	void buttonPressed(int id);
	bool drawMenu;

	bool isHidingGui;
	
	void log(string msg) {
		statusMessage = msg;
	}
	string statusMessage;
};

#endif
