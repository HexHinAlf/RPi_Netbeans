/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   main.hpp
 * Author: Felix
 *
 * Created on 22. November 2015, 16:40
 */

#ifndef MAIN_HPP
#define MAIN_HPP

#include <boost/thread/thread.hpp>

#include <ctime>
#include <iostream>
#include <stdlib.h>
#include <stdio.h>

#include <raspicam/raspicam_cv.h>

#include <opencv2/core/core.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <tesseract/baseapi.h>
#include <leptonica/allheaders.h>


void getTextblocks(cv::Mat src, std::vector<cv::Mat> *textblocks);
std::vector<cv::Rect> detectLetters(cv::Mat img);
void getText(cv::Mat img);

#endif /* MAIN_HPP */

