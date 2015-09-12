#include <opencv2/opencv.hpp>

#include <string>
#include <vector>
#include <dirent.h>

using namespace std;
using namespace cv;

class gallery {
	
		string title;
		string directory;
		unsigned int width, height;
		vector<string> frameList;
		int currFrameId;
		
		Mat canvas;
		
		//util functions
		int loadDirectory( void );	//this function lists all the entries in directory
	
	public:
		gallery( void );
		gallery( string, string, unsigned int = 1024, unsigned int = 600 );
		
		//Functions to navigate
		int displayNext( void );
		int displayPrev( void );
		int displayFrame( int );
		int getCurrFrameId( void );
		
		//gallery settings
		char setDirectory( string );
		string getDirectory( void );
		int getGallerySize( void );
				
		//file handling functions
		vector<string> listGallery( void );
		
};
