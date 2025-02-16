#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QString>
#include <QLabel>
#include <opencv2/opencv.hpp>
#include <stack>

#include "contDetector.h"
QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onLoadImage();               //load an image
    void onConvertToGrayscale();      //convert image to grayscale
    void onSaveImage();               //save grayscale image
    void onUndo();                    //undo last operation
    void onShowOriginal();            //show original image in a new window
    void onShowGrayscale();           //show grayscale image in a new window
    void onFindContours();            //placeholder for finding contours
    void onShowContours();            //placeholder for displaying contours

private:
    Ui::MainWindow *ui;
    QString loadedImagePath;          //path to the loaded image
    cv::Mat loadedImage;              //the loaded image
    cv::Mat grayscaleImage;           //the grayscale version of the image
    std::stack<cv::Mat> imageHistory; //stack to store previous images
    contDetector worker;
    void displayImage(const cv::Mat& image); //display image in QLabel
};

#endif // MAINWINDOW_H
