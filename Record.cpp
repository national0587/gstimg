/*-------------------------------------------------------------/
/ 2020-06-17 Added Record.cpp / .h
/           - Recording function
/ ------------------------------------------------------------*/
#include "Record.h"

#include <unistd.h>
#include <stdio.h>

static bool g_recStartFlg = false;
static bool g_recStopFlg = false;
static bool g_recImgSetFlg = false;

static guint64 g_frameCount = 0;

Record::Record()
{
    m_width = 0;
    m_height = 0;
    m_pFilePath = NULL;
    m_pData = NULL;
}

Record::~Record()
{
    if( m_pFilePath != NULL ){
        free(m_pFilePath);
        m_pFilePath = NULL;
    }
    if( m_pData != NULL ){
        free(m_pData);
        m_pData = NULL;
    }
}

void Record::cb_need_data( GstAppSrc* src, guint gsize, gpointer gSelf )
{
    GstBuffer* buffer;
    guint size;
    GstMapInfo info;

    Record* self = static_cast<Record*>(gSelf);
    if( g_recImgSetFlg == false ){
        while( !g_recImgSetFlg ){
            usleep( 10000 );
            if( g_recStopFlg == true ){             // Recording Stop
                gst_app_src_end_of_stream( src );
                g_recStartFlg = false;
                return;
            }
        }
    }

    size = self->m_frameSize;

    buffer = gst_buffer_new_allocate( NULL, size, NULL );
    gst_buffer_map( buffer, &info, GST_MAP_WRITE );

    info.size = size;
    memcpy( info.data, self->m_pData, size );

    gst_buffer_unmap( buffer, &info );

    // set duration, frame_count
    buffer->offset = g_frameCount++;
    buffer->offset_end = g_frameCount;

    gst_app_src_push_buffer( GST_APP_SRC(src), buffer );

    g_recImgSetFlg = false;

    if( g_recStopFlg == true ){             // Recording Stop
        gst_app_src_end_of_stream( src );
        g_recStartFlg = false;
    }
}


int32_t Record::setImageSize(int32_t width, int32_t height)
{
    m_width = width;
    m_height = height;
    m_frameSize = m_width * m_height * 3;   // RGB format

    // Get image buffer
    if( m_pData != NULL ){
        free(m_pData);
    }
    m_pData = (uint8_t*)malloc(m_frameSize);
    if( m_pData == NULL ){
        // memory allocation error
        return -1;
    }

    return 0;
}

int32_t Record::startRec(const char *filePath)
{
    if( g_recStartFlg ){ return -1; }

        // Set file path
    if( m_pFilePath != NULL ){
        free(m_pFilePath);
    }
    m_pFilePath = (char*)malloc(strlen(filePath) + 1);
    strcpy(m_pFilePath, filePath);


    // Recording start
    GError* error = NULL;
    gchar launch_str[2048] = {};

    // 入力ピクセルフォーマットRGB -> RGBA変換 -> I420変換（NVMM）-> H.265保存
    /*
     * ターミナルで録画できたコマンド
     * gst-launch-1.0 dahuasrc ! bayer2rgb ! nvvidconv ! "video/x-raw(memory:NVMM),format=(string)I420,width=1920,height=1080" ! omxh265enc preset-level=FastPreset bitrate=16000000 iframeinterval=1 ! "video/x-h265" ! h265parse ! matroskamux ! filesink location=./teset.mp4 -e
    */

#if 1
    sprintf( launch_str,""
                        "appsrc name=appsrc emit-signal=true is-live=true do-timestamp=true block=true format=3 ! "
                        "video/x-raw, format=(string)RGB, width=(int)%d, height=(int)%d, framerate=(fraction)60/1 ! "
                        "videoconvert ! "
                        "video/x-raw, format=(string)RGBA ! "
                        "nvvidconv ! "
                        "video/x-raw(memory:NVMM), format=(string)I420 ! "
                        "omxh265enc ! "
                        "video/x-h265, stream-format=(string)hvc1 ! "
                        "matroskamux ! "
                        "filesink location=%s",
             m_width, m_height, m_pFilePath );
#else
    sprintf( launch_str,""
                        "appsrc name=appsrc emit-signal=true is-live=true do-timestamp=true block=true format=3 ! "
                        "video/x-raw, format=(string)RGB, width=(int)%d, height=(int)%d ! "
                        "videoconvert ! "
                        "video/x-raw, format=(string)RGBA ! "
                        "nvvidconv ! "
                        "video/x-raw(memory:NVMM), format=(string)I420 ! "
                        "omxh265enc preset-level=FastPreset bitrate=16000000 iframeinterval=1 ! "
                        "video/x-h265 ! "
                        "h265parse ! "
                        "matroskamux ! "
                        "filesink location=%s -e",
             m_width, m_height, m_pFilePath );
#endif
    m_pipeline = gst_parse_launch( launch_str, &error );
//    g_print("Record::startRec launch_str=%s\n", launch_str);

    if( m_pipeline == NULL ){
        g_error_free( error );
        return -1;
    }

    m_appsrc = gst_bin_get_by_name( GST_BIN(m_pipeline), "appsrc" );

    g_signal_connect( m_appsrc, "need-data", G_CALLBACK(cb_need_data), this );

    g_recStartFlg = true;
    g_recImgSetFlg = false;
    g_recStopFlg = false;
    g_frameCount = 0;
    gst_element_set_state( m_pipeline, GST_STATE_PLAYING );

    return 0;
}

int32_t Record::setRecFrame(uint8_t *data)
{
    if( data == NULL ){ return -1; }
    if( m_pData == NULL ){ return -1; }
    if( g_recImgSetFlg ){ return -1; }

    memcpy(m_pData, data, m_frameSize);

    g_recImgSetFlg = true;

    return 0;
}

int32_t Record::stopRec()
{
    if( m_pData == NULL ){ return -1; }

    if( g_recStopFlg == false ){
        g_recStopFlg = true;
        usleep(500000);
    }
    gst_element_set_state( m_pipeline, GST_STATE_NULL );
    gst_object_unref( m_pipeline );

    return 0;
}


