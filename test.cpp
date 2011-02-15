#include <stdio.h>
#include <fstream>
using namespace std;
#define MAX_FRAMES 1000000

struct Frame {
	unsigned long time;
	int note;
	int velocity;
};

// the 'score'
Frame frames[MAX_FRAMES];
int main() {
	fstream file;
	file.open("record_cache", fstream::in | fstream::binary);
	if ( (file.rdstate() & fstream::failbit ) != 0 ) {
		printf("Failed to load\n");
		return 1;
	}
	file.read((char*) frames, sizeof(Frame)*MAX_FRAMES);
	file.close();
	
	for(int i = 0; i < 100; i++) {
		printf("%d\n", frames[i].note);
	}
	return 0;
}