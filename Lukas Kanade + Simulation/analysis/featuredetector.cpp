#include "featuredetector.h"
#include "utility.h"

#include <iostream>
#include <sstream>

#include <QCheckBox>
#include <QVBoxLayout>

using namespace std;

FeatureDetector::FeatureDetector(FeatureSelector *image, FeatureTracker *tracker, QWidget *parent)
    : QWidget(parent), image(image), tracker(tracker), featureCountSlider(Qt::Horizontal)
{
    setupGUI();

    QVBoxLayout* layout = new QVBoxLayout();

    layout->addWidget(&featureCountSlider);
    layout->addWidget(&selectFeaturePoint);
    layout->addWidget(loadButton);
    layout->addWidget(saveButton);
    layout->addWidget(endButton);
    layout->addWidget(introText);

    stringstream ss;
    ss << "Count = " << 25 << endl;
    this->setLayout(layout);
    this->setWindowTitle(ss.str().c_str());
}

void FeatureDetector::setupGUI() {
    featureCountSlider.resize(200, 40);
    featureCountSlider.setMinimum(1);
    featureCountSlider.setMaximum(1000);
    featureCountSlider.setValue(25);
    featureCountSlider.setTickPosition(QSlider::TicksBelow);
    featureCountSlider.setTickInterval(5);

    connect(&featureCountSlider, SIGNAL(valueChanged(int)), this, SLOT(sliderChange()));

    this->selectFeaturePoint.setChecked(false);
    this->selectFeaturePoint.setText("Add Feature Points");
    connect(&selectFeaturePoint, SIGNAL(toggled(bool)), image, SLOT(setSelectFeatures(bool)));

    saveButton = new QPushButton("Save feature points", this);
    connect(saveButton, SIGNAL(clicked(bool)), this, SLOT(saveFeaturePoints()));

    endButton = new QPushButton("End Feature Point Selection", this);
    connect(endButton, SIGNAL(clicked(bool)), this, SLOT(endSession()));

    fpFile = new QFileDialog(this, tr("Select feature point file"), "/", tr("*-fp.txt"));
    fpFile->setFileMode(QFileDialog::AnyFile);
    connect(fpFile, SIGNAL(fileSelected(QString)), this, SLOT(loadFeaturePoints(QString)));
    loadButton = new QPushButton("Load Feature Points File");
    connect(loadButton, SIGNAL(clicked(bool)), this, SLOT(showFpDialog()));

    introText = new QLabel(this);
    introText->setText(QString::fromStdString(
                           "Use mouse to select points\n"
                           "Toggle 'Add Feature Points' to select feature points to track"
                           "Press 'd' to remove the last added point\n"
                           "Use slider to control automatically detected feature points"));
}

void FeatureDetector::loadFeaturePoints(const QString &file) {
    DelaunyTriangulation* tri = FileUtil::readFeaturePoints(file.toStdString());

    cv::Mat feats = Utility::vectorPoint2fToCvMat(tri->getFeaturePoints());
    cv::Mat fixed = Utility::vectorPoint2fToCvMat(tri->getFixedPoints());
    float w = tri->getWidth();
    float h = tri->getHeight();
    float scale = 1.0;
    if (h > image->maxH) {
        scale = scale * image->maxH / h;
        w = w * image->maxH / h;
        h = image->maxH;
    }
    if (w > image->maxW) {
        scale = scale * image->maxW / w;
        w = image->maxW;
        h = h * image->maxW / w;
    }
    feats *= scale;
    fixed *= scale;

    image->resetToBoundaryPoints();
    image->addFeaturePoint(feats);
    image->addFixedPoint(fixed);
    image->repaint();
}

void FeatureDetector::endSession() {
    tracker->setName(this->name);
    tracker->show();
    image->close();
    this->close();
}

void FeatureDetector::saveFeaturePoints() {
    image->saveFeaturePoints(this->name);
}

void FeatureDetector::sliderChange() {
    stringstream ss;
    ss << "Count = " << featureCountSlider.value() << endl;
    this->setWindowTitle(ss.str().c_str());
    image->setFeatureCount(featureCountSlider.value());
}

void FeatureDetector::setName(const string &name) {
    this->name = name;
}
