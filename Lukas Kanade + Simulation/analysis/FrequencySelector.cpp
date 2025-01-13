#include "FrequencySelector.h"

#include <iostream>
#include <limits>
#include <sstream>

#include <QGridLayout>
#include <QHBoxLayout>
#include <QVBoxLayout>

using namespace std;
QT_CHARTS_USE_NAMESPACE

FrequencySelector::FrequencySelector(QWidget *parent)
    : QWidget(parent), name("wireman"), dataDir("tmp"), currentSelection(0), simulation(nullptr)
{
    label = new QLabel("Selected 0 frequencies", this);
    saveButton = new QPushButton("Save");
    connect(saveButton, SIGNAL(clicked(bool)), this, SLOT(saveSelection()));
    deleteButton = new QPushButton("Delete");
    connect(deleteButton, SIGNAL(clicked(bool)), this, SLOT(deleteSelection()));
    clearButton = new QPushButton("Clear");
    connect(clearButton, SIGNAL(clicked(bool)), this, SLOT(clearSelection()));
    addButton = new QPushButton("Add");
    connect(addButton, SIGNAL(clicked(bool)), this, SLOT(addSelection()));
    simulateButton = new QPushButton("Simulate!");
    connect(simulateButton, SIGNAL(clicked(bool)), this, SLOT(startSimulation()));

    initCharts();

    xShape = new DisplayImageWidget();
    yShape = new DisplayImageWidget();

    QHBoxLayout* fftImages = new QHBoxLayout();
    fftImages->addWidget(xFFTChart);
    fftImages->addWidget(yFFTChart);

    QHBoxLayout* shapes = new QHBoxLayout();
    shapes->addWidget(xShape);
    shapes->addWidget(yShape);

    QHBoxLayout* uis = new QHBoxLayout();
    uis->addWidget(addButton);
    uis->addWidget(deleteButton);
    uis->addWidget(clearButton);
    uis->addWidget(saveButton);
    uis->addWidget(simulateButton);

    QGridLayout* layout = new QGridLayout();
    layout->addWidget(xFFTChart, 0, 0);
    layout->addWidget(yFFTChart, 0, 1);
    layout->addWidget(xShape, 1, 0);
    layout->addWidget(yShape, 1, 1);
    layout->addWidget(label, 2, 0);
    layout->addLayout(uis, 2, 1);

    this->setLayout(layout);
    this->resize(1280, 720);
}

void FrequencySelector::init() {
    string fftFile = "tmp/" + name + "-fft.txt";
    frequencies = FileUtil::readFFT(fftFile, &xFFTReal, &xFFTImag, &yFFTReal, &yFFTImag, xMags, yMags);
    drawFFTs();
    reloadShapes(0);
}

void FrequencySelector::initCharts() {
    xFFTChart = new QChartView(this);
    yFFTChart = new QChartView(this);
    xFFT = new QLineSeries();
    xFFT->setName("Magnitudes");
    connect(xFFT, SIGNAL(clicked(QPointF)), this, SLOT(selectXPoint(QPointF)));
    yFFT = new QLineSeries();
    yFFT->setName("Magnitudes");
    connect(yFFT, SIGNAL(clicked(QPointF)), this, SLOT(selectYPoint(QPointF)));

    selectedFreqsX = new QScatterSeries();
    selectedFreqsX->setName("Selected Frequencies");
    selectedFreqsX->setMarkerShape(QScatterSeries::MarkerShapeCircle);
    selectedFreqsX->setMarkerSize(markerSize);
    selectedFreqsX->setColor(markerColor);

    selectedFreqsY = new QScatterSeries();
    selectedFreqsY->setName("Selected Frequencies");
    selectedFreqsY->setMarkerShape(QScatterSeries::MarkerShapeCircle);
    selectedFreqsY->setMarkerSize(markerSize);
    selectedFreqsY->setColor(markerColor);

    xFFTChart->chart()->addSeries(xFFT);
    xFFTChart->chart()->addSeries(selectedFreqsX);
    xFFTChart->setRenderHint(QPainter::Antialiasing);
    xFFTChart->chart()->setTitle("x-direction FFT");
    xFFTChart->chart()->createDefaultAxes();
    xFFTChart->chart()->axisY()->setMin(0.0);
    xFFTChart->chart()->setDropShadowEnabled(false);

    yFFTChart->chart()->addSeries(yFFT);
    yFFTChart->chart()->addSeries(selectedFreqsY);
    yFFTChart->setRenderHint(QPainter::Antialiasing);
    yFFTChart->chart()->setTitle("y-direction FFT");
    yFFTChart->chart()->createDefaultAxes();
    yFFTChart->chart()->axisY()->setMin(0.0);
    yFFTChart->chart()->setDropShadowEnabled(false);
}

void FrequencySelector::drawFFTs() {
    if (xFFT->count() == 0 && yFFT->count() == 0) {
        double maxXMag = 0.0;
        double maxYMag = 0.0;

        for (unsigned long i = 0; i < frequencies.size(); ++i) {
            xFFT->append(frequencies[i], xMags[i]);
            yFFT->append(frequencies[i], yMags[i]);
            if (maxXMag < xMags[i]) maxXMag = xMags[i];
            if (maxYMag < yMags[i]) maxYMag = yMags[i];
        }

        xFFTChart->chart()->axisY()->setMax(maxXMag * 1.1);
        yFFTChart->chart()->axisY()->setMax(maxYMag * 1.1);
        xFFTChart->chart()->axisX()->setMax(frequencies[frequencies.size() - 1]);
        yFFTChart->chart()->axisX()->setMax(frequencies[frequencies.size() - 1]);
    }

    selectedFreqsX->clear();
    selectedFreqsY->clear();
    for (unsigned long i = 0; i < selectedFreqs.size(); ++i) {
        selectedFreqsX->append(frequencies[selectedFreqs[i]], xMags[selectedFreqs[i]]);
        selectedFreqsY->append(frequencies[selectedFreqs[i]], yMags[selectedFreqs[i]]);
    }

    stringstream ss;
    ss << "Selected " << selectedFreqs.size() << " points\nSelecting frequency " << frequencies[currentSelection];
    label->setText(QString::fromStdString(ss.str()));
}

void FrequencySelector::selectXPoint(const QPointF& point) {
    QPointF clickedPoint = point;
    double minDist = numeric_limits<double>::max();
    double maxFreq = frequencies[frequencies.size() - 1];
    double maxX = 0.0;
    for (unsigned long i = 0; i < xMags.size(); ++i) {
        if (maxX < xMags[i]) maxX = xMags[i];
    }
    for (unsigned long i = 0; i < frequencies.size(); ++i) {
        double dist = pow(clickedPoint.x() - frequencies[i], 2) / pow(maxFreq, 2);
        dist += pow(clickedPoint.y() - xMags[i], 2) / pow(maxX, 2);
        if (minDist > dist) {
            minDist = dist;
            currentSelection = i;
        }
    }
    reloadShapes(currentSelection);
}

void FrequencySelector::selectYPoint(const QPointF& point) {
    QPointF clickedPoint = point;
    double minDist = numeric_limits<double>::max();
    double maxFreq = frequencies[frequencies.size() - 1];
    double maxY = 0.0;
    for (unsigned long i = 0; i < yMags.size(); ++i) {
        if (maxY < yMags[i]) maxY = yMags[i];
    }
    for (unsigned long i = 0; i < frequencies.size(); ++i) {
        double dist = pow(clickedPoint.x() - frequencies[i], 2) / pow(maxFreq, 2);
        dist += pow(clickedPoint.y() - yMags[i], 2) / pow(maxY, 2);
        if (minDist > dist) {
            minDist = dist;
            currentSelection = i;
        }
    }
    reloadShapes(currentSelection);
}

void FrequencySelector::clearSelection() {
    selectedFreqs.clear();
    drawFFTs();
}

void FrequencySelector::deleteSelection() {
    selectedFreqs.pop_back();
    drawFFTs();
}

void FrequencySelector::saveSelection() {
    if (selectedFreqs.size() < 1L) return;
    cv::Mat xFFTRealSelected = xFFTReal.row(selectedFreqs[0]);
    cv::Mat xFFTImagSelected = xFFTImag.row(selectedFreqs[0]);
    cv::Mat yFFTRealSelected = yFFTReal.row(selectedFreqs[0]);
    cv::Mat yFFTImagSelected = yFFTImag.row(selectedFreqs[0]);

    vector<double> frequencySelected = { frequencies[selectedFreqs[0]] };

    for (unsigned long i = 1; i < selectedFreqs.size(); ++i) {
        frequencySelected.push_back(frequencies[selectedFreqs[i]]);
        cv::vconcat(xFFTRealSelected, xFFTReal.row(selectedFreqs[i]), xFFTRealSelected);
        cv::vconcat(xFFTImagSelected, xFFTImag.row(selectedFreqs[i]), xFFTImagSelected);
        cv::vconcat(yFFTRealSelected, yFFTReal.row(selectedFreqs[i]), yFFTRealSelected);
        cv::vconcat(yFFTImagSelected, yFFTImag.row(selectedFreqs[i]), yFFTImagSelected);
    }

    FileUtil::saveFFT(name + "-selected", xFFTRealSelected, xFFTImagSelected,
                      yFFTRealSelected, yFFTImagSelected, frequencySelected);
}

void FrequencySelector::reloadShapes(int index) {
    stringstream ssx;
    ssx << dataDir << "/xShape/" << name << "-x-" << frequencies[index] << ".png";
    stringstream ssy;
    ssy << dataDir << "/yShape/" << name << "-y-" << frequencies[index] << ".png";
    xShape->loadAndRepaint(ssx.str());
    yShape->loadAndRepaint(ssy.str());
    stringstream ss;
    ss << "Selected " << selectedFreqs.size() << " points\nSelecting frequency " << frequencies[index];
    label->setText(QString::fromStdString(ss.str()));
}

void FrequencySelector::addSelection() {
    bool selected = false;
    for (auto ind : selectedFreqs) {
        if (ind == currentSelection) selected = true;
    }
    if (!selected) {
        selectedFreqs.push_back(currentSelection);
        drawFFTs();
    }
}

void FrequencySelector::keyPressEvent(QKeyEvent *event) {
    switch(event->key()) {
    case Qt::Key_A:
        addSelection();
    }
}

void FrequencySelector::startSimulation() {
    if (!FileUtil::fileExists(dataDir + "/" + name + "-selected-fft.txt")) return;
    if (simulation && simulation->isActive()) {
        simulation->close();
    }
    std::vector<InteractiveWindow*> simulations;


    InteractiveWindow* newSimulation = new InteractiveWindow(dataDir, name, "lk");
    simulations.push_back(newSimulation);
    newSimulation->show();
    newSimulation = new InteractiveWindow(dataDir, name, "berthy");
    simulations.push_back(newSimulation);
    newSimulation->show();
    newSimulation = new InteractiveWindow(dataDir, name, "raft");
    simulations.push_back(newSimulation);
    newSimulation->show();
    newSimulation = new InteractiveWindow(dataDir, name, "lkberthy");
    simulations.push_back(newSimulation);
    newSimulation->show();
}
