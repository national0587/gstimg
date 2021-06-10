#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDesktopWidget>
#include <QLabel>
#include <QTimer>
#include <QtGui>
#include <QFileDialog>
#include <QGraphicsScene>
#include <QtGlobal>
#include <QDoubleSpinBox>
#include <QSlider>
#include <QMessageBox>
#include <QDateTime>

#include "capture.h"
#include "cameraselectdialog.h" // & "cameraModel.h"
#include "Record.h"
#include "parser.h"
#include "gstcapture.h"

// プロパティ名
#define DEF_CAMPL_EXPOSUR           "exposure"
#define DEF_CAMPL_GAMMA             "gamma"
#define DEF_CAMPL_GAIN_RAW          "gainRaw"
#define DEF_CAMPL_GAIN_R            "gainRed"
#define DEF_CAMPL_GAIN_G            "gainGreen"
#define DEF_CAMPL_GAIN_B            "gainBlue"

#define DEF_CAMPL_WIDTH             "width"
#define DEF_CAMPL_HEIGHT            "height"
#define DEF_CAMPL_OFFSET_X          "offsetX"
#define DEF_CAMPL_OFFSET_Y          "offsetY"
#define DEF_CAMPL_FORMAT            "format"

// カメラプラグインプロパティの最大/最小
#define DEF_CAMPL_EXPOSURE_MIN      0
#define DEF_CAMPL_EXPOSURE_MAX      99999
#define DEF_CAMPL_GAMMA_MIN         0
#define DEF_CAMPL_GAMMA_MAX         99999
#define DEF_CAMPL_GAIN_RAW_MIN      0
#define DEF_CAMPL_GAIN_RAW_MAX      99999
#define DEF_CAMPL_GAIN_R_MIN        0
#define DEF_CAMPL_GAIN_R_MAX        15
#define DEF_CAMPL_GAIN_G_MIN        0
#define DEF_CAMPL_GAIN_G_MAX        15
#define DEF_CAMPL_GAIN_B_MIN        0
#define DEF_CAMPL_GAIN_B_MAX        15

#define DEF_CAMPL_WIDTH_MIN         1
#define DEF_CAMPL_WIDTH_MAX         9999
#define DEF_CAMPL_HEIGHT_MIN        1
#define DEF_CAMPL_HEIGHT_MAX        9999
#define DEF_CAMPL_OFFSET_X_MIN      0
#define DEF_CAMPL_OFFSET_X_MAX      9999
#define DEF_CAMPL_OFFSET_Y_MIN      0
#define DEF_CAMPL_OFFSET_Y_MAX      9999

// MainWindow GUI Setting
#define DEF_CAMPL_EXPOSURE_STEP     100
#define DEF_CAMPL_GAMMA_STEP        1
#define DEF_CAMPL_GAIN_RAW_STEP     1
#define DEF_CAMPL_GAIN_R_STEP       1
#define DEF_CAMPL_GAIN_G_STEP       1
#define DEF_CAMPL_GAIN_B_STEP       1

#define DEF_CAMPL_WIDTH_STEP        16
#define DEF_CAMPL_HEIGHT_STEP       16
#define DEF_CAMPL_OFFSET_X_STEP     16
#define DEF_CAMPL_OFFSET_Y_STEP     16


// 録画1秒あたりのファイルサイズ(予想)
#define DEF_REC_FILE_1SEC_MBYTE  1

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
protected:
    void resizeEvent(QResizeEvent * event); // Window Resize Event
    void paintEvent(QPaintEvent* paintEvent);

public slots:

    // エラーメッセージ表示
    void errorMsgBox(QString);

private slots:
    void on_comboBox_WB_Auto_currentIndexChanged(const QString &arg1);

    void on_comboBox_Exp_Auto_currentIndexChanged(const QString &arg1);

    void doubleSetProperty();
    void slidebar_event();

    // width, height, offsetX, offsetY
    void imageSetProperty();
    void imageSlidebar_event();

    void on_pushButton_conf_cancel_clicked();
    void on_pushButton_conf_save_clicked();

    // Toolbar button
    void on_actionCaptureStart_triggered();
    void on_actionOneShot_triggered();
    void on_actionRecordingStart_triggered();
    void on_actionPngImageSave_triggered();
    void on_actionFit_triggered();

    void updateStatus();

private:
    Ui::MainWindow *ui;

    // CameraPlugin
    capture* m_capture;
    GstCapture *cap;
    QThread *thCap;
    bool m_camPluginSettingFlg; // true:プラグイン設定OK , false:設定NG
    bool m_imageSizeFit;    // true:Fit on , false:Fit off

    void initialize();
    void initCameraPluginGui();
    void updateCameraPluginParam();

    //QStandardItemModel itemModel;
    parser p;
    QGraphicsScene scene_;
    QList<QDoubleSpinBox *> list_dspinbox;
    QList<QSlider *> list_slider;
    // widht, height, offsetX, offsetY
    QList<QSpinBox *> list_image_spinbox;
    QList<QSlider *> list_image_slider;

    // 引数
    void getArgments();
    bool checkArgConfigFile(QString path);
    bool checkArgCameraModel(QString model);

    // configuration file
    QString m_savepath;

    // Camera model
    QString m_camModel;

    // Capture
    cv::Mat m_captureBgrMat;
    bool    m_captureStartFlg;

    void updateImage();
    void fitResize();
    void fitCancel();

    // Recording
    Record      m_record;
    QDateTime   m_recDateTime;
    QTimer      m_recTimer;

    int32_t GetStorageSize( QString path );


    void startCapture();
    void stopCapture();
    void oneShotCapture();
    void startRec();
    void stopRec();
    void saceImagePng();
    void setFitOn();
    void setFitOff();


    // Toolbar
    void updateToolbarButton();


};

#endif // MAINWINDOW_H
