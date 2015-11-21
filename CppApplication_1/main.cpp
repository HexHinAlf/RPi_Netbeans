/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   main.cpp
 * Author: Felix
 *
 * Created on 11. November 2015, 22:11
 */

 /**
*/
#include <ctime>
#include <iostream>
#include <raspicam/raspicam_cv.h>
#include <opencv2/core/core.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <tesseract/baseapi.h>
#include <leptonica/allheaders.h>

//using namespace cv;
//using namespace std;

int main(int argc, char **argv) {
    raspicam::RaspiCam_Cv Camera;
    tesseract::TessBaseAPI *tess = new tesseract::TessBaseAPI();
    // Initialize tesseract-ocr with German, without specifying tessdata path
    if (tess->Init(NULL, "deu")) {
        fprintf(stderr, "Could not initialize tesseract.\n");
        exit(1);
    }
    char *outText;
    
    //Matrix image for openCV
    cv::Mat cameraImage;
    cv::Mat editorImage;
    cv::Mat resultImage;
    //Windows
    std::string cameraWindowName = "Camera";
    std::string editorWindowName = "Corrected Image";
    std::string resultWindowName = "Tesseract Output";

    //cv::namedWindow(cameraWindowName, cv::WINDOW_NORMAL); // Create a window for display.
    cv::namedWindow(editorWindowName, cv::WINDOW_NORMAL);
    cv::namedWindow(resultWindowName, cv::WINDOW_NORMAL);

    //set camera parameter
    //Camera.set(CV_CAP_PROP_EXPOSURE, 1);
    //Camera.set(CV_CAP_PROP_FRAME_WIDTH, 800);
    //Camera.set(CV_CAP_PROP_FRAME_HEIGHT, 600);
    Camera.set(CV_CAP_PROP_FORMAT, CV_8UC3);
    
    //Open camera
    std::cout << "Opening Camera..." << std::endl;
    if (!Camera.open()) {
        std::cerr << "Error opening the camera" << std::endl;
        return -1;
    }

    
    //Start capture
    cv::waitKey(2000);
    Camera.grab();
    Camera.retrieve(cameraImage);
    
    // create a simple window to display the video
    //cv::imshow(cameraWindowName, cameraImage);
    
    std::cout << "Stop camera..." << std::endl;
    Camera.release();
    
    
    //edit image to greyscale
    cv::cvtColor(cameraImage,editorImage,CV_RGB2GRAY);
    cv::imshow(editorWindowName, editorImage);
    
    
    
    //convert to text
    tess->SetImage((uchar*)editorImage.data, editorImage.size().width, editorImage.size().height, editorImage.channels(), editorImage.step1());
    tess->Recognize(0);
    outText = tess->GetUTF8Text();
    
    //display text
    resultImage = cv::Mat::zeros(250,250,CV_8U);
    cv::putText(resultImage, outText, cv::Point(0,50), CV_FONT_HERSHEY_SIMPLEX, 0.4, cv::Scalar(255),1,8,false);
    cv::imshow(resultWindowName, resultImage);
    
    

    //show time statistics
//    time(&timer_end); /* get current time; same as: timer = time(NULL)  */
//
//    double secondsElapsed = difftime(timer_end, timer_begin);
//    cout << secondsElapsed << " seconds for " << nCount << "  frames : FPS = " << (float) ((float) (nCount) / secondsElapsed) << endl;

    // save image 
    //cv::imwrite("raspicam_cv_image.jpg", image);
    std::cout << "Image saved at raspicam_cv_image.jpg" << std::endl;
    std::cout << outText << std::endl;

    cv::waitKey(0);
    
    cv::destroyAllWindows();
    tess->End();
    delete [] outText;
    
}