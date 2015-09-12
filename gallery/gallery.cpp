#include "gallery.hpp"

gallery::gallery( void ) {
	
	title = "gallery::default";
	//Set current directory as gallery directory
	directory = ".";		//current directory
	width = 1024;
	height = 600;
	
	loadDirectory();	
}

gallery::gallery( string galName, string galleryDir, unsigned int w , unsigned int h ) {
	
	//Set gallery title
	title = galName;
	//Set directory to load to gallery
	directory = galleryDir;
	
	width = w; height = h;	
	
	if( loadDirectory() > 0 ) {
		currFrameId = 0;
	}
	else {
		currFrameId = -1;
	}
	
}

int gallery::displayNext( void ) {
	
	if( currFrameId < 0 ) {
		return -1;
	}
	
	currFrameId++;	
	if( currFrameId >= frameList.size() ) {
		//roll over
		currFrameId = 0;		// goto to first
	}
	
	displayFrame( currFrameId );
	
	return currFrameId;
}

int gallery::displayPrev( void ) {
	
	if( currFrameId < 0 ) {
		return -1;
	}
		
	currFrameId--;
	if( currFrameId < 0 ) {
		//roll over
		currFrameId = frameList.size() - 1;	//goto last
	}
	
	displayFrame( currFrameId );
}

int gallery::displayFrame( int frameId ) {	//negative ID displays curr frame
	
	if( currFrameId < 0 ) {
		return -1;
	}
	
	//clear current canvas
	canvas = Mat( Size( width, height ), CV_8UC3, Scalar( 0, 0, 0 ) );
	
	if( frameId >= 0 ) {
		//make this the new frame id
		currFrameId = frameId;
	}
	Mat tmpFrame = imread( frameList[currFrameId].c_str() );
	
	int startX = 0, startY = 0;
	
	int newWidth = 0;
	int newHeight = 0;
	float resizeRatio = 0.0f;
	
	if( tmpFrame.cols > width ) {
		newWidth = width;
	}
	else {
		newWidth = tmpFrame.cols;
	}
	if( tmpFrame.rows > height ) {
		newHeight = height;
	}
	else {
		newHeight = tmpFrame.rows;
	}
		
	if( (float)newHeight/tmpFrame.rows < (float)newWidth/tmpFrame.cols ) {
		resizeRatio = (float) newHeight/tmpFrame.rows;
	}
	else {
		resizeRatio = ( float ) newWidth/tmpFrame.cols;
	}
	
	resize( tmpFrame,
			tmpFrame,
			Size( 0, 0 ),
			resizeRatio ,
			resizeRatio,
			INTER_AREA );	
	
	startX =  ( width - tmpFrame.cols ) / 2.0;
	startY =  ( height - tmpFrame.rows ) / 2.0;
	
	Mat roiCanvas( canvas, Rect( startX, startY, tmpFrame.cols, tmpFrame.rows ) );
	
	roiCanvas = Scalar( 255, 255, 255 );
	tmpFrame.copyTo( roiCanvas );
	
	imshow( title, canvas );
	
	return currFrameId;
}

int gallery::getCurrFrameId( void ) {
	
	return currFrameId;
}


string gallery::getDirectory( void ) {
	
	return directory;
}

char gallery::setDirectory( string galleryDir ) {
	
	directory = galleryDir;
	directory.clear();		//clear all elements before loading new
	loadDirectory();
}

int gallery::getGallerySize( void ) {
	
	return frameList.size();
} 

int gallery::loadDirectory( void ) {
	if( directory.empty() ) {
		return -1;
	}
	
	//supported formats
	char *formats[] = {
						".jpg",
						".jpeg",
						".png",
						".bmp",						
						".tif",
						".tiff",
						".dib",
						".jpe",
						".jp2",
						".pbm",
						".pgm",
						".ppm",
						".sr",
						".ras"
					};
	
	DIR *srchDir;
	struct dirent *dirEntry;
	
	srchDir = opendir( directory.c_str() );
	if( srchDir ) {
		while( ( dirEntry = readdir( srchDir ) ) != NULL ) {
			
			string frameName = string( dirEntry -> d_name );
			
			if( int( frameName.rfind( ".jpg", frameName.size() - 4 ) ) > 0 ) {
				//push this element to frameList
				frameList.push_back( directory + string( "/" ) + string( dirEntry -> d_name ) );
			}
		}		
		closedir( srchDir );
		return frameList.size();
	}
	else {
		return -2;
	}
}


vector<string> gallery::listGallery( void ) {
	
	//for( vector<string>::const_iterator entry = frameList.begin(); entry != frameList.end(); entry++ ) {
	//	cout << entry -> c_str() << "\n";
	//}
	
	return frameList;
}
