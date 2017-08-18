/********************************************************************************
** Form generated from reading UI file 'OptionDialog.ui'
**
** Created by: Qt User Interface Compiler version 5.6.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_OPTIONDIALOG_H
#define UI_OPTIONDIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QDialog>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpinBox>

QT_BEGIN_NAMESPACE

class Ui_OptionDialog
{
public:
    QLabel *label;
    QLabel *label_6;
    QLabel *label_10;
    QPushButton *pushButtonOK;
    QPushButton *pushButtonCancel;
    QLabel *label_14;
    QLabel *label_18;
    QLabel *label_19;
    QLabel *label_20;
    QLabel *label_21;
    QPushButton *pushButtonHorizontalLeftLineColor;
    QLabel *label_22;
    QPushButton *pushButtonHorizontalRightLineColor;
    QPushButton *pushButtonVerticalLineColor;
    QPushButton *pushButtonSilhouetteColor;
    QSpinBox *spinBoxVanishingLineWidth;
    QSpinBox *spinBoxSilhouetteLineWidth;
    QSpinBox *spinBoxMassGrammarId;
    QSpinBox *spinBoxFacadeGrammarId;
    QSpinBox *spinBoxWindowGrammarId;

    void setupUi(QDialog *OptionDialog)
    {
        if (OptionDialog->objectName().isEmpty())
            OptionDialog->setObjectName(QStringLiteral("OptionDialog"));
        OptionDialog->resize(323, 340);
        label = new QLabel(OptionDialog);
        label->setObjectName(QStringLiteral("label"));
        label->setGeometry(QRect(20, 20, 101, 16));
        label_6 = new QLabel(OptionDialog);
        label_6->setObjectName(QStringLiteral("label_6"));
        label_6->setGeometry(QRect(20, 80, 121, 16));
        label_10 = new QLabel(OptionDialog);
        label_10->setObjectName(QStringLiteral("label_10"));
        label_10->setGeometry(QRect(20, 110, 121, 16));
        pushButtonOK = new QPushButton(OptionDialog);
        pushButtonOK->setObjectName(QStringLiteral("pushButtonOK"));
        pushButtonOK->setGeometry(QRect(50, 300, 91, 31));
        pushButtonCancel = new QPushButton(OptionDialog);
        pushButtonCancel->setObjectName(QStringLiteral("pushButtonCancel"));
        pushButtonCancel->setGeometry(QRect(190, 300, 91, 31));
        label_14 = new QLabel(OptionDialog);
        label_14->setObjectName(QStringLiteral("label_14"));
        label_14->setGeometry(QRect(20, 200, 121, 16));
        label_18 = new QLabel(OptionDialog);
        label_18->setObjectName(QStringLiteral("label_18"));
        label_18->setGeometry(QRect(20, 170, 121, 16));
        label_19 = new QLabel(OptionDialog);
        label_19->setObjectName(QStringLiteral("label_19"));
        label_19->setGeometry(QRect(20, 140, 101, 16));
        label_20 = new QLabel(OptionDialog);
        label_20->setObjectName(QStringLiteral("label_20"));
        label_20->setGeometry(QRect(20, 230, 121, 16));
        label_21 = new QLabel(OptionDialog);
        label_21->setObjectName(QStringLiteral("label_21"));
        label_21->setGeometry(QRect(20, 260, 121, 16));
        pushButtonHorizontalLeftLineColor = new QPushButton(OptionDialog);
        pushButtonHorizontalLeftLineColor->setObjectName(QStringLiteral("pushButtonHorizontalLeftLineColor"));
        pushButtonHorizontalLeftLineColor->setGeometry(QRect(160, 50, 31, 23));
        pushButtonHorizontalLeftLineColor->setStyleSheet(QLatin1String("border: 2px solid black;\n"
"background-color: blue;"));
        pushButtonHorizontalLeftLineColor->setFlat(true);
        label_22 = new QLabel(OptionDialog);
        label_22->setObjectName(QStringLiteral("label_22"));
        label_22->setGeometry(QRect(20, 50, 121, 16));
        pushButtonHorizontalRightLineColor = new QPushButton(OptionDialog);
        pushButtonHorizontalRightLineColor->setObjectName(QStringLiteral("pushButtonHorizontalRightLineColor"));
        pushButtonHorizontalRightLineColor->setGeometry(QRect(160, 80, 31, 23));
        pushButtonHorizontalRightLineColor->setStyleSheet(QStringLiteral("border: 1px solid black;"));
        pushButtonHorizontalRightLineColor->setFlat(true);
        pushButtonVerticalLineColor = new QPushButton(OptionDialog);
        pushButtonVerticalLineColor->setObjectName(QStringLiteral("pushButtonVerticalLineColor"));
        pushButtonVerticalLineColor->setGeometry(QRect(160, 110, 31, 23));
        pushButtonVerticalLineColor->setStyleSheet(QStringLiteral("border: 1px solid black;"));
        pushButtonVerticalLineColor->setFlat(true);
        pushButtonSilhouetteColor = new QPushButton(OptionDialog);
        pushButtonSilhouetteColor->setObjectName(QStringLiteral("pushButtonSilhouetteColor"));
        pushButtonSilhouetteColor->setGeometry(QRect(160, 170, 31, 23));
        pushButtonSilhouetteColor->setStyleSheet(QStringLiteral("border: 1px solid black;"));
        pushButtonSilhouetteColor->setFlat(true);
        spinBoxVanishingLineWidth = new QSpinBox(OptionDialog);
        spinBoxVanishingLineWidth->setObjectName(QStringLiteral("spinBoxVanishingLineWidth"));
        spinBoxVanishingLineWidth->setGeometry(QRect(160, 20, 42, 22));
        spinBoxSilhouetteLineWidth = new QSpinBox(OptionDialog);
        spinBoxSilhouetteLineWidth->setObjectName(QStringLiteral("spinBoxSilhouetteLineWidth"));
        spinBoxSilhouetteLineWidth->setGeometry(QRect(160, 140, 42, 22));
        spinBoxMassGrammarId = new QSpinBox(OptionDialog);
        spinBoxMassGrammarId->setObjectName(QStringLiteral("spinBoxMassGrammarId"));
        spinBoxMassGrammarId->setGeometry(QRect(160, 200, 42, 22));
        spinBoxFacadeGrammarId = new QSpinBox(OptionDialog);
        spinBoxFacadeGrammarId->setObjectName(QStringLiteral("spinBoxFacadeGrammarId"));
        spinBoxFacadeGrammarId->setGeometry(QRect(160, 230, 42, 22));
        spinBoxWindowGrammarId = new QSpinBox(OptionDialog);
        spinBoxWindowGrammarId->setObjectName(QStringLiteral("spinBoxWindowGrammarId"));
        spinBoxWindowGrammarId->setGeometry(QRect(160, 260, 42, 22));
        QWidget::setTabOrder(pushButtonOK, pushButtonCancel);

        retranslateUi(OptionDialog);

        QMetaObject::connectSlotsByName(OptionDialog);
    } // setupUi

    void retranslateUi(QDialog *OptionDialog)
    {
        OptionDialog->setWindowTitle(QApplication::translate("OptionDialog", "OptionDialog", 0));
        label->setText(QApplication::translate("OptionDialog", "Vanishing line width:", 0));
        label_6->setText(QApplication::translate("OptionDialog", "Horizontal right line color:", 0));
        label_10->setText(QApplication::translate("OptionDialog", "Vertical line color:", 0));
        pushButtonOK->setText(QApplication::translate("OptionDialog", "OK", 0));
        pushButtonCancel->setText(QApplication::translate("OptionDialog", "Cancel", 0));
        label_14->setText(QApplication::translate("OptionDialog", "Mass grammar id:", 0));
        label_18->setText(QApplication::translate("OptionDialog", "Silhouette color:", 0));
        label_19->setText(QApplication::translate("OptionDialog", "Silhouette width:", 0));
        label_20->setText(QApplication::translate("OptionDialog", "facade grammar id:", 0));
        label_21->setText(QApplication::translate("OptionDialog", "window grammar id:", 0));
        pushButtonHorizontalLeftLineColor->setText(QString());
        label_22->setText(QApplication::translate("OptionDialog", "Horizontal left line color:", 0));
        pushButtonHorizontalRightLineColor->setText(QString());
        pushButtonVerticalLineColor->setText(QString());
        pushButtonSilhouetteColor->setText(QString());
    } // retranslateUi

};

namespace Ui {
    class OptionDialog: public Ui_OptionDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_OPTIONDIALOG_H
