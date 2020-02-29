/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.10.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QSlider>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QSplitter>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QTableWidget>
#include <QtWidgets/QToolButton>
#include <QtWidgets/QTreeWidget>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>
#include <threadprofiletreewidget.h>
#include "framejumpslider.h"
#include "profiletreewidget.h"

QT_BEGIN_NAMESPACE

class Ui_PVDMainClass
{
public:
    QAction *actionOpen;
    QAction *actionExit;
    QAction *actionAbout;
    QAction *actionGo_to_frame;
    QAction *actionSave;
    QAction *actionFind_Object_by_Id;
    QAction *actionPlot_frame_graph;
    QAction *actionDefaultCamera;
    QAction *actionClip_Camera;
    QAction *actionReset_Camera;
    QAction *actionLook_at_point;
    QAction *actionTrack_selected;
    QAction *actionReport_issue;
    QAction *actionSplit_at_playhead;
    QAction *actionTrim_to_start_from_playhead;
    QAction *actionTrim_to_end_from_playhead;
    QAction *actionExportAllRepX;
    QAction *actionExportSelectedRepX;
    QAction *actionManual;
    QAction *actionSettings;
    QAction *actionZoom_to_scene;
    QAction *actionCenter_at_selected;
    QAction *actionZoom_to_selected;
    QAction *actionFind_Objects;
    QAction *actionRestore_default_visual_settings;
    QAction *actionFind_profile_events;
    QAction *actionExportVisibleRepX;
    QWidget *centralWidget;
    QVBoxLayout *verticalLayout;
    QHBoxLayout *horizontalLayout;
    QToolButton *rewindToolButton;
    QToolButton *previousFrameToolButton;
    QToolButton *playBackwardsToolButton;
    QToolButton *playToolButton;
    QToolButton *nextFrameToolButton;
    QToolButton *rewindForwardToolButton;
    QToolButton *allowConnectionToolButton;
    QToolButton *disconnectToolButton;
    QLabel *frameLabel;
    FrameJumpSlider *FramehorizontalSlider;
    QLineEdit *FramelcdNumber;
    QSplitter *horizontalSplitter;
    QSplitter *splitter_2;
    QComboBox *scenesComboBox;
    QTreeWidget *inspectorTreeWidget;
    QTreeWidget *inspectorPropertyTreeWidget;
    QSplitter *splitter;
    QTabWidget *tabWidget;
    QWidget *renderTab;
    QHBoxLayout *horizontalLayout_3;
    QVBoxLayout *renderLayout;
    QWidget *profileTab;
    QHBoxLayout *horizontalLayout_4;
    ProfileTreeWidget *profileTreeWidget;
    QWidget *memoryTab;
    QVBoxLayout *verticalLayout_7;
    QTableWidget *memoryTableWidget;
    QWidget *errorStreamTab;
    QVBoxLayout *verticalLayout_8;
    QTableWidget *errorTableWidget;
    QWidget *threadProfileTab;
    QVBoxLayout *verticalLayout_6;
    ThreadProfileTreeWidget *threadProfileTreeWidget;
    QTableWidget *clipsTableWidget;
    QTabWidget *rightTabWidget;
    QWidget *settingsTab;
    QVBoxLayout *verticalLayout_2;
    QGroupBox *visualizationGroupBox;
    QVBoxLayout *verticalLayout_3;
    QHBoxLayout *horizontalLayout_2;
    QVBoxLayout *verticalLayout_4;
    QLabel *gizmoScaleLabel_2;
    QSlider *nearPlaneSlider;
    QVBoxLayout *verticalLayout_5;
    QLabel *cameraSpeedLabel;
    QHBoxLayout *horizontalLayout_5;
    QSlider *cameraSpeedScaleHorizontalSlider;
    QGridLayout *gridLayout;
    QLabel *label_6;
    QComboBox *dbgvizContactComboBox;
    QLabel *label_9;
    QComboBox *dbgvizBBComboBox;
    QLabel *label_7;
    QLabel *label;
    QLabel *label_8;
    QComboBox *dbgvizSQComboBox;
    QLabel *label_3;
    QComboBox *dbgvizTransComboBox;
    QComboBox *dbgvizVelComboBox;
    QComboBox *dbgvizJointsComboBox;
    QComboBox *dbgvizCSComboBox;
    QLabel *label_2;
    QComboBox *dbgvizWireframeComboBox;
    QLabel *label_4;
    QLabel *cameraModeLabel;
    QComboBox *cameraModeComboBox;
    QComboBox *dbgvizSleepingObjectsComboBox;
    QLabel *label_14;
    QLabel *label_15;
    QComboBox *dbgvizTriggerShapesComboBox;
    QLabel *label_13;
    QSlider *transparencyHorizontalSlider;
    QLabel *gizmoScaleLabel;
    QSlider *gizmoScaleHorizontalSlider;
    QLabel *label_10;
    QSlider *jointsScaleHorizontalSlider;
    QLabel *label_12;
    QSlider *particleSizeHorizontalSlider;
    QGridLayout *gridLayout_2;
    QLabel *label_5;
    QComboBox *upAxisComboBox;
    QLabel *label_11;
    QComboBox *coordinateSystemComboBox;
    QSpacerItem *verticalSpacer;
    QMenuBar *menuBar;
    QMenu *menuFile;
    QMenu *menuHelp;
    QMenu *menuCommands;
    QMenu *menuCamera;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *PVDMainClass)
    {
        if (PVDMainClass->objectName().isEmpty())
            PVDMainClass->setObjectName(QStringLiteral("PVDMainClass"));
        PVDMainClass->resize(1386, 889);
        actionOpen = new QAction(PVDMainClass);
        actionOpen->setObjectName(QStringLiteral("actionOpen"));
        actionOpen->setMenuRole(QAction::NoRole);
        actionExit = new QAction(PVDMainClass);
        actionExit->setObjectName(QStringLiteral("actionExit"));
        actionExit->setMenuRole(QAction::QuitRole);
        actionAbout = new QAction(PVDMainClass);
        actionAbout->setObjectName(QStringLiteral("actionAbout"));
        actionAbout->setMenuRole(QAction::NoRole);
        actionGo_to_frame = new QAction(PVDMainClass);
        actionGo_to_frame->setObjectName(QStringLiteral("actionGo_to_frame"));
        actionGo_to_frame->setEnabled(false);
        actionGo_to_frame->setMenuRole(QAction::NoRole);
        actionSave = new QAction(PVDMainClass);
        actionSave->setObjectName(QStringLiteral("actionSave"));
        actionSave->setEnabled(false);
        actionSave->setMenuRole(QAction::NoRole);
        actionFind_Object_by_Id = new QAction(PVDMainClass);
        actionFind_Object_by_Id->setObjectName(QStringLiteral("actionFind_Object_by_Id"));
        actionFind_Object_by_Id->setEnabled(false);
        actionPlot_frame_graph = new QAction(PVDMainClass);
        actionPlot_frame_graph->setObjectName(QStringLiteral("actionPlot_frame_graph"));
        actionPlot_frame_graph->setEnabled(false);
        actionDefaultCamera = new QAction(PVDMainClass);
        actionDefaultCamera->setObjectName(QStringLiteral("actionDefaultCamera"));
        actionDefaultCamera->setCheckable(true);
        actionDefaultCamera->setChecked(true);
        actionDefaultCamera->setEnabled(false);
        actionClip_Camera = new QAction(PVDMainClass);
        actionClip_Camera->setObjectName(QStringLiteral("actionClip_Camera"));
        actionClip_Camera->setCheckable(true);
        actionClip_Camera->setEnabled(false);
        actionReset_Camera = new QAction(PVDMainClass);
        actionReset_Camera->setObjectName(QStringLiteral("actionReset_Camera"));
        actionReset_Camera->setEnabled(false);
        actionLook_at_point = new QAction(PVDMainClass);
        actionLook_at_point->setObjectName(QStringLiteral("actionLook_at_point"));
        actionLook_at_point->setEnabled(false);
        actionTrack_selected = new QAction(PVDMainClass);
        actionTrack_selected->setObjectName(QStringLiteral("actionTrack_selected"));
        actionTrack_selected->setCheckable(true);
        actionTrack_selected->setEnabled(false);
        actionReport_issue = new QAction(PVDMainClass);
        actionReport_issue->setObjectName(QStringLiteral("actionReport_issue"));
        actionSplit_at_playhead = new QAction(PVDMainClass);
        actionSplit_at_playhead->setObjectName(QStringLiteral("actionSplit_at_playhead"));
        actionTrim_to_start_from_playhead = new QAction(PVDMainClass);
        actionTrim_to_start_from_playhead->setObjectName(QStringLiteral("actionTrim_to_start_from_playhead"));
        actionTrim_to_end_from_playhead = new QAction(PVDMainClass);
        actionTrim_to_end_from_playhead->setObjectName(QStringLiteral("actionTrim_to_end_from_playhead"));
        actionExportAllRepX = new QAction(PVDMainClass);
        actionExportAllRepX->setObjectName(QStringLiteral("actionExportAllRepX"));
        actionExportAllRepX->setEnabled(false);
        actionExportSelectedRepX = new QAction(PVDMainClass);
        actionExportSelectedRepX->setObjectName(QStringLiteral("actionExportSelectedRepX"));
        actionExportSelectedRepX->setEnabled(false);
        actionManual = new QAction(PVDMainClass);
        actionManual->setObjectName(QStringLiteral("actionManual"));
        actionManual->setMenuRole(QAction::NoRole);
        actionSettings = new QAction(PVDMainClass);
        actionSettings->setObjectName(QStringLiteral("actionSettings"));
        actionZoom_to_scene = new QAction(PVDMainClass);
        actionZoom_to_scene->setObjectName(QStringLiteral("actionZoom_to_scene"));
        actionZoom_to_scene->setEnabled(false);
        actionCenter_at_selected = new QAction(PVDMainClass);
        actionCenter_at_selected->setObjectName(QStringLiteral("actionCenter_at_selected"));
        actionCenter_at_selected->setCheckable(false);
        actionCenter_at_selected->setEnabled(false);
        actionZoom_to_selected = new QAction(PVDMainClass);
        actionZoom_to_selected->setObjectName(QStringLiteral("actionZoom_to_selected"));
        actionZoom_to_selected->setEnabled(false);
        actionFind_Objects = new QAction(PVDMainClass);
        actionFind_Objects->setObjectName(QStringLiteral("actionFind_Objects"));
        actionFind_Objects->setEnabled(false);
        actionRestore_default_visual_settings = new QAction(PVDMainClass);
        actionRestore_default_visual_settings->setObjectName(QStringLiteral("actionRestore_default_visual_settings"));
        actionFind_profile_events = new QAction(PVDMainClass);
        actionFind_profile_events->setObjectName(QStringLiteral("actionFind_profile_events"));
        actionFind_profile_events->setEnabled(false);
        actionExportVisibleRepX = new QAction(PVDMainClass);
        actionExportVisibleRepX->setObjectName(QStringLiteral("actionExportVisibleRepX"));
        actionExportVisibleRepX->setEnabled(false);
        centralWidget = new QWidget(PVDMainClass);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
        verticalLayout = new QVBoxLayout(centralWidget);
        verticalLayout->setSpacing(3);
        verticalLayout->setContentsMargins(11, 11, 11, 11);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        verticalLayout->setContentsMargins(3, 3, 3, 3);
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setSpacing(6);
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        rewindToolButton = new QToolButton(centralWidget);
        rewindToolButton->setObjectName(QStringLiteral("rewindToolButton"));
        rewindToolButton->setEnabled(false);
        QIcon icon;
        icon.addFile(QStringLiteral(":/icons/transportRewind.png"), QSize(), QIcon::Normal, QIcon::Off);
        rewindToolButton->setIcon(icon);

        horizontalLayout->addWidget(rewindToolButton);

        previousFrameToolButton = new QToolButton(centralWidget);
        previousFrameToolButton->setObjectName(QStringLiteral("previousFrameToolButton"));
        previousFrameToolButton->setEnabled(false);
        QIcon icon1;
        icon1.addFile(QStringLiteral(":/icons/transportStepBack.png"), QSize(), QIcon::Normal, QIcon::Off);
        previousFrameToolButton->setIcon(icon1);

        horizontalLayout->addWidget(previousFrameToolButton);

        playBackwardsToolButton = new QToolButton(centralWidget);
        playBackwardsToolButton->setObjectName(QStringLiteral("playBackwardsToolButton"));
        playBackwardsToolButton->setEnabled(false);
        QIcon icon2;
        icon2.addFile(QStringLiteral(":/icons/transportPlayBackwards.png"), QSize(), QIcon::Normal, QIcon::Off);
        playBackwardsToolButton->setIcon(icon2);

        horizontalLayout->addWidget(playBackwardsToolButton);

        playToolButton = new QToolButton(centralWidget);
        playToolButton->setObjectName(QStringLiteral("playToolButton"));
        playToolButton->setEnabled(false);
        QIcon icon3;
        icon3.addFile(QStringLiteral(":/icons/transportPlay.png"), QSize(), QIcon::Normal, QIcon::Off);
        playToolButton->setIcon(icon3);

        horizontalLayout->addWidget(playToolButton);

        nextFrameToolButton = new QToolButton(centralWidget);
        nextFrameToolButton->setObjectName(QStringLiteral("nextFrameToolButton"));
        nextFrameToolButton->setEnabled(false);
        QIcon icon4;
        icon4.addFile(QStringLiteral(":/icons/transportStepForward.png"), QSize(), QIcon::Normal, QIcon::Off);
        nextFrameToolButton->setIcon(icon4);

        horizontalLayout->addWidget(nextFrameToolButton);

        rewindForwardToolButton = new QToolButton(centralWidget);
        rewindForwardToolButton->setObjectName(QStringLiteral("rewindForwardToolButton"));
        rewindForwardToolButton->setEnabled(false);
        QIcon icon5;
        icon5.addFile(QStringLiteral(":/icons/transportRewindForward.png"), QSize(), QIcon::Normal, QIcon::Off);
        rewindForwardToolButton->setIcon(icon5);

        horizontalLayout->addWidget(rewindForwardToolButton);

        allowConnectionToolButton = new QToolButton(centralWidget);
        allowConnectionToolButton->setObjectName(QStringLiteral("allowConnectionToolButton"));
        QIcon icon6;
        icon6.addFile(QStringLiteral(":/icons/allowConnection.png"), QSize(), QIcon::Normal, QIcon::Off);
        allowConnectionToolButton->setIcon(icon6);

        horizontalLayout->addWidget(allowConnectionToolButton);

        disconnectToolButton = new QToolButton(centralWidget);
        disconnectToolButton->setObjectName(QStringLiteral("disconnectToolButton"));
        disconnectToolButton->setEnabled(false);
        QIcon icon7;
        icon7.addFile(QStringLiteral(":/icons/disconnect.png"), QSize(), QIcon::Normal, QIcon::Off);
        disconnectToolButton->setIcon(icon7);

        horizontalLayout->addWidget(disconnectToolButton);

        frameLabel = new QLabel(centralWidget);
        frameLabel->setObjectName(QStringLiteral("frameLabel"));
        QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(frameLabel->sizePolicy().hasHeightForWidth());
        frameLabel->setSizePolicy(sizePolicy);
        frameLabel->setMaximumSize(QSize(16777215, 20));

        horizontalLayout->addWidget(frameLabel);

        FramehorizontalSlider = new FrameJumpSlider(centralWidget);
        FramehorizontalSlider->setObjectName(QStringLiteral("FramehorizontalSlider"));
        FramehorizontalSlider->setEnabled(false);
        QSizePolicy sizePolicy1(QSizePolicy::Expanding, QSizePolicy::Preferred);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(FramehorizontalSlider->sizePolicy().hasHeightForWidth());
        FramehorizontalSlider->setSizePolicy(sizePolicy1);
        FramehorizontalSlider->setFocusPolicy(Qt::NoFocus);
        FramehorizontalSlider->setOrientation(Qt::Horizontal);

        horizontalLayout->addWidget(FramehorizontalSlider);

        FramelcdNumber = new QLineEdit(centralWidget);
        FramelcdNumber->setObjectName(QStringLiteral("FramelcdNumber"));
        QSizePolicy sizePolicy2(QSizePolicy::Minimum, QSizePolicy::Fixed);
        sizePolicy2.setHorizontalStretch(0);
        sizePolicy2.setVerticalStretch(0);
        sizePolicy2.setHeightForWidth(FramelcdNumber->sizePolicy().hasHeightForWidth());
        FramelcdNumber->setSizePolicy(sizePolicy2);
        FramelcdNumber->setMaximumSize(QSize(100, 16777215));
        QFont font;
        font.setPointSize(13);
        font.setBold(true);
        font.setWeight(75);
        FramelcdNumber->setFont(font);
        FramelcdNumber->setFocusPolicy(Qt::NoFocus);
        FramelcdNumber->setStyleSheet(QStringLiteral("background:#EEE;"));
        FramelcdNumber->setMaxLength(16);
        FramelcdNumber->setFrame(false);
        FramelcdNumber->setAlignment(Qt::AlignCenter);
        FramelcdNumber->setReadOnly(true);

        horizontalLayout->addWidget(FramelcdNumber);


        verticalLayout->addLayout(horizontalLayout);

        horizontalSplitter = new QSplitter(centralWidget);
        horizontalSplitter->setObjectName(QStringLiteral("horizontalSplitter"));
        horizontalSplitter->setOrientation(Qt::Horizontal);
        splitter_2 = new QSplitter(horizontalSplitter);
        splitter_2->setObjectName(QStringLiteral("splitter_2"));
        splitter_2->setOrientation(Qt::Vertical);
        scenesComboBox = new QComboBox(splitter_2);
        scenesComboBox->setObjectName(QStringLiteral("scenesComboBox"));
        scenesComboBox->setEnabled(false);
        QSizePolicy sizePolicy3(QSizePolicy::Preferred, QSizePolicy::Minimum);
        sizePolicy3.setHorizontalStretch(0);
        sizePolicy3.setVerticalStretch(0);
        sizePolicy3.setHeightForWidth(scenesComboBox->sizePolicy().hasHeightForWidth());
        scenesComboBox->setSizePolicy(sizePolicy3);
        scenesComboBox->setMinimumSize(QSize(0, 32));
        scenesComboBox->setMaximumSize(QSize(16777215, 32));
        splitter_2->addWidget(scenesComboBox);
        inspectorTreeWidget = new QTreeWidget(splitter_2);
        inspectorTreeWidget->setObjectName(QStringLiteral("inspectorTreeWidget"));
        QSizePolicy sizePolicy4(QSizePolicy::Minimum, QSizePolicy::Preferred);
        sizePolicy4.setHorizontalStretch(0);
        sizePolicy4.setVerticalStretch(0);
        sizePolicy4.setHeightForWidth(inspectorTreeWidget->sizePolicy().hasHeightForWidth());
        inspectorTreeWidget->setSizePolicy(sizePolicy4);
        inspectorTreeWidget->setFocusPolicy(Qt::WheelFocus);
        inspectorTreeWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
        splitter_2->addWidget(inspectorTreeWidget);
        inspectorTreeWidget->header()->setVisible(false);
        inspectorPropertyTreeWidget = new QTreeWidget(splitter_2);
        inspectorPropertyTreeWidget->setObjectName(QStringLiteral("inspectorPropertyTreeWidget"));
        sizePolicy4.setHeightForWidth(inspectorPropertyTreeWidget->sizePolicy().hasHeightForWidth());
        inspectorPropertyTreeWidget->setSizePolicy(sizePolicy4);
        inspectorPropertyTreeWidget->setFocusPolicy(Qt::WheelFocus);
        inspectorPropertyTreeWidget->setContextMenuPolicy(Qt::DefaultContextMenu);
        inspectorPropertyTreeWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
        inspectorPropertyTreeWidget->setProperty("showDropIndicator", QVariant(false));
        inspectorPropertyTreeWidget->setAlternatingRowColors(true);
        inspectorPropertyTreeWidget->setUniformRowHeights(true);
        inspectorPropertyTreeWidget->setColumnCount(2);
        splitter_2->addWidget(inspectorPropertyTreeWidget);
        horizontalSplitter->addWidget(splitter_2);
        splitter = new QSplitter(horizontalSplitter);
        splitter->setObjectName(QStringLiteral("splitter"));
        splitter->setOrientation(Qt::Vertical);
        splitter->setOpaqueResize(false);
        tabWidget = new QTabWidget(splitter);
        tabWidget->setObjectName(QStringLiteral("tabWidget"));
        tabWidget->setEnabled(true);
        QSizePolicy sizePolicy5(QSizePolicy::Expanding, QSizePolicy::Expanding);
        sizePolicy5.setHorizontalStretch(0);
        sizePolicy5.setVerticalStretch(0);
        sizePolicy5.setHeightForWidth(tabWidget->sizePolicy().hasHeightForWidth());
        tabWidget->setSizePolicy(sizePolicy5);
        tabWidget->setTabsClosable(true);
        tabWidget->setMovable(false);
        renderTab = new QWidget();
        renderTab->setObjectName(QStringLiteral("renderTab"));
        horizontalLayout_3 = new QHBoxLayout(renderTab);
        horizontalLayout_3->setSpacing(3);
        horizontalLayout_3->setContentsMargins(11, 11, 11, 11);
        horizontalLayout_3->setObjectName(QStringLiteral("horizontalLayout_3"));
        horizontalLayout_3->setContentsMargins(1, 1, 1, 1);
        renderLayout = new QVBoxLayout();
        renderLayout->setSpacing(0);
        renderLayout->setObjectName(QStringLiteral("renderLayout"));
        renderLayout->setSizeConstraint(QLayout::SetDefaultConstraint);

        horizontalLayout_3->addLayout(renderLayout);

        tabWidget->addTab(renderTab, QString());
        profileTab = new QWidget();
        profileTab->setObjectName(QStringLiteral("profileTab"));
        profileTab->setEnabled(true);
        horizontalLayout_4 = new QHBoxLayout(profileTab);
        horizontalLayout_4->setSpacing(3);
        horizontalLayout_4->setContentsMargins(11, 11, 11, 11);
        horizontalLayout_4->setObjectName(QStringLiteral("horizontalLayout_4"));
        horizontalLayout_4->setContentsMargins(3, 3, 3, 3);
        profileTreeWidget = new ProfileTreeWidget(profileTab);
        profileTreeWidget->headerItem()->setText(2, QString());
        profileTreeWidget->setObjectName(QStringLiteral("profileTreeWidget"));

        horizontalLayout_4->addWidget(profileTreeWidget);

        tabWidget->addTab(profileTab, QString());
        memoryTab = new QWidget();
        memoryTab->setObjectName(QStringLiteral("memoryTab"));
        memoryTab->setEnabled(true);
        verticalLayout_7 = new QVBoxLayout(memoryTab);
        verticalLayout_7->setSpacing(3);
        verticalLayout_7->setContentsMargins(11, 11, 11, 11);
        verticalLayout_7->setObjectName(QStringLiteral("verticalLayout_7"));
        verticalLayout_7->setContentsMargins(3, 3, 3, 3);
        memoryTableWidget = new QTableWidget(memoryTab);
        if (memoryTableWidget->columnCount() < 5)
            memoryTableWidget->setColumnCount(5);
        QTableWidgetItem *__qtablewidgetitem = new QTableWidgetItem();
        memoryTableWidget->setHorizontalHeaderItem(0, __qtablewidgetitem);
        QTableWidgetItem *__qtablewidgetitem1 = new QTableWidgetItem();
        memoryTableWidget->setHorizontalHeaderItem(1, __qtablewidgetitem1);
        QTableWidgetItem *__qtablewidgetitem2 = new QTableWidgetItem();
        memoryTableWidget->setHorizontalHeaderItem(2, __qtablewidgetitem2);
        QTableWidgetItem *__qtablewidgetitem3 = new QTableWidgetItem();
        memoryTableWidget->setHorizontalHeaderItem(3, __qtablewidgetitem3);
        QTableWidgetItem *__qtablewidgetitem4 = new QTableWidgetItem();
        memoryTableWidget->setHorizontalHeaderItem(4, __qtablewidgetitem4);
        memoryTableWidget->setObjectName(QStringLiteral("memoryTableWidget"));
        memoryTableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
        memoryTableWidget->setAlternatingRowColors(true);
        memoryTableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
        memoryTableWidget->setGridStyle(Qt::DashLine);
        memoryTableWidget->setSortingEnabled(true);
        memoryTableWidget->setWordWrap(false);
        memoryTableWidget->setColumnCount(5);
        memoryTableWidget->horizontalHeader()->setCascadingSectionResizes(true);
        memoryTableWidget->horizontalHeader()->setDefaultSectionSize(50);
        memoryTableWidget->horizontalHeader()->setStretchLastSection(true);

        verticalLayout_7->addWidget(memoryTableWidget);

        tabWidget->addTab(memoryTab, QString());
        errorStreamTab = new QWidget();
        errorStreamTab->setObjectName(QStringLiteral("errorStreamTab"));
        errorStreamTab->setEnabled(true);
        verticalLayout_8 = new QVBoxLayout(errorStreamTab);
        verticalLayout_8->setSpacing(3);
        verticalLayout_8->setContentsMargins(11, 11, 11, 11);
        verticalLayout_8->setObjectName(QStringLiteral("verticalLayout_8"));
        verticalLayout_8->setContentsMargins(3, 3, 3, 3);
        errorTableWidget = new QTableWidget(errorStreamTab);
        if (errorTableWidget->columnCount() < 3)
            errorTableWidget->setColumnCount(3);
        QTableWidgetItem *__qtablewidgetitem5 = new QTableWidgetItem();
        errorTableWidget->setHorizontalHeaderItem(0, __qtablewidgetitem5);
        QTableWidgetItem *__qtablewidgetitem6 = new QTableWidgetItem();
        errorTableWidget->setHorizontalHeaderItem(1, __qtablewidgetitem6);
        QTableWidgetItem *__qtablewidgetitem7 = new QTableWidgetItem();
        errorTableWidget->setHorizontalHeaderItem(2, __qtablewidgetitem7);
        errorTableWidget->setObjectName(QStringLiteral("errorTableWidget"));
        errorTableWidget->setProperty("showDropIndicator", QVariant(false));
        errorTableWidget->setAlternatingRowColors(true);
        errorTableWidget->setGridStyle(Qt::DashLine);
        errorTableWidget->horizontalHeader()->setStretchLastSection(true);

        verticalLayout_8->addWidget(errorTableWidget);

        tabWidget->addTab(errorStreamTab, QString());
        threadProfileTab = new QWidget();
        threadProfileTab->setObjectName(QStringLiteral("threadProfileTab"));
        verticalLayout_6 = new QVBoxLayout(threadProfileTab);
        verticalLayout_6->setSpacing(6);
        verticalLayout_6->setContentsMargins(11, 11, 11, 11);
        verticalLayout_6->setObjectName(QStringLiteral("verticalLayout_6"));
        threadProfileTreeWidget = new ThreadProfileTreeWidget(threadProfileTab);
        threadProfileTreeWidget->headerItem()->setText(1, QString());
        threadProfileTreeWidget->setObjectName(QStringLiteral("threadProfileTreeWidget"));
        threadProfileTreeWidget->setAutoScroll(false);
        threadProfileTreeWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);

        verticalLayout_6->addWidget(threadProfileTreeWidget);

        tabWidget->addTab(threadProfileTab, QString());
        splitter->addWidget(tabWidget);
        clipsTableWidget = new QTableWidget(splitter);
        if (clipsTableWidget->columnCount() < 4)
            clipsTableWidget->setColumnCount(4);
        QTableWidgetItem *__qtablewidgetitem8 = new QTableWidgetItem();
        clipsTableWidget->setHorizontalHeaderItem(0, __qtablewidgetitem8);
        QTableWidgetItem *__qtablewidgetitem9 = new QTableWidgetItem();
        clipsTableWidget->setHorizontalHeaderItem(1, __qtablewidgetitem9);
        QTableWidgetItem *__qtablewidgetitem10 = new QTableWidgetItem();
        clipsTableWidget->setHorizontalHeaderItem(2, __qtablewidgetitem10);
        QTableWidgetItem *__qtablewidgetitem11 = new QTableWidgetItem();
        clipsTableWidget->setHorizontalHeaderItem(3, __qtablewidgetitem11);
        clipsTableWidget->setObjectName(QStringLiteral("clipsTableWidget"));
        sizePolicy4.setHeightForWidth(clipsTableWidget->sizePolicy().hasHeightForWidth());
        clipsTableWidget->setSizePolicy(sizePolicy4);
        clipsTableWidget->setFocusPolicy(Qt::NoFocus);
        clipsTableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
        clipsTableWidget->setTabKeyNavigation(false);
        clipsTableWidget->setProperty("showDropIndicator", QVariant(false));
        clipsTableWidget->setDragDropOverwriteMode(false);
        clipsTableWidget->setAlternatingRowColors(false);
        clipsTableWidget->setSelectionMode(QAbstractItemView::SingleSelection);
        clipsTableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
        splitter->addWidget(clipsTableWidget);
        clipsTableWidget->horizontalHeader()->setHighlightSections(false);
        clipsTableWidget->horizontalHeader()->setStretchLastSection(true);
        clipsTableWidget->verticalHeader()->setVisible(false);
        clipsTableWidget->verticalHeader()->setStretchLastSection(false);
        horizontalSplitter->addWidget(splitter);
        rightTabWidget = new QTabWidget(horizontalSplitter);
        rightTabWidget->setObjectName(QStringLiteral("rightTabWidget"));
        rightTabWidget->setMinimumSize(QSize(0, 0));
        rightTabWidget->setMaximumSize(QSize(256, 16777215));
        rightTabWidget->setTabPosition(QTabWidget::North);
        rightTabWidget->setTabShape(QTabWidget::Rounded);
        rightTabWidget->setElideMode(Qt::ElideLeft);
        rightTabWidget->setUsesScrollButtons(true);
        rightTabWidget->setDocumentMode(false);
        rightTabWidget->setTabBarAutoHide(false);
        settingsTab = new QWidget();
        settingsTab->setObjectName(QStringLiteral("settingsTab"));
        verticalLayout_2 = new QVBoxLayout(settingsTab);
        verticalLayout_2->setSpacing(6);
        verticalLayout_2->setContentsMargins(11, 11, 11, 11);
        verticalLayout_2->setObjectName(QStringLiteral("verticalLayout_2"));
        visualizationGroupBox = new QGroupBox(settingsTab);
        visualizationGroupBox->setObjectName(QStringLiteral("visualizationGroupBox"));
        QSizePolicy sizePolicy6(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy6.setHorizontalStretch(0);
        sizePolicy6.setVerticalStretch(0);
        sizePolicy6.setHeightForWidth(visualizationGroupBox->sizePolicy().hasHeightForWidth());
        visualizationGroupBox->setSizePolicy(sizePolicy6);
        visualizationGroupBox->setFlat(false);
        visualizationGroupBox->setCheckable(false);
        verticalLayout_3 = new QVBoxLayout(visualizationGroupBox);
        verticalLayout_3->setSpacing(3);
        verticalLayout_3->setContentsMargins(11, 11, 11, 11);
        verticalLayout_3->setObjectName(QStringLiteral("verticalLayout_3"));
        verticalLayout_3->setContentsMargins(3, 3, 3, 3);
        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setSpacing(3);
        horizontalLayout_2->setObjectName(QStringLiteral("horizontalLayout_2"));
        verticalLayout_4 = new QVBoxLayout();
        verticalLayout_4->setSpacing(6);
        verticalLayout_4->setObjectName(QStringLiteral("verticalLayout_4"));
        gizmoScaleLabel_2 = new QLabel(visualizationGroupBox);
        gizmoScaleLabel_2->setObjectName(QStringLiteral("gizmoScaleLabel_2"));

        verticalLayout_4->addWidget(gizmoScaleLabel_2);

        nearPlaneSlider = new QSlider(visualizationGroupBox);
        nearPlaneSlider->setObjectName(QStringLiteral("nearPlaneSlider"));
        nearPlaneSlider->setFocusPolicy(Qt::NoFocus);
        nearPlaneSlider->setMinimum(1);
        nearPlaneSlider->setMaximum(100);
        nearPlaneSlider->setPageStep(1);
        nearPlaneSlider->setValue(5);
        nearPlaneSlider->setOrientation(Qt::Horizontal);

        verticalLayout_4->addWidget(nearPlaneSlider);


        horizontalLayout_2->addLayout(verticalLayout_4);

        verticalLayout_5 = new QVBoxLayout();
        verticalLayout_5->setSpacing(6);
        verticalLayout_5->setObjectName(QStringLiteral("verticalLayout_5"));

        horizontalLayout_2->addLayout(verticalLayout_5);


        verticalLayout_3->addLayout(horizontalLayout_2);

        cameraSpeedLabel = new QLabel(visualizationGroupBox);
        cameraSpeedLabel->setObjectName(QStringLiteral("cameraSpeedLabel"));
        sizePolicy6.setHeightForWidth(cameraSpeedLabel->sizePolicy().hasHeightForWidth());
        cameraSpeedLabel->setSizePolicy(sizePolicy6);

        verticalLayout_3->addWidget(cameraSpeedLabel);

        horizontalLayout_5 = new QHBoxLayout();
        horizontalLayout_5->setSpacing(6);
        horizontalLayout_5->setObjectName(QStringLiteral("horizontalLayout_5"));
        cameraSpeedScaleHorizontalSlider = new QSlider(visualizationGroupBox);
        cameraSpeedScaleHorizontalSlider->setObjectName(QStringLiteral("cameraSpeedScaleHorizontalSlider"));
        QSizePolicy sizePolicy7(QSizePolicy::Expanding, QSizePolicy::Fixed);
        sizePolicy7.setHorizontalStretch(0);
        sizePolicy7.setVerticalStretch(0);
        sizePolicy7.setHeightForWidth(cameraSpeedScaleHorizontalSlider->sizePolicy().hasHeightForWidth());
        cameraSpeedScaleHorizontalSlider->setSizePolicy(sizePolicy7);
        cameraSpeedScaleHorizontalSlider->setFocusPolicy(Qt::NoFocus);
        cameraSpeedScaleHorizontalSlider->setMinimum(1);
        cameraSpeedScaleHorizontalSlider->setMaximum(100);
        cameraSpeedScaleHorizontalSlider->setValue(5);
        cameraSpeedScaleHorizontalSlider->setOrientation(Qt::Horizontal);

        horizontalLayout_5->addWidget(cameraSpeedScaleHorizontalSlider);


        verticalLayout_3->addLayout(horizontalLayout_5);

        gridLayout = new QGridLayout();
        gridLayout->setSpacing(3);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        label_6 = new QLabel(visualizationGroupBox);
        label_6->setObjectName(QStringLiteral("label_6"));

        gridLayout->addWidget(label_6, 5, 0, 1, 1);

        dbgvizContactComboBox = new QComboBox(visualizationGroupBox);
        dbgvizContactComboBox->addItem(QString());
        dbgvizContactComboBox->addItem(QString());
        dbgvizContactComboBox->setObjectName(QStringLiteral("dbgvizContactComboBox"));
        dbgvizContactComboBox->setFocusPolicy(Qt::NoFocus);

        gridLayout->addWidget(dbgvizContactComboBox, 8, 1, 1, 1);

        label_9 = new QLabel(visualizationGroupBox);
        label_9->setObjectName(QStringLiteral("label_9"));

        gridLayout->addWidget(label_9, 8, 0, 1, 1);

        dbgvizBBComboBox = new QComboBox(visualizationGroupBox);
        dbgvizBBComboBox->addItem(QString());
        dbgvizBBComboBox->addItem(QString());
        dbgvizBBComboBox->addItem(QString());
        dbgvizBBComboBox->addItem(QString());
        dbgvizBBComboBox->setObjectName(QStringLiteral("dbgvizBBComboBox"));
        dbgvizBBComboBox->setFocusPolicy(Qt::NoFocus);
        dbgvizBBComboBox->setMaxVisibleItems(5);
        dbgvizBBComboBox->setInsertPolicy(QComboBox::NoInsert);

        gridLayout->addWidget(dbgvizBBComboBox, 1, 1, 1, 1);

        label_7 = new QLabel(visualizationGroupBox);
        label_7->setObjectName(QStringLiteral("label_7"));

        gridLayout->addWidget(label_7, 6, 0, 1, 1);

        label = new QLabel(visualizationGroupBox);
        label->setObjectName(QStringLiteral("label"));

        gridLayout->addWidget(label, 1, 0, 1, 1);

        label_8 = new QLabel(visualizationGroupBox);
        label_8->setObjectName(QStringLiteral("label_8"));

        gridLayout->addWidget(label_8, 7, 0, 1, 1);

        dbgvizSQComboBox = new QComboBox(visualizationGroupBox);
        dbgvizSQComboBox->addItem(QString());
        dbgvizSQComboBox->addItem(QString());
        dbgvizSQComboBox->addItem(QString());
        dbgvizSQComboBox->addItem(QString());
        dbgvizSQComboBox->setObjectName(QStringLiteral("dbgvizSQComboBox"));
        dbgvizSQComboBox->setFocusPolicy(Qt::NoFocus);

        gridLayout->addWidget(dbgvizSQComboBox, 7, 1, 1, 1);

        label_3 = new QLabel(visualizationGroupBox);
        label_3->setObjectName(QStringLiteral("label_3"));

        gridLayout->addWidget(label_3, 3, 0, 1, 1);

        dbgvizTransComboBox = new QComboBox(visualizationGroupBox);
        dbgvizTransComboBox->addItem(QString());
        dbgvizTransComboBox->addItem(QString());
        dbgvizTransComboBox->addItem(QString());
        dbgvizTransComboBox->addItem(QString());
        dbgvizTransComboBox->setObjectName(QStringLiteral("dbgvizTransComboBox"));
        dbgvizTransComboBox->setFocusPolicy(Qt::NoFocus);
        dbgvizTransComboBox->setMaxVisibleItems(5);
        dbgvizTransComboBox->setInsertPolicy(QComboBox::NoInsert);

        gridLayout->addWidget(dbgvizTransComboBox, 4, 1, 1, 1);

        dbgvizVelComboBox = new QComboBox(visualizationGroupBox);
        dbgvizVelComboBox->addItem(QString());
        dbgvizVelComboBox->addItem(QString());
        dbgvizVelComboBox->addItem(QString());
        dbgvizVelComboBox->addItem(QString());
        dbgvizVelComboBox->setObjectName(QStringLiteral("dbgvizVelComboBox"));
        dbgvizVelComboBox->setFocusPolicy(Qt::NoFocus);
        dbgvizVelComboBox->setMaxVisibleItems(5);
        dbgvizVelComboBox->setInsertPolicy(QComboBox::NoInsert);

        gridLayout->addWidget(dbgvizVelComboBox, 3, 1, 1, 1);

        dbgvizJointsComboBox = new QComboBox(visualizationGroupBox);
        dbgvizJointsComboBox->addItem(QString());
        dbgvizJointsComboBox->addItem(QString());
        dbgvizJointsComboBox->setObjectName(QStringLiteral("dbgvizJointsComboBox"));
        dbgvizJointsComboBox->setEnabled(true);
        dbgvizJointsComboBox->setFocusPolicy(Qt::NoFocus);
        dbgvizJointsComboBox->setMaxVisibleItems(5);
        dbgvizJointsComboBox->setInsertPolicy(QComboBox::NoInsert);

        gridLayout->addWidget(dbgvizJointsComboBox, 5, 1, 1, 1);

        dbgvizCSComboBox = new QComboBox(visualizationGroupBox);
        dbgvizCSComboBox->addItem(QString());
        dbgvizCSComboBox->addItem(QString());
        dbgvizCSComboBox->addItem(QString());
        dbgvizCSComboBox->addItem(QString());
        dbgvizCSComboBox->setObjectName(QStringLiteral("dbgvizCSComboBox"));
        dbgvizCSComboBox->setFocusPolicy(Qt::NoFocus);
        dbgvizCSComboBox->setMaxVisibleItems(5);
        dbgvizCSComboBox->setInsertPolicy(QComboBox::NoInsert);

        gridLayout->addWidget(dbgvizCSComboBox, 2, 1, 1, 1);

        label_2 = new QLabel(visualizationGroupBox);
        label_2->setObjectName(QStringLiteral("label_2"));

        gridLayout->addWidget(label_2, 2, 0, 1, 1);

        dbgvizWireframeComboBox = new QComboBox(visualizationGroupBox);
        dbgvizWireframeComboBox->addItem(QString());
        dbgvizWireframeComboBox->addItem(QString());
        dbgvizWireframeComboBox->setObjectName(QStringLiteral("dbgvizWireframeComboBox"));
        dbgvizWireframeComboBox->setFocusPolicy(Qt::NoFocus);
        dbgvizWireframeComboBox->setMaxVisibleItems(5);
        dbgvizWireframeComboBox->setInsertPolicy(QComboBox::NoInsert);

        gridLayout->addWidget(dbgvizWireframeComboBox, 6, 1, 1, 1);

        label_4 = new QLabel(visualizationGroupBox);
        label_4->setObjectName(QStringLiteral("label_4"));

        gridLayout->addWidget(label_4, 4, 0, 1, 1);

        cameraModeLabel = new QLabel(visualizationGroupBox);
        cameraModeLabel->setObjectName(QStringLiteral("cameraModeLabel"));

        gridLayout->addWidget(cameraModeLabel, 0, 0, 1, 1);

        cameraModeComboBox = new QComboBox(visualizationGroupBox);
        cameraModeComboBox->addItem(QString());
        cameraModeComboBox->addItem(QString());
        cameraModeComboBox->setObjectName(QStringLiteral("cameraModeComboBox"));
        cameraModeComboBox->setFocusPolicy(Qt::NoFocus);
        cameraModeComboBox->setMaxVisibleItems(2);
        cameraModeComboBox->setInsertPolicy(QComboBox::NoInsert);

        gridLayout->addWidget(cameraModeComboBox, 0, 1, 1, 1);

        dbgvizSleepingObjectsComboBox = new QComboBox(visualizationGroupBox);
        dbgvizSleepingObjectsComboBox->addItem(QString());
        dbgvizSleepingObjectsComboBox->addItem(QString());
        dbgvizSleepingObjectsComboBox->addItem(QString());
        dbgvizSleepingObjectsComboBox->addItem(QString());
        dbgvizSleepingObjectsComboBox->setObjectName(QStringLiteral("dbgvizSleepingObjectsComboBox"));

        gridLayout->addWidget(dbgvizSleepingObjectsComboBox, 9, 1, 1, 1);

        label_14 = new QLabel(visualizationGroupBox);
        label_14->setObjectName(QStringLiteral("label_14"));

        gridLayout->addWidget(label_14, 9, 0, 1, 1);

        label_15 = new QLabel(visualizationGroupBox);
        label_15->setObjectName(QStringLiteral("label_15"));

        gridLayout->addWidget(label_15, 10, 0, 1, 1);

        dbgvizTriggerShapesComboBox = new QComboBox(visualizationGroupBox);
        dbgvizTriggerShapesComboBox->addItem(QString());
        dbgvizTriggerShapesComboBox->addItem(QString());
        dbgvizTriggerShapesComboBox->addItem(QString());
        dbgvizTriggerShapesComboBox->addItem(QString());
        dbgvizTriggerShapesComboBox->setObjectName(QStringLiteral("dbgvizTriggerShapesComboBox"));

        gridLayout->addWidget(dbgvizTriggerShapesComboBox, 10, 1, 1, 1);


        verticalLayout_3->addLayout(gridLayout);

        label_13 = new QLabel(visualizationGroupBox);
        label_13->setObjectName(QStringLiteral("label_13"));

        verticalLayout_3->addWidget(label_13);

        transparencyHorizontalSlider = new QSlider(visualizationGroupBox);
        transparencyHorizontalSlider->setObjectName(QStringLiteral("transparencyHorizontalSlider"));
        transparencyHorizontalSlider->setFocusPolicy(Qt::NoFocus);
        transparencyHorizontalSlider->setMaximum(100);
        transparencyHorizontalSlider->setValue(75);
        transparencyHorizontalSlider->setOrientation(Qt::Horizontal);

        verticalLayout_3->addWidget(transparencyHorizontalSlider);

        gizmoScaleLabel = new QLabel(visualizationGroupBox);
        gizmoScaleLabel->setObjectName(QStringLiteral("gizmoScaleLabel"));

        verticalLayout_3->addWidget(gizmoScaleLabel);

        gizmoScaleHorizontalSlider = new QSlider(visualizationGroupBox);
        gizmoScaleHorizontalSlider->setObjectName(QStringLiteral("gizmoScaleHorizontalSlider"));
        gizmoScaleHorizontalSlider->setFocusPolicy(Qt::NoFocus);
        gizmoScaleHorizontalSlider->setMinimum(2);
        gizmoScaleHorizontalSlider->setMaximum(100);
        gizmoScaleHorizontalSlider->setValue(5);
        gizmoScaleHorizontalSlider->setOrientation(Qt::Horizontal);

        verticalLayout_3->addWidget(gizmoScaleHorizontalSlider);

        label_10 = new QLabel(visualizationGroupBox);
        label_10->setObjectName(QStringLiteral("label_10"));

        verticalLayout_3->addWidget(label_10);

        jointsScaleHorizontalSlider = new QSlider(visualizationGroupBox);
        jointsScaleHorizontalSlider->setObjectName(QStringLiteral("jointsScaleHorizontalSlider"));
        jointsScaleHorizontalSlider->setFocusPolicy(Qt::NoFocus);
        jointsScaleHorizontalSlider->setMaximum(100);
        jointsScaleHorizontalSlider->setValue(100);
        jointsScaleHorizontalSlider->setOrientation(Qt::Horizontal);

        verticalLayout_3->addWidget(jointsScaleHorizontalSlider);

        label_12 = new QLabel(visualizationGroupBox);
        label_12->setObjectName(QStringLiteral("label_12"));

        verticalLayout_3->addWidget(label_12);

        particleSizeHorizontalSlider = new QSlider(visualizationGroupBox);
        particleSizeHorizontalSlider->setObjectName(QStringLiteral("particleSizeHorizontalSlider"));
        particleSizeHorizontalSlider->setFocusPolicy(Qt::NoFocus);
        particleSizeHorizontalSlider->setMaximum(100);
        particleSizeHorizontalSlider->setValue(10);
        particleSizeHorizontalSlider->setOrientation(Qt::Horizontal);

        verticalLayout_3->addWidget(particleSizeHorizontalSlider);

        gridLayout_2 = new QGridLayout();
        gridLayout_2->setSpacing(3);
        gridLayout_2->setObjectName(QStringLiteral("gridLayout_2"));
        label_5 = new QLabel(visualizationGroupBox);
        label_5->setObjectName(QStringLiteral("label_5"));

        gridLayout_2->addWidget(label_5, 0, 0, 1, 1);

        upAxisComboBox = new QComboBox(visualizationGroupBox);
        upAxisComboBox->setObjectName(QStringLiteral("upAxisComboBox"));
        upAxisComboBox->setFocusPolicy(Qt::NoFocus);

        gridLayout_2->addWidget(upAxisComboBox, 0, 1, 1, 1);

        label_11 = new QLabel(visualizationGroupBox);
        label_11->setObjectName(QStringLiteral("label_11"));

        gridLayout_2->addWidget(label_11, 1, 0, 1, 1);

        coordinateSystemComboBox = new QComboBox(visualizationGroupBox);
        coordinateSystemComboBox->setObjectName(QStringLiteral("coordinateSystemComboBox"));
        coordinateSystemComboBox->setFocusPolicy(Qt::NoFocus);

        gridLayout_2->addWidget(coordinateSystemComboBox, 1, 1, 1, 1);


        verticalLayout_3->addLayout(gridLayout_2);

        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout_3->addItem(verticalSpacer);


        verticalLayout_2->addWidget(visualizationGroupBox);

        rightTabWidget->addTab(settingsTab, QString());
        horizontalSplitter->addWidget(rightTabWidget);

        verticalLayout->addWidget(horizontalSplitter);

        PVDMainClass->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(PVDMainClass);
        menuBar->setObjectName(QStringLiteral("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 1386, 17));
        menuFile = new QMenu(menuBar);
        menuFile->setObjectName(QStringLiteral("menuFile"));
        menuHelp = new QMenu(menuBar);
        menuHelp->setObjectName(QStringLiteral("menuHelp"));
        menuCommands = new QMenu(menuBar);
        menuCommands->setObjectName(QStringLiteral("menuCommands"));
        menuCamera = new QMenu(menuBar);
        menuCamera->setObjectName(QStringLiteral("menuCamera"));
        PVDMainClass->setMenuBar(menuBar);
        statusBar = new QStatusBar(PVDMainClass);
        statusBar->setObjectName(QStringLiteral("statusBar"));
        PVDMainClass->setStatusBar(statusBar);

        menuBar->addAction(menuFile->menuAction());
        menuBar->addAction(menuCamera->menuAction());
        menuBar->addAction(menuCommands->menuAction());
        menuBar->addAction(menuHelp->menuAction());
        menuFile->addAction(actionOpen);
        menuFile->addAction(actionSave);
        menuFile->addSeparator();
        menuFile->addAction(actionExit);
        menuHelp->addAction(actionManual);
        menuHelp->addAction(actionReport_issue);
        menuHelp->addSeparator();
        menuHelp->addAction(actionAbout);
        menuCommands->addAction(actionGo_to_frame);
        menuCommands->addAction(actionFind_Objects);
        menuCommands->addAction(actionFind_profile_events);
        menuCommands->addAction(actionPlot_frame_graph);
        menuCommands->addSeparator();
        menuCommands->addAction(actionSplit_at_playhead);
        menuCommands->addAction(actionTrim_to_start_from_playhead);
        menuCommands->addAction(actionTrim_to_end_from_playhead);
        menuCommands->addSeparator();
        menuCommands->addAction(actionExportSelectedRepX);
        menuCommands->addAction(actionExportVisibleRepX);
        menuCommands->addAction(actionExportAllRepX);
        menuCommands->addSeparator();
        menuCommands->addAction(actionSettings);
        menuCommands->addAction(actionRestore_default_visual_settings);
        menuCamera->addAction(actionLook_at_point);
        menuCamera->addAction(actionReset_Camera);
        menuCamera->addAction(actionZoom_to_scene);
        menuCamera->addSeparator();
        menuCamera->addAction(actionTrack_selected);
        menuCamera->addAction(actionCenter_at_selected);
        menuCamera->addAction(actionZoom_to_selected);
        menuCamera->addSeparator();
        menuCamera->addAction(actionDefaultCamera);
        menuCamera->addAction(actionClip_Camera);

        retranslateUi(PVDMainClass);

        tabWidget->setCurrentIndex(0);
        rightTabWidget->setCurrentIndex(0);
        dbgvizBBComboBox->setCurrentIndex(0);
        dbgvizTransComboBox->setCurrentIndex(0);
        dbgvizVelComboBox->setCurrentIndex(0);
        dbgvizJointsComboBox->setCurrentIndex(0);
        dbgvizCSComboBox->setCurrentIndex(0);
        dbgvizWireframeComboBox->setCurrentIndex(0);
        cameraModeComboBox->setCurrentIndex(0);


        QMetaObject::connectSlotsByName(PVDMainClass);
    } // setupUi

    void retranslateUi(QMainWindow *PVDMainClass)
    {
        PVDMainClass->setWindowTitle(QApplication::translate("PVDMainClass", "PhysX Visual Debugger", nullptr));
        actionOpen->setText(QApplication::translate("PVDMainClass", "Open", nullptr));
#ifndef QT_NO_SHORTCUT
        actionOpen->setShortcut(QApplication::translate("PVDMainClass", "Ctrl+O", nullptr));
#endif // QT_NO_SHORTCUT
        actionExit->setText(QApplication::translate("PVDMainClass", "Exit", nullptr));
        actionAbout->setText(QApplication::translate("PVDMainClass", "About", nullptr));
        actionGo_to_frame->setText(QApplication::translate("PVDMainClass", "Go to frame", nullptr));
#ifndef QT_NO_SHORTCUT
        actionGo_to_frame->setShortcut(QApplication::translate("PVDMainClass", "Ctrl+G", nullptr));
#endif // QT_NO_SHORTCUT
        actionSave->setText(QApplication::translate("PVDMainClass", "Save", nullptr));
#ifndef QT_NO_SHORTCUT
        actionSave->setShortcut(QApplication::translate("PVDMainClass", "Ctrl+S", nullptr));
#endif // QT_NO_SHORTCUT
        actionFind_Object_by_Id->setText(QApplication::translate("PVDMainClass", "Find object by id", nullptr));
        actionPlot_frame_graph->setText(QApplication::translate("PVDMainClass", "Plot frame graph", nullptr));
        actionDefaultCamera->setText(QApplication::translate("PVDMainClass", "Default", nullptr));
        actionClip_Camera->setText(QApplication::translate("PVDMainClass", "Clip camera", nullptr));
        actionReset_Camera->setText(QApplication::translate("PVDMainClass", "Reset camera", nullptr));
#ifndef QT_NO_SHORTCUT
        actionReset_Camera->setShortcut(QApplication::translate("PVDMainClass", "Alt+R", nullptr));
#endif // QT_NO_SHORTCUT
        actionLook_at_point->setText(QApplication::translate("PVDMainClass", "Look at point", nullptr));
        actionTrack_selected->setText(QApplication::translate("PVDMainClass", "Track selected", nullptr));
#ifndef QT_NO_SHORTCUT
        actionTrack_selected->setShortcut(QApplication::translate("PVDMainClass", "Alt+T", nullptr));
#endif // QT_NO_SHORTCUT
        actionReport_issue->setText(QApplication::translate("PVDMainClass", "Report issue ...", nullptr));
        actionSplit_at_playhead->setText(QApplication::translate("PVDMainClass", "Split at playhead", nullptr));
#ifndef QT_NO_SHORTCUT
        actionSplit_at_playhead->setShortcut(QApplication::translate("PVDMainClass", "Ctrl+Alt+Y", nullptr));
#endif // QT_NO_SHORTCUT
        actionTrim_to_start_from_playhead->setText(QApplication::translate("PVDMainClass", "Trim left", nullptr));
#ifndef QT_NO_SHORTCUT
        actionTrim_to_start_from_playhead->setShortcut(QApplication::translate("PVDMainClass", "Ctrl+Alt+S", nullptr));
#endif // QT_NO_SHORTCUT
        actionTrim_to_end_from_playhead->setText(QApplication::translate("PVDMainClass", "Trim right", nullptr));
#ifndef QT_NO_SHORTCUT
        actionTrim_to_end_from_playhead->setShortcut(QApplication::translate("PVDMainClass", "Ctrl+Alt+E", nullptr));
#endif // QT_NO_SHORTCUT
        actionExportAllRepX->setText(QApplication::translate("PVDMainClass", "Export all to RepX", nullptr));
        actionExportSelectedRepX->setText(QApplication::translate("PVDMainClass", "Export selected to RepX", nullptr));
        actionManual->setText(QApplication::translate("PVDMainClass", "Manual", nullptr));
        actionSettings->setText(QApplication::translate("PVDMainClass", "Settings", nullptr));
#ifndef QT_NO_SHORTCUT
        actionSettings->setShortcut(QApplication::translate("PVDMainClass", "Ctrl+Shift+S", nullptr));
#endif // QT_NO_SHORTCUT
        actionZoom_to_scene->setText(QApplication::translate("PVDMainClass", "Zoom to scene", nullptr));
#ifndef QT_NO_SHORTCUT
        actionZoom_to_scene->setShortcut(QApplication::translate("PVDMainClass", "Ctrl+Z", nullptr));
#endif // QT_NO_SHORTCUT
        actionCenter_at_selected->setText(QApplication::translate("PVDMainClass", "Center at selected", nullptr));
#ifndef QT_NO_SHORTCUT
        actionCenter_at_selected->setShortcut(QApplication::translate("PVDMainClass", "Alt+C", nullptr));
#endif // QT_NO_SHORTCUT
        actionZoom_to_selected->setText(QApplication::translate("PVDMainClass", "Zoom to selected", nullptr));
#ifndef QT_NO_SHORTCUT
        actionZoom_to_selected->setShortcut(QApplication::translate("PVDMainClass", "Alt+Z", nullptr));
#endif // QT_NO_SHORTCUT
        actionFind_Objects->setText(QApplication::translate("PVDMainClass", "Find objects", nullptr));
#ifndef QT_NO_SHORTCUT
        actionFind_Objects->setShortcut(QApplication::translate("PVDMainClass", "Ctrl+F", nullptr));
#endif // QT_NO_SHORTCUT
        actionRestore_default_visual_settings->setText(QApplication::translate("PVDMainClass", "Restore default visual settings", nullptr));
#ifndef QT_NO_SHORTCUT
        actionRestore_default_visual_settings->setShortcut(QApplication::translate("PVDMainClass", "Ctrl+Shift+R", nullptr));
#endif // QT_NO_SHORTCUT
        actionFind_profile_events->setText(QApplication::translate("PVDMainClass", "Find profile events", nullptr));
#ifndef QT_NO_SHORTCUT
        actionFind_profile_events->setShortcut(QApplication::translate("PVDMainClass", "Alt+F", nullptr));
#endif // QT_NO_SHORTCUT
        actionExportVisibleRepX->setText(QApplication::translate("PVDMainClass", "Export visible to RepX", nullptr));
        rewindToolButton->setText(QApplication::translate("PVDMainClass", "...", nullptr));
        previousFrameToolButton->setText(QApplication::translate("PVDMainClass", "...", nullptr));
        playBackwardsToolButton->setText(QApplication::translate("PVDMainClass", "Play", nullptr));
        playToolButton->setText(QApplication::translate("PVDMainClass", "Play", nullptr));
        nextFrameToolButton->setText(QApplication::translate("PVDMainClass", "...", nullptr));
        rewindForwardToolButton->setText(QApplication::translate("PVDMainClass", "...", nullptr));
        allowConnectionToolButton->setText(QApplication::translate("PVDMainClass", "Allow new connection to PVD", nullptr));
        disconnectToolButton->setText(QApplication::translate("PVDMainClass", "Disconnect", nullptr));
        frameLabel->setText(QApplication::translate("PVDMainClass", "Frame", nullptr));
        FramelcdNumber->setText(QApplication::translate("PVDMainClass", "0", nullptr));
        QTreeWidgetItem *___qtreewidgetitem = inspectorTreeWidget->headerItem();
        ___qtreewidgetitem->setText(0, QApplication::translate("PVDMainClass", "Name", nullptr));
        QTreeWidgetItem *___qtreewidgetitem1 = inspectorPropertyTreeWidget->headerItem();
        ___qtreewidgetitem1->setText(1, QApplication::translate("PVDMainClass", "Value", nullptr));
        ___qtreewidgetitem1->setText(0, QApplication::translate("PVDMainClass", "Name", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(renderTab), QApplication::translate("PVDMainClass", "Render", nullptr));
        QTreeWidgetItem *___qtreewidgetitem2 = profileTreeWidget->headerItem();
        ___qtreewidgetitem2->setText(1, QApplication::translate("PVDMainClass", "%", nullptr));
        ___qtreewidgetitem2->setText(0, QApplication::translate("PVDMainClass", "Event Name", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(profileTab), QApplication::translate("PVDMainClass", "Profile", nullptr));
        QTableWidgetItem *___qtablewidgetitem = memoryTableWidget->horizontalHeaderItem(0);
        ___qtablewidgetitem->setText(QApplication::translate("PVDMainClass", "File", nullptr));
        QTableWidgetItem *___qtablewidgetitem1 = memoryTableWidget->horizontalHeaderItem(1);
        ___qtablewidgetitem1->setText(QApplication::translate("PVDMainClass", "Total outstanding", nullptr));
        QTableWidgetItem *___qtablewidgetitem2 = memoryTableWidget->horizontalHeaderItem(2);
        ___qtablewidgetitem2->setText(QApplication::translate("PVDMainClass", "Allocated (frame)", nullptr));
        QTableWidgetItem *___qtablewidgetitem3 = memoryTableWidget->horizontalHeaderItem(3);
        ___qtablewidgetitem3->setText(QApplication::translate("PVDMainClass", "Deallocated (frame)", nullptr));
        QTableWidgetItem *___qtablewidgetitem4 = memoryTableWidget->horizontalHeaderItem(4);
        ___qtablewidgetitem4->setText(QApplication::translate("PVDMainClass", "Churn (frame)", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(memoryTab), QApplication::translate("PVDMainClass", "Memory View", nullptr));
        QTableWidgetItem *___qtablewidgetitem5 = errorTableWidget->horizontalHeaderItem(0);
        ___qtablewidgetitem5->setText(QApplication::translate("PVDMainClass", "Code", nullptr));
        QTableWidgetItem *___qtablewidgetitem6 = errorTableWidget->horizontalHeaderItem(1);
        ___qtablewidgetitem6->setText(QApplication::translate("PVDMainClass", "Message", nullptr));
        QTableWidgetItem *___qtablewidgetitem7 = errorTableWidget->horizontalHeaderItem(2);
        ___qtablewidgetitem7->setText(QApplication::translate("PVDMainClass", "Location", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(errorStreamTab), QApplication::translate("PVDMainClass", "Error Stream", nullptr));
        QTreeWidgetItem *___qtreewidgetitem3 = threadProfileTreeWidget->headerItem();
        ___qtreewidgetitem3->setText(0, QApplication::translate("PVDMainClass", "Thread", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(threadProfileTab), QApplication::translate("PVDMainClass", "Thread Profiler", nullptr));
        QTableWidgetItem *___qtablewidgetitem8 = clipsTableWidget->horizontalHeaderItem(0);
        ___qtablewidgetitem8->setText(QApplication::translate("PVDMainClass", "Name", nullptr));
        QTableWidgetItem *___qtablewidgetitem9 = clipsTableWidget->horizontalHeaderItem(1);
        ___qtablewidgetitem9->setText(QApplication::translate("PVDMainClass", "Start Frame", nullptr));
        QTableWidgetItem *___qtablewidgetitem10 = clipsTableWidget->horizontalHeaderItem(2);
        ___qtablewidgetitem10->setText(QApplication::translate("PVDMainClass", "End Frame", nullptr));
        QTableWidgetItem *___qtablewidgetitem11 = clipsTableWidget->horizontalHeaderItem(3);
        ___qtablewidgetitem11->setText(QApplication::translate("PVDMainClass", "Frame Count", nullptr));
        visualizationGroupBox->setTitle(QApplication::translate("PVDMainClass", "Visualization", nullptr));
        gizmoScaleLabel_2->setText(QApplication::translate("PVDMainClass", "Near plane", nullptr));
        cameraSpeedLabel->setText(QApplication::translate("PVDMainClass", "<html><head/><body><p>Camera speed scale</p></body></html>", nullptr));
        label_6->setText(QApplication::translate("PVDMainClass", "Joints", nullptr));
        dbgvizContactComboBox->setItemText(0, QApplication::translate("PVDMainClass", "None", nullptr));
        dbgvizContactComboBox->setItemText(1, QApplication::translate("PVDMainClass", "All", nullptr));

        label_9->setText(QApplication::translate("PVDMainClass", "Contacts", nullptr));
        dbgvizBBComboBox->setItemText(0, QApplication::translate("PVDMainClass", "None", nullptr));
        dbgvizBBComboBox->setItemText(1, QApplication::translate("PVDMainClass", "All", nullptr));
        dbgvizBBComboBox->setItemText(2, QApplication::translate("PVDMainClass", "Selected", nullptr));
        dbgvizBBComboBox->setItemText(3, QApplication::translate("PVDMainClass", "Unselected", nullptr));

        label_7->setText(QApplication::translate("PVDMainClass", "Wireframe", nullptr));
        label->setText(QApplication::translate("PVDMainClass", "Bounding box", nullptr));
        label_8->setText(QApplication::translate("PVDMainClass", "Scene queries", nullptr));
        dbgvizSQComboBox->setItemText(0, QApplication::translate("PVDMainClass", "None", nullptr));
        dbgvizSQComboBox->setItemText(1, QApplication::translate("PVDMainClass", "All", nullptr));
        dbgvizSQComboBox->setItemText(2, QApplication::translate("PVDMainClass", "Selected", nullptr));
        dbgvizSQComboBox->setItemText(3, QApplication::translate("PVDMainClass", "Unselected", nullptr));

        label_3->setText(QApplication::translate("PVDMainClass", "Velocity", nullptr));
        dbgvizTransComboBox->setItemText(0, QApplication::translate("PVDMainClass", "None", nullptr));
        dbgvizTransComboBox->setItemText(1, QApplication::translate("PVDMainClass", "All", nullptr));
        dbgvizTransComboBox->setItemText(2, QApplication::translate("PVDMainClass", "Selected", nullptr));
        dbgvizTransComboBox->setItemText(3, QApplication::translate("PVDMainClass", "Unselected", nullptr));

        dbgvizVelComboBox->setItemText(0, QApplication::translate("PVDMainClass", "None", nullptr));
        dbgvizVelComboBox->setItemText(1, QApplication::translate("PVDMainClass", "All", nullptr));
        dbgvizVelComboBox->setItemText(2, QApplication::translate("PVDMainClass", "Selected", nullptr));
        dbgvizVelComboBox->setItemText(3, QApplication::translate("PVDMainClass", "Unselected", nullptr));

        dbgvizJointsComboBox->setItemText(0, QApplication::translate("PVDMainClass", "None", nullptr));
        dbgvizJointsComboBox->setItemText(1, QApplication::translate("PVDMainClass", "All", nullptr));

        dbgvizCSComboBox->setItemText(0, QApplication::translate("PVDMainClass", "None", nullptr));
        dbgvizCSComboBox->setItemText(1, QApplication::translate("PVDMainClass", "All", nullptr));
        dbgvizCSComboBox->setItemText(2, QApplication::translate("PVDMainClass", "Selected", nullptr));
        dbgvizCSComboBox->setItemText(3, QApplication::translate("PVDMainClass", "Unselected", nullptr));

        label_2->setText(QApplication::translate("PVDMainClass", "Coordinate system", nullptr));
        dbgvizWireframeComboBox->setItemText(0, QApplication::translate("PVDMainClass", "None", nullptr));
        dbgvizWireframeComboBox->setItemText(1, QApplication::translate("PVDMainClass", "All", nullptr));

        label_4->setText(QApplication::translate("PVDMainClass", "Transparent", nullptr));
        cameraModeLabel->setText(QApplication::translate("PVDMainClass", "Camera controller", nullptr));
        cameraModeComboBox->setItemText(0, QApplication::translate("PVDMainClass", "1st Person", nullptr));
        cameraModeComboBox->setItemText(1, QApplication::translate("PVDMainClass", "3rd Person", nullptr));

        cameraModeComboBox->setCurrentText(QApplication::translate("PVDMainClass", "1st Person", nullptr));
        dbgvizSleepingObjectsComboBox->setItemText(0, QApplication::translate("PVDMainClass", "None", nullptr));
        dbgvizSleepingObjectsComboBox->setItemText(1, QApplication::translate("PVDMainClass", "All", nullptr));
        dbgvizSleepingObjectsComboBox->setItemText(2, QApplication::translate("PVDMainClass", "Selected", nullptr));
        dbgvizSleepingObjectsComboBox->setItemText(3, QApplication::translate("PVDMainClass", "Unselected", nullptr));

        label_14->setText(QApplication::translate("PVDMainClass", "Sleeping objects", nullptr));
        label_15->setText(QApplication::translate("PVDMainClass", "Trigger shapes", nullptr));
        dbgvizTriggerShapesComboBox->setItemText(0, QApplication::translate("PVDMainClass", "None", nullptr));
        dbgvizTriggerShapesComboBox->setItemText(1, QApplication::translate("PVDMainClass", "All", nullptr));
        dbgvizTriggerShapesComboBox->setItemText(2, QApplication::translate("PVDMainClass", "Selected", nullptr));
        dbgvizTriggerShapesComboBox->setItemText(3, QApplication::translate("PVDMainClass", "Unselected", nullptr));

        label_13->setText(QApplication::translate("PVDMainClass", "Transparency", nullptr));
        gizmoScaleLabel->setText(QApplication::translate("PVDMainClass", "Gizmo scale", nullptr));
        label_10->setText(QApplication::translate("PVDMainClass", "Joints scale", nullptr));
        label_12->setText(QApplication::translate("PVDMainClass", "Particle size", nullptr));
        label_5->setText(QApplication::translate("PVDMainClass", "Up Axis", nullptr));
        upAxisComboBox->setCurrentText(QString());
        label_11->setText(QApplication::translate("PVDMainClass", "Chirality", nullptr));
        rightTabWidget->setTabText(rightTabWidget->indexOf(settingsTab), QApplication::translate("PVDMainClass", "Settings", nullptr));
        menuFile->setTitle(QApplication::translate("PVDMainClass", "File", nullptr));
        menuHelp->setTitle(QApplication::translate("PVDMainClass", "Help", nullptr));
        menuCommands->setTitle(QApplication::translate("PVDMainClass", "Commands", nullptr));
        menuCamera->setTitle(QApplication::translate("PVDMainClass", "Camera", nullptr));
    } // retranslateUi

};

namespace Ui {
    class PVDMainClass: public Ui_PVDMainClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
