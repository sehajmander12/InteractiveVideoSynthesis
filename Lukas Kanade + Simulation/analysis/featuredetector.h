#ifndef FEATUREDETECTOR_H
#define FEATUREDETECTOR_H

#include <QCheckBox>
#include <QFileDialog>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QSlider>
#include <QWidget>
 
#include "FeatureSelector.h"
#include "FeatureTracker.h"

/**
 * class: FeatureDetector
 * The UI component of the feature detection part of the pipeline, manages
 * UI interactions to the image component in the class FeatureSelector.
 */ 
class FeatureDetector : public QWidget
{
    Q_OBJECT
public:
    explicit FeatureDetector(FeatureSelector *image, FeatureTracker *tracker, QWidget *parent = 0);
    // sets the name for the data to be stored
    void setName(const std::string& name);

private slots:
    // adjust count of automacically detected features with the slider
    void sliderChange();
    // save feature points as NAME-fp.txt file
    void saveFeaturePoints();
    // end the UI and starts the FeatureTracker
    void endSession();
    // load previously constructed feature point file
    void loadFeaturePoints(const QString& file);
    // show the feature point file selection dialog
    void showFpDialog() { fpFile->show(); }

private:
    void setupGUI();

    // the image component of the feature detector
    FeatureSelector *image;
    // FeatureTracker to be launched after this part of the pipeline ends
    FeatureTracker *tracker;

    // controlls whether manually selected points in the FeatureSelector is
    // a feature point or fixed point
    QCheckBox selectFeaturePoint;

    QSlider featureCountSlider;
    QLabel *introText;
    QPushButton* saveButton;
    QPushButton* endButton;
    QPushButton* loadButton;

    QFileDialog* fpFile;

    std::string name;


    int w = 200, h = 50;
};

#endif // FEATUREDETECTOR_H
