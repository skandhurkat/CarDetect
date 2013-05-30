# CarDetect #

Copyright 2011, 2012 Skand Hurkat

CarDetect is a free software which was written as part of a summer project
at the Indian Institute of Technology, Bombay.

The ultimate aim of CarDetect is to develop a system which can identify
cars in real time video streams, and also their make and model. The system
is also supposed to read numberplates and verify them with a database.

## Licence ##

CarDetect is free software: you can redistribute it and/or modify it under
the terms of the GNU General Public Licence as published by the Free
Software Foundation, either version 3 of the licence, or (at your option)
any later version.

CarDetect is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public Licence for
more details.

You should have received a copy of the GNU General Public Licence along
with CarDectect. If not, see <http://www.gnu.org/licenses/>.

## About ##

This project uses the OpenCV library for image and video processing.

### What has been implemented: ###

* Real time identification of cars is possible using the Viola and Jones
  method (haar cascades) implemented in OpenCV.
* Multi-threaded identification for faster performance and frame-skips.
* Numberplates can be read using GOCR, a free and open source OCR engine.
  This is in separate files; these files are not included in the project.
* PHOG method is used to detect car models. However, the code has some
  bugs, and I have not ironed them out.

### Still to do: ###

* Detecting car models using PHOG (pyramid histogram of oriented gradients)
  has some bugs. These were corrected later by someone else, but I don't
  have the modified code.
* Real time implementation of OCR. GOCR is extremely slow and painful.
* Integration of all parts of code into a complete unit.

## Note to those seeking to collaborate on the project ##

I have no more interest in this project. This project is hereby made
publicly available so that anyone in the academia or industry working on a
similar project can use this code (under the terms of the GNU GPL v3 or, at
his/her discretion, a higher version of the GNU GPL), thereby saving
valuable time to start again from scratch.

However, I would appreciate it if you could provide pull requests for any
modifications that you may make to the code. I am sufficiently active to
manage merges into this repository, and your contributions would make this
project way more awesome.
