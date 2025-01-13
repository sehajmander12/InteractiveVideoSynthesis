#ifndef MODALSHAPEPRODUCER_H
#define MODALSHAPEPRODUCER_H

#include <QWidget>
#include <QLabel>

#include <opencv2/highgui/highgui.hpp>

#include "fileutil.h"
#include "FrequencySelector.h"
#include "OpenGLWarper.h"

/**
 * class: ModalShapeProducer
 * An UI that computes the FFTs/modal shapes based on flow vectors in
 * framesDir/NAME-flowvec.txt, and manages the generation of FFTs/modal shapes
 * by creating ModalShapeDrawer and OpenGLWarper instances, and also saves
 * modal shape data file as framesDir/NAME-fft.txt.
 */ 
class ModalShapeProducer : public QWidget
{
    Q_OBJECT 
public:
    explicit ModalShapeProducer(FrequencySelector *freq, QWidget *parent = 0);
    void setName(const std::string& name) { this->name = name; }
    void setFramesDir(const std::string& framesDir) { this->framesDir = framesDir; }

    // computes FFTs with flowvec file framesDir/NAME-flowvec.txt and save
    // shapes and total magnitudes to framesDir/NAME-fft.txt
    void computeFFT();
    // ends this UI and launches the FrequencySelector instance
    void endSession();

signals:

public slots:
    void drawWarpedFrames();
    void drawModalShapes();

private:
    // FrequencySelector instance to be launched afterwards
    FrequencySelector* freqSelector;

    QLabel *progress;

    std::string name;
    std::string framesDir;
};

#endif // MODALSHAPEPRODUCER_H
