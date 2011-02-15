/*
 *  RecordButton.h
 *  openFrameworks
 *
 *  Created by Marek Bereza on 03/01/2010.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */
#include "ofxMSAInteractiveObject.h"
void buttonPressed(int id);
class mbButton: public ofxMSAInteractiveObject {
public:
	string title;
	int id;
	
	bool enabled;
	bool visible;
	mbButton() {
		enabled = true;
		visible = true;
		title = "";
	}
	void setEnabled(bool isEnabled) {
		if(isEnabled!=enabled) {
			if(isEnabled) enableMouseEvents();
			else disableMouseEvents();
		}
		enabled = isEnabled;
	}

	void setVisible(bool isVisible) {
		visible = isVisible;
	}
	void setup() {
		
		enableMouseEvents();
		width = 60;
		height = 20;
	}
	
	void load(int _x, int _y, string _title, int _id) {
		x = _x;
		y = _y;
		title = _title;
		id = _id;
	}
	
	void draw() {
		if(visible) {
			ofFill();
			if(!enabled) ofSetColor(100,100,100);
			else if(isMouseDown()) ofSetColor(0,255,0);
			else if(isMouseOver()) ofSetColor(255, 0, 0);
			else ofSetColor(150, 0, 0);
			ofRect(x,y, width, height);
			ofSetColor(255, 255, 255);
			ofDrawBitmapString(title, x, y+14);
		}
	}
	
	virtual void onPress(int x, int y, int button) {
		if(enabled) buttonPressed(id);
		
	}
};
