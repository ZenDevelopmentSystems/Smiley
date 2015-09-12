//airGest.cpp

#include "airGest.hpp"

//Constructor : Sets the state to NOT_INITIALIZED

airGest::airGest( void ) {
	
	currState = AIRGEST_NOTINITIALIZED;
}

//Constructor
//Copies given frame to prevFrame and sets the state to INITAILIZED
// sets accFlow to 0
airGest::airGest( Mat frame ) {
		
	//Convert the frame to grey scale and store as prev
	//Convert to gray scale
    if( frame.channels() == 3 ) {
		cvtColor( frame, prevFrame, CV_BGR2GRAY );
	}
	else if( frame.channels() == 4 ) {
		cvtColor( frame, prevFrame, CV_BGRA2GRAY );
	}
	else {	//already gray
		prevFrame = frame.clone();
	}
	//now resize it to macth the processing frame dim
	resize( prevFrame,
			prevFrame,
			Size( OPTFLW_FRAME_WIDTH, OPTFLW_FRAME_HEIGHT ),
			0,
			0,
			INTER_AREA );
	
	currState = AIRGEST_INITIALIZED;
	
	accFlow = Mat( OPTFLW_FRAME_WIDTH/FEATURE_STEP,
				   OPTFLW_FRAME_HEIGHT/FEATURE_STEP,
				   CV_32FC2,
				   Scalar( 0.00, 0.00 ) );

	//~ xMotionWeights[OPTFLW_FRAME_HEIGHT/FEATURE_STEP * OPTFLW_FRAME_WIDTH/FEATURE_STEP] = {
						//~ -2.00, -1.00, +0.00, +2.00, +2.00, +0.00, -2.00, -2.00,
						//~ -2.00, +0.00, +2.00, +2.00, +2.00, +2.00, +0.00, -2.00,
						//~ +2.00, +2.00, +2.00, +2.00, +2.00, +2.00, +2.00, +2.00,
						//~ +2.00, +2.00, +2.00, +2.00, +2.00, +2.00, +2.00, +2.00,
						//~ -2.00, +0.00, +2.00, +2.00, +2.00, +2.00, +0.00, -2.00,
						//~ -2.00, -2.00, +0.00, +2.00, +2.00, +0.00, -2.00, -2.00
					 //~ };
	static float weights[OPTFLW_FRAME_HEIGHT/FEATURE_STEP * OPTFLW_FRAME_WIDTH/FEATURE_STEP] = {
		-2.00, -1.00, +0.00, +2.00, +2.00, +0.00, -2.00, -2.00,
		-2.00, +0.00, +2.00, +2.00, +2.00, +2.00, +0.00, -2.00,
		+2.00, +2.00, +2.00, +2.00, +2.00, +2.00, +2.00, +2.00,
		+2.00, +2.00, +2.00, +2.00, +2.00, +2.00, +2.00, +2.00,
		-2.00, +0.00, +2.00, +2.00, +2.00, +2.00, +0.00, -2.00,
		-2.00, -2.00, +0.00, +2.00, +2.00, +0.00, -2.00, -2.00
	};
	xMotionWeights = new float[OPTFLW_FRAME_HEIGHT/FEATURE_STEP * OPTFLW_FRAME_WIDTH/FEATURE_STEP];
	memcpy( xMotionWeights, weights, (OPTFLW_FRAME_HEIGHT/FEATURE_STEP)*(OPTFLW_FRAME_WIDTH/FEATURE_STEP)*sizeof( float ));
	
}

/*  Member function
 *  Sets the state of detector to 'stateToState'
 *  but sets to DEACTIVE state if the prevFrame is empty
 *  always returns 0; 
//  #TODO : modify to return currentState
 */
unsigned char airGest::setState( airGestState stateToSet ) {
	
	currState = stateToSet;
	
	if( stateToSet == AIRGEST_ACTIVE && prevFrame.empty() ) {
		currState = AIRGEST_DEACTIVE;
	}
	
	return 0;
}

/*	Member function
 *  returns the current state of detector
 */
airGestState airGest::getState( void ) {
	
	return currState;
}

/*	Member function
 * retuns the detected gesture ID
 * Inputs :		frame -> current frame to analyse
 * 				faceRegion -> current face Region
 */
airGestType airGest::analyseGesture( Mat frame ) {
	
	airGestType gest = GEST_INVALID;
	
	resize( frame,
			frame,
			Size( OPTFLW_FRAME_WIDTH, OPTFLW_FRAME_HEIGHT ),
			0,
			0,
			INTER_AREA );
	
	//Prepare canvas to draw intermediate result
	//canvas = frame.clone();
	canvas = Mat( OPTFLW_FRAME_HEIGHT, OPTFLW_FRAME_WIDTH, CV_8UC3, Scalar( 0, 0, 0 ) );
	//accCanvas = Mat( OPTFLW_FRAME_HEIGHT, OPTFLW_FRAME_WIDTH, CV_8UC3, Scalar( 0, 0, 0 ) );
	
	Mat grayFrame;
	//Convert to gray scale
    if( frame.channels() == 3 ) {
		cvtColor( frame, grayFrame, CV_BGR2GRAY );
	}
	else if( frame.channels() == 4 ) {
		cvtColor( frame, grayFrame, CV_BGRA2GRAY );
	}
	else {	//already gray
		grayFrame = frame.clone();
	}
	
	if( currState != AIRGEST_ACTIVE ) { //unless active, return with an invalid code
		prevFrame = grayFrame.clone();
		return gest;
	}
	
	//Here, airGest is active
	currFrame = grayFrame;
	//~ std::cout << "[airGest::analyseGesture] calculating optical flow....";
	calcOpticalFlowFarneback( prevFrame,  // first 8-bit single channel input image
                              currFrame,  // Second image of the same size and same type as prevgray
                              flowMap,    // computed flow image tha has the same size as pregray and type CV_32FC2
							  0.5,       // pryScale, 0.5 means classical pyramid
							  3,          // number of pyramid layers including initial image
                              20,         // winSize
                              3,          // number of iterations the algorithm does at each pyramid level
                              5,          // Size of the pixel neighborhood used to find polynomial expansion in each pixel
                              1.1,        // standard deviation of Guassian
                              OPTFLOW_FARNEBACK_GAUSSIAN );        //
    //~ std::cout << "[COMPLETED]\n";
    //~ std::cout << "-> boxFilter";
    boxFilter( flowMap, flowMap , -1, BLUR_KERNEL_SIZE );
    //~ std::cout << "-> drawFlowMap";
    drawFlowMap();
    //~ std::cout << "-> filterFlow";
    filterFlow();
    
    //imshow( "Current flow", canvas );
    //imshow( "Accumulated Flow", accCanvas );
	
	//copy current frame to prev for using next time
	prevFrame = currFrame.clone();
	
	gest = ( decision == -1.00 )? GEST_PREV:
		   ( decision == +1.00 )? GEST_NEXT:
		   GEST_INVALID;
	
	return gest;
}

/*	Member function( private )
 * Draws the calculated optical flow on the canvas
 */
void airGest::drawFlowMap( void )
{
	int x, y;
	CvMat flowMat = flowMap;
	
	for( y = FEATURE_STEP/2; y < canvas.rows; y += FEATURE_STEP ) {
        for( x = FEATURE_STEP/2; x < canvas.cols; x += FEATURE_STEP ) {
            CvPoint2D32f fxy = CV_MAT_ELEM( flowMat, CvPoint2D32f, y, x);
            line( canvas, 
				  Point(x,y), 
				  Point(cvRound(x+fxy.x), cvRound(y+fxy.y)), 
				  COLOR_FLOWMAP, 1, 8, 0 );
		}
	}
	
	//CvMat accFlowMat = accFlow;
	
	//std::cout << "dim( accFlow ) = " << accFlow.rows << "x" << accFlow.cols << "\n";
	//for( y = FEATURE_STEP/2; y < canvas.rows; y += FEATURE_STEP ) {
		//for( x = FEATURE_STEP/2; x < canvas.cols; x += FEATURE_STEP ) {
			//CvPoint2D32f fxy = CV_MAT_ELEM( accFlowMat, CvPoint2D32f, y/FEATURE_STEP, x/FEATURE_STEP );
			//line( accCanvas,
				  //Point( x, y ),
				  //Point( cvRound( x + fxy.x ), cvRound( y + fxy.y ) ),
				  //COLOR_FLOWMAP, 1, 8, 0 );
			//std::cout << "(x,y) = " << x << "," << y << std::endl;
		//}
	//}
}


void airGest::filterFlow( void ) {
	//~ std::cout << "in filterFlow " << ", ";
	float xFlow, yFlow;
					 
	Mat currFlow = Mat( OPTFLW_FRAME_WIDTH/FEATURE_STEP,
						OPTFLW_FRAME_HEIGHT/FEATURE_STEP,
						CV_32FC2 );
	
	vector<Mat> sampledChannels;
	split( currFlow, sampledChannels );				
	
	CvMat flowMat = flowMap;	
	float xMin = CV_MAT_ELEM( flowMat, CvPoint2D32f, FEATURE_STEP/2, FEATURE_STEP/2).x;
	float xMax = CV_MAT_ELEM( flowMat, CvPoint2D32f, FEATURE_STEP/2, FEATURE_STEP/2).x;
	float yMin = CV_MAT_ELEM( flowMat, CvPoint2D32f, FEATURE_STEP/2, FEATURE_STEP/2).y;
	float yMax = CV_MAT_ELEM( flowMat, CvPoint2D32f, FEATURE_STEP/2, FEATURE_STEP/2).y;
	for( int y = FEATURE_STEP/2; y < canvas.rows; y += FEATURE_STEP ) {
        for( int x = FEATURE_STEP/2; x < canvas.cols; x += FEATURE_STEP ) {
            CvPoint2D32f fxy = CV_MAT_ELEM( flowMat, CvPoint2D32f, y, x);
            
            fxy.x = ( ( fxy.x > -2.0 && fxy.x < 2.0 ) || ( fxy.x != fxy.x ) )? 0.00: fxy.x;
            fxy.y = ( ( fxy.y > -2.0 && fxy.y < 2.0 ) || ( fxy.y != fxy.y ) )? 0.00: fxy.y;
            
            sampledChannels[PLANE_XCMP].at<float>( x/FEATURE_STEP, y/FEATURE_STEP ) = fxy.x;
            sampledChannels[PLANE_YCMP].at<float>( y/FEATURE_STEP, y/FEATURE_STEP ) = fxy.y;
            
            if( fxy.x < xMin ) xMin = fxy.x;
            else if( fxy.x > xMax ) xMax = fxy.x;
            
            if( fxy.y < yMin ) yMin = fxy.y;
            else if( fxy.y > yMax ) yMax = fxy.y;
            
		}
	}
	
	float xAbsMax = ( xMax > -xMin )? xMax: -xMin;
	float yAbsMax = ( yMax > -yMin )? yMax: -yMin;
	
	//std::cout << "Filter : xMax/yMax -- ["
	//		  << xMin << ", " << xMax << ", "
	//		  << yMin << ", " << yMax << "] => " 
	//		  << xAbsMax << "," << yAbsMax << "\t\t";
			  
	//std::cout << sampledChannels[PLANE_XCMP] << "\n"
	//		  << sampledChannels[PLANE_YCMP] << "\n";
			  
	vector<Mat> hAccFlow;
	split( accFlow, hAccFlow );
	
	hAccFlow[PLANE_XCMP] += sampledChannels[PLANE_XCMP];
	hAccFlow[PLANE_YCMP] += sampledChannels[PLANE_YCMP];	
	
	flowHistory.push_back( sampledChannels[PLANE_XCMP] );	//first push X
    flowHistory.push_back( sampledChannels[PLANE_YCMP] );	//then push Y
    if( flowHistory.size() > 2 * QUEUESIZE_FLOWHIST ) {
		//now remove the first element
		hAccFlow[PLANE_XCMP] -= flowHistory[0];		//substract the first element X_CMP from AccFlow
		hAccFlow[PLANE_YCMP] -= flowHistory[1];		//substract the second element Y_CMP from AccFlow
		
		//remove those planes from history
		flowHistory.pop_front();
		flowHistory.pop_front();
	}
	merge( hAccFlow, accFlow );
	//std::cout << "historySize#" << flowHistory.size() << "\n";
	//std::cout << "history = \n" << accFlow;
	
	Mat motionDir = Mat( OPTFLW_FRAME_WIDTH/FEATURE_STEP,
						 OPTFLW_FRAME_HEIGHT/FEATURE_STEP,
						 CV_32FC1 );
						 
	//~ float xMotion[OPTFLW_FRAME_HEIGHT/FEATURE_STEP][OPTFLW_FRAME_WIDTH/FEATURE_STEP];
	
	Mat xMotion( OPTFLW_FRAME_HEIGHT/FEATURE_STEP, OPTFLW_FRAME_WIDTH/FEATURE_STEP, CV_32FC1, Scalar( 0 ));
	xMotion = hAccFlow[PLANE_XCMP];
	for( int y = 0; y < accFlow.rows; y++ ) {
		for( int x = 0; x < accFlow.cols; x++ ) {
			//~ float angle = atan2f( hAccFlow[PLANE_YCMP].at<float>( y, x ),
								  //~ hAccFlow[PLANE_XCMP].at<float>( y, x ) );
			//~ float tmp = hAccFlow[PLANE_XCMP].at<float>( y, x );
			//~ xMotion.at<float>( y, x ) = tmp;
			
			//~ angle = ( angle < 0 )? 2.0*22.0/7.0 + angle: angle;
			//~ if( angle != angle ) angle = 0.00;	//avoid nan
			//~ motionDir.at<float>( y, x ) = angle;
		}
	}
	//~ std::cout << "::calculating decision::" << ", ";
	decision = 0.00;
	for( int y = 0; y < OPTFLW_FRAME_HEIGHT/FEATURE_STEP; y++ ) {
		for( int x = 0; x < OPTFLW_FRAME_WIDTH/FEATURE_STEP; x++ ) {
			// decision += xMotionWeights[y][x] * xMotion[y][x];
			//~ decision += (*xMotionWeights[y*OPTFLW_FRAME_WIDTH/FEATURE_STEP+x]) * xMotion.at<float>(y,x);//[y][x];
			decision += *(xMotionWeights+y*OPTFLW_FRAME_WIDTH/FEATURE_STEP+x) * xMotion.at<float>(y,x);//[y][x];
			// std::cout << xMotion[y][x] << ", ";
		}
		//std::cout << "\n";
	}
	
	std::cout << "decision = " << decision << "\t";
	
	decision = ( decision < -50.00 ) ? -1.00: 
			   ( decision > +50.00 ) ? +1.00: 0.00;
	
	std::cout << "actual decision = " << decision << "\t";;
	
	Mat sampledVector = Mat( 1, motionDir.rows * motionDir.cols, CV_32FC1 );
	for( int rowId = 0; rowId < motionDir.rows; rowId++ ) {
		sampledVector.colRange( rowId*motionDir.cols, rowId*motionDir.cols+motionDir.cols ) = motionDir.rowRange( rowId, rowId );
	}
	//std::cout << "Sampled vector : " << sampledVector << "\n";
}
