//      faceDetector.hpp
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


  


#ifndef FACEDETECTOR_HPP
#define FACEDETECTOR_HPP

#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/objdetect/objdetect.hpp>

#include <stdio.h>
#include <vector>

#define COLOR_FACEBOX CV_RGB(  20, 250,  20)
#define COLOR_SRCHBOX CV_RGB( 250,  20,  20)

//scale factors of the search window
#define SRCH_WIN_SCALE 0.40

using namespace cv;

class faceDetector
{
	private:
		CascadeClassifier coreClassifier;
		bool cascadeLoaded;
		
		unsigned char absX, absY;
		short roiX, roiY;
		short roiWidth, roiHeight;
		Mat searchWindow;
		Rect searchWindowRegion;
		int thresholdValue;		//To store the minimum number of faces to be detected to say it as a positive.
		int minFeatureX;
		int minFeatureY;
		
		vector<Rect> faces;
	
	public:
		faceDetector();
		faceDetector( Mat& img,
					  String cascadeName,
					  int faceThreshold = 5 );
		virtual ~faceDetector();
		vector<Rect> returnFaceRegions( Mat& img,
										int combineFeatures = 0 );
	
};

#endif /* FACEDETECTOR_HPP */ 
