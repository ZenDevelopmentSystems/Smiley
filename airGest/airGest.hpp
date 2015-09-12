//airGest.hpp

#ifndef AIRGESTURE_HPP
#define AIRGESTURE_HPP

#include <iostream>
#include <string.h>

#include <queue>
#include <math.h>

#include <opencv2/opencv.hpp>

//Frame dim to process
#define OPTFLW_FRAME_WIDTH	160
#define OPTFLW_FRAME_HEIGHT	120
//this is the step for taking features from global flow map
#define FEATURE_STEP 20

//definitions for Planes in sampled Flow Mat
#define PLANE_XCMP	0
#define PLANE_YCMP	1
#define PLANE_MAGN	2
#define PLANE_ANGL	3

//State machine definitions for gest detector
#define AIRGEST_NOTINITIALIZED 	0
#define AIRGEST_INITIALIZED		1
#define AIRGEST_ACTIVE			2
#define AIRGEST_DEACTIVE		3

//Gesture definitions
#define GEST_INVALID		0
#define GEST_NEXT			1
#define GEST_PREV			2

//Blur kernel size for filtering out noises
#define BLUR_KERNEL_SIZE Size( 7, 7 )

//Pen color to draw optical flow
#define COLOR_FLOWMAP CV_RGB(  20, 250,  20)

//Queue size of the flowHistory mats
#define QUEUESIZE_FLOWHIST	10			// only 1sec is considered now

typedef unsigned char airGestState;
typedef unsigned char airGestType;

using namespace cv;

struct flowVector {
	float mag;
	float ang;
	float xComp;
	float yComp;
};

class airGest {
		airGestState currState;
	
		Mat prevFrame;
		Mat	currFrame;
		Mat flowMap;
		std::deque<Mat> flowHistory;
		Mat accFlow;		//accumulated flow of last #QUEUESIZE_HIST frames, calculated from flowHistory
		
		//to implement psuedo logic
		// float xMotionWeights[OPTFLW_FRAME_HEIGHT/FEATURE_STEP * OPTFLW_FRAME_WIDTH/FEATURE_STEP];
		float *xMotionWeights;
		float decision;
		
		Mat canvas;			//to draw current flow
		//Mat accCanvas;		// to draw accumulated flow
		
		void drawFlowMap( void );
		void filterFlow( void );
		
	public:
		airGest( void );
		airGest( Mat );
		unsigned char setState( airGestState stateToSet );
		airGestState getState( void );
		airGestType analyseGesture( Mat frame );
		
		//debug
		//void printFMea( void );
		
};

#endif
