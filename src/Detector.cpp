/*
 *  Detector.cpp
 *  openFrameworks
 *
 *  Created by Marek Bereza on 17/01/2009.
 *  Copyright 2009 Royal College of Art. All rights reserved.
 *
 */

#include "Detector.h"
#define ABS(A) (A>0?A:-A)
float lastValue = 0.f;
float Detector::process(float* in, int length) {
	float max = 0;
	for(int i = 0; i < length; i++) {
		if(max<ABS(in[i])) max = ABS(in[i]);
	}
	if(max<lastValue) max = (lastValue*0.95f) + (max*0.05f);
	lastValue = max;
	return max;
}
