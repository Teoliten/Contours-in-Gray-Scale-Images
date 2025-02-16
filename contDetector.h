#ifndef CONTDETECTOR_H
#define CONTDETECTOR_H
#include <opencv2/opencv.hpp>
#include <cmath>
//1.1 gaussian blur
//1.2 gradients using sobel
//1.3 NMS
//1.4 double treshold
//1.5 edge tracking

class contDetector {

public:
    contDetector();
    //~contDetector();
    void applyBlur();
    void applySobel();
    void applyNMS();
    void applyDoubleThreshold();
    void applyEdgeTracking();
    void traceContour();

    cv::Mat getImg();

    void setImg(cv::Mat gsi);

    void calculateFreemanCodes();
    void saveFreemanCodesToFile();

private:
    cv::Mat grayScaleImage;
    cv::Mat blurredImage;
    cv::Mat sobelImage;
    cv::Mat nmsImage;
    cv::Mat doubleThresholdImage;
    cv::Mat edgeTrackingImage;

    std::vector<std::vector<int>> freemanCodes;
    std::vector<cv::Point> contourStartPoints;
};

#endif // CONTDETECTOR_H
