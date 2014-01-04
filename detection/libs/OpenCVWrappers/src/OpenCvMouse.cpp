#include "OpenCvMouse.h"

// Constructor

OpenCvMouseEventControl::OpenCvMouseEventControl()
{
    Handler = 0;
    RightAreaSelected = false;

    FirstMouseCallBack = true;			// true = 1st callbcak (must be reset by the user the object is passed to ...)
    LastCursorPosition.x = -1;
    LastCursorPosition.y = -1;
    RightClickCursorPosition1.x = -1;
    RightClickCursorPosition1.y = -1;
    RightClickCursorPosition2.x = -1;
    RightClickCursorPosition2.y = -1;
    RightAreaTopLeftPosition.x = -1;
    RightAreaTopLeftPosition.y = -1;
    RightAreaBottomRightPosition.x = -1;
    RightAreaBottomRightPosition.y = -1;
    ResetRightClicks();
}

// Destructor

OpenCvMouseEventControl::~OpenCvMouseEventControl()
{
    DisableMouse();
}

// Enable mouse -> will activate the callback-function

void OpenCvMouseEventControl::EnableMouse(const char* NameWindow,void (*Handler)(int, int, int, int, void *))
{
    DisableMouse();
    if ((NameWindow != 0) && (Handler != 0))
    {
        this->NameWindow = NameWindow;
        this->Handler = Handler;
        cvSetMouseCallback(this->NameWindow.c_str(), *this->Handler, (void*)(&*this) );
    }
}

void OpenCvMouseEventControl::ResetRightClicks()
{
    RightClickPosition1Ready = false;
    RightClickPosition2Ready = false;
}

// If the mouse has been activated, then the callback will be disabled

void OpenCvMouseEventControl::DisableMouse() 
{
    if ((Handler != 0) && !NameWindow.empty()) cvSetMouseCallback(NameWindow.c_str(),0, 0 );
    NameWindow.clear();
    Handler = 0;
}

// Process event

void OpenCvMouseEventControl::ProcessEvent(int event, int x, int y, int /*flags*/)
{
    StatusLeftMouse.AtStartProcessEvent();
    StatusMiddleMouse.AtStartProcessEvent();
    StatusRightMouse.AtStartProcessEvent();

    RightAreaSelected = false;

    // Process mouse-event (left/middle:right) (up/down/double-clicked)

    switch(event)
    {
        // left button down

    case CV_EVENT_LBUTTONDOWN:
    {
        StatusLeftMouse.SetMouseDown();
        StatusMiddleMouse.Down = false;
        StatusRightMouse.Down = false;

        ResetRightClicks();
    }
        break;

        // right button down
    case CV_EVENT_RBUTTONDOWN:
    {
        StatusLeftMouse.Down = false;
        StatusMiddleMouse.Down = false;
        StatusRightMouse.SetMouseDown();

        if (!RightClickPosition1Ready)
        {
            RightClickPosition1Ready = true;
            RightClickCursorPosition1.x = x;
            RightClickCursorPosition1.y = y;
        } else
        {
            RightClickPosition2Ready = true;
            RightClickCursorPosition2.x = x;
            RightClickCursorPosition2.y = y;
        }
    }
        break;

        // middle button down
    case CV_EVENT_MBUTTONDOWN:
    {
        StatusLeftMouse.Down = false;
        StatusMiddleMouse.SetMouseDown();
        StatusRightMouse.Down = false;

    }
        break;

        // left button up
    case CV_EVENT_LBUTTONUP:
    {
         StatusLeftMouse.Down = false;
    }
        break;

        // right button up
    case CV_EVENT_RBUTTONUP:
    {
         StatusRightMouse.Down = false;

        if (RightClickPosition1Ready && RightClickPosition2Ready)
        {
            RightAreaTopLeftPosition.x = std::min(RightClickCursorPosition1.x,RightClickCursorPosition2.x);
            RightAreaBottomRightPosition.x = std::max(RightClickCursorPosition1.x,RightClickCursorPosition2.x);
            RightAreaTopLeftPosition.y = std::min(RightClickCursorPosition1.y,RightClickCursorPosition2.y);
            RightAreaBottomRightPosition.y = std::max(RightClickCursorPosition1.y,RightClickCursorPosition2.y);

            RightAreaSelected = true;
            ResetRightClicks();
        }
    }
        break;

        // middle button up
    case CV_EVENT_MBUTTONUP:
    {
         StatusMiddleMouse.Down = false;
    }
        break;

        // Left-mouse double-clicked

    case CV_EVENT_LBUTTONDBLCLK :
    {
        StatusLeftMouse.DoubleClicked = true;
    }
        break;

        // Right-mouse double-clicked

    case CV_EVENT_RBUTTONDBLCLK :
    {
        StatusRightMouse.DoubleClicked = true;
    }
        break;

        // Middle-mouse double-clicked

    case CV_EVENT_MBUTTONDBLCLK :
    {
        StatusMiddleMouse.DoubleClicked = true;
    }
        break;

    default:
        break;
    }

    FirstMouseCallBack = false;
    LastCursorPosition.x = x;
    LastCursorPosition.y = y;
}

// Returns true if the cursor-position has been changed

bool OpenCvMouseEventControl::SetCursorPosition(int x,int y)
{
    bool PositionHasChanged = ((x != LastCursorPosition.x) || (y != LastCursorPosition.y));
    LastCursorPosition.x = x;
    LastCursorPosition.y = y;
    return PositionHasChanged;
}

void OpenCvMouseControl::ReleaseMouse(const char* NameWindow)
{
    for (std::list<OpenCvMouseEventControl>::iterator it = Mouses.begin();it != Mouses.end();it++)
    {
        if (it->NameWindow.compare(NameWindow) == 0)
        {
            // Mouse found => Disable and remove from list

            it->DisableMouse();
            Mouses.erase(it);
            break;
        }
    }
}

void OpenCvMouseControl::ReleaseAllMouses() 
{
    Mouses.clear();
}

// Remark : Images will be controlled by the mouse-control (deleted by destructor)

OpenCvMouseEventControl* OpenCvMouseControl::AddMouse(const char* NameWindow,void (*Handler)(int, int, int, int, void *))
{
    ReleaseMouse(NameWindow);
    Mouses.push_back(OpenCvMouseEventControl());
    OpenCvMouseEventControl* EventOI = &Mouses.back();
    EventOI->EnableMouse(NameWindow,Handler);
    return EventOI;
}

OpenCvMouseEventControl* OpenCvMouseControl::GetMouse(const char* NameWindow)
{
    for (std::list<OpenCvMouseEventControl>::iterator it = Mouses.begin();it != Mouses.end();it++)
    {
        if (it->NameWindow.compare(NameWindow) == 0) return &(*it);
    }

    return 0;
}
