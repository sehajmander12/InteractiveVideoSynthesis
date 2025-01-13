#include "FeatureTracker.h"
#include "fileutil.h"

#include <fstream>
#include <iostream>
#include <sstream>
#include <QVBoxLayout>
#include <QPixmap>

#include <../opencv/modules/video/include/opencv2/video/tracking.hpp>

using namespace std;

FeatureTracker::FeatureTracker(ModalShapeProducer *shapeProducer, QWidget *parent)
    : QWidget(parent), producer(shapeProducer), framesDir("tmp"), name("handheld")
{

    posFileDialog = new QFileDialog(this, tr("Select Feature Position File"), "/", tr("*-fp.txt"));
    posFileDialog->setFileMode(QFileDialog::AnyFile);
    connect(posFileDialog, SIGNAL(fileSelected(QString)), this, SLOT(posFileSelected(QString)));

    framesDirDialog = new QFileDialog(this);
    framesDirDialog->setFileMode(QFileDialog::DirectoryOnly);
    connect(framesDirDialog, SIGNAL(fileSelected(QString)), this, SLOT(framesDirSelected(QString)));

    selectPosFile = new QPushButton("Select Feature Pos File");
    selectFramesDir = new QPushButton("Select Frames Directory");
    startTracking = new QPushButton("Track and Compute Modal Shapes");
    QPushButton *end = new QPushButton("End Session");
    drawWarped = new QPushButton("Draw Warped Frames");
    drawShape = new QPushButton("Draw Modal Shapes");

    connect(selectPosFile, SIGNAL(clicked(bool)), this, SLOT(showPosFileDialog()));
    connect(selectFramesDir, SIGNAL(clicked(bool)), this, SLOT(showFramesDirDialog()));
    connect(startTracking, SIGNAL(clicked(bool)), this, SLOT(startTrackingFrames()));
    connect(end, SIGNAL(clicked(bool)), this, SLOT(endSession()));
    connect(drawWarped, SIGNAL(clicked(bool)), producer, SLOT(drawWarpedFrames()));
    connect(drawShape, SIGNAL(clicked(bool)), producer, SLOT(drawModalShapes()));

    pointsView = new QtCharts::QChartView(this);

    QVBoxLayout* layout = new QVBoxLayout();
    layout->addWidget(selectPosFile);
    layout->addWidget(selectFramesDir);
    layout->addWidget(startTracking);
    layout->addWidget(pointsView);
    layout->addWidget(drawWarped);
    layout->addWidget(drawShape);
    layout->addWidget(end);

    this->setLayout(layout);
}

void FeatureTracker::setName(const string& name) {
    if (!name.empty()) {
        this->name = name;
        producer->setName(name);
    }
}

void FeatureTracker::drawTrackedPoints(const vector<cv::Point2f>& points) {
    pointsView->chart()->removeAllSeries();

    trackedPoints = new QtCharts::QScatterSeries();
    trackedPoints->setName("Tracked Points");
    trackedPoints->setMarkerShape(QtCharts::QScatterSeries::MarkerShapeCircle);
    trackedPoints->setMarkerSize(5.0);
    for (unsigned long i = 0; i < points.size(); ++i) {
        trackedPoints->append((1.0 * points[i].x) / triangulation->getWidth(), (1.0 * points[i].y) / triangulation->getHeight());
    }
    pointsView->chart()->addSeries(trackedPoints);
    pointsView->setRenderHint(QPainter::Antialiasing);
    pointsView->chart()->setTitle("Tracked Points");
    pointsView->chart()->createDefaultAxes();
    pointsView->chart()->setDropShadowEnabled(false);
}

void FeatureTracker::framesDirSelected(const QString &file) {
    framesDir = file.toStdString();
    producer->setFramesDir(this->framesDir.substr(0, this->framesDir.size() - 7));
}

void FeatureTracker::posFileSelected(const QString &file) { 
//    delete triangulation;
    triangulation = FileUtil::readFeaturePoints(file.toStdString());
    drawTrackedPoints(triangulation->getFeaturePoints());
}

void FeatureTracker::showPosFileDialog() {
   posFileDialog->show();
}

void FeatureTracker::showFramesDirDialog() {
    framesDirDialog->show();
}

void FeatureTracker::startTrackingFrames() {
    if (!triangulation) cout << "Select feature position file before tracking" << endl;
    if (framesDir == "") cout << "Select frames directory before tracking" << endl;
    int nFrames = 1;
    while (true) {
        stringstream ss;
        ss << framesDir << "/" << name << nFrames << ".png";
        ifstream fs(ss.str());
        if (!fs) break;
        nFrames++;
    }

    vector<cv::Point2f> texcoords;
    for (cv::Point2f pos : triangulation->getFeaturePoints()) {
        texcoords.push_back(pos);
    }
    for (cv::Point2f pos : triangulation->getFixedPoints()) {
        texcoords.push_back(pos);
    }

    vector<cv::Point2f> featurePoints = triangulation->getFeaturePoints();
    int nFeatures = featurePoints.size();

    cout << "Start tracking " << nFrames - 1 << " frames of " << name << " in directory " << framesDir << endl;
    cout << "Tracking " << nFeatures << " feature points" << endl;

    cv::Mat xFlowVec = cv::Mat(nFrames - 1, nFeatures, CV_32F, 0.f);
    cv::Mat yFlowVec = cv::Mat(nFrames - 1, nFeatures, CV_32F, 0.f);

    stringstream ss;
    ss << framesDir << "/" << name << "1.png";

    cv::Mat referenceFrame = cv::imread(ss.str(), CV_LOAD_IMAGE_GRAYSCALE);

    cv::Size winSize(200,200);
    int maxLevel = 6;
    cv::TermCriteria termcrit(cv::TermCriteria::COUNT|cv::TermCriteria::EPS,30,0.01);

    for (int i = 2; i < nFrames; ++i) {
        ss.str(string());
        ss << framesDir << "/" << name << i << ".png";

        cv::Mat gray = cv::imread(ss.str(), CV_LOAD_IMAGE_GRAYSCALE);

        vector<cv::Point2f> newPoints;
        vector<uchar> status;
        vector<float> err;

        cv::calcOpticalFlowPyrLK(referenceFrame, gray, featurePoints, newPoints,
                                 status, err, winSize, maxLevel, termcrit);

        for (int p = 0; p < nFeatures; ++p) {
            xFlowVec.at<float>(i-1,p) = newPoints[p].x - featurePoints[p].x;
            yFlowVec.at<float>(i-1,p) = newPoints[p].y - featurePoints[p].y;
        }
        referenceFrame = gray;
        featurePoints = newPoints;
        drawTrackedPoints(newPoints);
    }
    FileUtil::saveFlowVec(this->name, xFlowVec, yFlowVec);

    producer->setFramesDir(this->framesDir.substr(0, this->framesDir.size() - 7));
    producer->setName(this->name);
    producer->computeFFT();
}

void FeatureTracker::endSession() {
    producer->endSession();
    this->close();
}
