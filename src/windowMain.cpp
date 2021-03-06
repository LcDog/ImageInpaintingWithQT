#include "windowMain.h"

WindowMain::WindowMain() : mTotalImages(0) {
    setupUi(this);

    mSubwindowActions = new QList<QAction*>();
    mSeparatorOpenWindowsAdded = false;

    mIconCriminisi = new QIcon(":/icons/Criminisi.png");
    mIconHe = new QIcon(":/icons/He.png");
    mIconNs = new QIcon(":/icons/Ns.png");
    mIconTelea = new QIcon(":/icons/Telea.png");

    setContextMenuPolicy(Qt::PreventContextMenu);

    mSettings = new QSettings("imageInpaintingSettings.ini", QSettings::IniFormat);
    resize(mSettings->value("size", QSize(700, 480)).toSize());
    move(mSettings->value("pos", QPoint(150, 40)).toPoint());
    uiToolBarFile->setVisible(mSettings->value("uiToolBarFile", true).toBool());
    uiToolBarZoom->setVisible(mSettings->value("uiToolBarZoom", true).toBool());
    uiToolBarAlgos->setVisible(mSettings->value("uiToolBarAlgos", true).toBool());

    mMenuRecentFiles = new QMenu(this);
    for (int n=0; n<maxRecentFiles; ++n) {
        mActionRecentFiles[n] = new QAction(this);
        mActionRecentFiles[n]->setVisible(false);
        connect(mActionRecentFiles[n], &QAction::triggered, this, &WindowMain::openRecentFile);
        uiMenuFile->addAction(mActionRecentFiles[n]);
    }
    mActionSeparatorRecentFiles = uiMenuFile->addSeparator();
    mActionSeparatorRecentFiles->setVisible(false);
    updateRecentFilesMenu();
    mActionExit = new QAction(this);
    mActionExit->setObjectName(QString::fromUtf8("actionExit"));
    mActionExit->setText(QApplication::translate("mainWindow", "Exit", 0));
    mActionExit->setShortcut(QApplication::translate("mainWindow", "Ctrl+Q", 0));
    mActionExit->setIcon(QIcon(":/icons/window-close.svg"));
    uiMenuFile->addAction(mActionExit);

    mToolButtonOpenRecent = new QToolButton(this);
    mToolButtonOpenRecent->setFocusPolicy(Qt::NoFocus);
    mToolButtonOpenRecent->setPopupMode(QToolButton::MenuButtonPopup);
    mToolButtonOpenRecent->setMenu(mMenuRecentFiles);
    mToolButtonOpenRecent->setToolButtonStyle(Qt::ToolButtonIconOnly);
    mToolButtonOpenRecent->setAutoRaise(true);
    mToolButtonOpenRecent->setDefaultAction(uiActionOpen);
    uiToolBarFile->insertWidget(uiActionCaptureWebcam, mToolButtonOpenRecent);

    mActionGroupZoom = new QActionGroup(this);
    mActionGroupZoom->setEnabled(false);
    mActionGroupZoom->addAction(uiActionZoomIn);
    mActionGroupZoom->addAction(uiActionZoomOut);
    mActionGroupZoom->addAction(uiActionZoomOriginal);
    mActionGroupZoom->addAction(uiActionZoomBestFit);

    mActionGroupAlgo = new QActionGroup(this);
    mActionGroupAlgo->setEnabled(false);
    mActionGroupAlgo->addAction(uiActionCriminisi);
    mActionGroupAlgo->addAction(uiActionHe);
    mActionGroupAlgo->addAction(uiActionNs);
    mActionGroupAlgo->addAction(uiActionTelea);
    mActionGroupAlgo->addAction(uiActionDo4);

    mActionGroupWindow = new QActionGroup(this);
    mActionGroupWindow->setEnabled(false);
    mActionGroupWindow->addAction(uiActionTile);
    mActionGroupWindow->addAction(uiActionCascade);
    mActionGroupWindow->addAction(uiActionNext);
    mActionGroupWindow->addAction(uiActionPrevious);
    mActionGroupWindow->addAction(uiActionDuplicate);
    mActionGroupWindow->addAction(uiActionClose);
    mActionGroupWindow->addAction(uiActionCloseAll);

    mStatusBarLabelZoom = new QLabel(uiStatusBar);
    mStatusBarLabelZoom->setFrameShape(QFrame::NoFrame);
    mStatusBarLabelZoom->setAlignment(Qt::AlignHCenter);
    mStatusBarLabelDimensions = new QLabel(uiStatusBar);
    mStatusBarLabelDimensions->setFrameShape(QFrame::NoFrame);
    mStatusBarLabelDimensions->setAlignment(Qt::AlignHCenter);
    mStatusBarLabelSize = new QLabel(uiStatusBar);
    mStatusBarLabelSize->setFrameShape(QFrame::NoFrame);
    mStatusBarLabelSize->setAlignment(Qt::AlignHCenter);
    mStatusBarLabelTime = new QLabel(uiStatusBar);
    mStatusBarLabelTime->setFrameShape(QFrame::NoFrame);
    mStatusBarLabelTime->setAlignment(Qt::AlignHCenter);
    mStatusBarLabelIcon = new QLabel(uiStatusBar);
    mStatusBarLabelIcon->setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed));
    mStatusBarLabelIcon->setMinimumSize(QSize(16, 16));
    mStatusBarLabelIcon->setMaximumSize(QSize(16, 16));
    mStatusBarLabelIcon->setScaledContents(true);
    mStatusBarLabelIcon->setAlignment(Qt::AlignHCenter);
    mStatusBarLabelIcon->setVisible(false);
    mStatusBarLabelSpaceRight = new QLabel(uiStatusBar);
    mStatusBarLabelSpaceRight->setMinimumSize(0, 1);
    mStatusBarLabelSpaceLeft = new QLabel(uiStatusBar);
    mStatusBarLabelSpaceLeft->setMinimumSize(0, 1);
    mStatusBarLine = new QFrame(uiStatusBar);
    mStatusBarLine->setVisible(false);
    mStatusBarLine->setFrameShape(QFrame::VLine);
    mStatusBarLine->setFrameShadow(QFrame::Sunken);
    mStatusBarLine2 = new QFrame(mStatusBarLabelTime);
    mStatusBarLine2->setFrameShape(QFrame::VLine);
    mStatusBarLine2->setFrameShadow(QFrame::Sunken);
    mStatusBarLine2->setVisible(false);
    mStatusBarLine3 = new QFrame(mStatusBarLabelTime);
    mStatusBarLine3->setFrameShape(QFrame::VLine);
    mStatusBarLine3->setFrameShadow(QFrame::Sunken);
    mStatusBarLine3->setVisible(false);
    uiStatusBar->addWidget(mStatusBarLabelSpaceLeft);
    uiStatusBar->addWidget(mStatusBarLabelZoom);
    uiStatusBar->addPermanentWidget(mStatusBarLabelTime);
    uiStatusBar->addPermanentWidget(mStatusBarLine2);
    uiStatusBar->addPermanentWidget(mStatusBarLabelIcon);
    uiStatusBar->addPermanentWidget(mStatusBarLine3);
    uiStatusBar->addPermanentWidget(mStatusBarLabelDimensions);
    uiStatusBar->addPermanentWidget(mStatusBarLine);
    uiStatusBar->addPermanentWidget(mStatusBarLabelSize);
    uiStatusBar->addPermanentWidget(mStatusBarLabelSpaceRight);

    mCriminisiToolBar = new QWidget();
    mCriminisiToolBar->setVisible(false);
    mUICriminisi.setupUi(mCriminisiToolBar);
    mUICriminisi.uiSpinBoxPatchSize->setValue(mSettings->value("Criminisi/PatchSize", 4).toInt());
    connect(mUICriminisi.uiPushButtonShowProcessedImage, &QAbstractButton::clicked, this, &WindowMain::showOutImage);
    connect(mUICriminisi.uiSpinBoxPatchSize, &QSpinBox::editingFinished, this, &WindowMain::saveCriminisiParams);
    connect(mUICriminisi.uiButtonBox->button(QDialogButtonBox::Apply), &QAbstractButton::clicked, this, &WindowMain::applyCriminisi);
    connect(mUICriminisi.uiButtonBox->button(QDialogButtonBox::Reset), &QAbstractButton::clicked, this, &WindowMain::resetCriminisiParams);
    mCriminisiAction = uiToolBarParameters->addWidget(mCriminisiToolBar);


    mHeToolBar = new QWidget();
    mHeToolBar->setVisible(false);
    mUIHe.setupUi(mHeToolBar);
    connect(mUIHe.uiRadioButtonColorYellow, &QAbstractButton::clicked, this, &WindowMain::showMask);
    connect(mUIHe.uiRadioButtonColorBlack, &QAbstractButton::clicked, this, &WindowMain::showMask);
    connect(mUIHe.uiRadioButtonColorRed, &QAbstractButton::clicked, this, &WindowMain::showMask);
    mHeAction = uiToolBarParameters->addWidget(mHeToolBar);

    mButtonGroupMask = new QButtonGroup(mHeToolBar);
    mButtonGroupMask->addButton(mUIHe.uiRadioButtonColorYellow, 0);
    mButtonGroupMask->addButton(mUIHe.uiRadioButtonColorBlack, 1);
    mButtonGroupMask->addButton(mUIHe.uiRadioButtonColorRed, 2);

    mNSToolBar = new QWidget();
    mNSToolBar->setVisible(false);
    mUIpde_ns.setupUi(mNSToolBar);
    mUIpde_ns.uiSpinBoxRadius->setValue(mSettings->value("Ns/Radius", 4).toInt());
    connect(mUIpde_ns.uiSpinBoxRadius, &QSpinBox::editingFinished, this, &WindowMain::saveNsParams);
    connect(mUIpde_ns.uiPushButtonShowProcessedImage, &QAbstractButton::clicked, this, &WindowMain::showOutImage);
    connect(mUIpde_ns.uiButtonBox->button(QDialogButtonBox::Apply), &QAbstractButton::clicked, this, &WindowMain::applyNs);
    connect(mUIpde_ns.uiButtonBox->button(QDialogButtonBox::Reset), &QAbstractButton::clicked, this, &WindowMain::resetNsParams);
    mNSAction = uiToolBarParameters->addWidget(mNSToolBar);

    mTeleaToolBar = new QWidget();
    mTeleaToolBar->setVisible(false);
    mUItelea.setupUi(mTeleaToolBar);
    mUItelea.uiSpinBoxRadius->setValue(mSettings->value("Ns/Radius", 4).toInt());
    connect(mUItelea.uiSpinBoxRadius, &QSpinBox::editingFinished, this, &WindowMain::saveTeleaParams);
    connect(mUItelea.uiPushButtonShowProcessedImage, &QAbstractButton::clicked, this, &WindowMain::showOutImage);
    connect(mUItelea.uiButtonBox->button(QDialogButtonBox::Apply), &QAbstractButton::clicked, this, &WindowMain::applyTelea);
    connect(mUItelea.uiButtonBox->button(QDialogButtonBox::Reset), &QAbstractButton::clicked, this, &WindowMain::resetTeleaParams);
    mTeleaAction = uiToolBarParameters->addWidget(mTeleaToolBar);

    switch (mSettings->value("startupParameters", 0).toInt()) {
    case 0: showCriminisiToolBar(); break;
    case 1: showHeToolBar(); break;
    case 2: showNsToolBar(); break;
    case 3: showTeleaToolBar(); break;
    }

    connect(uiActionOpen, &QAction::triggered, this, &WindowMain::open);
    connect(uiActionCaptureWebcam, &QAction::triggered, this, &WindowMain::captureWebcam);
    connect(uiActionSaveCopyAs, &QAction::triggered, this, &WindowMain::saveCopyAs);
    connect(uiActionPreferences, &QAction::triggered, this, &WindowMain::preferences);
    connect(mActionExit, &QAction::triggered, this, &WindowMain::exit);
    connect(uiActionCopy, &QAction::triggered, this, &WindowMain::copy);
    connect(uiActionResetImage, &QAction::triggered, this, &WindowMain::resetImage);
    connect(uiActionStartupDialog, &QAction::triggered, this, &WindowMain::startupDialog);
    connect(uiActionZoomIn, &QAction::triggered, this, &WindowMain::zoom);
    connect(uiActionZoomOut, &QAction::triggered, this, &WindowMain::zoom);
    connect(uiActionZoomOriginal, &QAction::triggered, this, &WindowMain::zoom);
    connect(uiActionZoomBestFit, &QAction::triggered, this, &WindowMain::zoom);
    connect(uiActionCriminisi, &QAction::triggered, this, &WindowMain::showCriminisiToolBar);
    connect(uiActionHe, &QAction::triggered, this, &WindowMain::showHeToolBar);
    connect(uiActionNs, &QAction::triggered, this, &WindowMain::showNsToolBar);
    connect(uiActionTelea, &QAction::triggered, this, &WindowMain::showTeleaToolBar);
    connect(uiActionDo4, &QAction::triggered, this, &WindowMain::do4);
    connect(uiActionTile, &QAction::triggered, this, &WindowMain::tile);
    connect(uiActionCascade, &QAction::triggered, this, &WindowMain::cascade);
    connect(uiActionNext, &QAction::triggered, uiMdiArea, &QMdiArea::activateNextSubWindow);
    connect(uiActionPrevious, &QAction::triggered, uiMdiArea, &QMdiArea::activatePreviousSubWindow);
    connect(uiActionDuplicate, &QAction::triggered, this, &WindowMain::duplicate);
    connect(uiActionClose, &QAction::triggered, this, &WindowMain::closeActiveSubWindow);
    connect(uiActionCloseAll, &QAction::triggered, this, &WindowMain::closeAllSubWindows);
    connect(uiActionWebsite, &QAction::triggered, this, &WindowMain::website);
    connect(uiActionAbout, &QAction::triggered, this, &WindowMain::about);
    connect(uiMdiArea, &QMdiArea::subWindowActivated, this, &WindowMain::updateWindowMenu);
    mSignalMapper = new QSignalMapper(this); // for the Open Windows menu entries
    connect(mSignalMapper, static_cast<void (QSignalMapper::*)(QWidget*)>(&QSignalMapper::mapped), this, &WindowMain::setActiveSubWindow);


    if (mSettings->value("maximized", true).toBool())
        showMaximized();
    else show();

    if (mSettings->value("startupDialog", true).toBool())
        startupDialog();
}




void WindowMain::open() {
    loadFile(QFileDialog::getOpenFileName(this, tr("Open File"), "/home", tr("Images (*.png *.bmp *.jpg)")));
}




void WindowMain::captureWebcam() {
    new WindowCaptureWebcam(this);
}




void WindowMain::saveCopyAs() {
    QString fileName = QFileDialog::getSaveFileName(0, tr("Save Copy As"), QFileInfo(mActiveWindowImage->mWindowTitle).baseName().append(".png"), tr("Images (*.bmp *.png)"));
    if (!fileName.isEmpty())
        mActiveWindowImage->mPixmap.save(fileName);
}




void WindowMain::exit() {
    saveSettings();
    close();
}




void WindowMain::copy() {
    QApplication::clipboard()->setPixmap(mActiveWindowImage->mPixmap);
}




void WindowMain::preferences() {
    new WindowPreferences(this);
}




void WindowMain::startupDialog() {
    new WindowStartup(this);
}




void WindowMain::zoom() {
    if (sender()==uiActionZoomIn)
        mActiveWindowImage->zoomIn();
    else if (sender()==uiActionZoomOut)
        mActiveWindowImage->zoomOut();
    else if (sender()==uiActionZoomOriginal)
        mActiveWindowImage->zoomOriginal();
    else if (sender()==uiActionZoomBestFit)
        mActiveWindowImage->zoomBestFit();

    uiActionZoomIn->setEnabled(mActiveWindowImage->mCurrentFactor < 3.0);
    uiActionZoomOut->setEnabled(mActiveWindowImage->mCurrentFactor > 0.25);
    mStatusBarLabelZoom->setText(mActiveWindowImage->mImageZoom);
}

void WindowMain::showOutImage() {
    changeProcessedUi();
    mActiveWindowImage->showOutImage();
}

void WindowMain::changeProcessedUi() {
    if (mActiveWindowImage->mShowOutImg) {
        mUICriminisi.uiPushButtonShowProcessedImage->setText("Show Original Image");
        mUIpde_ns.uiPushButtonShowProcessedImage->setText("Show Original Image");
        mUItelea.uiPushButtonShowProcessedImage->setText("Show Original Image");
    }
    else {
        mUICriminisi.uiPushButtonShowProcessedImage->setText("Show Processed Image");
        mUIpde_ns.uiPushButtonShowProcessedImage->setText("Show Processed Image");
        mUItelea.uiPushButtonShowProcessedImage->setText("Show Processed Image");
    }
}

void WindowMain::showCriminisiToolBar() {
    if (mCurrentAlgoAction)
        mCurrentAlgoAction->setVisible(false);
    mCurrentAlgoAction = mCriminisiAction;
    mCurrentAlgoAction->setVisible(true);
    mSettings->setValue("startupParameters", 0);
    uiActionCriminisi->setChecked(true);
    if (mActiveWindowImage){
        if (mActiveWindowImage->mImageProcessed) {
            mActiveWindowImage->resetImage();
            changeProcessedUi();
        }
    }
}

void WindowMain::resetCriminisiParams() {
    mUICriminisi.uiSpinBoxPatchSize->setValue(4);
    saveCriminisiParams();
    mActiveWindowImage->resetImage();
    changeProcessedUi();
}

void WindowMain::applyCriminisi() {
    mActiveWindowImage->applyCriminisi(mSettings->value("Criminisi/PatchSize", 4).toInt());
    mStatusBarLabelIcon->setPixmap(QPixmap::fromImage(QImage(":/icons/Criminisi.png")));
    mActiveWindow->setWindowIcon(*mIconCriminisi);
    applyCommonTasks();
    showOutImage();
}

void WindowMain::saveCriminisiParams() {
    mSettings->setValue("Criminisi/PatchSize", mUICriminisi.uiSpinBoxPatchSize->value());
}




void WindowMain::showHeToolBar() {
    if (mCurrentAlgoAction)
        mCurrentAlgoAction->setVisible(false);
    mCurrentAlgoAction = mHeAction;
    mCurrentAlgoAction->setVisible(true);
    mSettings->setValue("startupParameters", 1);
    uiActionHe->setChecked(true);
    mUIHe.uiRadioButtonColorYellow->click();
    mUIHe.uiRadioButtonColorYellow->setChecked(true);
}




void WindowMain::showMask() {
    switch (mButtonGroupMask->checkedId()) {
    case 0: mActiveWindowImage->applyHe(WindowImage::colorType::yellow); break;
    case 1: mActiveWindowImage->applyHe(WindowImage::colorType::black); break;
    case 2: mActiveWindowImage->applyHe(WindowImage::colorType::red); break;
    }
    mStatusBarLabelIcon->setPixmap(QPixmap::fromImage(QImage(":/icons/He.png")));
    mActiveWindow->setWindowIcon(*mIconHe);
}



void WindowMain::showNsToolBar() {
    if (mCurrentAlgoAction)
        mCurrentAlgoAction->setVisible(false);
    mCurrentAlgoAction = mNSAction;
    mCurrentAlgoAction->setVisible(true);
    mSettings->setValue("startupParameters", 2);
    uiActionNs->setChecked(true);
    if (mActiveWindowImage){
        if (mActiveWindowImage->mImageProcessed) {
            mActiveWindowImage->resetImage();
            changeProcessedUi();
        }
    }
}




void WindowMain::resetNsParams() {
    mUIpde_ns.uiSpinBoxRadius->setValue(4);
    saveNsParams();
    mActiveWindowImage->resetImage();
    changeProcessedUi();
}




void WindowMain::applyNs() {
    mActiveWindowImage->applyNs(mSettings->value("Ns/Radius", 4).toInt());
    mStatusBarLabelIcon->setPixmap(QPixmap::fromImage(QImage(":/icons/Ns.png")));
    mActiveWindow->setWindowIcon(*mIconNs);
    applyCommonTasks();
    showOutImage();
}




void WindowMain::saveNsParams() {
    mSettings->setValue("Ns/Radius", mUIpde_ns.uiSpinBoxRadius->value());
}




void WindowMain::showTeleaToolBar() {
    if (mCurrentAlgoAction)
        mCurrentAlgoAction->setVisible(false);
    mCurrentAlgoAction = mTeleaAction;
    mCurrentAlgoAction->setVisible(true);
    mSettings->setValue("startupParameters", 3);
    uiActionTelea->setChecked(true);
    if (mActiveWindowImage){
        if (mActiveWindowImage->mImageProcessed) {
            mActiveWindowImage->resetImage();
            changeProcessedUi();
        }
    }
}




void WindowMain::resetTeleaParams() {
    mUItelea.uiSpinBoxRadius->setValue(4);
    saveTeleaParams();
    mActiveWindowImage->resetImage();
    changeProcessedUi();
}




void WindowMain::applyTelea() {
    mActiveWindowImage->applyTelea(mSettings->value("Telea/Radius", 4).toInt());
    mStatusBarLabelIcon->setPixmap(QPixmap::fromImage(QImage(":/icons/Telea.png")));
    mActiveWindow->setWindowIcon(*mIconTelea);
    applyCommonTasks();
    showOutImage();
}




void WindowMain::saveTeleaParams() {
    mSettings->setValue("Telea/Radius", mUItelea.uiSpinBoxRadius->value());
}

void WindowMain::do4() {
    WindowImage* CriminisiImage = new WindowImage(mActiveWindowImage->mImage, mActiveWindowImage->mWindowTitle);
    WindowImage* HeImage = new WindowImage(mActiveWindowImage->mImage, mActiveWindowImage->mWindowTitle);
    WindowImage* NsImage = new WindowImage(mActiveWindowImage->mImage, mActiveWindowImage->mWindowTitle);
    WindowImage* TeleaImage = new WindowImage(mActiveWindowImage->mImage, mActiveWindowImage->mWindowTitle);

//    QMessageBox::information(NULL, "Oh...sorry", mActiveWindowImage->mWindowTitle);
    CriminisiImage->applyCriminisi(mSettings->value("Criminisi/PatchSize", 4).toInt());
    CriminisiImage->showOutImage();
    HeImage->applyHe(WindowImage::colorType::yellow);
    NsImage->applyNs(mSettings->value("Ns/Radius", 4).toInt());
    NsImage->showOutImage();
    TeleaImage->applyTelea(mSettings->value("Telea/Radius", 4).toInt());
    TeleaImage->showOutImage();

    new WindowDo4(mActiveWindowImage->mWindowTitle, HeImage, CriminisiImage, NsImage, TeleaImage);

}
/*
*/

// apply result params
void WindowMain::applyCommonTasks() {
    uiActionResetImage->setEnabled(true);
    mStatusBarLabelTime->setText(mActiveWindowImage->mImageTime + " ms");
    mStatusBarLabelIcon->setVisible(true);
    mStatusBarLine2->setVisible(true);
    mStatusBarLine3->setVisible(true);
}




void WindowMain::resetImage() {
    mActiveWindowImage->resetImage();
    uiActionResetImage->setEnabled(false);
    mActiveWindow->setWindowIcon(QApplication::windowIcon());
    mStatusBarLabelIcon->clear();
    mStatusBarLabelIcon->setVisible(false);
    mStatusBarLabelTime->clear();
    mStatusBarLine2->setVisible(false);
    mStatusBarLine3->setVisible(false);
}






void WindowMain::tile() {
    uiMdiArea->tileSubWindows();
    if (mSettings->value("bestFit").toBool()) {
        QList<QMdiSubWindow*> subwindows = uiMdiArea->subWindowList();
        for (int n=0; n<subwindows.size(); ++n)
            qobject_cast<WindowImage*>(subwindows.at(n)->widget())->zoomBestFit();
    }
}




void WindowMain::cascade() {
    uiMdiArea->cascadeSubWindows();
    if (mSettings->value("bestFit").toBool()) {
        QList<QMdiSubWindow*> subwindows = uiMdiArea->subWindowList();
        for (int n=0; n<subwindows.size(); ++n)
            qobject_cast<WindowImage*>(subwindows.at(n)->widget())->zoomBestFit();
    }
}




void WindowMain::duplicate() {
    WindowImage* imageOriginal = uiMdiArea->findChild<WindowImage*>(QString(mActiveWindowImage->mOriginalUid));
    ++imageOriginal->mImageN;
    WindowImage* imageDuplicated = new WindowImage(mActiveWindowImage->mImage,
                                                   imageOriginal->mWindowTitle + QString(" (Duplicated %1)").arg(imageOriginal->mImageN),
                                                   WindowImage::duplicated);
    imageDuplicated->mOriginalUid = imageOriginal->mUid;
    showWindowImage(imageDuplicated);
}




void WindowMain::closeActiveSubWindow() {
    uiMdiArea->closeActiveSubWindow();
}




void WindowMain::closeAllSubWindows() {
    uiMdiArea->closeAllSubWindows();
}




void WindowMain::website() {
    QDesktopServices::openUrl(QUrl::fromEncoded("http://opencv.org/"));
}




void WindowMain::about() {
    new WindowAbout(this);
}




// http://doc.qt.io/qt-5/qtwidgets-mainwindows-mdi-example.html
void WindowMain::updateWindowMenu(QMdiSubWindow* mdiSubWindow) {
    if (!mSeparatorOpenWindowsAdded) { // Adding the separator on Qt Designer doesn't work
        uiMenuWindow->addSeparator();
        mSeparatorOpenWindowsAdded = true;
    }
    for (int n=0; n<mSubwindowActions->size(); ++n) {
        // 		uiMenuWindow->removeAction(mSubwindowActions->at(n)); // Makes not to trigger new actions added
        mSubwindowActions->at(n)->setVisible(false);
    }
    mSubwindowActions->clear();

    if (mdiSubWindow != 0) {
        mActiveWindow = mdiSubWindow;
        mActiveWindowImage = qobject_cast<WindowImage*> (mdiSubWindow->widget());

        uiActionSaveCopyAs->setEnabled(true);
        uiActionCopy->setEnabled(true);
        mActionGroupZoom->setEnabled(true);
        mActionGroupAlgo->setEnabled(true);
        mActionGroupWindow->setEnabled(true);
        uiActionZoomIn->setEnabled(mActiveWindowImage->mCurrentFactor < 3.0);
        uiActionZoomOut->setEnabled(mActiveWindowImage->mCurrentFactor > 0.25);

        if (!mActiveWindowImage->mImageTime.isEmpty()) {
            uiActionResetImage->setEnabled(true);
            switch (mActiveWindowImage->mAlgoType) {
            case WindowImage::criminisi:
                mStatusBarLabelIcon->setPixmap(QPixmap::fromImage(QImage(":/:/icons/Criminisi.png")));
                break;
            case WindowImage::He:
                mStatusBarLabelIcon->setPixmap(QPixmap::fromImage(QImage(":/:/icons/He.png")));
                break;
            case WindowImage::Ns:
                mStatusBarLabelIcon->setPixmap(QPixmap::fromImage(QImage(":/:/icons/Ns.png")));
                break;
            case WindowImage::Telea:
                mStatusBarLabelIcon->setPixmap(QPixmap::fromImage(QImage(":/icons/Telea.png")));
                break;
            }
            mStatusBarLabelIcon->setVisible(true);
            mStatusBarLabelTime->setText(mActiveWindowImage->mImageTime + " ms");
            mStatusBarLine2->setVisible(true);
            mStatusBarLine3->setVisible(true);
        } else {
            uiActionResetImage->setEnabled(false);
            mStatusBarLabelIcon->setVisible(false);
            mStatusBarLabelTime->clear();
            mStatusBarLine2->setVisible(false);
            mStatusBarLine3->setVisible(false);
        }

        mStatusBarLabelZoom->setText(mActiveWindowImage->mImageZoom);
        mStatusBarLabelDimensions->setText(mActiveWindowImage->mImageDimensions);
        mStatusBarLabelSize->setText(mActiveWindowImage->mImageSize);
        mStatusBarLine->setVisible(true);

        QList<QMdiSubWindow*> subwindows = uiMdiArea->subWindowList();
        for (int n=0; n<subwindows.size(); ++n) {
            WindowImage* windowImage = qobject_cast<WindowImage*> (subwindows.at(n)->widget());
            QString actionName;
            if (n<9)
                actionName = tr("&%1 %2").arg(n+1).arg(windowImage->windowTitle());
            else actionName = tr("%1 %2").arg(n+1).arg(windowImage->windowTitle());
            QAction* actionSubwindow = uiMenuWindow->addAction(actionName);
            mSubwindowActions->append(actionSubwindow);
            actionSubwindow->setCheckable(true);
            if (uiMdiArea->activeSubWindow())
                actionSubwindow->setChecked(windowImage == mActiveWindowImage);
            else actionSubwindow->setChecked(false);
            mActionGroupWindow->addAction(actionSubwindow);
            mSignalMapper->setMapping(actionSubwindow, subwindows.at(n));
            connect(actionSubwindow, &QAction::triggered, mSignalMapper, static_cast<void (QSignalMapper::*)()>(&QSignalMapper::map));
        }
    } else if (uiMdiArea->subWindowList().size()==0) {
        uiActionSaveCopyAs->setEnabled(false);
        uiActionCopy->setEnabled(false);
        mActionGroupZoom->setEnabled(false);
        mActionGroupAlgo->setEnabled(false);
        mActionGroupWindow->setEnabled(false);
        uiActionResetImage->setEnabled(false);
        uiToolBarParameters->setEnabled(false);

        mStatusBarLabelTime->clear();
        mStatusBarLabelIcon->setVisible(false);
        mStatusBarLabelZoom->clear();
        mStatusBarLabelDimensions->clear();
        mStatusBarLabelSize->clear();
        mStatusBarLine->setVisible(false);
        mStatusBarLine2->setVisible(false);
        mStatusBarLine3->setVisible(false);

        mActiveWindowImage = NULL;
    }
}




void WindowMain::loadFile(QString filePath) {
    if (!filePath.isEmpty()) {
        QImage* image = new QImage(filePath);
        if (!image->isNull()) {
            setRecentFile(filePath);
            showWindowImage(new WindowImage(image, filePath));
            if (mHeAction->isVisible())
                mUIHe.uiRadioButtonColorYellow->click();
        } else {
            removeRecentFile(filePath);
            QMessageBox::warning(this, tr("Image Inpainting"), tr("Cannot open %1.").arg(filePath));
        }
    }
}




void WindowMain::showWindowImage(WindowImage* windowImage) {
    windowImage->mUid = ++mTotalImages;
    if (windowImage->mWindowType != WindowImage::duplicated)
        windowImage->mOriginalUid = mTotalImages;
    windowImage->setObjectName(QString(mTotalImages));
    uiMdiArea->addSubWindow(windowImage);
    windowImage->parentWidget()->setGeometry(0, 0, windowImage->mImage->width()+8, windowImage->mImage->height()+31); // 8 and 31 are hardcoded values for the decorations of the subwindow
    if (windowImage->mImage->width() > uiMdiArea->width())
        windowImage->parentWidget()->setGeometry(0, 0, uiMdiArea->width(), windowImage->parentWidget()->height());
    if (windowImage->mImage->height() > uiMdiArea->height())
        windowImage->parentWidget()->setGeometry(0, 0, windowImage->parentWidget()->width(), uiMdiArea->height());
    windowImage->show();
    uiToolBarParameters->setEnabled(true);
}




void WindowMain::setRecentFile(QString filePath) {
    if (mSettings->value("rememberRecentFiles", true).toBool()) {
        QStringList files = mSettings->value("recentFiles").toStringList();
        files.removeAll(filePath);
        files.prepend(filePath);
        while (files.size()>maxRecentFiles)
            files.removeLast();
        mSettings->setValue("recentFiles", files);
        updateRecentFilesMenu();
    }
}




void WindowMain::removeRecentFile(QString filePath) {
    QStringList files = mSettings->value("recentFiles").toStringList();
    files.removeAll(filePath);
    mSettings->setValue("recentFiles", files);
    updateRecentFilesMenu();
}




// http://doc.qt.io/qt-5/qtwidgets-mainwindows-recentfiles-example.html
void WindowMain::updateRecentFilesMenu() {
    QStringList files = mSettings->value("recentFiles").toStringList();
    int numRecentFiles;
    if (files.size() < maxRecentFiles)
        numRecentFiles = files.size();
    else numRecentFiles = maxRecentFiles;
    mActionSeparatorRecentFiles->setVisible(numRecentFiles>0);
    mMenuRecentFiles->clear();
    for (int n=0; n<numRecentFiles; ++n) {
        mActionRecentFiles[n]->setText(tr("&%1 %2").arg(n+1).arg(QFileInfo(files[n]).fileName()));
        mActionRecentFiles[n]->setData(files[n]);
        mActionRecentFiles[n]->setVisible(true);
        mMenuRecentFiles->addAction(mActionRecentFiles[n]);
    }
    for (int n=numRecentFiles; n<maxRecentFiles; ++n)
        mActionRecentFiles[n]->setVisible(false);
}




void WindowMain::openRecentFile() {
    loadFile(qobject_cast<QAction*>(sender())->data().toString());
}




void WindowMain::setActiveSubWindow(QWidget* subWindow) {
    uiMdiArea->setActiveSubWindow(qobject_cast<QMdiSubWindow*>(subWindow));
}




void WindowMain::closeEvent(QCloseEvent* eventConstr) {
    saveSettings();
    QWidget::closeEvent(eventConstr);
}




void WindowMain::saveSettings() {
    mSettings->setValue("maximized", isMaximized());
    if (!isMaximized()) {
        mSettings->setValue("pos", pos());
        mSettings->setValue("size", size());
    }
}
