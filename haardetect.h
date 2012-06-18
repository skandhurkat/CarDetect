//haardetect.h
//© Skand Hurkat, 2011

/*     This file is part of CarDetect.
 *
 *     CarDetect is free software: you can redistribute it and/or modify
 *     it under the terms of the GNU General Public License as published by
 *     the Free Software Foundation, either version 3 of the License, or
 *     (at your option) any later version.
 *
 *     CarDetect is distributed in the hope that it will be useful,
 *     but WITHOUT ANY WARRANTY; without even the implied warranty of
 *     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *     GNU General Public License for more details.
 *
 *     You should have received a copy of the GNU General Public License
 *     along with CarDetect.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef HAARDETECT_H_INCLUDED
#define HAARDETECT_H_INCLUDED

#include <opencv2/objdetect/objdetect.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include "globals.h"

using namespace cv;

extern unsigned __stdcall haardetect(void* param);

#endif // HAARDETECT_H_INCLUDED
