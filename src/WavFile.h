/**
 * WavFile.h
 *
 * Marek Bereza 2010
 * 
 * Originally stolen from somewhere.
 * 
 * Version 3
 */
#include <stdio.h>

#include <math.h>
#include <fstream>
#include <iostream>
#include <string>
using namespace std;

#ifndef MIN
#define MIN(x,y) (((x) < (y)) ? (x) : (y))
#endif

#define WAVFILE_READ	1
#define WAVFILE_WRITE	2

class WavFile {
private:
	
	fstream *file;
	
public:
	float *data;
	int channels;
	int samplerate;
	int bitsPerSample;
	int length;
	
	void setFormat(int _channels, int _samplerate, int _bitsPerSample) {
		channels = _channels;
		samplerate = _samplerate;
		bitsPerSample = _bitsPerSample;
	}
	
	WavFile() {
		channels = 1;
		samplerate = 44100;
		bitsPerSample = 16;
		data = NULL;
		file = NULL;
	}
	
	
	~WavFile() {
		close();
	}
	
	
	bool open(const char *path, int RW, int buffLength) {
		close();
		if(RW==WAVFILE_WRITE) {
			short myFormat = 1;
			
			int mySubChunk1Size = 16;
			int myByteRate = samplerate * channels * bitsPerSample/8;
			short myBlockAlign = channels * bitsPerSample/8;
			
			int myChunkSize = 36 + buffLength*bitsPerSample/8;
			int myDataSize = buffLength*bitsPerSample/8;
			
			close();
			file = new fstream(path, ios::out | ios::binary);
			
			// write the wav file per the wav file format
			file->seekp (0, ios::beg); 
			file->write ("RIFF", 4);
			file->write ((char*) &myChunkSize, 4);
			file->write ("WAVE", 4);
			file->write ("fmt ", 4);
			file->write ((char*) &mySubChunk1Size, 4);
			file->write ((char*) &myFormat, 2); // should be 1 for PCM
			file->write ((char*) &channels, 2); // # channels (1 or 2)
			file->write ((char*) &samplerate, 4); // 44100
			file->write ((char*) &myByteRate, 4); //
			file->write ((char*) &myBlockAlign, 2);
			file->write ((char*) &bitsPerSample, 2); //16
			file->write ("data", 4);
			file->write ((char*) &myDataSize, 4);
			return true;
		} else {
			printf("WavFile: WAVFILE_READ not implemented yet!\n");
			return false;
		}
	}
	
	
	bool write(float *samples, int buffLength) {
		if(file==NULL) return false;
		short buff[buffLength];
		for(int i = 0; i < buffLength; i++) {
			buff[i] = round(samples[i]*32760.f);
		}
		file->write ((char*)buff, buffLength*sizeof(short));
		return true;
	}
	
	
	
	void close() {
		if(file!=NULL) {
			file->close();
			file = NULL;
		}
	}
	
	
#define WRITE_BUFF_SIZE 256
	// write out the wav file
	bool save(const char *path) {
		
		if(data==NULL) return false;
		
		open(path, WAVFILE_WRITE, length);
		
		short buff[WRITE_BUFF_SIZE];
		int pos = 0;
		while(pos<length) {
			int len = MIN(WRITE_BUFF_SIZE, length-pos);
			for(int i = 0; i < len; i++) {
				buff[i] = round(data[pos]*32760.f);
				pos++;
			}
			file->write ((char*)buff, len*bitsPerSample/8);
		}
		
		close();
		return true;
	}
	
	// this gives you data that you have to free yourself
	float *getData() {
		float *d = data;
		data = NULL;
		return d;
	}
	
	// length is the length in samples, not frames 
	// - must give malloced memory
	void setData(float *_data, int _length) {
		data = _data;
		length = _length;
	}
	
	/**
	 * Reads an entire file into the data member of this class. It dynamically
	 * allocates the memory, and never bothers to delete it (unless you call this
	 * method again), so take care.
	 */
	bool load(char* path) {
		
		if(file!=NULL) close();
		file = new fstream(path, ios::in | ios::binary);
		short myFormat,myBlockAlign;		
		int mySubChunk1Size,myByteRate;
		int myChunkSize,myDataSize;
		short format = 1;
		file->seekg(4, ios::beg);
		file->read( (char*) &myChunkSize, 4 ); // read the ChunkSize
		file->seekg(16, ios::beg);
		file->read( (char*) &mySubChunk1Size, 4 ); // read the SubChunk1Size
		file->read( (char*) &format, sizeof(short) ); // read the file format.  This should be 1 for PCM
		file->read( (char*) &channels, sizeof(short) ); // read the # of channels (1 or 2)
		file->read( (char*) &samplerate, sizeof(int) ); // read the samplerate
		file->read( (char*) &myByteRate, sizeof(int) ); // read the byterate
		file->read( (char*) &myBlockAlign, sizeof(short) ); // read the blockalign
		file->read( (char*) &bitsPerSample, sizeof(short) ); // read the bitspersample
		
		file->seekg(40, ios::beg);
		file->read( (char*) &length, sizeof(int) ); // read the size of the data
		
		length /= sizeof(short);
		if(bitsPerSample!=16 || length<=0 || format!=1) {
			return false;
		}
		
		// read the data chunk
		short *rawData = new short[length];
		file->seekg(44, ios::beg);
		file->read((char*)rawData, length*sizeof(short));
		file->close(); // close the input file
		
		if(data!=NULL) free(data);
		// bytes to shorts
		data = new float[length];
		for(int i = 0; i < length; i++) {
			data[i] = ((float)rawData[i])/32767.f;
		}
		delete rawData;
		
		close();
		
		return true; // this should probably be something more descriptive
	}
};