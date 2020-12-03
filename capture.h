#ifndef CAPTURE_H
#define CAPTURE_H
#include <gst/gst.h>
#include <gst/app/gstappsink.h>

#include <QObject>
#include <QImage>
#include <QMutex>
#include <QTextCodec>
#include <opencv2/opencv.hpp>

//for tictoc
#include <time.h>

#include "Record.h"

class capture : public QObject
{
    Q_OBJECT
public:
    explicit capture(QString config, QObject *parent = nullptr);
    ~capture();
    bool cameraPluginSetting();
    cv::Mat getFrame();
    void change_property(int i, QString str);
    void change_property(QString prop, double num);

    void get_property(QString prop, double *value);

    void get_stringproperty(QString prop, QString &value);
    void get_intproperty(QString prop, int &value);
    void get_floatproperty(QString prop, float &value);
    void event_bringup();
    void setStatePlaying();
    void setStatePause();
    void setStateReady();
//    void oneShot();
    void convertImageRgb(cv::Mat *pRecvMat, cv::Mat *pDstMat);

    QString m_MediaType;
    QString m_format;

    // Recording
    Record  *m_pRecord;
    bool    m_setRecFrameFlg;

signals:
    void frameArived();
    // Error message
    void errorMsgSend(QString);

public slots:

private:
    static GstFlowReturn newSample(GstAppSink* sink, gpointer gSelf);
    QString m_config;
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

    // 200610-1 Error check
    GstBus  *m_bus;
    GstMessage  *m_msg;
    void bus_message(GstMessage *msg);
    bool bus_pop_filter_message();

    //int idxs;

};

#endif // CAPTURE_H
