/*
 *  CaptureAV.cpp
 *  openFrameworks
 *
 *  Created by Marek Bereza on 03/01/2010.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#include "CaptureAV.h"

float CaptureAV::progress = 0.f;
bool CaptureAV::busy = false;
string CaptureAV::busyString = "";

CaptureAV *CaptureAV::instance = NULL;
void loadCallback() {
	CaptureAV::busyString = "Loading...";
	CaptureAV::instance->sampler.load("default");
}

void saveCallback() {
	CaptureAV::busyString = "Saving...";
	CaptureAV::instance->sampler.save("default");
}
void exportCallback() {
	printf("Starting the export\n");
	CaptureAV::busyString = "Exporting...";
	CaptureAV::busy = true;
	CaptureAV::instance->enablePlayback();
	CaptureAV::instance->disablePlayback();
	CaptureAV::busy = false;
}