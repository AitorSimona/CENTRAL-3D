/********************************************************************************
** Form generated from reading UI file 'pvdsettingsdialog.ui'
**
** Created by: Qt User Interface Compiler version 5.10.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_PVDSETTINGSDIALOG_H
#define UI_PVDSETTINGSDIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QDoubleSpinBox>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpinBox>

QT_BEGIN_NAMESPACE

class Ui_PvdSettingsDialog
{
public:
    QDialogButtonBox *buttonBox;
    QGroupBox *groupBoxColors;
    QLabel *label;
    QPushButton *PxRigidDynamicSleepColor;
    QLabel *label_2;
    QPushButton *PxRigidStaticColor;
    QLabel *label_3;
    QPushButton *PxKinematicColor;
    QLabel *label_4;
    QPushButton *PxRigidDynamicActiveColor;
    QPushButton *PxClothColor;
    QLabel *label_5;
    QGroupBox *groupBox;
    QCheckBox *cboxAntialiasing;
    QLabel *label_7;
    QDoubleSpinBox *dsbGeomScaleFactor;
    QGroupBox *groupBox_2;
    QPushButton *ProfileFunctionColor;
    QLabel *label_6;
    QGroupBox *groupBox_3;
    QLabel *label_8;
    QSpinBox *sbFloatPrecision;
    QCheckBox *cboxShowHex;
    QComboBox *comboBoxFloatFormat;
    QLabel *label_9;
    QLabel *label_11;
    QLabel *label_12;
    QSpinBox *sbGroupSize;
    QSpinBox *sbMaxArraySize;
    QGroupBox *groupBox_4;
    QCheckBox *cboxDrawOrbitCenter;
    QLabel *label_13;
    QComboBox *comboBoxDrawPlanes;
    QLabel *label_10;
    QComboBox *comboBoxFaceCulling;

    void setupUi(QDialog *PvdSettingsDialog)
    {
        if (PvdSettingsDialog->objectName().isEmpty())
            PvdSettingsDialog->setObjectName(QStringLiteral("PvdSettingsDialog"));
        PvdSettingsDialog->resize(521, 423);
        PvdSettingsDialog->setMaximumSize(QSize(16777215, 16777215));
        buttonBox = new QDialogButtonBox(PvdSettingsDialog);
        buttonBox->setObjectName(QStringLiteral("buttonBox"));
        buttonBox->setGeometry(QRect(20, 380, 481, 32));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::RestoreDefaults|QDialogButtonBox::Save);
        groupBoxColors = new QGroupBox(PvdSettingsDialog);
        groupBoxColors->setObjectName(QStringLiteral("groupBoxColors"));
        groupBoxColors->setGeometry(QRect(270, 110, 231, 181));
        label = new QLabel(groupBoxColors);
        label->setObjectName(QStringLiteral("label"));
        label->setGeometry(QRect(10, 30, 171, 22));
        PxRigidDynamicSleepColor = new QPushButton(groupBoxColors);
        PxRigidDynamicSleepColor->setObjectName(QStringLiteral("PxRigidDynamicSleepColor"));
        PxRigidDynamicSleepColor->setGeometry(QRect(180, 30, 32, 23));
        label_2 = new QLabel(groupBoxColors);
        label_2->setObjectName(QStringLiteral("label_2"));
        label_2->setGeometry(QRect(10, 90, 101, 22));
        PxRigidStaticColor = new QPushButton(groupBoxColors);
        PxRigidStaticColor->setObjectName(QStringLiteral("PxRigidStaticColor"));
        PxRigidStaticColor->setGeometry(QRect(180, 90, 32, 23));
        label_3 = new QLabel(groupBoxColors);
        label_3->setObjectName(QStringLiteral("label_3"));
        label_3->setGeometry(QRect(10, 120, 101, 22));
        PxKinematicColor = new QPushButton(groupBoxColors);
        PxKinematicColor->setObjectName(QStringLiteral("PxKinematicColor"));
        PxKinematicColor->setGeometry(QRect(180, 120, 32, 23));
        label_4 = new QLabel(groupBoxColors);
        label_4->setObjectName(QStringLiteral("label_4"));
        label_4->setGeometry(QRect(10, 60, 111, 22));
        PxRigidDynamicActiveColor = new QPushButton(groupBoxColors);
        PxRigidDynamicActiveColor->setObjectName(QStringLiteral("PxRigidDynamicActiveColor"));
        PxRigidDynamicActiveColor->setGeometry(QRect(180, 60, 32, 23));
        PxClothColor = new QPushButton(groupBoxColors);
        PxClothColor->setObjectName(QStringLiteral("PxClothColor"));
        PxClothColor->setGeometry(QRect(180, 150, 32, 23));
        label_5 = new QLabel(groupBoxColors);
        label_5->setObjectName(QStringLiteral("label_5"));
        label_5->setGeometry(QRect(10, 150, 101, 22));
        groupBox = new QGroupBox(PvdSettingsDialog);
        groupBox->setObjectName(QStringLiteral("groupBox"));
        groupBox->setGeometry(QRect(270, 10, 231, 91));
        cboxAntialiasing = new QCheckBox(groupBox);
        cboxAntialiasing->setObjectName(QStringLiteral("cboxAntialiasing"));
        cboxAntialiasing->setGeometry(QRect(10, 30, 131, 22));
        label_7 = new QLabel(groupBox);
        label_7->setObjectName(QStringLiteral("label_7"));
        label_7->setGeometry(QRect(10, 60, 111, 22));
        dsbGeomScaleFactor = new QDoubleSpinBox(groupBox);
        dsbGeomScaleFactor->setObjectName(QStringLiteral("dsbGeomScaleFactor"));
        dsbGeomScaleFactor->setGeometry(QRect(150, 60, 62, 22));
        dsbGeomScaleFactor->setMinimum(0.01);
        dsbGeomScaleFactor->setSingleStep(0.01);
        dsbGeomScaleFactor->setValue(1);
        groupBox_2 = new QGroupBox(PvdSettingsDialog);
        groupBox_2->setObjectName(QStringLiteral("groupBox_2"));
        groupBox_2->setGeometry(QRect(270, 300, 231, 61));
        ProfileFunctionColor = new QPushButton(groupBox_2);
        ProfileFunctionColor->setObjectName(QStringLiteral("ProfileFunctionColor"));
        ProfileFunctionColor->setGeometry(QRect(180, 30, 32, 23));
        label_6 = new QLabel(groupBox_2);
        label_6->setObjectName(QStringLiteral("label_6"));
        label_6->setGeometry(QRect(10, 30, 161, 22));
        groupBox_3 = new QGroupBox(PvdSettingsDialog);
        groupBox_3->setObjectName(QStringLiteral("groupBox_3"));
        groupBox_3->setGeometry(QRect(20, 140, 231, 181));
        label_8 = new QLabel(groupBox_3);
        label_8->setObjectName(QStringLiteral("label_8"));
        label_8->setGeometry(QRect(10, 30, 131, 22));
        sbFloatPrecision = new QSpinBox(groupBox_3);
        sbFloatPrecision->setObjectName(QStringLiteral("sbFloatPrecision"));
        sbFloatPrecision->setGeometry(QRect(160, 30, 53, 22));
        sbFloatPrecision->setMaximum(25);
        sbFloatPrecision->setValue(3);
        cboxShowHex = new QCheckBox(groupBox_3);
        cboxShowHex->setObjectName(QStringLiteral("cboxShowHex"));
        cboxShowHex->setGeometry(QRect(10, 90, 181, 22));
        comboBoxFloatFormat = new QComboBox(groupBox_3);
        comboBoxFloatFormat->setObjectName(QStringLiteral("comboBoxFloatFormat"));
        comboBoxFloatFormat->setGeometry(QRect(120, 60, 93, 22));
        label_9 = new QLabel(groupBox_3);
        label_9->setObjectName(QStringLiteral("label_9"));
        label_9->setGeometry(QRect(10, 60, 101, 22));
        label_11 = new QLabel(groupBox_3);
        label_11->setObjectName(QStringLiteral("label_11"));
        label_11->setGeometry(QRect(10, 120, 131, 22));
        label_12 = new QLabel(groupBox_3);
        label_12->setObjectName(QStringLiteral("label_12"));
        label_12->setGeometry(QRect(10, 150, 131, 22));
        sbGroupSize = new QSpinBox(groupBox_3);
        sbGroupSize->setObjectName(QStringLiteral("sbGroupSize"));
        sbGroupSize->setGeometry(QRect(140, 120, 75, 22));
        sbGroupSize->setMinimum(10);
        sbGroupSize->setMaximum(9999);
        sbGroupSize->setSingleStep(10);
        sbGroupSize->setValue(20);
        sbMaxArraySize = new QSpinBox(groupBox_3);
        sbMaxArraySize->setObjectName(QStringLiteral("sbMaxArraySize"));
        sbMaxArraySize->setGeometry(QRect(140, 150, 75, 22));
        sbMaxArraySize->setMaximum(99999);
        sbMaxArraySize->setValue(10000);
        groupBox_4 = new QGroupBox(PvdSettingsDialog);
        groupBox_4->setObjectName(QStringLiteral("groupBox_4"));
        groupBox_4->setGeometry(QRect(20, 10, 231, 121));
        cboxDrawOrbitCenter = new QCheckBox(groupBox_4);
        cboxDrawOrbitCenter->setObjectName(QStringLiteral("cboxDrawOrbitCenter"));
        cboxDrawOrbitCenter->setGeometry(QRect(10, 30, 201, 22));
        label_13 = new QLabel(groupBox_4);
        label_13->setObjectName(QStringLiteral("label_13"));
        label_13->setGeometry(QRect(10, 60, 101, 22));
        comboBoxDrawPlanes = new QComboBox(groupBox_4);
        comboBoxDrawPlanes->setObjectName(QStringLiteral("comboBoxDrawPlanes"));
        comboBoxDrawPlanes->setGeometry(QRect(120, 60, 93, 22));
        label_10 = new QLabel(groupBox_4);
        label_10->setObjectName(QStringLiteral("label_10"));
        label_10->setGeometry(QRect(10, 90, 101, 22));
        comboBoxFaceCulling = new QComboBox(groupBox_4);
        comboBoxFaceCulling->setObjectName(QStringLiteral("comboBoxFaceCulling"));
        comboBoxFaceCulling->setGeometry(QRect(120, 90, 93, 22));

        retranslateUi(PvdSettingsDialog);
        QObject::connect(buttonBox, SIGNAL(accepted()), PvdSettingsDialog, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), PvdSettingsDialog, SLOT(reject()));

        comboBoxFloatFormat->setCurrentIndex(-1);
        comboBoxDrawPlanes->setCurrentIndex(-1);
        comboBoxFaceCulling->setCurrentIndex(-1);


        QMetaObject::connectSlotsByName(PvdSettingsDialog);
    } // setupUi

    void retranslateUi(QDialog *PvdSettingsDialog)
    {
        PvdSettingsDialog->setWindowTitle(QApplication::translate("PvdSettingsDialog", "PVD Settings", nullptr));
        groupBoxColors->setTitle(QApplication::translate("PvdSettingsDialog", "Colors Actors", nullptr));
        label->setText(QApplication::translate("PvdSettingsDialog", "Sleeping dynamic rigid", nullptr));
        PxRigidDynamicSleepColor->setText(QString());
        label_2->setText(QApplication::translate("PvdSettingsDialog", "Static rigid", nullptr));
        PxRigidStaticColor->setText(QString());
        label_3->setText(QApplication::translate("PvdSettingsDialog", "Kinematic", nullptr));
        PxKinematicColor->setText(QString());
        label_4->setText(QApplication::translate("PvdSettingsDialog", "Active dynamic rigid", nullptr));
        PxRigidDynamicActiveColor->setText(QString());
        PxClothColor->setText(QString());
        label_5->setText(QApplication::translate("PvdSettingsDialog", "Cloth", nullptr));
        groupBox->setTitle(QApplication::translate("PvdSettingsDialog", "General Graphics", nullptr));
        cboxAntialiasing->setText(QApplication::translate("PvdSettingsDialog", "Antialiasing", nullptr));
        label_7->setText(QApplication::translate("PvdSettingsDialog", "Geometry scale factor", nullptr));
        groupBox_2->setTitle(QApplication::translate("PvdSettingsDialog", "Profile View", nullptr));
        ProfileFunctionColor->setText(QString());
        label_6->setText(QApplication::translate("PvdSettingsDialog", "Function bar color", nullptr));
        groupBox_3->setTitle(QApplication::translate("PvdSettingsDialog", "Property View", nullptr));
        label_8->setText(QApplication::translate("PvdSettingsDialog", "Float precision", nullptr));
        cboxShowHex->setText(QApplication::translate("PvdSettingsDialog", "Show hex for integer", nullptr));
        label_9->setText(QApplication::translate("PvdSettingsDialog", "Float format", nullptr));
        label_11->setText(QApplication::translate("PvdSettingsDialog", "Array group size", nullptr));
        label_12->setText(QApplication::translate("PvdSettingsDialog", "Max array to load", nullptr));
#ifndef QT_NO_TOOLTIP
        sbGroupSize->setToolTip(QApplication::translate("PvdSettingsDialog", "On expanding array in property view you will see not more then specified number of elements. If array contains more elements. It will be placed to child nodes.", nullptr));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_TOOLTIP
        sbMaxArraySize->setToolTip(QApplication::translate("PvdSettingsDialog", "Array will be loaded to property tree if it contains not more then specified number of elements. Loading too big array may take some time.", nullptr));
#endif // QT_NO_TOOLTIP
        groupBox_4->setTitle(QApplication::translate("PvdSettingsDialog", "Render view", nullptr));
        cboxDrawOrbitCenter->setText(QApplication::translate("PvdSettingsDialog", "Draw camera orbit center", nullptr));
        label_13->setText(QApplication::translate("PvdSettingsDialog", "Draw planes", nullptr));
        label_10->setText(QApplication::translate("PvdSettingsDialog", "Face culling", nullptr));
    } // retranslateUi

};

namespace Ui {
    class PvdSettingsDialog: public Ui_PvdSettingsDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_PVDSETTINGSDIALOG_H
