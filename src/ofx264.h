/*
 *  x264.h
 *  openFrameworks
 *
 *  Created by Marek Bereza on 03/01/2010.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */
#include "common.h"
#include "output.h"

class ofx264 {

public:
	void setup() {   
		
		
		x264_param_t param;
		//cli_opt_t opt;
		int ret;
		x264_param_default( &param );
		param.i_width = VIDEO_WIDTH;
		param.i_height = VIDEO_HEIGHT;
		
		//param.i_deblocking_filter_alphac0 = -1;
		//param.i_deblocking_filter_beta = -1;
		//param.analyse.f_psy_trellis = 0.15;
		
		
		param.i_frame_total = 100;
		//param.i_log_level = X264_LOG_NONE;
		param.i_log_level = X264_LOG_DEBUG;
		x264_t *h;
		x264_picture_t pic;
		/*if( ( h = x264_encoder_open( &param ) ) == NULL )
		{
			fprintf( stderr, "x264 [error]: x264_encoder_open failed\n" );
			return;
		}*/
		
	}
};