#include "VideoSelector.h"

#include <QVBoxLayout>

#include <iostream>

#include <opencv2/core.hpp>
#include <../opencv/include/opencv2/opencv.hpp>

using namespace std;

VideoSelector::VideoSelector(FeatureDetector* fd, FeatureSelector* fs, FrequencySelector *freqS, QWidget *parent)
    : QWidget(parent), fd(fd), fs(fs), freqS(freqS), framesDir("tmp"), name("handheld"), converted(false)
{

    videoFileDialog = new QFileDialog(this, tr("Select video file"));
    videoFileDialog->setFileMode(QFileDialog::AnyFile);
    connect(videoFileDialog, SIGNAL(fileSelected(QString)), this, SLOT(selectVideoFile(QString)));

    framesDirDialog = new QFileDialog(this, tr("Select video file"));
    framesDirDialog->setFileMode(QFileDialog::DirectoryOnly);
    connect(framesDirDialog, SIGNAL(fileSelected(QString)), this, SLOT(selectFramesDir(QString)));

    selectVideoButton = new QPushButton("Select Video File");
    selectFramesDirButton = new QPushButton("Select Directory to Store a Folder of Frames");
    convertButton =  new QPushButton("Convert video to frames!");
    endButton = new QPushButton("End video conversion");
    simulateButton = new QPushButton("Directly simulate from existing files");

    connect(selectVideoButton, SIGNAL(clicked(bool)), this, SLOT(showVideoFileDialog()));
    connect(selectFramesDirButton, SIGNAL(clicked(bool)), this, SLOT(showFramesDirDialog()));
    connect(convertButton, SIGNAL(clicked(bool)), this, SLOT(convertVideo()));
    connect(endButton, SIGNAL(clicked(bool)), this, SLOT(endSession()));
    connect(simulateButton, SIGNAL(clicked(bool)), this, SLOT(directlySimulate()));

    nameField = new QLineEdit(this);
    nameField->setPlaceholderText("Name the data!");
    connect(nameField, SIGNAL(textEdited(QString)), this, SLOT(setName(QString)));

    QVBoxLayout *layout = new QVBoxLayout();
    layout->addWidget(selectVideoButton);
    layout->addWidget(selectFramesDirButton);
    layout->addWidget(convertButton);
    layout->addWidget(nameField);
    layout->addWidget(endButton);
    layout->addWidget(simulateButton);

    this->setLayout(layout);
}

void VideoSelector::showFramesDirDialog() {
    framesDirDialog->show();
}

void VideoSelector::showVideoFileDialog() {
    videoFileDialog->show();
}

void VideoSelector::selectVideoFile(const QString& name) {
    videoFilename = name.toStdString();
    converted = false;
}

void VideoSelector::selectFramesDir(const QString& name) {
    framesDir = name.toStdString();
    converted = false;
}

void VideoSelector::convertVideo() {
    if (!FileUtil::fileExists(videoFilename) || !FileUtil::dirExists(framesDir)) {
        return;
    }

    if (!FileUtil::endsWith(framesDir, "/frames")) {
        if (!FileUtil::dirExists(framesDir + "/frames")) {
            FileUtil::createDir(framesDir + "/frames");
        }
    }

    cv::Mat frame;
    cv::VideoCapture cap(videoFilename);

    if (!cap.isOpened()) {
        return;
    }

    int n = 1;
    while (true) {
        if (!cap.read(frame)) break;

        stringstream ss;
        ss << framesDir << "/frames/" << name << n << ".png";
        imwrite(ss.str(), frame);
        ++n;
    }
    assert(abs(n - cap.get(CV_CAP_PROP_FRAME_COUNT)) <= 3);

    converted = true;
}

void VideoSelector::setName(const QString &text) {
    this->name = text.toStdString();
}

void VideoSelector::endSession() {
//    if (converted) {
        string firstFrame = framesDir + "/frames/" + this->name + "1.png";
        cv::Mat frame = cv::imread(firstFrame.c_str());
        cvtColor(frame, frame, CV_BGR2RGB);

        fs->setFrame(frame);
        fs->show();
        fd->setName(this->name);
        fd->show();
        this->close();
//    }
}

void VideoSelector::directlySimulate() {
    if (!FileUtil::dirExists(framesDir) || !FileUtil::fileExists(framesDir + "/" + name + "-selected-fft.txt")) {
        return;
    }
    freqS->setName(this->name);
    freqS->setDataDir(this->framesDir);
    freqS->init();
    freqS->show();

    this->close();
}
