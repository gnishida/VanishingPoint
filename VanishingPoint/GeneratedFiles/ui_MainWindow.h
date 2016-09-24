/********************************************************************************
** Form generated from reading UI file 'MainWindow.ui'
**
** Created by: Qt User Interface Compiler version 5.6.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindowClass
{
public:
    QAction *actionExit;
    QAction *actionOpenImage;
    QAction *actionUndo;
    QAction *actionSaveLines;
    QAction *actionComputeVanishingPoint;
    QAction *actionLoadLines;
    QAction *actionComputeCameraMatrix;
    QAction *actionClearLines;
    QAction *actionCentering;
    QAction *actionOption;
    QWidget *centralWidget;
    QMenuBar *menuBar;
    QMenu *menuFile;
    QMenu *menuEdit;
    QMenu *menuTool;
    QToolBar *mainToolBar;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *MainWindowClass)
    {
        if (MainWindowClass->objectName().isEmpty())
            MainWindowClass->setObjectName(QStringLiteral("MainWindowClass"));
        MainWindowClass->resize(800, 853);
        actionExit = new QAction(MainWindowClass);
        actionExit->setObjectName(QStringLiteral("actionExit"));
        actionOpenImage = new QAction(MainWindowClass);
        actionOpenImage->setObjectName(QStringLiteral("actionOpenImage"));
        actionUndo = new QAction(MainWindowClass);
        actionUndo->setObjectName(QStringLiteral("actionUndo"));
        actionSaveLines = new QAction(MainWindowClass);
        actionSaveLines->setObjectName(QStringLiteral("actionSaveLines"));
        actionComputeVanishingPoint = new QAction(MainWindowClass);
        actionComputeVanishingPoint->setObjectName(QStringLiteral("actionComputeVanishingPoint"));
        actionLoadLines = new QAction(MainWindowClass);
        actionLoadLines->setObjectName(QStringLiteral("actionLoadLines"));
        actionComputeCameraMatrix = new QAction(MainWindowClass);
        actionComputeCameraMatrix->setObjectName(QStringLiteral("actionComputeCameraMatrix"));
        actionClearLines = new QAction(MainWindowClass);
        actionClearLines->setObjectName(QStringLiteral("actionClearLines"));
        actionCentering = new QAction(MainWindowClass);
        actionCentering->setObjectName(QStringLiteral("actionCentering"));
        actionOption = new QAction(MainWindowClass);
        actionOption->setObjectName(QStringLiteral("actionOption"));
        centralWidget = new QWidget(MainWindowClass);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
        MainWindowClass->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(MainWindowClass);
        menuBar->setObjectName(QStringLiteral("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 800, 21));
        menuFile = new QMenu(menuBar);
        menuFile->setObjectName(QStringLiteral("menuFile"));
        menuEdit = new QMenu(menuBar);
        menuEdit->setObjectName(QStringLiteral("menuEdit"));
        menuTool = new QMenu(menuBar);
        menuTool->setObjectName(QStringLiteral("menuTool"));
        MainWindowClass->setMenuBar(menuBar);
        mainToolBar = new QToolBar(MainWindowClass);
        mainToolBar->setObjectName(QStringLiteral("mainToolBar"));
        MainWindowClass->addToolBar(Qt::TopToolBarArea, mainToolBar);
        statusBar = new QStatusBar(MainWindowClass);
        statusBar->setObjectName(QStringLiteral("statusBar"));
        MainWindowClass->setStatusBar(statusBar);

        menuBar->addAction(menuFile->menuAction());
        menuBar->addAction(menuEdit->menuAction());
        menuBar->addAction(menuTool->menuAction());
        menuFile->addAction(actionOpenImage);
        menuFile->addSeparator();
        menuFile->addAction(actionClearLines);
        menuFile->addAction(actionLoadLines);
        menuFile->addAction(actionSaveLines);
        menuFile->addSeparator();
        menuFile->addAction(actionExit);
        menuEdit->addAction(actionUndo);
        menuTool->addAction(actionComputeVanishingPoint);
        menuTool->addAction(actionComputeCameraMatrix);
        menuTool->addSeparator();
        menuTool->addAction(actionOption);

        retranslateUi(MainWindowClass);

        QMetaObject::connectSlotsByName(MainWindowClass);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindowClass)
    {
        MainWindowClass->setWindowTitle(QApplication::translate("MainWindowClass", "Vanishing Point", 0));
        actionExit->setText(QApplication::translate("MainWindowClass", "Exit", 0));
        actionOpenImage->setText(QApplication::translate("MainWindowClass", "Open Image", 0));
        actionOpenImage->setShortcut(QApplication::translate("MainWindowClass", "Ctrl+O", 0));
        actionUndo->setText(QApplication::translate("MainWindowClass", "Undo", 0));
        actionUndo->setShortcut(QApplication::translate("MainWindowClass", "Ctrl+Z", 0));
        actionSaveLines->setText(QApplication::translate("MainWindowClass", "Save Lines", 0));
        actionComputeVanishingPoint->setText(QApplication::translate("MainWindowClass", "Compute Vanishing Point", 0));
        actionLoadLines->setText(QApplication::translate("MainWindowClass", "Load Lines", 0));
        actionComputeCameraMatrix->setText(QApplication::translate("MainWindowClass", "Compute Camera Matrix", 0));
        actionClearLines->setText(QApplication::translate("MainWindowClass", "Clear Lines", 0));
        actionClearLines->setShortcut(QApplication::translate("MainWindowClass", "Ctrl+N", 0));
        actionCentering->setText(QApplication::translate("MainWindowClass", "Centering", 0));
        actionOption->setText(QApplication::translate("MainWindowClass", "Option", 0));
        menuFile->setTitle(QApplication::translate("MainWindowClass", "File", 0));
        menuEdit->setTitle(QApplication::translate("MainWindowClass", "Edit", 0));
        menuTool->setTitle(QApplication::translate("MainWindowClass", "Tool", 0));
    } // retranslateUi

};

namespace Ui {
    class MainWindowClass: public Ui_MainWindowClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
