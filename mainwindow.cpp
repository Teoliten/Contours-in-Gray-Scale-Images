#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "contDetector.h"
#include <QMessageBox>
#include <QFileDialog>
#include <QPixmap>
#include <QImage>
#include <QWidget>
#include <QVBoxLayout>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);


    //connect buttons to their respective slots
    connect(ui->checkFileButton, &QPushButton::clicked, this, &MainWindow::onLoadImage);
    connect(ui->convertToGrayscaleButton, &QPushButton::clicked, this, &MainWindow::onConvertToGrayscale);
    connect(ui->saveImageButton, &QPushButton::clicked, this, &MainWindow::onSaveImage);
    connect(ui->undoButton, &QPushButton::clicked, this, &MainWindow::onUndo);
    connect(ui->showOriginalButton, &QPushButton::clicked, this, &MainWindow::onShowOriginal);
    connect(ui->showGrayscaleButton, &QPushButton::clicked, this, &MainWindow::onShowGrayscale);

    //placeholders, waiting for gwi to do his work :3
    connect(ui->findContoursButton, &QPushButton::clicked, this, &MainWindow::onFindContours); // connecting placeholder
    connect(ui->showContoursButton, &QPushButton::clicked, this, &MainWindow::onShowContours); // connecting placeholder
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::onShowContours()
{
    if (grayscaleImage.empty()) {
        QMessageBox::warning(this, "Show Contours", "No grayscale image available. Convert an image to grayscale first.");
        return;
    }

    QMessageBox::information(this, "Show Contours", "Contour display will be implemented soon.");
}

void MainWindow::onFindContours()
{
    if (grayscaleImage.empty()) {
        QMessageBox::warning(this, "Find Contours", "No grayscale image available. Convert an image to grayscale first.");
        return;
    }
          std::cout << "runs once" << std::endl;
    worker.applyBlur();
    worker.applySobel();
    worker.applyNMS();
    worker.applyDoubleThreshold();
    worker.applyEdgeTracking();
    cv::Mat gsi = worker.getImg();
    QImage qImage(gsi.data, gsi.cols, gsi.rows, gsi.step, QImage::Format_Grayscale8);
    QWidget *imageWindow = new QWidget;
    imageWindow->setWindowTitle("contours");

    //create a QLabel to display the image
    QLabel *imageLabel = new QLabel(imageWindow);
    imageLabel->setPixmap(QPixmap::fromImage(qImage));
    imageLabel->setScaledContents(true);

    //allow QLabel to expand and shrink when resizing
    imageLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    //use a layout to allow resizing
    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(imageLabel);
    imageWindow->setLayout(layout);

    //allow the window to be freely resizable
    imageWindow->setMinimumSize(300, 300); //prevent it from getting too small
    //imageWindow->resize(600, 400);         //used for testing, purposes u can make funny jumpscares with this

    imageWindow->show();
    //QMessageBox::information(this, "Find Contours", "Contour detection will be implemented soon.");
    worker.calculateFreemanCodes();
    worker.saveFreemanCodesToFile();
}

void MainWindow::onShowGrayscale()
{
    if (grayscaleImage.empty()) {
        QMessageBox::warning(this, "Show Grayscale", "No grayscale image available to display.");
        return;
    }

    //convert cv::Mat to QImage for grayscale image
    QImage qImage(grayscaleImage.data, grayscaleImage.cols, grayscaleImage.rows, grayscaleImage.step, QImage::Format_Grayscale8);
    //create a new window
    QWidget *imageWindow = new QWidget;
    imageWindow->setWindowTitle("Grayscale Image");

    //create a QLabel to display the image
    QLabel *imageLabel = new QLabel(imageWindow);
    imageLabel->setPixmap(QPixmap::fromImage(qImage));
    imageLabel->setScaledContents(true);

    //allow QLabel to expand and shrink when resizing
    imageLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    //use a layout to allow resizing
    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(imageLabel);
    imageWindow->setLayout(layout);

    //allow the window to be freely resizable
    imageWindow->setMinimumSize(300, 300); //prevent it from getting too small
    //imageWindow->resize(600, 400);         //used for testing, purposes u can make funny jumpscares with this

    imageWindow->show();
}



void MainWindow::onShowOriginal()
{
    if (loadedImage.empty()) {
        QMessageBox::warning(this, "Show Original", "No image loaded to display.");
        return;
    }

    //convert cv::Mat to QImage
    QImage qImage;
    cv::Mat rgbImage;
    cv::cvtColor(loadedImage, rgbImage, cv::COLOR_BGR2RGB);
    qImage = QImage(rgbImage.data, rgbImage.cols, rgbImage.rows, rgbImage.step, QImage::Format_RGB888);

    //create a new window
    QWidget *imageWindow = new QWidget;
    imageWindow->setWindowTitle("Original Image");

    //create a QLabel to display the image
    QLabel *imageLabel = new QLabel(imageWindow);
    imageLabel->setPixmap(QPixmap::fromImage(qImage));
    imageLabel->setScaledContents(true);

    //set QLabel to expand and shrink when resizing the window
    imageLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    //use a layout to allow resizing
    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(imageLabel);
    imageWindow->setLayout(layout);

    //allow the window to be freely resizable
    imageWindow->setMinimumSize(300, 300); //set a minimum size to prevent it from getting too small
    //imageWindow->resize(600, 400);         //used for testing, purposes u can make funny jumpscares with this

    imageWindow->show();
}


void MainWindow::onLoadImage()
{
    QString filePath = QFileDialog::getOpenFileName(this, "Open Image File", "", "Image Files (*.png *.jpg *.jpeg)");
    if (filePath.isEmpty()) {
        QMessageBox::warning(this, "Load Image", "No file selected!");
        return;
    }

    //clear previously loaded image
    loadedImage.release();

    //load the new image
    loadedImage = cv::imread(filePath.toStdString());

    if (loadedImage.empty()) {
        QMessageBox::critical(this, "Load Image", "Failed to load the image. Please check the file path or format.");
        return;
    }

    //display the new image
    displayImage(loadedImage);
}


void MainWindow::onConvertToGrayscale()
{
    if (loadedImage.empty()) {
        QMessageBox::warning(this, "Convert to Grayscale", "No image loaded to convert.");
        return;
    }

    //save current state before converting
    imageHistory.push(loadedImage.clone());

    cv::cvtColor(loadedImage, grayscaleImage, cv::COLOR_BGR2GRAY);
    worker.setImg(grayscaleImage);
    displayImage(grayscaleImage);
}

void MainWindow::onUndo()
{
    if (imageHistory.empty()) {
        QMessageBox::warning(this, "Undo", "No previous image to restore.");
        return;
    }

    //restore the last saved image
    loadedImage = imageHistory.top();
    imageHistory.pop();

    displayImage(loadedImage);
}

void MainWindow::onSaveImage()
{
    if (!ui->imageDisplayLabel->pixmap() || ui->imageDisplayLabel->pixmap().isNull()) {
        QMessageBox::warning(this, "Save Image", "No image to save.");
        return;
    }

    QString savePath = QFileDialog::getSaveFileName(this, "Save Image", "", "Image Files (*.png *.jpg *.jpeg)");
    if (savePath.isEmpty()) {
        QMessageBox::warning(this, "Save Image", "No file selected for saving!");
        return;
    }

    //retrieve the current displayed image from QLabel
    QPixmap pixmap = ui->imageDisplayLabel->pixmap();
    QImage qImage = pixmap.toImage();

    //convert QImage to cv::Mat
    cv::Mat imageToSave(qImage.height(), qImage.width(), CV_8UC4, (void*)qImage.bits(), qImage.bytesPerLine());

    //convert from BGRA (Qt format) to BGR (OpenCV format)
    cv::Mat finalImage;
    cv::cvtColor(imageToSave, finalImage, cv::COLOR_BGRA2BGR);

    //save the extracted image
    if (!cv::imwrite(savePath.toStdString(), finalImage)) {
        QMessageBox::critical(this, "Save Image", "Failed to save the image.");
        return;
    }

    QMessageBox::information(this, "Save Image", "Image saved successfully.");
}


void MainWindow::displayImage(const cv::Mat &image)
{
    if (image.empty()) {
        QMessageBox::warning(this, "Display Image", "No image available to display.");
        return;
    }

    //convert cv::Mat to QImage safely
    QImage qImage;
    if (image.channels() == 3) {
        cv::Mat rgb;
        cv::cvtColor(image, rgb, cv::COLOR_BGR2RGB);
        qImage = QImage(rgb.data, rgb.cols, rgb.rows, rgb.step, QImage::Format_RGB888).copy();
    } else {
        qImage = QImage(image.data, image.cols, image.rows, image.step, QImage::Format_Grayscale8).copy();
    }

    //assign QPixmap safely
    QPixmap pixmap = QPixmap::fromImage(qImage);
    ui->imageDisplayLabel->setPixmap(pixmap.scaled(ui->imageDisplayLabel->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));

    ui->imageDisplayLabel->setScaledContents(true);
    ui->imageDisplayLabel->update();
}



