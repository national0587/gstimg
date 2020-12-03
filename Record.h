#ifndef RECORD_H
#define RECORD_H

#include <gst/gst.h>
#include <gst/app/gstappsrc.h>
#include <gst/app/gstappsink.h>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

#include <string.h>

#include <omp.h>
#include <arm_neon.h>

#define DEFAULT_FPS_S       60
#define DEFAULT_FPS_N       1

class Record
{
public:
    explicit Record();
    ~Record();

    // GStreamer argument
    GstElement* m_pipeline;
    GstElement* m_appsrc;


    // Image info
    int32_t m_width;
    int32_t m_height;
    uint32_t m_frameSize;
    char    *m_pFilePath;
    uint8_t *m_pData;

    // Function
    int32_t setImageSize(int32_t width, int32_t height);
    int32_t startRec(const char *filePath);
    int32_t setRecFrame(uint8_t *data);
    int32_t stopRec();

private:
    static void cb_need_data( GstAppSrc* src, guint gsize, gpointer gSelf );

};

#endif // RECORD_H
