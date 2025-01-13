#ifndef VIDEOSELECTOR_H
#define VIDEOSELECTOR_H

#include <QFileDialog>
#include <QLineEdit>
#include <QPushButton>
#include <QWidget>

#include "OpenGLWarper.h"
#include "featuredetector.h"
#include "FrequencySelector.h"

/**
 * class: VideoSelector
 * The first UI component that allows the user to choose a video and a dir,
 * then converts the video into frames, store in DIR/frames/NAMEx.png, where
 * x is the frame number starting from 1. Also allows the user to directly
 * start FrequencySelector and simulation if data files of the given name has
 * already been generated.
 */ 
class VideoSelector : public QWidget
{
    Q_OBJECT
public:
    explicit VideoSelector(FeatureDetector* fd, FeatureSelector* fs, FrequencySelector* freqS, QWidget *parent = 0);

signals:
 
public slots:

private slots:
    // UI slots for selecting video file and frames dir
    void selectVideoFile(const QString &name);
    void selectFramesDir(const QString &dir);
    void showVideoFileDialog();
    void showFramesDirDialog();
    // UI slot to convert the video into frames
    void convertVideo();
    // UI slot to set the name of the ensuing pipeline
    void setName(const QString& text);
    // UI slot to end the UI and start FeatureSelector and FeatureDetector
    void endSession();
    // UI slot to start FrequencySelector and simulation
    void directlySimulate();

private:
    // UIs to be launched for determining feature positions
    FeatureDetector* fd;
    FeatureSelector* fs;
    // UIs to be launched for selecting modal shapes
    FrequencySelector* freqS;

    QFileDialog *videoFileDialog;
    QFileDialog *framesDirDialog;

    std::string framesDir;
    std::string videoFilename;

    QPushButton *selectVideoButton;
    QPushButton *selectFramesDirButton;
    QPushButton *convertButton;
    QPushButton *endButton;
    QPushButton *simulateButton;

    // UI component to name the following pipeline
    QLineEdit* nameField;
    std::string name;

    bool converted;
};

#endif // VIDEOSELECTOR_H
