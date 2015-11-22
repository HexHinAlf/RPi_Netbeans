/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include "main.hpp"


/* 
 * File:   main.cpp
 * Author: Felix
 *
 * Created on 11. November 2015, 22:11
 */

 /**
*/

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
    // Create a window for display.
    cv::namedWindow(cameraWindowName, cv::WINDOW_NORMAL);
    cv::namedWindow(editorWindowName, cv::WINDOW_AUTOSIZE);
    //cv::namedWindow(resultWindowName, cv::WINDOW_NORMAL);

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
    cv::imshow(cameraWindowName, cameraImage);
    
    std::cout << "Stop camera..." << std::endl;
    Camera.release();
    
    
    //edit image to greyscale
    cv::cvtColor(cameraImage,editorImage,CV_RGB2GRAY);
    //cv::imshow(editorWindowName, editorImage);
    
    
    
    //convert to text
//    tess->SetImage((uchar*)editorImage.data, editorImage.size().width, editorImage.size().height, editorImage.channels(), editorImage.step1());
//    tess->Recognize(0);
//    outText = tess->GetUTF8Text();
    
    //display text
//    resultImage = cv::Mat::zeros(250,250,CV_8U);
//    cv::putText(resultImage, outText, cv::Point(0,50), CV_FONT_HERSHEY_SIMPLEX, 0.4, cv::Scalar(255),1,8,false);
//    cv::imshow(resultWindowName, resultImage);
    
    
    // save image 
    //cv::imwrite("raspicam_cv_image.jpg", image);
    //std::cout << "Image saved at raspicam_cv_image.jpg" << std::endl;
    std::cout << outText << std::endl;
    
    
    
    

    

    /// Create a Trackbar for user to enter threshold
    //cv::createTrackbar( "Min Threshold:", window_name, &lowThreshold, max_lowThreshold, CannyThreshold );

    /// Show the image
    //getTextblocks(cameraImage, NULL);
    
    cv::Mat textMask = cv::Mat::zeros( cameraImage.size(), editorImage.type());
    cv::Point correctionPoint = cv::Point(20,20);
    for (auto box : detectLetters(cameraImage))
    {
        cv::rectangle(textMask, box.tl()-correctionPoint, box.br()+correctionPoint, cv::Scalar(255,255,255), CV_FILLED, 8, 0 );      
    }
    
    editorImage.copyTo( resultImage, textMask);
    cv::imshow(editorWindowName, resultImage);

    cv::waitKey(0);
    
    cv::destroyAllWindows();
    tess->End();
    delete [] outText;
    
}


//find blocks of text in an image using different filter techniques
void getTextblocks(cv::Mat src, std::vector<cv::Mat> *textblocks)
{
    //step one: canny edge filter
    cv::Mat dst, dst2, src_gray, detected_edges, linek;
    
    int lowThreshold = 60;
    int ratio = 3;
    int kernel_size = 3;
    char* window_name = "Edge Map";
    
    /// Create a matrix of the same type and size as src (for dst)
    dst.create( src.size(), src.type() );
    dst2.create( src.size(), src.type() );
    /// Convert the image to grayscale
    cv::cvtColor( src, src_gray, CV_BGR2GRAY );
    /// Reduce noise with a kernel 3x3
    cv::blur( src_gray, detected_edges, cv::Size(3,3) );
    /// Canny detector
    cv::Canny( detected_edges, detected_edges, lowThreshold, lowThreshold*ratio, kernel_size );
    
    /// Using Canny's output as a mask, we display our result
    //dst = cv::Scalar::all(0);

    //src.copyTo( dst, detected_edges);
    
    //Rahmen entfernen

    std::vector<std::vector<cv::Point> > contours;
    std::vector<cv::Vec4i> hierarchy;
    
    cv::findContours(detected_edges, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE, cv::Point(0, 0) );
     /// Approximate contours to polygons + get bounding rects and circles
     std::vector<std::vector<cv::Point> > contours_poly( contours.size() );
     std::vector<cv::Rect> boundRect( contours.size() );
     std::vector<cv::Point2f> center( contours.size() );
     std::vector<float> radius( contours.size() );

     for( int i = 0; i < contours.size(); i++ )
     { 
         cv::approxPolyDP( cv::Mat(contours[i]), contours_poly[i], 3, true );
         boundRect[i] = cv::boundingRect( cv::Mat(contours_poly[i]) );
     }

     /// Draw polygonal contour + bonding rects
     cv::Mat textMask = cv::Mat::zeros( detected_edges.size(), detected_edges.type());
     cv::Point correctionPoint = cv::Point(20,20);
     for( int i = 0; i< contours.size(); i++ )
     {
         cv::Scalar color = cv::Scalar(255,0,255);
         //cv::drawContours( drawing, contours_poly, i, color, 1, 8, std::vector<cv::Vec4i>(), 0, cv::Point() );
         if(boundRect[i].width > 300 && boundRect[i].height > 150)
            cv::rectangle( textMask, boundRect[i].tl()+correctionPoint, boundRect[i].br()-correctionPoint, color, CV_FILLED, 8, 0 );          
     }
     
     src_gray.copyTo( dst, textMask);
     
    /// Create a window
    cv::namedWindow( window_name, CV_WINDOW_AUTOSIZE );
    cv::imshow( window_name, dst );
    

}


//code from stackoverflow 23506105, William
std::vector<cv::Rect> detectLetters(cv::Mat img)
{
    std::vector<cv::Rect> boundRect;
    cv::Mat img_gray, img_sobel, img_threshold, element;
    cv::cvtColor(img, img_gray, CV_BGR2GRAY);
    cv::Sobel(img_gray, img_sobel, CV_8U, 1, 0, 3, 1, 0, cv::BORDER_DEFAULT);
    cv::threshold(img_sobel, img_threshold, 0, 255, CV_THRESH_OTSU+CV_THRESH_BINARY);
    element = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(30, 30) );
    cv::morphologyEx(img_threshold, img_threshold, CV_MOP_CLOSE, element); //Does the trick
    std::vector< std::vector< cv::Point> > contours;
    cv::findContours(img_threshold, contours, 0, 1); 
    std::vector<std::vector<cv::Point> > contours_poly( contours.size() );
    for( int i = 0; i < contours.size(); i++ )
        if (contours[i].size()>100)
        { 
            cv::approxPolyDP( cv::Mat(contours[i]), contours_poly[i], 3, true );
            cv::Rect appRect( cv::boundingRect( cv::Mat(contours_poly[i]) ));
            if (appRect.width>appRect.height) 
                boundRect.push_back(appRect);
        }
    return boundRect;
}