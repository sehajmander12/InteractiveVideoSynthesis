#ifndef DISPLAYIMAGEWIDGET_H
#define DISPLAYIMAGEWIDGET_H
 
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

#include <QImage>
#include <QWidget>

/**
 * class: DisplayImageWidget
 * This is essentially just a QWidget that displays an OpenCV image.
 */ 
class DisplayImageWidget : public QWidget
{
    Q_OBJECT
public:
    explicit DisplayImageWidget(QWidget *parent = 0);

    const int w = 640;
    const int h = 360;

    void loadImage(const std::string& str);

    void paintEvent(QPaintEvent* event) Q_DECL_OVERRIDE;

    void loadAndRepaint(const std::string& str);

    virtual QSize sizeHint () const Q_DECL_OVERRIDE {
        return QSize(w, h);
    }

signals:

public slots:

private:
    cv::Mat cvImage;
    QImage qImage;
};

#endif // DISPLAYIMAGEWIDGET_H
