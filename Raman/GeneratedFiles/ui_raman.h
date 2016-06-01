/********************************************************************************
** Form generated from reading UI file 'raman.ui'
**
** Created by: Qt User Interface Compiler version 5.4.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_RAMAN_H
#define UI_RAMAN_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_RamanClass
{
public:
    QMenuBar *menuBar;
    QToolBar *mainToolBar;
    QWidget *centralWidget;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *RamanClass)
    {
        if (RamanClass->objectName().isEmpty())
            RamanClass->setObjectName(QStringLiteral("RamanClass"));
        RamanClass->resize(600, 400);
        menuBar = new QMenuBar(RamanClass);
        menuBar->setObjectName(QStringLiteral("menuBar"));
        RamanClass->setMenuBar(menuBar);
        mainToolBar = new QToolBar(RamanClass);
        mainToolBar->setObjectName(QStringLiteral("mainToolBar"));
        RamanClass->addToolBar(mainToolBar);
        centralWidget = new QWidget(RamanClass);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
        RamanClass->setCentralWidget(centralWidget);
        statusBar = new QStatusBar(RamanClass);
        statusBar->setObjectName(QStringLiteral("statusBar"));
        RamanClass->setStatusBar(statusBar);

        retranslateUi(RamanClass);

        QMetaObject::connectSlotsByName(RamanClass);
    } // setupUi

    void retranslateUi(QMainWindow *RamanClass)
    {
        RamanClass->setWindowTitle(QApplication::translate("RamanClass", "Raman", 0));
    } // retranslateUi

};

namespace Ui {
    class RamanClass: public Ui_RamanClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_RAMAN_H
