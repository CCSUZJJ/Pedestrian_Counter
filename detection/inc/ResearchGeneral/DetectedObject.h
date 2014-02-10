#ifndef DetectedObject_H
#define DetectedObject_H

#include <vector>

struct FrameRectPair
{
    FrameRectPair(): s_Nr(), s_BBox()
    {}

    int s_Nr;
    cv::Rect s_BBox;
};

class DetectedObject
{
   public:
    DetectedObject(){}
    ~DetectedObject() {}

    void output(){
        std::cout << "CatID = " << CatID << std::endl;
        for(FrameRectPair i : Frames){
            std::cout << "BBox(x, y): (" << i.s_BBox.x << ", " << i.s_BBox.y << ") on frame " << i.s_Nr << std::endl;
        }
    }

    vector<FrameRectPair> Frames;
    int CatID;
    int ID;               //used to check if the detected object has already been detected
                          //in previous frames
    //int MustMaybe = 0;  //1 = Must, 0 = Maybe


};

#endif
