#ifndef CAPTURE_H
#define CAPTURE_H
#include <gst/gst.h>
#include <gst/app/gstappsink.h>

#include <QObject>
#include <QImage>
#include <QMutex>
#include <opencv2/opencv.hpp>

//for tictoc
#include <time.h>


class capture : public QObject
{
    Q_OBJECT
public:
    explicit capture(QString config, QObject *parent = nullptr);
    ~capture();
    cv::Mat getFrame();
    void change_property(int i, QString str);
    void change_property(QString prop, float num);
    QString get_stringproperty(QString prop, QString &value);
    int get_intproperty(QString prop, int &value);
    float get_floatproperty(QString prop, float &value);
    void event_bringup();
    void setStatePlaying();
    void setStatePause();
    void setStateReady();
//    void oneShot();
    QString m_MediaType;
    QString m_format;
signals:
    void frameArived();
public slots:

private:
    static GstFlowReturn newSample(GstAppSink* sink, gpointer gSelf);
    GstElement* m_pipeline;
    GstElement *sink, *src, *conv;
//    QImage m_image;
//    QImage m_image2;
    cv::Mat m_cv1;
    cv::Mat m_cv2;
//    cv::Mat m_cv[2];
    int m_imFlag;
    QMutex m_mutex;
    GstEvent * event;
    int counter;


    //int idxs;
};

#endif // CAPTURE_H
