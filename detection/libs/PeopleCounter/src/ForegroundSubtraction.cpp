#include "ForegroundSubtraction.h"
#include "OpenCvUtilities.h"

void ForegroundSegmentation::runningAvgBackground(cv::Mat frame, cv::Mat bg, cv::Mat fg){
    cv::Mat dst;
    bg.convertTo(dst, CV_32FC1);    //dst = (32FC1)background
    cv::accumulateWeighted(frame, dst, 0.025);  //dst = (1-a)*dst + a*src
    dst.convertTo(bg, CV_8UC1);     //back to uchar
    cv::absdiff(frame, bg, fg);
    cv::threshold(fg, fg, 40, 255, 0);
}

void ForegroundSegmentation::sigmaDeltaBackground(cv::Mat frame, cv::Mat bg, cv::Mat fg){
    for(int i = 0; i < frame.rows; i++){
        for(int j = 0; j < frame.cols; j++){
            if(frame.at<uchar>(i,j) > bg.at<uchar>(i,j))
                bg.at<uchar>(i,j)++;
            else if(frame.at<uchar>(i,j) < bg.at<uchar>(i,j))
                bg.at<uchar>(i,j)--;
        }
    }
    cv::absdiff(frame, bg, fg);
    cv::threshold(fg, fg, 20, 255, 0);
}

void ForegroundSegmentation::sigmaDeltaCMBackground(cv::Mat frame, cv::Mat bg, cv::Mat fg, cv::Mat frameCount,
                                                    cv::Mat confidence, cv::Mat variance, cv::Mat detectionCount,
                                                    bool toUpdate){
    frameCount += 1;
    //For each pixel
    for(int i = 0; i < frameCount.rows; i++){
        for(int j = 0; j < frameCount.cols; j++){
            if(frameCount.at<uchar>(i,j) < confidence.at<uchar>(i,j)){  //Current confidence period not expired
                if(frameCount.at<uchar>(i,j) % 10){         //Refresh period expires
                    if(variance.at<uchar>(i,j) <= 38){      //Low variance: Vt <= Vth: reliable info
                        if((detectionCount.at<uchar>(i,j) / frameCount.at<uchar>(i,j)) <= 0.8){ //No heavy traffic
                            toUpdate = true;                //Refresh period updating mode
                        }
                    }
                }
            }
            else{
                if(variance.at<uchar>(i,j) <= 38){          //Low variance
                    confidence.at<uchar>(i,j) +=            //Confidence updating as function of detection ratio
                            round(11 * exp(-4 * (detectionCount.at<uchar>(i,j) / frameCount.at<uchar>(i,j))) -1);
                    if(confidence.at<uchar>(i,j) == 10){    //Confidence = Cmin
                        toUpdate = true;                    //Force updating
                    }
                }
                else{                                       //High variance: no reliable info
                    toUpdate = true;                        //Confidence period updating mode
                                                            //to avoid background model deadlock
                }
                detectionCount = cv::Scalar(0);             //Reset Detection Counter
                frameCount = cv::Scalar(0);                 //Reset Frame Counter
            }
        }
    }

    if(toUpdate == true){                                   //Updating recommended
                                                            //Update Background Model
        for(int i = 0; i < bg.rows; i++){
            for(int j = 0; j < bg.cols; j++){
                if(frame.at<uchar>(i,j) > bg.at<uchar>(i,j))
                    bg.at<uchar>(i,j)++;
                else if(frame.at<uchar>(i,j) < bg.at<uchar>(i,j))
                    bg.at<uchar>(i,j)--;
            }
        }
        cv::Mat difference;
        cv::absdiff(frame, bg, difference); //Compute difference
        //Update Variance
        for(int i = 0; i < variance.rows; i++){
            for(int j = 0; j < variance.cols; j++){
                if(variance.at<uchar>(i,j) > 10 + 4*difference.at<uchar>(i,j))
                    variance.at<uchar>(i,j)--;
                if(variance.at<uchar>(i,j) < 10 + 4*difference.at<uchar>(i,j))
                    variance.at<uchar>(i,j)++;
            }
        }
        //Compute Foreground
        for(int i = 0; i < difference.rows; i++){
            for(int j = 0; j < difference.cols; j++){
                if(difference.at<uchar>(i,j) > variance.at<uchar>(i,j))
                    fg.at<uchar>(i,j) = 255;
                else
                    fg.at<uchar>(i,j) = 0;
            }
        }
    }
    else{                                                   //Do not update, just detect
        cv::Mat difference;
        cv::absdiff(frame, bg, difference); //Compute difference
        //Compute Foreground
        for(int i = 0; i < difference.rows; i++){
            for(int j = 0; j < difference.cols; j++){
                if(difference.at<uchar>(i,j) > variance.at<uchar>(i,j))
                    fg.at<uchar>(i,j) = 255;
                else
                    fg.at<uchar>(i,j) = 0;
            }
        }
    }
    //Update Detection Counter
    for(int i = 0; i < detectionCount.rows; i++){
        for(int j = 0; j < detectionCount.cols; j++){
            if(fg.at<uchar>(i,j) == 255)
                detectionCount.at<uchar>(i,j)++;
        }
    }
}
