/********************************************************************************
** Form generated from reading UI file 'finddialog.ui'
**
** Created by: Qt User Interface Compiler version 5.10.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_FINDDIALOG_H
#define UI_FINDDIALOG_H

#include <QtCore/QLocale>
#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QTreeView>

QT_BEGIN_NAMESPACE

class Ui_FindDialog
{
public:
    QGridLayout *gridLayout_2;
    QGridLayout *gridLayout;
    QPushButton *cancelButton;
    QPushButton *findButton;
    QSpacerItem *horizontalSpacer;
    QLineEdit *findEdit;
    QComboBox *findByComboBox;
    QPushButton *selectButton;
    QTreeView *findTreeView;

    void setupUi(QDialog *FindDialog)
    {
        if (FindDialog->objectName().isEmpty())
            FindDialog->setObjectName(QStringLiteral("FindDialog"));
        FindDialog->setWindowModality(Qt::WindowModal);
        FindDialog->resize(400, 300);
        gridLayout_2 = new QGridLayout(FindDialog);
        gridLayout_2->setObjectName(QStringLiteral("gridLayout_2"));
        gridLayout = new QGridLayout();
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        cancelButton = new QPushButton(FindDialog);
        cancelButton->setObjectName(QStringLiteral("cancelButton"));

        gridLayout->addWidget(cancelButton, 2, 3, 1, 1);

        findButton = new QPushButton(FindDialog);
        findButton->setObjectName(QStringLiteral("findButton"));

        gridLayout->addWidget(findButton, 0, 3, 1, 1);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout->addItem(horizontalSpacer, 2, 0, 1, 1);

        findEdit = new QLineEdit(FindDialog);
        findEdit->setObjectName(QStringLiteral("findEdit"));

        gridLayout->addWidget(findEdit, 0, 0, 1, 2);

        findByComboBox = new QComboBox(FindDialog);
        findByComboBox->setObjectName(QStringLiteral("findByComboBox"));
        findByComboBox->setFocusPolicy(Qt::StrongFocus);

        gridLayout->addWidget(findByComboBox, 0, 2, 1, 1);

        selectButton = new QPushButton(FindDialog);
        selectButton->setObjectName(QStringLiteral("selectButton"));
        selectButton->setEnabled(false);

        gridLayout->addWidget(selectButton, 2, 2, 1, 1);

        findTreeView = new QTreeView(FindDialog);
        findTreeView->setObjectName(QStringLiteral("findTreeView"));
        findTreeView->setFocusPolicy(Qt::StrongFocus);
        findTreeView->setAutoFillBackground(true);
        findTreeView->setLocale(QLocale(QLocale::English, QLocale::UnitedStates));
        findTreeView->setTabKeyNavigation(true);
        findTreeView->setIndentation(0);
        findTreeView->setExpandsOnDoubleClick(false);
        findTreeView->header()->setCascadingSectionResizes(false);
        findTreeView->header()->setDefaultSectionSize(75);
        findTreeView->header()->setMinimumSectionSize(50);

        gridLayout->addWidget(findTreeView, 1, 0, 1, 4);


        gridLayout_2->addLayout(gridLayout, 0, 0, 1, 1);

        QWidget::setTabOrder(findEdit, findByComboBox);
        QWidget::setTabOrder(findByComboBox, findButton);
        QWidget::setTabOrder(findButton, findTreeView);
        QWidget::setTabOrder(findTreeView, selectButton);
        QWidget::setTabOrder(selectButton, cancelButton);

        retranslateUi(FindDialog);

        QMetaObject::connectSlotsByName(FindDialog);
    } // setupUi

    void retranslateUi(QDialog *FindDialog)
    {
        FindDialog->setWindowTitle(QApplication::translate("FindDialog", "Find objects", nullptr));
        cancelButton->setText(QApplication::translate("FindDialog", "Cancel", nullptr));
        findButton->setText(QApplication::translate("FindDialog", "Find", nullptr));
        selectButton->setText(QApplication::translate("FindDialog", "Select", nullptr));
    } // retranslateUi

};

namespace Ui {
    class FindDialog: public Ui_FindDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_FINDDIALOG_H
