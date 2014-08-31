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

void /*std::vector<cv::Rect>*/ BlobSegmentation::intensitySegment(cv::Mat fg){
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

std::vector<DetectedBlob> BlobSegmentation::connectedComponentSegment(cv::Mat fg, int frameNumber){
    std::vector<DetectedBlob> detectedBlobs;

    //Run-length encode
    std::vector<Run> runs;
    for(int i = 0; i < fg.rows; i++){
        for(int j = 0; j < fg.cols; j++){
            if(fg.at<uchar>(i,j) == 255){
                Run run;
                run.length = 0;
                run.start = cv::Point(j,i); //(x, y) -> (column, row)
                run.label = -1;             //if != -1 later, then OK
                while(fg.at<uchar>(i,j) == 255){
                    run.length++;
                    j++;
                }
                runs.push_back(run);
            }
        }
    }

    if(!runs.empty()){
        //Label the runs
        int nextNewLabel = 1;
        std::vector<Run>::iterator run;
        for(run = runs.begin(); run != runs.end(); run++){
            if(run->label == -1){ //If run has no label (run on first row with runs or unconnected with a run above
                run->label = nextNewLabel;  //Assign new label
                nextNewLabel++;
            }
            std::vector<Run>::iterator belowRun;
            for(belowRun = runs.begin(); belowRun != runs.end(); belowRun++){
                if(belowRun->start.y == run->start.y+1){                        //If run below
                    if(belowRun->start.x <= (run->start.x + run->length) &&     //If connected
                       (belowRun->start.x + belowRun->length) >= run->start.x){
                        if(belowRun->label == -1){          //If no label yet
                            belowRun->label = run->label; //Take label from current run
                        }
                        else{   //Assign the run below (and all others with the same label)
                                //the label of the current run
                            std::vector<Run>::iterator toChangeRun;
                            for(toChangeRun = runs.begin(); toChangeRun != runs.end(); toChangeRun++){
                                if(toChangeRun->label == belowRun->label){
                                    toChangeRun->label = run->label;
                                }
                            }
                        }
                    }
                }
            }
        }

        //Make blobs, determine bounding box, leanness, compactness
        for(int i = 1; i < nextNewLabel; i++){
            std::vector<Run> blob;
            std::vector<Run>::iterator currRun;
            for(currRun = runs.begin(); currRun != runs.end(); currRun++){
                if(currRun->label == i){
                    blob.push_back(*currRun);
                }
            }
            if(!blob.empty()){
                Run firstRun = blob[0];
                Run lastRun = blob[blob.size()-1];
                int perimeter = 0;
                int area = 0;
                int minX = firstRun.start.x;
                int maxX = firstRun.start.x + firstRun.length;
                int minY = firstRun.start.y;
                int maxY = firstRun.start.y;
                std::vector<Run>::iterator blobRun;
                for(blobRun = blob.begin(); blobRun != blob.end(); blobRun++){
                    perimeter += 2; //beginning and end
                    area += blobRun->length;

                    if(blobRun->start.x < minX)
                        minX = blobRun->start.x;
                    if((blobRun->start.x + blobRun->length) > maxX)
                        maxX = blobRun->start.x + blobRun->length;
                    if(blobRun->start.y < minY)
                        minY = blobRun->start.y;
                    else if(blobRun->start.y > maxY)
                        maxY = blobRun->start.y;
                }

                perimeter += (firstRun.length-2); //correction for first and last run
                perimeter += (lastRun.length-2);

                DetectedBlob b = DetectedBlob();
                b.frameNr = frameNumber;
                b.BBox = cv::Rect(minX, minY, maxX-minX, maxY-minY);
                b.label = i;
                b.leanness = b.BBox.height*1.0/b.BBox.width;
                b.compactness = (perimeter*perimeter*1.0)/area;


                if(area > 220){
                    detectedBlobs.push_back(b);
                    //std::cout << "Perimeter: "<<perimeter<<"    Area: "<<area<<std::endl;
                    //std::cout << "Leanness: "<<b.leanness<<"    Compactness: "<<b.compactness<<std::endl;
                }
            }
        }
    } //END if(!runs.empty())

    return detectedBlobs;
}
