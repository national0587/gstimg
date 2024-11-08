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
    GError* error = NULL;
    idxs=0;
    /*make elements*/
    this->src = gst_element_factory_make("dahuasrc", "src");
    //this->conv = gst_element_factory_make("bayer2rgb", "conv");
    this->sink = gst_element_factory_make("appsink", "sink");

    /*set property*/

    if (config!=""){
        char *config_c = new char[config.size()+1];
        std::strcpy(config_c, config.toUtf8().data() );

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

    gst_app_sink_set_emit_signals(GST_APP_SINK(sink), TRUE);
    g_signal_connect(sink, "new-sample", G_CALLBACK(newSample), (gpointer)this);
    gst_element_set_state(m_pipeline, GST_STATE_PLAYING);
    clock_gettime(CLOCK_REALTIME, &startTime);

}

capture::~capture()
{
    gst_element_set_state(m_pipeline, GST_STATE_NULL);
    gst_object_unref(m_pipeline);
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

}

void capture::change_property(QString prop, float num)
{
    // QString to char*
    QByteArray str_arr = prop.toLocal8Bit();
    const char* pChar = str_arr.constData();
    g_object_set(G_OBJECT(src), pChar, num, NULL);

}

QString capture::get_stringproperty(QString prop, QString &value)
{
    char *prop_c = new char[prop.size()+1];
    std::strcpy(prop_c, prop.toUtf8().data() );

//        config_c = config.toUtf8().data();

    delete[] prop_c;

    //g_object_get(G_OBJECT(src), prop, )
}

int capture::get_intproperty(QString prop, int &value)
{
    int val;
    char *prop_c = new char[prop.size()+1];
    std::strcpy(prop_c, prop.toUtf8().data() );

//        config_c = config.toUtf8().data();
    g_object_get(G_OBJECT(src), prop_c, &value);
    delete[] prop_c;



}

float capture::get_floatproperty(QString prop, float &value)
{
    float val;
    //char *prop_c = new char[prop.size()+1];
    //std::strcpy(prop_c, prop.toUtf8().data() );

//        config_c = config.toUtf8().data();
    g_object_get(G_OBJECT(src), "exposure", &val);
   // delete[] prop_c;
    return val;
}

void capture::event_bringup()
{
    GstStructure * structure = gst_structure_new("hoge", "type", G_TYPE_INT, 1, NULL);
    event = gst_event_new_custom(GST_EVENT_CUSTOM_UPSTREAM, structure);
    if(gst_element_send_event(m_pipeline, event)==FALSE){
        //g_print("event send error\n");
    }else{
        //g_print("event send success\n");
    }

    GstQuery * query = gst_query_new_custom(GST_QUERY_CUSTOM, structure);
    gst_element_query(m_pipeline, query);
}
GstFlowReturn capture::newSample(GstAppSink *sink, gpointer gSelf)
{
    capture* self = static_cast<capture* >(gSelf);


    GstSample* sample = NULL;
    GstBuffer* sampleBuffer = NULL;
    GstMapInfo bufferInfo;

    sample = gst_app_sink_pull_sample(GST_APP_SINK(sink));

    GstCaps *caps = gst_sample_get_caps(sample);
    GstStructure *structure = gst_caps_get_structure(caps, 0);
//    QString pixelarray = gst_structure_get_name(structure);
//    QString format = gst_structure_get_string(structure, "format");

    self->m_MediaType = gst_structure_get_name(structure);
    self->m_format = gst_structure_get_string(structure, "format");
//    qDebug() << "structure " <<  pixelarray;
//    qDebug() << "format" << format;
    int width, height;
    gst_structure_get_int(structure, "width", &width);
    gst_structure_get_int(structure, "height", &height);
//    qDebug() << "width" << width;
//    qDebug() << "height" << height;

//  Bayer is WxHx1channel
    cv::Mat recv_data = cv::Mat(height, width, CV_8UC1);
  //  cv::Mat rgb = cv::Mat(960, 1280, CV_8UC3);

    if(sample != NULL){
         sampleBuffer = gst_sample_get_buffer(sample);
         if(sampleBuffer != NULL)
         {
              self->m_mutex.lock();
              gst_buffer_map(sampleBuffer, &bufferInfo, GST_MAP_READ);

              recv_data.data = bufferInfo.data;
              if (self->m_imFlag==0){
                self->m_cv1 = recv_data.clone();

                self->m_imFlag=1;
              }else{
                self->m_cv2 = recv_data.clone();
                self->m_imFlag=0;
                }

//              cv::cvtColor(recv_data, recv_data, color);
//              cv::resize(recv_data, recv_data,cv::Size(640, 480),0,0,cv::INTER_CUBIC);
//              if (self->m_imFlag==0){

//              self->m_image = QImage(recv_data.data, 640, 480, QImage::Format_RGB888);
//              //qDebug()<<self->m_imFlag;
//              self->m_imFlag=1;

//              }else{
//              self->m_image2 = QImage(recv_data.data, 640, 480, QImage::Format_RGB888);
//              //qDebug()<<self->m_imFlag;
//              self->m_imFlag=0;
//              }

              gst_buffer_unmap(sampleBuffer, &bufferInfo);

              //QString str = QString("%1.png").arg(idxs);
              //cv::imwrite(str.toStdString(), rgb);
              idxs++;
              recv_data.release();
              self->m_mutex.unlock();
              //rgb.release();
         }
         self->counter++;
         gst_sample_unref(sample);

      }
//        clock_gettime(CLOCK_REALTIME, &endTime);
//        if (endTime.tv_nsec < startTime.tv_nsec) {
//            qInfo("%10ld.%09ld", endTime.tv_sec - startTime.tv_sec - 1
//                    ,endTime.tv_nsec + 1000000000 - startTime.tv_nsec);
//        } else {
//            qInfo("%10ld.%09ld", endTime.tv_sec - startTime.tv_sec
//                    ,endTime.tv_nsec - startTime.tv_nsec);
//        }
//        g_print("\n");
//        startTime=endTime;
    ok=1;
    return GST_FLOW_OK;
}

void capture::setStatePause(){
    gst_element_set_state(m_pipeline, GST_STATE_PAUSED);
}

void capture::setStatePlaying(){
    gst_element_set_state(m_pipeline, GST_STATE_PLAYING);
}

void capture::setStateReady(){
    gst_element_set_state(m_pipeline, GST_STATE_NULL);
}
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
