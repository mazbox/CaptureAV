/*
 *  SamplerFileIO.cpp
 *  openFrameworks
 *
 *  Created by Marek Bereza on 18/01/2009.
 *  Copyright 2009 Royal College of Art. All rights reserved.
 *
 */

#include "Sampler.h"
#include "Sample.h"
#include <fstream>
#include <sys/stat.h>
#include <errno.h>
#include "ofMain.h"
#include <sys/types.h>
#include <dirent.h>
#include <vector>
#include "gig.h"
#include "CaptureAV.h"

#ifndef DEBUG
#define DEBUG false
#endif



// reverse:  reverse string s in place 
void reverse(char s[])
{
    int c, i, j;
	
    for (i = 0, j = strlen(s)-1; i<j; i++, j--) {
        c = s[i];
        s[i] = s[j];
        s[j] = c;
    }
}
// itoa:  convert n to characters in s 
void itoa(int n, char s[])
{
    int i, sign;
	
    if ((sign = n) < 0)  // record sign 
        n = -n;          // make n positive 
    i = 0;
    do {       // generate digits in reverse order 
        s[i++] = n % 10 + '0';   // get next digit 
    } while ((n /= 10) > 0);     // delete it 
    if (sign < 0)
        s[i++] = '-';
    s[i] = '\0';
    reverse(s);
} 




void floatToPCM16(float *in, short int *out, long length) {
	for(long l = 0; l < length; l++) {
		out[l] = (short int) (in[l]*32767.f);
	}
}

void pcm16ToFloat(short int *in, float *out, long length) {
	for(long l = 0; l < length; l++) {
		out[l] = (float) (in[l]*1.f/32767.f);
	}
}

/**
 * Saves the contents of the sampler
 * to a file.
 */
bool Sampler::save(string path) {
	CaptureAV::busy = true;
	bool ret = saveGig(ofToDataPath(path, true)+".gig") && saveVideo(path);
	CaptureAV::busy = false;
	return ret;

}



bool Sampler::load(string path) {
	CaptureAV::busy = true;
	bool ret = loadGig(ofToDataPath(path+".gig", true)) && loadVideo(path);
	CaptureAV::busy = false;
	return ret;
}

#include "ofxDirList.h"

/*
 
 DIR.setVerbose(false);
 nImages = DIR.listDir("images/of_logos");
 images = new ofImage[nImages];
 //you can now iterate through the files as you like
 for(int i = 0; i < nImages; i++){
 images[i].loadImage(DIR.getPath(i));
 }
 currentImage = 0;
 
 ofBackground(255,255,255);
 */

bool Sampler::loadVideo(string path) {
	ofxDirList   DIR;
	int nImages = DIR.listDir("");
	printf("There are %d images\n", nImages);
	ofImage img;
	int loadedImages = 0;
	map<int,Sample*>::iterator it;
	for ( it=samples.begin() ; it != samples.end(); it++ ) {
		int key = (*it).first;
		Sample *_sample = (*it).second;
		_sample->frameCount = 0;
		
		for(int i = 0; i < nImages; i++) {
			string filenameStart = "sample-"+ofToString(key)+"_";
			
			if(DIR.getPath(i).find(filenameStart)!=string::npos) {
				_sample->frameCount++;
			}
		}
		printf("Loading %d frames from sample %d--------------------------\n", _sample->frameCount, key);
		for(int i = 0; i < _sample->frameCount; i++) {
			// load all video into videobuffer
			string f = "sample-";
			f += ofToString(key);
			f += "_";
			f += ofToString(i);
			f += ".bmp";
			img.setUseTexture(false);
			img.loadImage(f);
			
			memcpy(videoBuffer[i], img.getPixels(), VIDEO_WIDTH*VIDEO_HEIGHT*3*sizeof(char));
			printf("Loading %s\n", f.c_str());
			loadedImages++;
			CaptureAV::progress = (float)loadedImages/nImages;
		}
		if(_sample->frameCount>0) _sample->setFrames(videoBuffer, _sample->frameCount);
	}
	return true;
}

bool Sampler::saveVideo(string path) {
	int imagesSaved = 0;
	ofImage img;
	map<int,Sample*>::iterator it;
	
	// count how many frames there are to save
	int totalImages = 0;
	for ( it=samples.begin() ; it != samples.end(); it++ ) {
		totalImages += (*it).second->frameCount;
	}
	
	img.setUseTexture(false);
	
	img.allocate(VIDEO_WIDTH, VIDEO_HEIGHT, OF_IMAGE_COLOR);
	
	for ( it=samples.begin() ; it != samples.end(); it++ ) {
		int key = (*it).first;
		Sample *_sample = (*it).second;
		printf("Saving sample %d to disk---------------------------------\n", key);
		for(int i = 0; i < _sample->frameCount; i++) {
			img.setFromPixels(_sample->videoData[i], VIDEO_WIDTH, VIDEO_HEIGHT, OF_IMAGE_COLOR, true);
			string f = "sample-";
			f += ofToString(key);
			f += "_";
			f += ofToString(i);
			f += ".bmp";
			img.saveImage(f);
			printf("Saving %s\n", f.c_str());
			imagesSaved++;
			CaptureAV::progress = (float)imagesSaved/totalImages;
		}
	}
}



bool Sampler::saveDLS(string path) {
	try {
		
		// where to store it
		//string path = "presets/"+name+".gig";
		
		printf("Saving to %s\n", path.c_str());
		// create a file object
		DLS::File file;
		
		//DLS::Group* group = file.AddGroup();
		//group->Name = "Main";
		
		file.pInfo->Name = "TEST";
		
		// create an instrument
		DLS::Instrument* pInstrument = file.AddInstrument();
		DLS::Sample* pSamples[samples.size()];
		DLS::Region* pRegions[samples.size()];

		pInstrument->IsDrum = false;
		
		map<int,Sample*>::iterator it;
		int i = 0;
		for ( it=samples.begin() ; it != samples.end(); it++ ) {
			
			int key = (*it).first;
			sample = (*it).second;
			
			
			char chars[20];
			
			itoa(i, chars);
			string name = "Sample "+ string(chars);
			
			printf("sample %d is %d long\n", i, sample->length);
			pSamples[i] = file.AddSample();
			pSamples[i]->pInfo->Name = name;
			pSamples[i]->Channels = 1; // mono
			pSamples[i]->BitDepth = 16; // 16 bits
			pSamples[i]->FrameSize = 16/*bitdepth*/ / 8/*1 byte are 8 bits*/ * 1/*mono*/;
			pSamples[i]->SamplesPerSecond = 44100;
			pSamples[i]->Resize(sample->length);
			
			//group->AddSample(pSamples[i]);
			
			pRegions[i] = pInstrument->AddRegion();
			pRegions[i]->SetSample(pSamples[i]);
			pRegions[i]->SetKeyRange(key, key);
			pRegions[i]->UnityNote = key;
			//pRegions[i]->pDimensionRegions[0]->pSample = pSamples[i];
			//pRegions[i]->pDimensionRegions[0]->UnityNote = key;
			i++;
		}
		
				
		file.Save(path);
		i = 0;
		for ( it=samples.begin() ; it != samples.end(); it++ ) {
			
			int key = (*it).first;
			sample = (*it).second;
			
			
			printf("again sample %d is %d long\n", i, sample->length);
			
			short int data[sample->length];
			floatToPCM16(sample->data, data, sample->length);
			pSamples[i]->Write(data, sample->length);
			i++;
		}
		printf("Save successful\n");
		return true;
	} catch (RIFF::Exception e) {
		e.PrintMessage();
		return false;
	}
}



bool Sampler::loadDLS(string path) {
	samples.clear();
	printf("Sampler::load called with '%s'\n", path.c_str());
			
	RIFF::File* riff = new RIFF::File(path);
	DLS::File*  DLS  = new DLS::File(riff);

	if(!DLS || DLS->Instruments!=1) {
		printf("invalid dls file\n");
		return false;
	}
	DLS::Instrument *instrument = DLS->GetFirstInstrument();
	if(instrument) {
		DLS::Region* pRegion = instrument->GetFirstRegion();
		while (pRegion) {
			
			DLS::Sample* pSample = pRegion->GetSample();		
			
			short int *sampleData = (short int*)pSample->LoadSampleData();//.pStart;
			long length = pSample->GetSize();
			int key = pRegion->KeyRange.low;
			float fData[length];
			pcm16ToFloat(sampleData, fData, length);
			samples[key] = new Sample(fData, length);
	
			printf("Sample for key %d is %ld samples long\n", key, length);
			
			pRegion = instrument->GetNextRegion();
	
		}
		
		delete DLS;
		delete riff;
		
		return true;
	}
	return false;
}












/**
 * Saves the contents of the sampler
 * to a file.
 */
bool Sampler::saveGig(string path) {
	try {
		
		// where to store it
		//string path = "presets/"+name+".gig";
		
		printf("Saving to %s\n", path.c_str());
		// create a file object
		gig::File file;
		
		gig::Group* group = file.AddGroup();
		group->Name = "Main";
		
		file.pInfo->Name = "TEST";
		
		// create an instrument
		gig::Instrument* pInstrument = file.AddInstrument();
		gig::Sample* pSamples[samples.size()];
		gig::Region* pRegions[samples.size()];

		pInstrument->IsDrum = false;
		
		map<int,Sample*>::iterator it;
		int i = 0;
		for ( it=samples.begin() ; it != samples.end(); it++ ) {
			
			int key = (*it).first;
			sample = (*it).second;
			
			
			char chars[20];
			
			itoa(i, chars);
			string name = "Sample "+ string(chars);
			
			pSamples[i] = file.AddSample();
			pSamples[i]->pInfo->Name = name;
			pSamples[i]->Channels = 1; // mono
			pSamples[i]->BitDepth = 16; // 16 bits
			pSamples[i]->FrameSize = 16/*bitdepth*/ / 8/*1 byte are 8 bits*/ * 1/*mono*/;
			pSamples[i]->SamplesPerSecond = 44100;
			pSamples[i]->Resize(sample->length);
			
			group->AddSample(pSamples[i]);
			
			pRegions[i] = pInstrument->AddRegion();
			pRegions[i]->SetSample(pSamples[i]);
			pRegions[i]->SetKeyRange(key, key);
			pRegions[i]->UnityNote = key;
			pRegions[i]->pDimensionRegions[0]->pSample = pSamples[i];
			pRegions[i]->pDimensionRegions[0]->UnityNote = key;
			i++;
		}
		
				
		file.Save(path);
		i = 0;
		for ( it=samples.begin() ; it != samples.end(); it++ ) {
			sample = (*it).second;
			short int data[sample->length];
			floatToPCM16(sample->data, data, sample->length);
			pSamples[i]->Write(data, sample->length);
			i++;
		}
		
		return true;
	} catch (RIFF::Exception e) {
		e.PrintMessage();
		return false;
	}
}


bool Sampler::loadGig(string path) {
	try {
	samples.clear();
	printf("Sampler::load called with '%s'\n", path.c_str());
			
	RIFF::File* riff = new RIFF::File(path);
	gig::File*  gig  = new gig::File(riff);

	if(gig->Instruments!=1) {
		printf("invalid giga file\n");
	}
	gig::Instrument *instrument = gig->GetFirstInstrument();
	gig::Region* pRegion = instrument->GetFirstRegion();
    while (pRegion) {
		
		gig::Sample* pSample = pRegion->GetSample();		
		
		short int *sampleData = (short int*)pSample->LoadSampleData().pStart;
		long length = pSample->GetSize();
		int key = pRegion->KeyRange.low;
		float fData[length];
		pcm16ToFloat(sampleData, fData, length);
		samples[key] = new Sample(fData, length);

		printf("Sample for key %d is %ld samples long\n", key, length);
		
		pRegion = instrument->GetNextRegion();

	}
	
	delete gig;
	delete riff;
	
	return true;
	} catch(RIFF::Exception e) {
		e.PrintMessage();
		return false;
	}
}
