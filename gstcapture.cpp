#include "gstcapture.h"
#include <QMutexLocker>
#include <QThread>

GstCapture::GstCapture(QObject *parent) : QObject(parent)
  , m_launchStr("")
  , pipeline(nullptr)
  , handlerId(0)
  , watchDog(new QTimer(this))
  , capCount(0)
  , currentStatus(CAP_STOPPED)

{
    qDebug() << "GstCapture";

    if(!gst_is_initialized()){
        gst_init(nullptr, nullptr);
    }
    // キャプチャ監視タイマ
    watchDog->setInterval(3000);
    watchDog->setSingleShot(false);

    connect(watchDog, &QTimer::timeout, this, &GstCapture::on_watchDogTimer);

}

GstCapture::~GstCapture()
{

}

QString GstCapture::getCameraList()
{
    GstElement *plugin;
     if(pipeline == nullptr){
         plugin = gst_element_factory_make("dahuasrc", "src");

     }else{
         plugin = gst_bin_get_by_name(GST_BIN(pipeline), "dahuasrc");
     }
     if(plugin == nullptr){
         return nullptr;
     }
     const gchar *recv;
     GstStructure *arg_structure = gst_structure_new("info", "args", G_TYPE_INT, 1, NULL);
     GstQuery *query = gst_query_new_custom(GST_QUERY_CUSTOM, arg_structure);
     if(gst_element_query(plugin, query)){
         const GstStructure *structure = NULL;
         structure = gst_query_get_structure(query);
         recv = gst_structure_get_string(structure, "cameralist");
     }
     gst_query_unref(query);
     g_object_unref(plugin);
     QString str = recv;
     return str;
}

GstCapture::CurrentStatus GstCapture::getCurrentStatus()
{
    return currentStatus;
}

void GstCapture::setCurrentStatus(GstCapture::CurrentStatus state)
{
    currentStatus = state;
    emit signal_currentStatusUpdated();
}

bool GstCapture::getProperty(QString name, int &value)
{
    if(pipeline == nullptr){
        return false;
    }
    GstElement *appsink = gst_bin_get_by_name(GST_BIN(pipeline), "dahuasrc");
    if(appsink == nullptr){
        return false;
    }
    gint gvalue;

    g_object_get(G_OBJECT(appsink), name.toStdString().c_str(), &gvalue, NULL);
    value = gvalue;
    return true;

}

bool GstCapture::getProperty(QString name, double &value)
{
    if(pipeline == nullptr){
        return false;
    }
    GstElement *appsink = gst_bin_get_by_name(GST_BIN(pipeline), "dahuasrc");
    if(appsink == nullptr){
        return false;
    }
    gfloat gvalue;

    g_object_get(G_OBJECT(appsink), name.toStdString().c_str(), &gvalue, NULL);
    value = gvalue;
    return true;

}

bool GstCapture::setProperty(QString name, int value)
{
    if(pipeline == nullptr){
        return false;
    }
    GstElement *appsink = gst_bin_get_by_name(GST_BIN(pipeline), "dahuasrc");
    if(appsink == nullptr){
        return false;
    }
    g_object_set(appsink, name.toStdString().c_str(), value, NULL);
    return true;

}

bool GstCapture::setProperty(QString name, double value)
{
    if(pipeline == nullptr){
        return false;
    }
    GstElement *appsink = gst_bin_get_by_name(GST_BIN(pipeline), "dahuasrc");
    if(appsink == nullptr){
        return false;
    }
    g_object_set(appsink, name.toStdString().c_str(), (float)value, NULL);
    return true;

}

bool GstCapture::initializePipeline()
{
    GString *launch_str;
    launch_str = g_string_new(nullptr);
    g_string_printf(launch_str, m_launchStr.toStdString().c_str(), nullptr);
    pipeline = gst_parse_launch(launch_str->str, nullptr);
    if(pipeline == nullptr){
        qDebug() << "parse_launch failed";
        return false;
    }

    GstElement *appsink = gst_bin_get_by_name(GST_BIN(pipeline), "appsink");
    if (appsink == nullptr) {
        g_print("appsink is NULL\n");
        return false;
    }

    handlerId = g_signal_connect(appsink, "new-sample", G_CALLBACK(newSample), (gpointer)this);
    gst_object_unref(appsink);

    GstBus *bus = gst_element_get_bus(pipeline);
    if (bus == nullptr) {
        g_print("bus Is NULL\n");
        return false;
    }

    gst_bus_set_sync_handler(bus, (GstBusSyncHandler)busCallback, (gpointer)this, nullptr);
    gst_object_unref(bus);

    gst_element_set_state(pipeline, GST_STATE_PLAYING);
    return true;
}

void GstCapture::terminatePipeline()
{
    currentStatus = CAP_STOPPED;
    if(pipeline != nullptr){
        GstBus *bus = gst_element_get_bus(pipeline);
        if(bus != nullptr) {
            gst_bus_set_sync_handler(bus, nullptr, nullptr, nullptr);
            gst_object_unref(bus);
        }

        if(handlerId > 0) {
            GstElement *appsink = gst_bin_get_by_name(GST_BIN(pipeline), "appsink");
            g_signal_handler_disconnect(appsink, handlerId);
            gst_object_unref(appsink);
            handlerId = 0;
        }

        gst_element_set_state(pipeline, GST_STATE_NULL);
        gst_object_unref(GST_OBJECT(pipeline));
        pipeline = nullptr;
    }

    return;
}

GstFlowReturn GstCapture::newSample(GstAppSink *sink, gpointer gSelf)
{
    //    qDebug() << "newSample";
        GstCapture* self = static_cast<GstCapture* >(gSelf);
        self->currentStatus = CAP_RUNNING;

        GstSample* sample = NULL;
        GstBuffer* sampleBuffer = NULL;
        GstMapInfo bufferInfo;

        sample = gst_app_sink_pull_sample(GST_APP_SINK(sink));

        GstCaps *caps = gst_sample_get_caps(sample);
        GstStructure *structure = gst_caps_get_structure(caps, 0);
    //    QString pixelarray = gst_structure_get_name(structure);
    //    QString format = gst_structure_get_string(structure, "format");

    //    self->m_MediaType = gst_structure_get_name(structure);
    //    self->m_format = gst_structure_get_string(structure, "format");
        int width, height, channel;
//        if(self->m_clsBuff==nullptr){
//            return GST_FLOW_ERROR;
//        }
//        self->m_clsBuff->m_MediaType = gst_structure_get_name(structure);
//        self->m_clsBuff->m_format = gst_structure_get_string(structure, "format");
        gst_structure_get_int(structure, "width", &width);
        gst_structure_get_int(structure, "height", &height);
         // BayerやGRAYは1chほかは3ch
//        if (self->m_clsBuff->m_MediaType == "video/x-bayer" || self->m_clsBuff->m_format == "GRAY8"){
//            channel = 1;
//        }else{
//            channel = 3;
//        }
    //    qDebug() << channel;
        if(sample != nullptr){
            sampleBuffer = gst_sample_get_buffer(sample);
            if(sampleBuffer != nullptr){
                gst_buffer_map(sampleBuffer, &bufferInfo, GST_MAP_READ);
//                QMutexLocker locker(&(self->m_clsBuff->mutex));
//                self->m_clsBuff->currentImage = cv::Mat(height, width, CV_8UC(channel));
//                std::copy(bufferInfo.data, bufferInfo.data+bufferInfo.size, self->m_clsBuff->currentImage.data);

    //            if(!self->m_clsBuff->m_capQqueue.empty()){
    //                int idx = self->m_clsBuff->m_capQqueue.front();
    //                self->m_clsBuff->m_capQqueue.pop();
    //                self->m_clsBuff->m_Img[idx] = cv::Mat(height, width, CV_8UC(channel));
    //                std::copy(bufferInfo.data, bufferInfo.data+bufferInfo.size, self->m_clsBuff->m_Img[idx].data);
    //                self->m_clsBuff->m_readQueue.push(idx);
    //            }
            }
            gst_buffer_unmap(sampleBuffer, &bufferInfo);
        }
        gst_sample_unref(sample);
        self->capCount += 1;
        emit self->signal_updateImage();
        return GST_FLOW_OK;

}

GstBusSyncReply GstCapture::busCallback(GstBus *bus, GstMessage *msg, gpointer data)
{
    Q_UNUSED(bus)

//    qDebug() << "Bus callback. Thread ID:" << QThread::currentThreadId();

    GstCapture* self = static_cast<GstCapture* >(data);
    Q_UNUSED(self);

    switch (GST_MESSAGE_TYPE(msg)) {
//    case GST_MESSAGE_STATE_CHANGED:
//        GstState oldState;
//        GstState newState;
//        GstState pending;
//        gst_message_parse_state_changed(msg, &oldState, &newState, &pending);
//        g_printerr("State changed [%s] to [%s]\n ",
//                   gst_element_state_get_name(oldState),
//                   gst_element_state_get_name(newState));
//        break;
     case GST_MESSAGE_ERROR:
        GError *err;
        gchar *debug_info;
        gst_message_parse_error(msg, &err, &debug_info);
        g_printerr("Error received from element %s: %s\n", GST_OBJECT_NAME(msg->src), err->message);
        g_printerr("Debugging information: %s\n", debug_info ? debug_info : "none");
        g_clear_error(&err);
        g_free(debug_info);
//        self->terminatePipeline();
        break;
    case GST_MESSAGE_EOS:
        g_print("End-Of-Stream reached.\n");
//        self->terminatePipeline();
        break;
    default:
        break;
    }

    return GST_BUS_DROP;

}

void GstCapture::createPipeline()
{
    currentStatus = CAP_STARTED;
    QMetaObject::invokeMethod(watchDog, "start", Qt::QueuedConnection);

    qDebug() << "createPipeline. Thread ID:" << QThread::currentThreadId();

    if(m_launchStr == ""){
        qDebug() << "Launch String not set";
        return;
    }

    int retry = 0;
    for(; retry < MaxRetryCount; retry++) {
        if(initializePipeline())
            break;
        QThread::msleep(RetryInterval);
        qWarning() << "Gstreamer connect retry. " << QString::number(retry + 1);
    }

    if(MaxRetryCount <= retry) {
        qWarning() << "Gstreamer connect failed. " ;
        return;
    }

//    if(m_clsBuff == nullptr){
//        m_clsBuff = new clsImgBuffer();
//    }

    emit signal_cameraConnected();
    return;

}

void GstCapture::releasePipeline()
{
    QMetaObject::invokeMethod(watchDog, "stop", Qt::QueuedConnection);

    qDebug()<< "relasePipeline";

    terminatePipeline();

//    if(m_clsBuff != nullptr) {
//        delete m_clsBuff;
//        m_clsBuff = nullptr;
//    }
    return;

}

void GstCapture::on_watchDogTimer()
{
    if(capCount == 0 &&
            (currentStatus == CAP_STARTED || currentStatus == CAP_RUNNING)){
        // キャプチャされていない かつ接続状態
        emit signal_cameraDisconnected();
        qDebug() << "signal_cameraDisconnected";
        currentStatus = CAP_ERROR;
    }
    capCount = 0;

}
