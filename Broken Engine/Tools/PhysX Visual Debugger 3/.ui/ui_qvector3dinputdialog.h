/********************************************************************************
** Form generated from reading UI file 'qvector3dinputdialog.ui'
**
** Created by: Qt User Interface Compiler version 5.10.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_QVECTOR3DINPUTDIALOG_H
#define UI_QVECTOR3DINPUTDIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_QVector3DInputDialog
{
public:
    QVBoxLayout *verticalLayout;
    QGridLayout *gridLayout;
    QLabel *label;
    QLineEdit *xEdit;
    QLabel *label_2;
    QLineEdit *yEdit;
    QLabel *label_3;
    QLineEdit *zEdit;
    QDialogButtonBox *buttonBox;

    void setupUi(QDialog *QVector3DInputDialog)
    {
        if (QVector3DInputDialog->objectName().isEmpty())
            QVector3DInputDialog->setObjectName(QStringLiteral("QVector3DInputDialog"));
        QVector3DInputDialog->resize(174, 106);
        QSizePolicy sizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(QVector3DInputDialog->sizePolicy().hasHeightForWidth());
        QVector3DInputDialog->setSizePolicy(sizePolicy);
        QVector3DInputDialog->setMaximumSize(QSize(180, 106));
        QVector3DInputDialog->setModal(true);
        verticalLayout = new QVBoxLayout(QVector3DInputDialog);
        verticalLayout->setSpacing(3);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        verticalLayout->setSizeConstraint(QLayout::SetFixedSize);
        verticalLayout->setContentsMargins(3, 3, 3, 3);
        gridLayout = new QGridLayout();
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        label = new QLabel(QVector3DInputDialog);
        label->setObjectName(QStringLiteral("label"));

        gridLayout->addWidget(label, 0, 0, 1, 1);

        xEdit = new QLineEdit(QVector3DInputDialog);
        xEdit->setObjectName(QStringLiteral("xEdit"));
        xEdit->setMaxLength(32767);

        gridLayout->addWidget(xEdit, 0, 1, 1, 1);

        label_2 = new QLabel(QVector3DInputDialog);
        label_2->setObjectName(QStringLiteral("label_2"));

        gridLayout->addWidget(label_2, 1, 0, 1, 1);

        yEdit = new QLineEdit(QVector3DInputDialog);
        yEdit->setObjectName(QStringLiteral("yEdit"));
        yEdit->setMaxLength(32767);

        gridLayout->addWidget(yEdit, 1, 1, 1, 1);

        label_3 = new QLabel(QVector3DInputDialog);
        label_3->setObjectName(QStringLiteral("label_3"));

        gridLayout->addWidget(label_3, 2, 0, 1, 1);

        zEdit = new QLineEdit(QVector3DInputDialog);
        zEdit->setObjectName(QStringLiteral("zEdit"));
        zEdit->setMaxLength(32767);

        gridLayout->addWidget(zEdit, 2, 1, 1, 1);


        verticalLayout->addLayout(gridLayout);

        buttonBox = new QDialogButtonBox(QVector3DInputDialog);
        buttonBox->setObjectName(QStringLiteral("buttonBox"));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);

        verticalLayout->addWidget(buttonBox);


        retranslateUi(QVector3DInputDialog);
        QObject::connect(buttonBox, SIGNAL(accepted()), QVector3DInputDialog, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), QVector3DInputDialog, SLOT(reject()));

        QMetaObject::connectSlotsByName(QVector3DInputDialog);
    } // setupUi

    void retranslateUi(QDialog *QVector3DInputDialog)
    {
        QVector3DInputDialog->setWindowTitle(QApplication::translate("QVector3DInputDialog", "Please specify a point", nullptr));
        label->setText(QApplication::translate("QVector3DInputDialog", "X", nullptr));
        xEdit->setInputMask(QString());
        xEdit->setText(QApplication::translate("QVector3DInputDialog", "0.0", nullptr));
        label_2->setText(QApplication::translate("QVector3DInputDialog", "Y", nullptr));
        yEdit->setInputMask(QString());
        yEdit->setText(QApplication::translate("QVector3DInputDialog", "0.0", nullptr));
        label_3->setText(QApplication::translate("QVector3DInputDialog", "Z", nullptr));
        zEdit->setInputMask(QString());
        zEdit->setText(QApplication::translate("QVector3DInputDialog", "0.0", nullptr));
    } // retranslateUi

};

namespace Ui {
    class QVector3DInputDialog: public Ui_QVector3DInputDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_QVECTOR3DINPUTDIALOG_H
