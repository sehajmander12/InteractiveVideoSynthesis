#ifndef FREQUENCYSELECTOR_H
#define FREQUENCYSELECTOR_H

#include <vector>

#include <QLabel>
#include <QPushButton>
#include <QWidget>
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <QtCharts/QScatterSeries>

#include <opencv2/core.hpp>
 
#include "fileutil.h"
#include "OpenGLWarper.h"
#include "DisplayImageWidget.h"
#include "InteractiveWindow.h"

/**
 * class: FrequencySelector
 * The UI for displaying and selecting modal shapes to be used in simulation.
 */ 
class FrequencySelector : public QWidget
{
    Q_OBJECT
public:
    explicit FrequencySelector(QWidget *parent = 0);

    void setName(const std::string& name) { this->name = name; }
    void setDataDir(const std::string& name) { this->dataDir = name; }
    void init();

private slots:
    // UI event to select points by clicking on the chart view
    void selectXPoint(const QPointF& point);
    void selectYPoint(const QPointF& point);

    // selection modifying UI slots
    void clearSelection();
    void deleteSelection();
    void saveSelection();
    void addSelection();

    // launch simulation window
    void startSimulation();

private:
    void keyPressEvent(QKeyEvent* event) Q_DECL_OVERRIDE;

    // loads FFT info and draws a line chart that shows the total x/y modal
    // shape magnitudes against the frequency for selection
    void initCharts();
    void drawFFTs();

    // loads the selected frequency's modal shape viz into the window
    void reloadShapes(int index);

    // label showing the selected frequency and number of selected freqs
    QLabel *label;
    // buttons for modifying the set of selections
    QPushButton *saveButton;
    QPushButton *deleteButton;
    QPushButton *clearButton;
    QPushButton *addButton;
    // button to launch simulation
    QPushButton *simulateButton;

    // chart views of total modal shape magnitudes against frequency
    QtCharts::QChartView *xFFTChart, *yFFTChart;
    QtCharts::QLineSeries *xFFT, *yFFT;
    const double markerSize = 7.5;
    const QColor markerColor = QColor(255,0,0);
    QtCharts::QScatterSeries* selectedFreqsX, *selectedFreqsY;

    // maintain list of selected frequencies to be saved
    std::vector<int> selectedFreqs;

    // FFT/modal shape information loaded from the designated NAME-fft.txt file
    std::vector<double> frequencies;
    std::vector<double> xMags, yMags;
    cv::Mat xFFTReal, yFFTReal, xFFTImag, yFFTImag;

    // image displaying window in the UI for modal shape viz
    DisplayImageWidget* xShape, *yShape;

    std::string name;
    std::string dataDir;

    int currentSelection;

    // simulation window to be launched
    InteractiveWindow* simulation;
};

#endif // FREQUENCYSELECTOR_H
