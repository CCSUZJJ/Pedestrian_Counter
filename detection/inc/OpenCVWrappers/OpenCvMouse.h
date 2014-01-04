#ifndef _CV_MOUSE_UTILITIES_H_INCLUDE_
#define _CV_MOUSE_UTILITIES_H_INCLUDE_

	// OpenCV mouse-call-back-utilities

#include <opencv/cv.h> // includes OpenCV definitions
#include <opencv/highgui.h> // includes highGUI definitions
#include <vector>
#include <string>
#include <list>

	// Object passed through the MouseHandler as a void pointer *param
	// void MouseHandler(int event, int x, int y, int flags, void *param)


// TODO : Add middle-mouse-button and double-clicked-events

class OpenCvMouseEventControl
{
public:

class MouseStatus
{
public:

    MouseStatus()
        : Down(false),Clicked(false),DoubleClicked(false) {}

    bool Down;                  // true = mouse = down
    bool Clicked;               // true = mouse has been clicked (status-changed up-> down)
    bool DoubleClicked;         // true = double-clicked

    // Basic status-calls

    void AtStartProcessEvent()
    {
        Clicked = false;
        DoubleClicked = false;
    }

    void SetMouseDown()
    {
        Clicked = !Down;
        Down = true;
    }
};


public:

	void ProcessEvent(int event, int x, int y, int flags);

    // Left-mouse

    bool IsLeftMouseDown() const { return StatusLeftMouse.Down; }
    bool IsLeftMouseClicked() const { return StatusLeftMouse.Clicked; }
    bool IsLeftMouseDoubleClicked() const { return StatusLeftMouse.DoubleClicked; }

    // Middle-mouse

    bool IsMiddleMouseDown() const { return StatusMiddleMouse.Down; }
    bool IsMiddleMouseClicked() const { return StatusMiddleMouse.Clicked; }
    bool IsMiddleMouseDoubleClicked() const { return StatusMiddleMouse.DoubleClicked; }

    // Right-mouse

    bool IsRightMouseDown() const { return StatusRightMouse.Down; }
    bool IsRightMouseClicked() const { return StatusRightMouse.Clicked; }
    bool IsRightMouseDoubleClicked() const { return StatusRightMouse.DoubleClicked; }

	std::string NameWindow;				// Name of the window involved
	bool FirstMouseCallBack;			// true = 1st callbcak (must be reset by the user the object is passed to ...)

    CvPoint LastCursorPosition;
	bool RightClickPosition1Ready;
	bool RightClickPosition2Ready;
	bool RightAreaSelected;
    CvPoint RightAreaTopLeftPosition;
	CvPoint RightAreaBottomRightPosition;
	CvPoint RightClickCursorPosition1;
	CvPoint RightClickCursorPosition2;
	
	// Pointers to data-structures OI
	
	std::vector<void *> DataReferences;
	
	// Static function. Param will point to this object
	
	void EnableMouse(const char* NameWindow,void (*Handler)(int, int, int, int, void *));
	void DisableMouse();
	void ResetRightClicks();

	// Updates the cursor-position. Exits true if the cursor-position
	// has changed (else exit false)

	bool SetCursorPosition(int x,int y);

	// Constructor
	
	OpenCvMouseEventControl();

	// Destructor
	
	virtual ~OpenCvMouseEventControl();
	
	// Private data
	
private:
	
	// A null-pointer indicates a disabled mouse-handler !
	
	void (*Handler)(int event, int x, int y, int flags, void *param);

    // Event-handling

    MouseStatus StatusLeftMouse;
    MouseStatus StatusMiddleMouse;
    MouseStatus StatusRightMouse;
};

	// Control class for managing mouse-event-controls

class OpenCvMouseControl
{
public :

    OpenCvMouseEventControl* GetMouse(const char* NameWindow);
	OpenCvMouseEventControl* AddMouse(const char* NameWindow,void (*Handler)(int, int, int, int, void *));
	void ReleaseMouse(const char* NameWindow);
	void ReleaseAllMouses();
	
private :
	
	std::list<OpenCvMouseEventControl> Mouses;
};

#endif //_CV_MOUSE_UTILITIES_H_INCLUDE_
