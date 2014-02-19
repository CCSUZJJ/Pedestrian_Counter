#include "BlobSegmentation.h"
#include "OpenCvUtilities.h"
#include <math.h>
#include <vector>

std::vector<cv::Rect> BlobSegmentation::contourSegment(cv::Mat fg){ //fg = 480x640

    std::vector<std::vector<cv::Point>> contours;
    cv::findContours(fg, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE, cv::Point(0,0));
    std::vector<std::vector<cv::Point>> contours_poly(contours.size());
    std::vector<cv::Rect> boundRect(contours.size());

    for(int i = 0; i < contours.size(); i++){
        cv::approxPolyDP(cv::Mat(contours[i]), contours_poly[i], 3, true);
        boundRect[i] = cv::boundingRect(cv::Mat(contours_poly[i]));
    }

    return boundRect;
}

std::vector<cv::Rect> BlobSegmentation::intensitySegment(cv::Mat fg){
    //Horizontal Segmentation
    int* horizHist = new int[fg.cols];
    for(int i = 0; i < fg.cols; i++){
        horizHist[i] = cv::countNonZero(fg.col(i));
    }
    //Vertical Segmentation
    int* vertHist = new int[fg.rows];
    for(int i = 0; i < fg.cols; i++){
        vertHist[i] = cv::countNonZero(fg.row(i));
    }
//    //Display the Histogram
//    //Width, hight, number of bins of the histogram and width of a bin
//    int hHist_w = fg.cols;
//    int hHist_h = fg.rows;
//    int hHistSize = fg.cols; //1 bin per column
//    int hBin_w = round((double)(hHist_w/fg.cols));
//    cv::Mat hHistImg = cv::Mat::ones(hHist_h, hHist_w, CV_8UC1)*255;
//    //Normalize histogram values so they fall in range 0 - hHistImg.rows
//    for(int i = 0; i < fg.cols; i++){
//        horizHist[i] *= round((double)hHistImg.rows/fg.rows);
//    }

//    //draw lines
//    for(int i = 0; i < histSize; i++){
//        cv::line( hHistImg, cv::Point(hBin_w*(i-1), hHist_h - horizHist[i-1]),
//                            cv::Point(hBin_w*i, hHist_h - horizHist[i]),
//                            cv::Scalar(0), 2, 8, 0);
//    }
//    cv::namedWindow("Horizontal Histogram");
//    cv::imshow("Horizontal Histogram", hHistImg);
//    cv::waitKey(1);
}
