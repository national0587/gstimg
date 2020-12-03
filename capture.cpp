#include "capture.h"
#include <QMutexLocker>
#include <QDebug>
#include <QString>
int idxs;
int ok=0;
//tictoc
struct timespec startTime,  endTime;
capture::capture(QString config, QObject *parent) : QObject(parent), m_pipeline(NULL)
{
    m_imFlag=0;
    counter=0;
    idxs=0;
    m_pipeline = NULL;

    m_config = config;

    // Error check
    m_bus = NULL;

    m_pRecord = NULL;
    m_setRecFrameFlg = false;
}

capture::~capture()
{
    if( m_bus != NULL ){
        gst_object_unref(m_bus);
    }

    if( m_pipeline != NULL ){
        gst_element_set_state(m_pipeline, GST_STATE_NULL);
        gst_object_unref(m_pipeline);
    }
}

bool capture::cameraPluginSetting()
{
    bool resultFlg = true;
    GError* error = NULL;
    this->src = gst_element_factory_make("dahuasrc", "src");
    this->sink = gst_element_factory_make("appsink", "sink");

    /*set property*/

    if (m_config!=""){
        char *config_c = new char[m_config.size()+1];
        std::strcpy(config_c, m_config.toUtf8().data() );

        g_object_set(G_OBJECT(src), "config", config_c,  NULL);
        delete[] config_c;
    }
    g_object_set(G_OBJECT(sink), "name", "sink", NULL);

    /*create pipeleine*/
    m_pipeline = gst_pipeline_new("testpipeline");
    gst_bin_add_many(GST_BIN(m_pipeline), src, sink, NULL);

    /*link elements*/
    gst_element_link_many(src, sink, NULL);

    if(m_pipeline == NULL){

        qDebug("ERROR %s", error->message);
        g_error_free(error);
    }
    m_bus = gst_pipeline_get_bus(GST_PIPELINE(m_pipeline));

    gst_app_sink_set_emit_signals(GST_APP_SINK(sink), TRUE);
    g_signal_connect(sink, "new-sample", G_CALLBACK(newSample), (gpointer)this);
    gst_element_set_state(m_pipeline, GST_STATE_PLAYING);

    // Error check
    resultFlg = bus_pop_filter_message();

    clock_gettime(CLOCK_REALTIME, &startTime);

    return resultFlg;
}

// パイプラインから送信されるメッセージをすべて受信する（今回は使用しない。エラーのみ受信する関数を使用）
void capture::bus_message(GstMessage *msg)
{
    if( msg != NULL ){
        switch(GST_MESSAGE_TYPE(msg)){
        case GST_MESSAGE_ERROR:
            g_print("message type:%d. GST_MESSAGE_ERROR\n", GST_MESSAGE_TYPE(msg));
            GError *err;
            gchar *debug;

            gst_message_parse_error(msg, &err, &debug);
            g_print("message error:%s\n", err->message);
            g_error_free(err);
            g_free(debug);
            break;
        case GST_MESSAGE_UNKNOWN:
            g_print("message type:%d. GST_MESSAGE_UNKNOWN\n", GST_MESSAGE_TYPE(msg));
            break;
        case GST_MESSAGE_EOS:
            g_print("message type:%d. GST_MESSAGE_EOS\n", GST_MESSAGE_TYPE(msg));
            break;
        case GST_MESSAGE_WARNING:
            g_print("message type:%d. GST_MESSAGE_WARNING\n", GST_MESSAGE_TYPE(msg));
            break;
        case GST_MESSAGE_INFO:
            g_print("message type:%d. GST_MESSAGE_INFO\n", GST_MESSAGE_TYPE(msg));
            break;
        case GST_MESSAGE_TAG:
            g_print("message type:%d. GST_MESSAGE_TAG\n", GST_MESSAGE_TYPE(msg));
            break;
        case GST_MESSAGE_BUFFERING:
            g_print("message type:%d. GST_MESSAGE_BUFFERING\n", GST_MESSAGE_TYPE(msg));
            break;
        case GST_MESSAGE_STATE_CHANGED:
            g_print("message type:%d. GST_MESSAGE_STATE_CHANGED\n", GST_MESSAGE_TYPE(msg));
            break;
        case GST_MESSAGE_STATE_DIRTY:
            g_print("message type:%d. GST_MESSAGE_STATE_DIRTY\n", GST_MESSAGE_TYPE(msg));
            break;
        case GST_MESSAGE_STEP_DONE:
            g_print("message type:%d. GST_MESSAGE_STEP_DONE\n", GST_MESSAGE_TYPE(msg));
            break;
        case GST_MESSAGE_CLOCK_PROVIDE:
            g_print("message type:%d. GST_MESSAGE_CLOCK_PROVIDE\n", GST_MESSAGE_TYPE(msg));
            break;
        case GST_MESSAGE_CLOCK_LOST:
            g_print("message type:%d. GST_MESSAGE_CLOCK_LOST\n", GST_MESSAGE_TYPE(msg));
            break;
        case GST_MESSAGE_NEW_CLOCK:
            g_print("message type:%d. GST_MESSAGE_NEW_CLOCK\n", GST_MESSAGE_TYPE(msg));
            break;
        case GST_MESSAGE_STRUCTURE_CHANGE:
            g_print("message type:%d. GST_MESSAGE_STRUCTURE_CHANGE\n", GST_MESSAGE_TYPE(msg));
            break;
        case GST_MESSAGE_STREAM_STATUS:
            g_print("message type:%d. GST_MESSAGE_STREAM_STATUS\n", GST_MESSAGE_TYPE(msg));
            break;
        case GST_MESSAGE_APPLICATION:
            g_print("message type:%d. GST_MESSAGE_APPLICATION\n", GST_MESSAGE_TYPE(msg));
            break;
        case GST_MESSAGE_ELEMENT:
            g_print("message type:%d. GST_MESSAGE_ELEMENT\n", GST_MESSAGE_TYPE(msg));
            break;
        case GST_MESSAGE_SEGMENT_START:
            g_print("message type:%d. GST_MESSAGE_SEGMENT_START\n", GST_MESSAGE_TYPE(msg));
            break;
        case GST_MESSAGE_SEGMENT_DONE:
            g_print("message type:%d. GST_MESSAGE_SEGMENT_DONE\n", GST_MESSAGE_TYPE(msg));
            break;
        case GST_MESSAGE_DURATION_CHANGED:
            g_print("message type:%d. GST_MESSAGE_DURATION_CHANGED\n", GST_MESSAGE_TYPE(msg));
            break;
        case GST_MESSAGE_LATENCY:
            g_print("message type:%d. GST_MESSAGE_LATENCY\n", GST_MESSAGE_TYPE(msg));
            break;
        case GST_MESSAGE_ASYNC_START:
            g_print("message type:%d. GST_MESSAGE_ASYNC_START\n", GST_MESSAGE_TYPE(msg));
            break;
        case GST_MESSAGE_ASYNC_DONE:
            g_print("message type:%d. GST_MESSAGE_ASYNC_DONE\n", GST_MESSAGE_TYPE(msg));
            break;
        case GST_MESSAGE_REQUEST_STATE:
            g_print("message type:%d. GST_MESSAGE_REQUEST_STATE\n", GST_MESSAGE_TYPE(msg));
            break;
        case GST_MESSAGE_STEP_START:
            g_print("message type:%d. GST_MESSAGE_STEP_START\n", GST_MESSAGE_TYPE(msg));
            break;
        case GST_MESSAGE_QOS:
            g_print("message type:%d. GST_MESSAGE_QOS\n", GST_MESSAGE_TYPE(msg));
            break;
        case GST_MESSAGE_PROGRESS:
            g_print("message type:%d. GST_MESSAGE_PROGRESS\n", GST_MESSAGE_TYPE(msg));
            break;
        case GST_MESSAGE_TOC:
            g_print("message type:%d. GST_MESSAGE_TOC\n", GST_MESSAGE_TYPE(msg));
            break;
        case GST_MESSAGE_RESET_TIME:
            g_print("message type:%d. GST_MESSAGE_RESET_TIME\n", GST_MESSAGE_TYPE(msg));
            break;
        case GST_MESSAGE_STREAM_START:
            g_print("message type:%d. GST_MESSAGE_STREAM_START\n", GST_MESSAGE_TYPE(msg));
            break;
        case GST_MESSAGE_NEED_CONTEXT:
            g_print("message type:%d. GST_MESSAGE_NEED_CONTEXT\n", GST_MESSAGE_TYPE(msg));
            break;
        case GST_MESSAGE_HAVE_CONTEXT:
            g_print("message type:%d. GST_MESSAGE_HAVE_CONTEXT\n", GST_MESSAGE_TYPE(msg));
            break;
        case GST_MESSAGE_EXTENDED:
            g_print("message type:%d. GST_MESSAGE_EXTENDED\n", GST_MESSAGE_TYPE(msg));
            break;
        case GST_MESSAGE_DEVICE_ADDED:
            g_print("message type:%d. GST_MESSAGE_DEVICE_ADDED\n", GST_MESSAGE_TYPE(msg));
            break;
        case GST_MESSAGE_DEVICE_REMOVED:
            g_print("message type:%d. GST_MESSAGE_DEVICE_REMOVED\n", GST_MESSAGE_TYPE(msg));
            break;
        case GST_MESSAGE_ANY:
            g_print("message type:%d. GST_MESSAGE_ANY\n", GST_MESSAGE_TYPE(msg));
            break;
        default:
            g_print("message type:%d.\n", GST_MESSAGE_TYPE(msg));
            break;
        }
    }

}

// capture::bus_pop_filter_message()
// パイプラインから送信されるエラーメッセージのみ受信する
// 戻り値
//    true  : エラーなし
//    false : エラーあり
bool capture::bus_pop_filter_message()
{
    GstMessage  *msg = gst_bus_pop_filtered(m_bus, GST_MESSAGE_ERROR);
    bool result = true;

    if( msg != NULL ){
        switch(GST_MESSAGE_TYPE(msg)){
        case GST_MESSAGE_ERROR:
            result = false;
            g_print("message type:%d. GST_MESSAGE_ERROR\n", GST_MESSAGE_TYPE(msg));
            GError *err;
            gchar *debug;

            gst_message_parse_error(msg, &err, &debug);
            g_print("message error:%s\n", err->message);
            emit errorMsgSend(QString(err->message));   // メイン画面でエラーメッセージ表示

            g_error_free(err);
            g_free(debug);
            break;
        default:
            result = false;
            g_print("message type:%d.\n", GST_MESSAGE_TYPE(msg));
            break;
        }
    }else{
//        g_print("no message.\n");
    }

    return result;
}

cv::Mat capture::getFrame()
{
 QMutexLocker locker(&m_mutex);
//    qDebug()<<m_imFlag;
    if (m_imFlag==0){
        return m_cv2.clone();
    }else{
        return m_cv1.clone();
    }
}

void capture::change_property(int i, QString str)
{
    // QString to char*
    QByteArray str_arr = str.toLocal8Bit();
    const char* pChar = str_arr.constData();
    switch(i){
    case 1:
        g_object_set(G_OBJECT(src), "autoWB" , pChar ,  NULL);
        break;
    case 2:
        g_object_set(G_OBJECT(src), "autoExpo" , pChar ,  NULL);
        break;
    default:
        break;
    }
    bus_pop_filter_message();

}

void capture::change_property(QString prop, double num)
{
    // QString to char*
    QByteArray str_arr = prop.toLocal8Bit();
    const char* pChar = str_arr.constData();
    float val = (float)num;
    g_object_set(G_OBJECT(src), pChar, val, NULL);
    bus_pop_filter_message();

}

void capture::get_property(QString prop, double *value)
{
    // QString to char*
    QByteArray str_arr = prop.toLocal8Bit();
    const char* pChar = str_arr.constData();
    float val;
    g_object_get(G_OBJECT(src), pChar, &val, NULL);
    *value = (double)val;
//    qDebug() << "get_property name:" << QString(pChar) << ", value=" << *value;
    bus_pop_filter_message();

}

void capture::get_stringproperty(QString prop, QString &value)
{
    // QString to char*
    QByteArray str_arr = prop.toLocal8Bit();
    const char* pChar = str_arr.constData();

    char *temp_char;
    g_object_get(G_OBJECT(src), pChar, &temp_char, NULL);
    QTextCodec* tc = QTextCodec::codecForLocale();
    value = QString(tc->toUnicode(temp_char));
}


void capture::get_intproperty(QString prop, int &value)
{
    // QString to char*
    QByteArray str_arr = prop.toLocal8Bit();
    const char* pChar = str_arr.constData();
    int val;
    g_object_get(G_OBJECT(src), pChar, &val, NULL);
    value = val;
    bus_pop_filter_message();
}

void capture::get_floatproperty(QString prop, float &value)
{
    char *prop_c = new char[prop.size()+1];
    std::strcpy(prop_c, prop.toUtf8().data() );

//        config_c = config.toUtf8().data();
    g_object_get(G_OBJECT(src), prop_c, &value, NULL);
    delete[] prop_c;
}

void capture::event_bringup()
{
    GstStructure * structure = gst_structure_new("hoge", "type", G_TYPE_INT, 1, NULL);

    event = gst_event_new_custom(GST_EVENT_CUSTOM_UPSTREAM, structure);

    if(gst_element_send_event(m_pipeline, event)==FALSE){
        g_print("event send error\n");
    }else{
        g_print("event send success\n");
    }
    GstQuery * query = gst_query_new_custom(GST_QUERY_CUSTOM, structure);
    gst_element_query(m_pipeline, query);
}

GstFlowReturn capture::newSample(GstAppSink *sink, gpointer gSelf)
{
    capture* self = static_cast<capture* >(gSelf);

//    g_print("newSample\n");

    GstSample* sample = NULL;
    GstBuffer* sampleBuffer = NULL;
    GstMapInfo bufferInfo;

    sample = gst_app_sink_pull_sample(GST_APP_SINK(sink));
    GstCaps *caps = gst_sample_get_caps(sample);
    GstStructure *structure = gst_caps_get_structure(caps, 0);

    self->m_MediaType = gst_structure_get_name(structure);
    self->m_format = gst_structure_get_string(structure, "format");
    int width, height;

    gst_structure_get_int(structure, "width", &width);
    gst_structure_get_int(structure, "height", &height);

    //  Bayer is WxHx1channel
    cv::Mat recv_data = cv::Mat(height, width, CV_8UC1);

    if(sample != NULL){
         sampleBuffer = gst_sample_get_buffer(sample);
         if(sampleBuffer != NULL)
         {
              self->m_mutex.lock();
              gst_buffer_map(sampleBuffer, &bufferInfo, GST_MAP_READ);

              recv_data.data = bufferInfo.data;
#if 0
              if (self->m_imFlag==0){
                self->m_cv1 = recv_data.clone();

                self->m_imFlag=1;
              }else{
                self->m_cv2 = recv_data.clone();
                self->m_imFlag=0;
                }
#else   // GStreamerからキャプチャした画像をその場でRGBフォーマットに変換(速度は問題ないか？)
              if (self->m_imFlag==0){
                  self->m_cv1 = recv_data.clone();
                  self->convertImageRgb(&self->m_cv1, &self->m_cv1);
                  self->m_imFlag=1;
                  if( self->m_pRecord != NULL && self->m_setRecFrameFlg ){
                      self->m_pRecord->setRecFrame(self->m_cv1.data);
                  }
              }else{
                  self->m_cv2 = recv_data.clone();
                  self->convertImageRgb(&self->m_cv2, &self->m_cv2);
                  self->m_imFlag=0;
                  if( self->m_pRecord != NULL && self->m_setRecFrameFlg ){
                      self->m_pRecord->setRecFrame(self->m_cv2.data);
                  }
              }
#endif
              gst_buffer_unmap(sampleBuffer, &bufferInfo);

              idxs++;
              recv_data.release();
              self->m_mutex.unlock();
         }
         self->counter++;
         gst_sample_unref(sample);

      }
    ok=1;
    return GST_FLOW_OK;
}

void capture::setStatePause(){
    gst_element_set_state(m_pipeline, GST_STATE_PAUSED);
    m_msg = gst_bus_pop(m_bus);
    bus_message(m_msg);
}

void capture::setStatePlaying(){
    gst_element_set_state(m_pipeline, GST_STATE_PLAYING);
    m_msg = gst_bus_pop(m_bus);
    bus_message(m_msg);
}

void capture::setStateReady(){
    gst_element_set_state(m_pipeline, GST_STATE_NULL);
    m_msg = gst_bus_pop(m_bus);
    bus_message(m_msg);
}

// このままだと使えないらしい @okano
//void capture::oneShot(){
//    gst_element_set_state(m_pipeline, GST_STATE_PLAYING);
//    while(1){
//        if(ok==1){
//            ok=0;
//            gst_element_set_state(m_pipeline, GST_STATE_PAUSED);
//            break;
//        }
//    }
//}


void capture::convertImageRgb(cv::Mat *pRecvMat, cv::Mat *pDstMat)
{
    int color = cv::COLOR_BayerGB2BGR;

    // カメラフォーマットからBGRへ変換
    if (m_MediaType=="video/x-bayer"){
        if(m_format=="rggb"){
            color = cv::COLOR_BayerRG2BGR;
        }else if(m_format=="gbrg"){
            color = cv::COLOR_BayerGB2BGR;

        }else if(m_format=="grbg"){
            color = cv::COLOR_BayerGR2BGR;
        }else{
            // bggr2BGR
            color = cv::COLOR_BayerBG2BGR;
        }
    }else if(m_format=="video/x-raw"){
        // RGB Mono,,,,,
        if(m_format=="rgb"){
            color = cv::COLOR_RGB2BGR;
        }else if(m_format=="UYVY"){
            color = cv::COLOR_YUV2RGB_UYVY;
        }else if (m_format=="GRAY8"){
            color = cv::COLOR_GRAY2BGR;
        }
    }
//    qDebug() << "color: " << color;
    cv::cvtColor(*pRecvMat, *pDstMat, color);
}

