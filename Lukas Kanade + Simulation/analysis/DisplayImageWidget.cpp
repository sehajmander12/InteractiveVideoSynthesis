#include "DisplayImageWidget.h"

#include <QPainter>

using namespace std;
 
DisplayImageWidget::DisplayImageWidget(QWidget *parent) : QWidget(parent)
{
    this->resize(w, h);
}

void DisplayImageWidget::loadImage(const string &str) {
    cvImage = cv::imread(str);
    if (cvImage.data) {
        cv::cvtColor(cvImage, cvImage, cv::COLOR_BGR2RGB);
        cv::resize(cvImage, cvImage, cv::Size(this->w,this->h), 0,0,cv::INTER_CUBIC);
        qImage = QImage((uchar*) cvImage.data, cvImage.cols, cvImage.rows, cvImage.step, QImage::Format_RGB888);
    }
}

void DisplayImageWidget::paintEvent(QPaintEvent *event) {
    Q_UNUSED(event);
    auto painter = new QPainter();
    painter->begin(this);
    if (!qImage.isNull()) {
        painter->drawImage(0,0,qImage);
    }
    painter->end();
}

void DisplayImageWidget::loadAndRepaint(const string &str) {
    loadImage(str);
    repaint();
}
