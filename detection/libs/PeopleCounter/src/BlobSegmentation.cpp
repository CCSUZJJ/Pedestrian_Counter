#include "BlobSegmentation.h"
#include "OpenCvUtilities.h"
#include <math.h>

void BlobSegmentation::intensitySegment(cv::Mat fg){ //fg = 480x640
    int histSize = fg.cols;         //number of bins: 640
    float range[] = {0, fg.rows};   //each bin contains 0 to 480 pixels
    const float* histRange = {range};
    bool uniform = true;            //each bin has the same size: 1
    bool accumulate = false;        //clear the histograms in the beginning
    cv::Mat horizHist;
    cv::calcHist(&fg, 1, 0, cv::Mat(), horizHist, 1, &histSize, &histRange, uniform, accumulate);
    int hist_w = fg.cols;
    int hist_h = fg.rows;
    int bin_w = round( (double)(hist_w/histSize) );
    cv::Mat histImg = cv::Mat::ones(hist_h, hist_w, CV_8UC1)*255;
    cv::normalize(horizHist, horizHist, 0, histImg.rows, cv::NORM_MINMAX, -1, cv::Mat());


    for(int i = 0; i < histSize; i++){
        cv::line( histImg, cv::Point(bin_w*(i-1), hist_h - round(horizHist.at<float>(i-1))),
                  cv::Point(bin_w*i, hist_h - round(horizHist.at<float>(i))),
                  cv::Scalar(0), 2, 8, 0);
    }
    cv::namedWindow("Horizontal Histogram");
    cv::imshow("Horizontal Histogram", histImg);
    cv::waitKey(1);
}
