#ifndef FEATURETRACKER_H
#define FEATURETRACKER_H

#include <QChartView>
#include <QFileDialog>
#include <QPushButton>
#include <QString>
#include <QWidget>
#include <QtCharts/QScatterSeries>

#include "DelaunyTriangulation.h"
#include "ModalShapeProducer.h"

 
/**
 * class: FeatureTracker
 * The UI for feature tracking through frames and generate modal shapes.
 */
class FeatureTracker : public QWidget
{
    Q_OBJECT
public:
    explicit FeatureTracker(ModalShapeProducer *shapeProducer, QWidget *parent = 0);
    void setName(const std::string& name);

signals:

public slots:

private slots:
    // UI slots for selecting the position files and dir of frames
    void framesDirSelected(const QString& file);
    void posFileSelected(const QString& file);
    // UI slots for selecting the position files and dir of frames
    void showPosFileDialog();
    void showFramesDirDialog();
    // UI slot for tracking points through frames and passing info to 
    // the ModalShapeProducer
    void startTrackingFrames();
    // UI slot for ending this and launching the FrequencySelector
    void endSession();

private:
    // functions to draw the tracked feature points onto the canvas
    void drawTrackedPoints(const std::vector<cv::Point2f>& points);
    void initCharts();
    QtCharts::QChartView* pointsView;
    QtCharts::QScatterSeries *trackedPoints;

    QWidget *parent;
    // UI component for feature position file and frames dir selection
    QFileDialog *posFileDialog;
    QFileDialog *framesDirDialog;
    QPushButton *selectPosFile;
    QPushButton *selectFramesDir;

    // button to start tracking
    QPushButton *startTracking;
    // button to let ModalShapeProducer draw warped frames
    QPushButton *drawWarped;
    // button to let ModalShapeProducer draw modal shape viz
    QPushButton *drawShape;

    // Triangulation read from the feature position file
    DelaunyTriangulation* triangulation;
    // produces fft data files, warped frames and modal shape viz
    ModalShapeProducer* producer;

    std::string framesDir;
    std::string name;
};

#endif // FEATURETRACKER_H
