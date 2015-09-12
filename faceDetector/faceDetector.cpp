//      faceDetector.cpp
//      
//      Copyright 2012 vichu <get_vichu@yahoo.com>
//      
//      This program is free software; you can redistribute it and/or modify
//      it under the terms of the GNU General Public License as published by
//      the Free Software Foundation; either version 2 of the License, or
//      (at your option) any later version.
//      
//      This program is distributed in the hope that it will be useful,
//      but WITHOUT ANY WARRANTY; without even the implied warranty of
//      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//      GNU General Public License for more details.
//      
//      You should have received a copy of the GNU General Public License
//      along with this program; if not, write to the Free Software
//      Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
//      MA 02110-1301, USA.




#include "faceDetector.hpp"


faceDetector::faceDetector()
{
	cascadeLoaded = false;
	
}

faceDetector::faceDetector( Mat& img,
							String cascadeName,
							int faceThreshold )
{
	cascadeLoaded = false;
	
	absX = 0; absY = 0;
	roiX = 0; roiY = 0;
	roiWidth  = img.cols; 
	roiHeight = img.rows;
	thresholdValue = faceThreshold;
	
	minFeatureX = 10;
	minFeatureY = 10;
	
	searchWindowRegion = Rect( roiX, roiY, roiWidth, roiHeight );	
	
	if( coreClassifier.load( cascadeName ))
		cascadeLoaded = true;
	
}


faceDetector::~faceDetector()
{
	
}

vector<Rect> faceDetector::returnFaceRegions( Mat& img,
											  int combineFeatures )
{
	faces.clear();
    searchWindow = img( searchWindowRegion );    
    // printf( "Window size : %3dx%3d ", searchWindow.cols, searchWindow.rows );
        
    if( !cascadeLoaded )
    {
		printf( "ERROR :: Cascade not loaded\n" );
		return faces;
	}

    Mat imgGray;
    vector<Rect> localFaces;
    
    //Convert to gray scale
    if( searchWindow.channels() == 3 ) {
		cvtColor( searchWindow, imgGray, CV_BGR2GRAY );
	}
	else if( searchWindow.channels() == 4 ) {
		cvtColor( searchWindow, imgGray, CV_BGRA2GRAY );
	}
	else {	//already gray
		imgGray = searchWindow.clone();
	}
    
    //Histogram equalise
    equalizeHist( imgGray, imgGray );
    
    coreClassifier.detectMultiScale( imgGray,
									 localFaces,
									 1.1,
									 combineFeatures ,
									 CV_HAAR_SCALE_IMAGE ,
									 Size( minFeatureX, minFeatureY ) );
									 
	for( vector<Rect>::const_iterator r = localFaces.begin(); r != localFaces.end(); r++ )
    {
        //find the absolute positions in the frame
        absX = roiX + r->x;     //this is the actual position of the face in frame
        absY = roiY + r->y;     //***"
        //saving these faces into a vector for future identification
        faces.push_back( Rect( absX, absY, r->width, r->height ));
    }
    
    //now identify the one and only one face
    groupRectangles( localFaces, thresholdValue );
    
    if( !localFaces.empty())
    {
		vector<Rect>::const_iterator r = localFaces.begin();
		
		//Save the current face width and height for the size of min feature for net frame
		minFeatureX = 0.9 * r->width;
		minFeatureY = 0.9 * r->width;
		
		//for defineng new ROI        
        roiX = absX - SRCH_WIN_SCALE*r->width;
        roiY = absY - SRCH_WIN_SCALE*r->height;
        roiWidth  = (1.0+2*SRCH_WIN_SCALE)*r->width;
        roiHeight = (1.0+2*SRCH_WIN_SCALE)*r->height;
        //check if the set ROI positions are valid( there is a chance of -ve positions due to faces at edges
        roiX = (roiX >= 0)?roiX:0;  //set to 0 if -ve
        roiY = (roiY >= 0)?roiY:0;  //***"
        roiWidth  = ((roiX+roiWidth )>img.cols)?(img.cols - roiX):roiWidth;
        roiHeight = ((roiY+roiHeight)>img.rows)?(img.rows - roiY):roiHeight;
        //debugging purpose
        //printf( "roi starts @ (%3d,%3d) with size (%3dx%3d) ", roiX, roiY, roiWidth, roiHeight );
        searchWindowRegion =  Rect( roiX, roiY, roiWidth, roiHeight );
        //rectangle( img, searchWindowRegion, COLOR_SRCHBOX, 1, 4, 0 );
	}
	else // no faces, reset the detector
	{
		absX = 0; absY = 0;
		roiX = 0; roiY = 0;
		roiWidth  = img.cols; 
		roiHeight = img.rows;
		searchWindowRegion = Rect( roiX, roiY, roiWidth, roiHeight );
		
		//reset minFeatures to 10x10
		minFeatureX = 10;
		minFeatureY = 10;
		
	}

    return faces;
}
