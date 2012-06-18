#ifndef HAARDETECT_H_INCLUDED
#define HAARDETECT_H_INCLUDED

#include <opencv2/objdetect/objdetect.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include "globals.h"

using namespace cv;

extern unsigned __stdcall haardetect(void* param);

#endif // HAARDETECT_H_INCLUDED
