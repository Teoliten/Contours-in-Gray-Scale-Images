/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 6.8.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralwidget;
    QPushButton *checkFileButton;
    QPushButton *convertToGrayscaleButton;
    QPushButton *saveImageButton;
    QLabel *imageDisplayLabel;
    QPushButton *undoButton;
    QPushButton *showOriginalButton;
    QPushButton *showGrayscaleButton;
    QPushButton *findContoursButton;
    QPushButton *showContoursButton;
    QMenuBar *menubar;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName("MainWindow");
        MainWindow->resize(800, 630);
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName("centralwidget");
        checkFileButton = new QPushButton(centralwidget);
        checkFileButton->setObjectName("checkFileButton");
        checkFileButton->setGeometry(QRect(20, 40, 88, 34));
        convertToGrayscaleButton = new QPushButton(centralwidget);
        convertToGrayscaleButton->setObjectName("convertToGrayscaleButton");
        convertToGrayscaleButton->setGeometry(QRect(20, 90, 141, 34));
        saveImageButton = new QPushButton(centralwidget);
        saveImageButton->setObjectName("saveImageButton");
        saveImageButton->setGeometry(QRect(20, 420, 88, 34));
        imageDisplayLabel = new QLabel(centralwidget);
        imageDisplayLabel->setObjectName("imageDisplayLabel");
        imageDisplayLabel->setGeometry(QRect(260, 40, 481, 451));
        undoButton = new QPushButton(centralwidget);
        undoButton->setObjectName("undoButton");
        undoButton->setGeometry(QRect(20, 510, 88, 34));
        showOriginalButton = new QPushButton(centralwidget);
        showOriginalButton->setObjectName("showOriginalButton");
        showOriginalButton->setGeometry(QRect(640, 510, 101, 34));
        showGrayscaleButton = new QPushButton(centralwidget);
        showGrayscaleButton->setObjectName("showGrayscaleButton");
        showGrayscaleButton->setGeometry(QRect(450, 510, 111, 34));
        findContoursButton = new QPushButton(centralwidget);
        findContoursButton->setObjectName("findContoursButton");
        findContoursButton->setGeometry(QRect(20, 140, 111, 34));
        showContoursButton = new QPushButton(centralwidget);
        showContoursButton->setObjectName("showContoursButton");
        showContoursButton->setGeometry(QRect(260, 510, 111, 34));
        MainWindow->setCentralWidget(centralwidget);
        menubar = new QMenuBar(MainWindow);
        menubar->setObjectName("menubar");
        menubar->setGeometry(QRect(0, 0, 800, 30));
        MainWindow->setMenuBar(menubar);
        statusbar = new QStatusBar(MainWindow);
        statusbar->setObjectName("statusbar");
        MainWindow->setStatusBar(statusbar);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "MainWindow", nullptr));
        checkFileButton->setText(QCoreApplication::translate("MainWindow", "Open File", nullptr));
        convertToGrayscaleButton->setText(QCoreApplication::translate("MainWindow", "Convert to Grayscale", nullptr));
        saveImageButton->setText(QCoreApplication::translate("MainWindow", "Save", nullptr));
        imageDisplayLabel->setText(QString());
        undoButton->setText(QCoreApplication::translate("MainWindow", "Undo", nullptr));
        showOriginalButton->setText(QCoreApplication::translate("MainWindow", "Show Original", nullptr));
        showGrayscaleButton->setText(QCoreApplication::translate("MainWindow", "Show Grayscale", nullptr));
        findContoursButton->setText(QCoreApplication::translate("MainWindow", "Find Contours", nullptr));
        showContoursButton->setText(QCoreApplication::translate("MainWindow", "Show Contours", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
