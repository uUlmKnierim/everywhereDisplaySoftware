#ifndef DEPTHHELPER_H
#define DEPTHHELPER_H

#include <stdio.h>
#include <vector>
#include <math.h>       /* pow */
#include <opencv2/opencv.hpp>

#define RED_MASK 0xFF0000
#define GREEN_MASK 0x00FF00
#define BLUE_MASK 0x0000FF

#define COLORS 384
#define INTERVAL 30


class DepthHelper {
    int colorTable[COLORS];
    double min, max, diff;
public:
	DepthHelper();
	void reset(){min = -1; max = -1;};
    void displayDepthImage(cv::Mat Image, const char* name);
    void generateDepthImage(cv::Mat* Image);
private:
	void generateGradient();
};

cv::Mat* erudeDepth(cv::Mat* Image);

#endif
