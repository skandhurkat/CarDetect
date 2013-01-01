// globals.h
// Copyright 2011, 2012 Skand Hurkat

//  This file is part of CarDetect.
//
//  CarDetect is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//
//  CarDetect is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with CarDetect.  If not, see <http://www.gnu.org/licenses/>.

#ifndef GLOBALS_H_INCLUDED
#define GLOBALS_H_INCLUDED

#include <opencv2/objdetect/objdetect.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <string>
#include <vector>

using namespace cv;
using namespace std;

extern double scale;

extern int cascade_flags;

extern CascadeClassifier cascade;

extern Mat img;

extern vector<Rect> haarrectangles;

extern bool terminate_program;

//mutexes for multithreading:
extern mutex img_mutex;
extern mutex haarrectangles_mutex;

extern thread haarthread;
extern unsigned int haarthreadid;

#endif // GLOBALS_H_INCLUDED
