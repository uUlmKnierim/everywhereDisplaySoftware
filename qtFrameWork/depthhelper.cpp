#include "depthhelper.h"


using namespace cv;

DepthHelper::DepthHelper(){
	min = -1;
	max = -1;
	generateGradient();
	std::cout<< "created helper" << std::endl;
}

void DepthHelper::generateGradient(){
		
		//red -> yellow
		for(int i=0; i<128; i++){
			colorTable[i] =0xFF0000 | (i*2) <<8;
		}
		
		//yellow -> green
		for(int i=0; i<128; i++){
			colorTable[i + 128] = 0x00FF00 | (255-(i*2)) << 16;
		}

		// green -> Blue
		for(int i=0; i<128; i++){
			colorTable[i + 2*128] = (255-(i*2)) << 8 | (i*2) ; 
		}

		// Set 0 to white for no data output
		colorTable[0] = 0xFFFFFF;

		std::cout<< "created colorTable" << std::endl;
}

void DepthHelper::displayDepthImage(cv::Mat Image, const char* name){
    generateDepthImage(&Image);
    cv::imshow(name,Image);
}


// Display a coloorized depth imaga with color gradient.
void DepthHelper::generateDepthImage(cv::Mat* Image){

    int channel = Image->channels();
    int nRows = Image->rows;
    int nCols = Image->cols * channel;

    cv::Mat r(nRows, nCols, CV_8U);
    cv::Mat g(nRows, nCols, CV_8U);
    cv::Mat b(nRows, nCols, CV_8U);

    ushort* p;
    int i,j;

    if(min <0 || max < 0){
        cv::minMaxLoc(*Image,&min,&max,NULL,NULL);
    	diff = max-min;
    }

    for( i = 0; i < nRows; ++i)
    {
        p = (*Image).ptr<ushort>(i);
        for ( j = 0; j < nCols; ++j){

            int c = colorTable[(int)(((*p)-min)*COLORS/diff)]; // scale
            unsigned short  r8 = (c & RED_MASK)   >> 16;
            unsigned short  g8 = (c & GREEN_MASK) >> 8;
            unsigned short  b8 = (c & BLUE_MASK);
 
            r.at<char>(i, j)  = r8;
            g.at<char>(i, j)  = g8;
            b.at<char>(i, j)  = b8;
            ++p;
        }
    }

    cv::vector<Mat> channels;
    channels.push_back(r);
    channels.push_back(g);
    channels.push_back(b);
    cv::merge(channels,*Image);
}

cv::Mat* erudeDepth(cv::Mat* image){
    Mat element = getStructuringElement(  MORPH_RECT,
                                       Size( 7, 7 ),
                                       Point( 3, 3 ) );


    cv::Mat imgErude;
    dilate((*image), imgErude, element, cv::Point(-1,-1));
    // Damn old C style

    int channels = image->channels();
    int nRows = image->rows;
    int nCols = image->cols * channels;

    if (image->isContinuous())
    {
        nCols *= nRows;
        nRows = 1;
    }

    int i,j;
    ushort* p;
    for( i = 0; i < nRows; ++i)
    {
        p = image->ptr<ushort>(i);
        for ( j = 0; j < nCols; ++j)
        {
          ushort my = *p;
          if( my==0){
                (*p) = imgErude.at<ushort>(j);
          }
        ++p;
        }
    }
    return image;
}
