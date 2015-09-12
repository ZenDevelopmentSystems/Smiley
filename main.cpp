#include <iostream>
#include <stdio.h>
#include <stdlib.h>

#include "faceDetector/faceDetector.hpp"
#include "airGest/airGest.hpp"
#include "gallery/gallery.hpp"

//to exit from loop
#define KEY_ESC 27

//face threshold to detect as positive
#define FACE_THRESHOLD 5

//frame dimension for processing
#define PROCESSING_FRAME_WIDTH  320	//160
#define PROCESSING_FRAME_HEIGHT 240	//120

//no of frames for which gest should be active even if face is lost
#define FACETRACE_UPPERLIMIT 		20
#define FACETRACE_ACTIVATIONLIMIT 	8

using namespace std;
using namespace cv;

String faceCascade 	= "/usr/share/opencv/haarcascades/haarcascade_frontalface_default.xml";

int main( int argc, char *argv[] ) {
	
	int faceCount = 0;
	int faceTrace = 0; // stores the no.of frames in which faces where present, MAX=GEST_ACTIVE_WINDOW
	
	int currGestID = GEST_INVALID;
	int prevGestID = GEST_INVALID;
	int cumGestCnt = 0;
	bool trigger = false;
	bool prevTrigger = false;
	
	//initialise gallery
	String pathToGallery;
	if( argc > 1 ) {
		pathToGallery = string( argv[1] );
	}
	else {
		cerr << "ERROR : No path to gallery given\n";
		return -1;
	}
	gallery wall( "Air gesture demo", pathToGallery, 800, 549 );
	wall.displayFrame( -1 );
	float tGal = 0.00;
	float tGalLastTrig = 0.00;
	float tGalPrev = 0.00;
	
	VideoCapture capDevice;
    //if( argc > 1 )
      //  capDevice.open( argv[1] );
    //else //capture from cam
    capDevice.open( 0 );
    if( !capDevice.isOpened()){
        cout << "Failed to open CAM.\n";
        return 1;
    }
    
    //detection time for debbugging purposes
    double t;
    Mat frame;
	
	vector<Rect> faceRegion;    
    
    Mat display;

    //namedWindow( "Result",    CV_WINDOW_FREERATIO );
        
    capDevice >> frame;

    cout << "Capture Properties : " << frame.cols << " x " << frame.rows << endl;
    resize( frame, 
			frame, 
			Size( 0, 0) , 
			(float) PROCESSING_FRAME_HEIGHT/ frame.rows,
			(float) PROCESSING_FRAME_WIDTH / frame.cols,
			INTER_AREA );
    
    
    cerr << "Initialising system............................";
    faceDetector haarFaceDetector( frame, faceCascade, FACE_THRESHOLD );
    airGest gestDetector( frame );
    cerr << "COMPLETED\n";
    
    while( !frame.empty())
    {
		t = (double) getTickCount();
            faceRegion = haarFaceDetector.returnFaceRegions( frame );
        //t = ((double)getTickCount() - t)/getTickFrequency()*1000.;
        faceCount  = faceRegion.size();

        //combine
        groupRectangles( faceRegion, FACE_THRESHOLD );

        //display result
        for( vector<Rect>::const_iterator r = faceRegion.begin(); r != faceRegion.end(); r++ )
        {
			//for each face, draw a rectangle
            rectangle( frame,
                       Point( r->x, r->y ),//Point( absX, absY ),
                       Point( r->x + r->width, r->y + r->height ),//Point( absX+ r->width, absY + r->height ),
                       COLOR_FACEBOX, 1, 4, 0 );

        }
        
        if( faceRegion.size() > 0 ) {	//update face trace
			faceTrace = ((faceTrace+1)>FACETRACE_UPPERLIMIT)?FACETRACE_UPPERLIMIT:faceTrace+1;
		}
		else {
			faceTrace--;
			faceTrace = ( faceTrace < 0 )?0:faceTrace;
		}
			
		
		if( ( faceTrace < 1 ) && ( gestDetector.getState() == AIRGEST_ACTIVE ) ) {
			gestDetector.setState( AIRGEST_DEACTIVE );
		}
		else if( 
					(
						(gestDetector.getState() == AIRGEST_DEACTIVE) ||
						(gestDetector.getState() == AIRGEST_INITIALIZED)
					) && 
					( faceTrace > FACETRACE_ACTIVATIONLIMIT ) 
				) {
			gestDetector.setState( AIRGEST_ACTIVE );
		}
		
		currGestID = gestDetector.analyseGesture( frame );
		
		if( currGestID != GEST_INVALID ) {
			if( prevGestID == currGestID )	cumGestCnt++;
			else cumGestCnt = 0;
			
			if( cumGestCnt > 3 )
				trigger = true;
			else
				trigger = false;
			
		}
		else {
			trigger = false;
		}
		
		
		t = ((double)getTickCount() - t)/getTickFrequency()*1000.;
		
		//display the wall in gallery
		if( trigger && !prevTrigger ) {	//to detect a FALSE -> TRUE transition of trigger
			//tGalLastTrig = ((double)getTickCount() - t)/getTickFrequency()*1000.;
			//cerr << "CHANGE" << "\t";
			if( currGestID == GEST_NEXT ) {
				//display next slide
				wall.displayNext();
				
			}
			else if( currGestID == GEST_PREV ) {
				//display previous slide
				wall.displayPrev();
			}
		}
		else {
			
		}
		
		prevGestID = currGestID;
		prevTrigger = trigger;
		
		
#ifdef ENBLE_DEBUG
		imshow( "Result", frame );
#endif
        cerr << faceCount << "\t" 
			 << faceTrace << "\t" 
			 << (int)gestDetector.getState() << "\t";
		if( currGestID == GEST_INVALID )	cerr << "DO NOTHING";
		else if( currGestID == GEST_NEXT )	cerr << "NEXT";
		else if( currGestID == GEST_PREV )	cerr << "PREV";
		cerr << "\t";
		if( trigger )	cerr << "TRIGGER" << "\t";	
		cerr << t << "ms\t" 
			 << "\n";
			 
        unsigned char keyIn = '*';
        if( waitKey( (t>50)?2:50-t ) >= 0 ) {
            break;
        }
        
        capDevice >> frame; //load next frame
        resize( frame, 
				frame, 
				Size( 0, 0) , 
				(float) PROCESSING_FRAME_HEIGHT/ frame.rows,
				(float) PROCESSING_FRAME_WIDTH / frame.cols,
				INTER_AREA );
	}
	
	return 0;
}

