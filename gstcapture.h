#ifndef GSTCAPTURE_H
#define GSTCAPTURE_H

#include <QObject>
#include <QDebug>
#include <QTimer>

#include <gst/gst.h>
#include <gst/app/gstappsink.h>

#include <iostream>


class GstCapture : public QObject
{
    Q_OBJECT
public:
    enum CurrentStatus{
        CAP_STOPPED,
        CAP_STARTED,
        CAP_RUNNING,
        CAP_ERROR,
        CAP_NOTFOUND
    };
private:
    static constexpr int MaxRetryCount = 10;
    static constexpr int RetryInterval = 100;

public:
    explicit GstCapture(QObject *parent = nullptr);
    ~GstCapture();
    QString getCameraList();
    CurrentStatus getCurrentStatus();
    void setCurrentStatus(CurrentStatus state);
    bool getProperty(QString name, int &value);
    bool getProperty(QString name, double &value);
    bool setProperty(QString name, int value);
    bool setProperty(QString name, double value);

private:
    bool initializePipeline();
    void terminatePipeline();

    static GstFlowReturn newSample(GstAppSink* sink, gpointer gSelf);
    static GstBusSyncReply busCallback(GstBus *bus, GstMessage *msg, gpointer data);

    QString m_launchStr;
    GstElement *pipeline;
    gulong handlerId;
    QTimer *watchDog;
    int capCount;

    CurrentStatus currentStatus;


signals:
    void signal_updateImage();
    void signal_cameraConnected();
    void signal_cameraDisconnected();
    void signal_currentStatusUpdated();

public slots:
    void createPipeline();
    void releasePipeline();
    void on_watchDogTimer();

};

#endif // GSTCAPTURE_H
