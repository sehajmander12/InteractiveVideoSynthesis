#include "FeatureSelector.h"
#include "featuredetector.h"
#include "FeatureTracker.h"
#include "ModalShapeProducer.h"
#include "FrequencySelector.h"
#include "VideoSelector.h"
#include "OpenGLWarper.h"

#include "simulationdata.h"
#include "simulationparams.h"
#include "readutility.h"
#include "InteractiveWindow.h"

#include "fileutil.h"

#include <QApplication>
#include <QDir>
#include <QLabel>

#include <iostream>
#include <sstream>
#include <string>

using namespace std;

int main(int argc, char *argv[])
{
    if (!QDir::setCurrent(QStringLiteral("../../../"))) {
        cerr << "Cannot set current directory" << endl;
        exit(1);
    }

    QApplication a(argc, argv);

    // GUI to select the desired frequency when simulating
    FrequencySelector freqSelector;
    // Produces the modal shapes for the tracked feature points
    ModalShapeProducer modeShapeProd(&freqSelector);
    // Track feature points through the video frames
    FeatureTracker tracker(&modeShapeProd);
    // GUI to select the feature points to track
    FeatureSelector selector;
    FeatureDetector detector(&selector, &tracker);
    // GUI to select the video to be used
    VideoSelector videoSelector(&detector, &selector, &freqSelector);
    videoSelector.show();

    return a.exec();
}
 
