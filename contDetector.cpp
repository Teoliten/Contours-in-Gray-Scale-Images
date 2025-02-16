#include "contDetector.h"
#include <cmath>
#include <stack>
#include <fstream>
#include <iostream>
/*
I can't believe I got this to work. Feels like it's held together by duct tape.
Resources used: https://en.wikipedia.org/wiki/Canny_edge_detector
                https://www.daslhub.org/unlv/wiki/lib/exe/fetch.php?media=cannytutorial.pdf
                https://masters.donntu.ru/2010/fknt/chudovskaja/library/article5.htm
                various stackoverflow questions(some mentioned below)
*/

contDetector::contDetector(){

}

cv::Mat contDetector::getImg(){
    return edgeTrackingImage;
}

void contDetector::setImg(cv::Mat gsi){
    grayScaleImage = gsi;
}
void contDetector::applyBlur() {
    blurredImage = cv::Mat::zeros(grayScaleImage.size(), CV_8U);

    const int KRNL_SIZE = 5;
    const int halfKernelSize = KRNL_SIZE / 2;

    const double GAUSSIAN_KERNEL[KRNL_SIZE][KRNL_SIZE] = {
        {2, 4, 5, 4, 2},
        {4, 9, 12, 9, 4},
        {5, 12, 15, 12, 5},
        {4, 9, 12, 9, 4},
        {2, 4, 5, 4, 2}
    };

    double kernelSum = 0.0;
    for (int i = 0; i < KRNL_SIZE; i++) {
        for (int j = 0; j < KRNL_SIZE; j++) {
            kernelSum += GAUSSIAN_KERNEL[i][j];
        }
    }

    const int height = grayScaleImage.rows;
    const int width  = grayScaleImage.cols;

    // This already sped up the program quite a bit. Will not use it later on, will reply on .at
    uchar* src = grayScaleImage.ptr<uchar>();
    uchar* dst = blurredImage.ptr<uchar>();

    for (int y = halfKernelSize; y < height - halfKernelSize; y++) {
        for (int x = halfKernelSize; x < width - halfKernelSize; x++) {
            double sum = 0.0;
            for (int ky = -halfKernelSize; ky <= halfKernelSize; ky++) {
                for (int kx = -halfKernelSize; kx <= halfKernelSize; kx++) {

                    int pixelY = y + ky;
                    int pixelX = x + kx;

                    double pixelVal = static_cast<double>(src[pixelY * width + pixelX]);
                    double kernelVal = GAUSSIAN_KERNEL[ky + halfKernelSize][kx + halfKernelSize];

                    sum += pixelVal * kernelVal;
                }
            }
            dst[y * width + x] = cv::saturate_cast<uchar>(sum / kernelSum);
        }
    }

}

void contDetector::applySobel(){//https://stackoverflow.com/questions/17815687/image-processing-implementing-sobel-filter
    const int SOBEL_R = 3;
    const int SOBEL_C = 3;
    const int SOBEL_X[SOBEL_R][SOBEL_C] = {
        {-1, 0, 1},
        {-2, 0, 2},
        {-1, 0, 1}
    };

    const int SOBEL_Y[SOBEL_R][SOBEL_C] = {
        {-1, -2, -1},
        {0, 0, 0,},
        {1, 2, 1}
    };

    sobelImage = cv::Mat::zeros(blurredImage.size(), CV_8U);
    for(int y = 0; y < blurredImage.rows - 1; y++){
        for(int x = 0; x < blurredImage.cols - 1; x++){
            int pixel_y = 0;
            int pixel_x = 0;

            for(int i = -1; i <= 1; i++){
                for(int j = -1; j <= 1; j++){
                    pixel_x += SOBEL_X[i + 1][j + 1] * blurredImage.at<uchar>(y + i, x + j);
                    pixel_y += SOBEL_Y[i + 1][j + 1] * blurredImage.at<uchar>(y + i, x + j);
                }
            }
            int magnitude = (int) sqrt(pixel_x * pixel_x + pixel_y * pixel_y);
            //std::cout << "crash after" << std::endl;
            sobelImage.at<uchar>(y, x) = static_cast<uchar>(magnitude);
        }

    }
    /*
    cv::Mat magDisplay;
    cv::normalize(mag, magDisplay, 0, 255, cv::NORM_MINMAX, CV_8U);
    cv::imshow("sobel", magDisplay);*/
}

void contDetector::applyNMS() {

    cv::Mat gradX, gradY;
    cv::Sobel(blurredImage, gradX, CV_32F, 1, 0, 3);
    cv::Sobel(blurredImage, gradY, CV_32F, 0, 1, 3);

    cv::Mat mag, angle;
    cv::cartToPolar(gradX, gradY, mag, angle, true); // 'true' => angle in degrees

    nmsImage = cv::Mat::zeros(mag.size(), CV_8U);

    //cv::Mat magDisplay;
    //cv::normalize(mag, magDisplay, 0, 255, cv::NORM_MINMAX, CV_8U);
    //cv::imshow("Magnitude", magDisplay);
    //cv::waitKey(0);
    for (int y = 1; y < mag.rows - 1; y++) {
        for (int x = 1; x < mag.cols - 1; x++) {
            float currentMag = mag.at<float>(y, x);
            float currentAngle = angle.at<float>(y, x);

            // Map angle to [0,180)
            if (currentAngle < 0)
                currentAngle += 180;

            // Quantize the angle to one of four directions: 0, 45, 90, or 135 degrees.
            float quantizedAngle;
            if ((currentAngle >= 0 && currentAngle < 22.5) || (currentAngle >= 157.5 && currentAngle <= 180))
                quantizedAngle = 0;
            else if (currentAngle >= 22.5 && currentAngle < 67.5)
                quantizedAngle = 45;
            else if (currentAngle >= 67.5 && currentAngle < 112.5)
                quantizedAngle = 90;
            else if (currentAngle >= 112.5 && currentAngle < 157.5)
                quantizedAngle = 135;

            float neighbor1 = 0, neighbor2 = 0;
            // Determine the two neighbor pixels to compare against.
            if (quantizedAngle == 0) {  // Horizontal edge: compare left & right
                neighbor1 = mag.at<float>(y, x - 1);
                neighbor2 = mag.at<float>(y, x + 1);
            }
            else if (quantizedAngle == 45) {  // Diagonal: top-right & bottom-left
                neighbor1 = mag.at<float>(y - 1, x + 1);
                neighbor2 = mag.at<float>(y + 1, x - 1);
            }
            else if (quantizedAngle == 90) {  // Vertical edge: compare top & bottom
                neighbor1 = mag.at<float>(y - 1, x);
                neighbor2 = mag.at<float>(y + 1, x);
            }
            else if (quantizedAngle == 135) { // Diagonal: top-left & bottom-right
                neighbor1 = mag.at<float>(y - 1, x - 1);
                neighbor2 = mag.at<float>(y + 1, x + 1);
            }

            if (currentMag >= neighbor1 && currentMag >= neighbor2) {

                int outputValue = std::min(static_cast<int>(currentMag), 255);
                nmsImage.at<uchar>(y, x) = static_cast<uchar>(outputValue);
            }
            else {
                nmsImage.at<uchar>(y, x) = 0;
            }
        }
    }

    double minVal, maxVal;
    cv::minMaxLoc(nmsImage, &minVal, &maxVal);
    std::cout << "NMS Image - min: " << minVal << ", max: " << maxVal << std::endl;
    /*
    cv::Mat magDisplay;
    cv::normalize(mag, magDisplay, 0, 255, cv::NORM_MINMAX, CV_8U);
    cv::imshow("NMS", magDisplay);*/
}
void contDetector::applyDoubleThreshold() {

    doubleThresholdImage = cv::Mat::zeros(nmsImage.size(), CV_8U);

    double maxVal;
    cv::minMaxLoc(nmsImage, nullptr, &maxVal);

    // high and low thresholds as percentages of the maximum value
    const int highThreshold = static_cast<int>(maxVal * 0.30);
    const int lowThreshold = static_cast<int>(maxVal * 0.10);

    // First pass: Mark strong and weak edges
    std::vector<cv::Point> weakEdges; // Store weak edge positions for second pass

    for(int y = 1; y < nmsImage.rows - 1; y++) {
        for(int x = 1; x < nmsImage.cols - 1; x++) {
            uchar pixelValue = nmsImage.at<uchar>(y, x);

            if(pixelValue >= highThreshold) {
                // Strong edge pixel
                doubleThresholdImage.at<uchar>(y, x) = 255;
            }
            else if(pixelValue >= lowThreshold) {
                // Potential weak edge pixel - save for second pass
                weakEdges.push_back(cv::Point(x, y));
            }
        }
    }

    // Second pass: Check weak edges for connectivity to strong edges
    const int dx[] = {-1, -1, -1,  0,  0,  1, 1, 1};
    const int dy[] = {-1,  0,  1, -1,  1, -1, 0, 1};

    for(const cv::Point& weakEdge : weakEdges) {
        // Check 8-connected neighbors
        bool connectedToStrong = false;

        for(int i = 0; i < 8; i++) {
            int newX = weakEdge.x + dx[i];
            int newY = weakEdge.y + dy[i];

            // Check if the neighbor is a strong edge
            if(doubleThresholdImage.at<uchar>(newY, newX) == 255) {
                connectedToStrong = true;
                break;
            }
        }

        // Keep weak edge if connected to strong edge
        if(connectedToStrong) {
            doubleThresholdImage.at<uchar>(weakEdge.y, weakEdge.x) = 255;
        }
    }
    /*
    cv::Mat magDisplay;
    cv::normalize(mag, magDisplay, 0, 255, cv::NORM_MINMAX, CV_8U);
    cv::imshow("double t", magDisplay);*/
}
void contDetector::applyEdgeTracking() {

    edgeTrackingImage = cv::Mat::zeros(doubleThresholdImage.size(), CV_8U);

    std::vector<cv::Point> strongEdges;
    for(int y = 1; y < doubleThresholdImage.rows - 1; y++) {
        for(int x = 1; x < doubleThresholdImage.cols - 1; x++) {
            if(doubleThresholdImage.at<uchar>(y, x) == 255) {
                strongEdges.push_back(cv::Point(x, y));
                edgeTrackingImage.at<uchar>(y, x) = 255;  // Mark strong edge in output
            }
        }
    }

    const int dx[] = {-1, -1, -1,  0,  0,  1, 1, 1};
    const int dy[] = {-1,  0,  1, -1,  1, -1, 0, 1};

    for(const cv::Point& edge : strongEdges) {
        // Create a stack for depth-first search
        std::stack<cv::Point> stack;
        stack.push(edge);

        // Perform depth-first search to trace connected edges
        while(!stack.empty()) {
            cv::Point current = stack.top();
            stack.pop();

            // Check all 8 neighbors
            for(int i = 0; i < 8; i++) {
                int newX = current.x + dx[i];
                int newY = current.y + dy[i];

                if(newX < 1 || newX >= doubleThresholdImage.cols - 1 ||
                    newY < 1 || newY >= doubleThresholdImage.rows - 1) {
                    continue;
                }

                // If neighbor is a weak edge in thresholded image and not yet marked in hysteresis
                if(doubleThresholdImage.at<uchar>(newY, newX) > 0 &&
                    edgeTrackingImage.at<uchar>(newY, newX) == 0) {
                    // Mark this pixel as an edge
                    edgeTrackingImage.at<uchar>(newY, newX) = 255;
                    // Add to stack to check its neighbors
                    stack.push(cv::Point(newX, newY));
                }
            }
        }
    }
    /*
    cv::Mat magDisplay;
    cv::normalize(mag, magDisplay, 0, 255, cv::NORM_MINMAX, CV_8U);
    cv::imshow("edge", magDisplay);
    */
}

void contDetector::calculateFreemanCodes() {
    freemanCodes.clear();
    contourStartPoints.clear();

//Coordinates STRUCTURE
    // 5 6 7
    // 4 X 0
    // 3 2 1

    const int dx[] = {1, 1, 0, -1, -1, -1, 0, 1};
    const int dy[] = {0, 1, 1, 1, 0, -1, -1, -1};

    cv::Mat visited = cv::Mat::zeros(edgeTrackingImage.size(), CV_8U);

    for (int y = 0; y < edgeTrackingImage.rows; y++) {
        for (int x = 0; x < edgeTrackingImage.cols; x++) {
            if (edgeTrackingImage.at<uchar>(y, x) == 255 && visited.at<uchar>(y, x) == 0) {

                std::vector<int> chainCode;
                cv::Point startPoint(x, y);
                contourStartPoints.push_back(startPoint);

                cv::Point current(x, y);
                visited.at<uchar>(y, x) = 1;

                int lastDirection = 0;

                do {
                    bool foundNext = false;
                    for (int i = 0; i < 8; i++) {
                        int dir = (lastDirection + i) % 8;
                        int newX = current.x + dx[dir];
                        int newY = current.y + dy[dir];

                        if (newX >= 0 && newX < edgeTrackingImage.cols &&
                            newY >= 0 && newY < edgeTrackingImage.rows &&
                            edgeTrackingImage.at<uchar>(newY, newX) == 255 &&
                            visited.at<uchar>(newY, newX) == 0) {

                            chainCode.push_back(dir);
                            visited.at<uchar>(newY, newX) = 1;
                            current = cv::Point(newX, newY);
                            lastDirection = dir;
                            foundNext = true;
                            break;
                        }
                    }

                    if (!foundNext) break;
                } while (current != startPoint);

                if (!chainCode.empty()) {
                    freemanCodes.push_back(chainCode);
                }
            }
        }
    }

    std::cout << "Total contours found: " << freemanCodes.size() << std::endl;
}

void contDetector::saveFreemanCodesToFile() {
    std::string filePath = "../freemanCodes/freemanCodes.txt";

    std::ofstream file(filePath);
    if (!file.is_open()) {
        std::cerr << "Error: could not open file " << filePath << " for writing." << std::endl;
        return;
    }

    for (size_t i = 0; i < freemanCodes.size(); ++i) {
        if (freemanCodes[i].empty()) continue;  // Skip empty contours

        // Get the starting coordinate of the contour
        cv::Point startPoint = contourStartPoints[i];
        file << "Contour " << (i + 1) << ": (" << startPoint.x << ", " << startPoint.y << ")\n";

        // Write Freeman chain code sequence
        for (size_t j = 0; j < freemanCodes[i].size(); ++j) {
            file << freemanCodes[i][j] << " ";
        }
        file << "\n\n";
    }

    file.close();
    std::cout << "Freeman codes saved to " << filePath << std::endl;
}
