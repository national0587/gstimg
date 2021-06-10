#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
  , cap(new GstCapture())
  , thCap(new QThread())
{
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::resizeEvent(QResizeEvent * event)
{
    if( m_imageSizeFit ){
        // Fit On:画像描画領域に画像をフィットさせる
        fitResize();
    }
}

void MainWindow::paintEvent(QPaintEvent *paintEvent)
{
    if( m_capture!=nullptr && m_camPluginSettingFlg && m_captureStartFlg ){
        updateImage();
    }
}

void MainWindow::updateImage()
{
    cv::Mat mat;
    cv::Mat recv;
    recv = m_capture->getFrame();
     if((!recv.empty()) && recv.size().area()>0){
         recv.copyTo(mat);

#if 0   // 画像のBGR変換をGStreamerからキャプチャした直後(capture::convertImageRgb)に行う（実験）
         // カメラフォーマットからBGRへ変換
         // OpenCVでのフォーマットBGRは実際はR,G,Bの順に格納されている
         int color;
         if (m_capture->m_MediaType=="video/x-bayer"){
             if(m_capture->m_format=="rggb"){
                 color = cv::COLOR_BayerRG2BGR;
             }else if(m_capture->m_format=="gbrg"){
                 color = cv::COLOR_BayerGB2BGR;

             }else if(m_capture->m_format=="grbg"){
                 color = cv::COLOR_BayerGR2BGR;
             }else{
                 // bggr2BGR
                 color = cv::COLOR_BayerBG2BGR;
             }
         }else if(m_capture->m_format=="video/x-raw"){
             // RGB Mono,,,,,
             if(m_capture->m_format=="rgb"){
                 color = cv::COLOR_RGB2BGR;
             }else if(m_capture->m_format=="UYVY"){
                 color = cv::COLOR_YUV2RGB_UYVY;
             }else if (m_capture->m_format=="GRAY8"){
                 color = cv::COLOR_GRAY2BGR;
             }
         }
//            qDebug() << "color: " << color;
         cv::cvtColor(mat, mat, color);
#endif

         m_captureBgrMat = mat.clone();
//         qDebug() << "capture image size width=" << mat.cols << ", height=" << mat.rows;

         QImage img = QImage(mat.data, mat.cols, mat.rows, QImage::Format_RGB888);
         scene_.clear();
         scene_.addPixmap(QPixmap::fromImage(img));
         ui->graphicsView->update();
     }
}

void MainWindow::fitResize()
{
//    if(scene_ == nullptr){
//        return;
//    }
    // 画像描画領域のサイズに合わせて描画画像をリサイズする
    ui->graphicsView->fitInView(scene_.sceneRect(), Qt::KeepAspectRatio);
}

void MainWindow::fitCancel()
{
    // 描画画像を原寸大で描画する
    int32_t x = 0;
    int32_t y = 0;
    int32_t w = ui->graphicsView->width();
    int32_t h = ui->graphicsView->height();
    ui->graphicsView->fitInView(QRectF(x,y,w,h), Qt::KeepAspectRatio);
}


// 引数をメンバ変数へ入れる
void MainWindow::getArgments()
{
    bool savePathFlg = false;
    bool camModelFlg = false;

    // initialize
    m_savepath.clear();
    m_camModel.clear();

    // Get
    QStringList argv = QCoreApplication::arguments();
    if (argv.size()>1){
        argv.removeAt(0);

        for(QString arg: argv){
            qDebug()<<"arg" << arg;
        }

        while(argv.size() >= 2){
            // 引数オプションチェック (-f) or (-m)
            if( argv.at(0) == "-f" ){
                // 設定ファイル
                if( !savePathFlg ){
                    m_savepath = argv.at(1);
                    savePathFlg = true;
                }
            }else if( argv.at(0) == "-m" ){
                // カメラ型番
                if( !camModelFlg ){
                    if( checkArgCameraModel(argv.at(1)) ){
                        m_camModel = argv.at(1);
                        camModelFlg = true;
                    }
                }
            }
            argv.removeAt(0);
            argv.removeAt(0);

            if( savePathFlg && camModelFlg ){ break; }
        }
    }
}

// カメラ設定アプリ起動時の引数-mチェック
bool MainWindow::checkArgCameraModel(QString model)
{
    if( model.compare(NAME_A5501CG20E_MODEL) == 0 ){
        return true;
    }else if( model.compare(NAME_A3131CG000E_MODEL) == 0 ){
        return true;
    }else if( model.compare(NAME_A7500MG20E_MODEL) == 0 ){
        return true;
    }else if( model.compare(NAME_A3200CU000E_MODEL) == 0 ){
        return true;
    }else if( model.compare(NAME_A3600MU60E_MODEL) == 0 ){
        return true;
    }else if( model.compare(NAME_A5131CU210E_MODEL) == 0 ){
        return true;
    }

    return false;
}


// カメラプラグイン制御のためのメイン画面GUI初期設定
void MainWindow::initCameraPluginGui()
{
    int32_t w = 0;
    int32_t h = 0;
    int32_t x = 0;
    int32_t y = 0;
    if( m_camModel.compare(NAME_A5501CG20E_MODEL) == 0 ){
        w = NAME_A5501CG20E_WIDTH;
        h = NAME_A5501CG20E_HEIGHT;
        x = NAME_A5501CG20E_WIDTH - 1;
        y = NAME_A5501CG20E_HEIGHT - 1;
    }else if( m_camModel.compare(NAME_A3131CG000E_MODEL) == 0 ){
        w = NAME_A3131CG000E_WIDTH;
        h = NAME_A3131CG000E_HEIGHT;
        x = NAME_A3131CG000E_WIDTH - 1;
        y = NAME_A3131CG000E_HEIGHT - 1;
    }else if( m_camModel.compare(NAME_A7500MG20E_MODEL) == 0 ){
        w = NAME_A7500MG20E_WIDTH;
        h = NAME_A7500MG20E_HEIGHT;
        x = NAME_A7500MG20E_WIDTH - 1;
        y = NAME_A7500MG20E_HEIGHT - 1;
    }else if( m_camModel.compare(NAME_A3200CU000E_MODEL) == 0 ){
        w = NAME_A3200CU000E_WIDTH;
        h = NAME_A3200CU000E_HEIGHT;
        x = NAME_A3200CU000E_WIDTH - 1;
        y = NAME_A3200CU000E_HEIGHT - 1;
    }else if( m_camModel.compare(NAME_A3600MU60E_MODEL) == 0 ){
        w = NAME_A3600MU60E_WIDTH;
        h = NAME_A3600MU60E_HEIGHT;
        x = NAME_A3600MU60E_WIDTH - 1;
        y = NAME_A3600MU60E_HEIGHT - 1;
    }else if( m_camModel.compare(NAME_A5131CU210E_MODEL) == 0 ){
        w = NAME_A5131CU210E_WIDTH;
        h = NAME_A5131CU210E_HEIGHT;
        x = NAME_A5131CU210E_WIDTH - 1;
        y = NAME_A5131CU210E_HEIGHT - 1;
    }

    // ComboBox
    ui->comboBox_WB_Auto->setEnabled(true);
    ui->comboBox_WB_Auto->addItem("Off");
    ui->comboBox_WB_Auto->addItem("Once");
    ui->comboBox_WB_Auto->addItem("Continuous");
    ui->comboBox_Exp_Auto->setEnabled(true);
    ui->comboBox_Exp_Auto->addItem("Off");
    ui->comboBox_Exp_Auto->addItem("Once");
    ui->comboBox_Exp_Auto->addItem("Continuous");

    ui->comboBox_Format->setEnabled(false);
    ui->comboBox_Format->addItem("BayerRG8");
    ui->comboBox_Format->addItem("BayerGB8");
    ui->comboBox_Format->addItem("Mono8");

    // Slider
    ui->horizontalSlider_exposure->setMaximum(DEF_CAMPL_EXPOSURE_MAX);
    ui->horizontalSlider_exposure->setMinimum(DEF_CAMPL_EXPOSURE_MIN);
    ui->horizontalSlider_exposure->setSingleStep(DEF_CAMPL_EXPOSURE_STEP);
    ui->horizontalSlider_exposure->setPageStep(DEF_CAMPL_EXPOSURE_STEP);
    ui->horizontalSlider_gamma->setMaximum(DEF_CAMPL_GAMMA_MAX);
    ui->horizontalSlider_gamma->setMinimum(DEF_CAMPL_GAMMA_MIN);
    ui->horizontalSlider_gamma->setSingleStep(DEF_CAMPL_GAMMA_STEP);
    ui->horizontalSlider_gamma->setPageStep(DEF_CAMPL_GAMMA_STEP);
    ui->horizontalSlider_gainRaw->setMaximum(DEF_CAMPL_GAIN_RAW_MAX);
    ui->horizontalSlider_gainRaw->setMinimum(DEF_CAMPL_GAIN_RAW_MIN);
    ui->horizontalSlider_gainRaw->setSingleStep(DEF_CAMPL_GAIN_RAW_STEP);
    ui->horizontalSlider_gainRaw->setPageStep(DEF_CAMPL_GAIN_RAW_STEP);
    ui->horizontalSlider_gainRed->setMaximum(DEF_CAMPL_GAIN_R_MAX*100);
    ui->horizontalSlider_gainRed->setMinimum(DEF_CAMPL_GAIN_R_MIN);
    ui->horizontalSlider_gainRed->setSingleStep(DEF_CAMPL_GAIN_R_STEP);
    ui->horizontalSlider_gainRed->setPageStep(DEF_CAMPL_GAIN_R_STEP);
    ui->horizontalSlider_gainGreen->setMaximum(DEF_CAMPL_GAIN_G_MAX*100);
    ui->horizontalSlider_gainGreen->setMinimum(DEF_CAMPL_GAIN_G_MIN);
    ui->horizontalSlider_gainGreen->setSingleStep(DEF_CAMPL_GAIN_G_STEP);
    ui->horizontalSlider_gainGreen->setPageStep(DEF_CAMPL_GAIN_G_STEP);
    ui->horizontalSlider_gainBlue->setMaximum(DEF_CAMPL_GAIN_B_MAX*100);
    ui->horizontalSlider_gainBlue->setMinimum(DEF_CAMPL_GAIN_B_MIN);
    ui->horizontalSlider_gainBlue->setSingleStep(DEF_CAMPL_GAIN_B_STEP);
    ui->horizontalSlider_gainBlue->setPageStep(DEF_CAMPL_GAIN_B_STEP);

    ui->horizontalSlider_width->setMaximum(w);
    ui->horizontalSlider_width->setMinimum(DEF_CAMPL_WIDTH_MIN);
    ui->horizontalSlider_width->setSingleStep(DEF_CAMPL_WIDTH_STEP);
    ui->horizontalSlider_height->setMaximum(h);
    ui->horizontalSlider_height->setMinimum(DEF_CAMPL_HEIGHT_MIN);
    ui->horizontalSlider_height->setSingleStep(DEF_CAMPL_HEIGHT_STEP);
    ui->horizontalSlider_offsetX->setMaximum(x);
    ui->horizontalSlider_offsetX->setMinimum(DEF_CAMPL_OFFSET_X_MIN);
    ui->horizontalSlider_offsetX->setSingleStep(DEF_CAMPL_OFFSET_X_STEP);
    ui->horizontalSlider_offsetY->setMaximum(y);
    ui->horizontalSlider_offsetY->setMinimum(DEF_CAMPL_OFFSET_Y_MIN);
    ui->horizontalSlider_offsetY->setSingleStep(DEF_CAMPL_OFFSET_Y_STEP);

    list_slider << ui->horizontalSlider_exposure;
    list_slider << ui->horizontalSlider_gamma;
    list_slider << ui->horizontalSlider_gainRaw;
    list_slider << ui->horizontalSlider_gainRed;
    list_slider << ui->horizontalSlider_gainGreen;
    list_slider << ui->horizontalSlider_gainBlue;
    for(auto const &horizonalslidebar: list_slider){
        connect(horizonalslidebar, &QSlider::actionTriggered, this, &MainWindow::slidebar_event);

    }

    // width, height, offsetX, offsetY
    list_image_slider << ui->horizontalSlider_width;
    list_image_slider << ui->horizontalSlider_height;
    list_image_slider << ui->horizontalSlider_offsetX;
    list_image_slider << ui->horizontalSlider_offsetY;
    for(auto const &horizonalslidebar: list_image_slider){
        connect(horizonalslidebar, &QSlider::actionTriggered, this, &MainWindow::imageSlidebar_event);

    }


    // SpinBox
    ui->SpinBox_exposure->setMaximum(DEF_CAMPL_EXPOSURE_MAX);
    ui->SpinBox_exposure->setMinimum(DEF_CAMPL_EXPOSURE_MIN);
    ui->SpinBox_exposure->setSingleStep(DEF_CAMPL_EXPOSURE_STEP);
    ui->SpinBox_gamma->setMaximum(DEF_CAMPL_GAMMA_MAX);
    ui->SpinBox_gamma->setMinimum(DEF_CAMPL_GAMMA_MIN);
    ui->SpinBox_gamma->setSingleStep(DEF_CAMPL_GAMMA_STEP);
    ui->SpinBox_gainRaw->setMaximum(DEF_CAMPL_GAIN_RAW_MAX);
    ui->SpinBox_gainRaw->setMinimum(DEF_CAMPL_GAIN_RAW_MIN);
    ui->SpinBox_gainRaw->setSingleStep(DEF_CAMPL_GAIN_RAW_STEP);
    ui->SpinBox_gainRed->setMaximum(DEF_CAMPL_GAIN_R_MAX);
    ui->SpinBox_gainRed->setMinimum(DEF_CAMPL_GAIN_R_MIN);
    ui->SpinBox_gainRed->setSingleStep(DEF_CAMPL_GAIN_R_STEP/10.0);
    ui->SpinBox_gainGreen->setMaximum(DEF_CAMPL_GAIN_G_MAX);
    ui->SpinBox_gainGreen->setMinimum(DEF_CAMPL_GAIN_G_MIN);
    ui->SpinBox_gainGreen->setSingleStep(DEF_CAMPL_GAIN_G_STEP/10.0);
    ui->SpinBox_gainBlue->setMaximum(DEF_CAMPL_GAIN_B_MAX);
    ui->SpinBox_gainBlue->setMinimum(DEF_CAMPL_GAIN_B_MIN);
    ui->SpinBox_gainBlue->setSingleStep(DEF_CAMPL_GAIN_B_STEP/10.0);

    ui->spinBox_width->setMaximum(w);
    ui->spinBox_width->setMinimum(DEF_CAMPL_WIDTH_MIN);
    ui->spinBox_width->setSingleStep(DEF_CAMPL_WIDTH_STEP);
    ui->spinBox_height->setMaximum(h);
    ui->spinBox_height->setMinimum(DEF_CAMPL_HEIGHT_MIN);
    ui->spinBox_height->setSingleStep(DEF_CAMPL_HEIGHT_STEP);
    ui->spinBox_offsetX->setMaximum(x);
    ui->spinBox_offsetX->setMinimum(DEF_CAMPL_OFFSET_X_MIN);
    ui->spinBox_offsetX->setSingleStep(DEF_CAMPL_OFFSET_X_STEP);
    ui->spinBox_offsetY->setMaximum(y);
    ui->spinBox_offsetY->setMinimum(DEF_CAMPL_OFFSET_Y_MIN);
    ui->spinBox_offsetY->setSingleStep(DEF_CAMPL_OFFSET_Y_STEP);

    /*DoubleSpinBox*/
    list_dspinbox << ui->SpinBox_exposure;
    list_dspinbox << ui->SpinBox_gamma;
    list_dspinbox << ui->SpinBox_gainRaw;
    list_dspinbox << ui->SpinBox_gainRed;
    list_dspinbox << ui->SpinBox_gainGreen;
    list_dspinbox << ui->SpinBox_gainBlue;
    for(auto const &doublespinbox : list_dspinbox)
    {
         connect(doublespinbox, static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), this, static_cast<void (MainWindow::*)(void)>(&MainWindow::doubleSetProperty));
    }

    // width, height, offsetX, offsetY
    list_image_spinbox << ui->spinBox_width;
    list_image_spinbox << ui->spinBox_height;
    list_image_spinbox << ui->spinBox_offsetX;
    list_image_spinbox << ui->spinBox_offsetY;
    for(auto const &spinbox : list_image_spinbox)
    {
         connect(spinbox, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), this, static_cast<void (MainWindow::*)(void)>(&MainWindow::imageSetProperty));
    }

}

// カメラプラグインの設定値（デフォルト値）を取得してGUIを更新する（パイプライン作成後に実行）
void MainWindow::updateCameraPluginParam()
{
    if( m_camPluginSettingFlg == false ){ return; }

    double  val;
    int     intVal;

    // exposure
    m_capture->get_property(DEF_CAMPL_EXPOSUR, &val);
    ui->horizontalSlider_exposure->setValue(val);
    ui->SpinBox_exposure->setValue(val);
    // gamma
    m_capture->get_property(DEF_CAMPL_GAMMA, &val);
    ui->horizontalSlider_gamma->setValue(val);
    ui->SpinBox_gamma->setValue(val);
    // gainRaw
    m_capture->get_property(DEF_CAMPL_GAIN_RAW, &val);
    ui->horizontalSlider_gainRaw->setValue(val);
    ui->SpinBox_gainRaw->setValue(val);
    // gainRed
    m_capture->get_property(DEF_CAMPL_GAIN_R, &val);
    ui->horizontalSlider_gainRed->setValue(val*100+0.9);
    ui->SpinBox_gainRed->setValue(val);
    // gainGreen
    m_capture->get_property(DEF_CAMPL_GAIN_G, &val);
    ui->horizontalSlider_gainGreen->setValue(val*100+0.9);
    ui->SpinBox_gainGreen->setValue(val);
    // gainBlue
    m_capture->get_property(DEF_CAMPL_GAIN_B, &val);
    ui->horizontalSlider_gainBlue->setValue(val*100+0.9);
    ui->SpinBox_gainBlue->setValue(val);

    // width
    m_capture->get_intproperty(DEF_CAMPL_WIDTH, intVal);
    ui->horizontalSlider_width->setValue(intVal);
    ui->spinBox_width->setValue(intVal);
    // height
    m_capture->get_intproperty(DEF_CAMPL_HEIGHT, intVal);
    ui->horizontalSlider_height->setValue(intVal);
    ui->spinBox_height->setValue(intVal);
    // offsetX
    m_capture->get_intproperty(DEF_CAMPL_OFFSET_X, intVal);
    ui->horizontalSlider_offsetX->setValue(intVal);
    ui->spinBox_offsetX->setValue(intVal);
    // offsetY
    m_capture->get_intproperty(DEF_CAMPL_OFFSET_Y, intVal);
    ui->horizontalSlider_offsetY->setValue(intVal);
    ui->spinBox_offsetY->setValue(intVal);
    // format
    QString str;
    m_capture->get_stringproperty(DEF_CAMPL_FORMAT, str);
    int idx = ui->comboBox_Format->findText(str);
    if (idx == -1){
        ui->comboBox_Format->addItem(str);
        ui->comboBox_Format->setCurrentText(str);
    }else{
        ui->comboBox_Format->setCurrentIndex(idx);
    }

}


void MainWindow::on_comboBox_WB_Auto_currentIndexChanged(const QString &arg1)
{
    if (m_capture!=nullptr){
        if(arg1=="Once" || arg1=="Continuous"){
            //do
            ui->horizontalSlider_gainRed->setEnabled(false);
            ui->horizontalSlider_gainGreen->setEnabled(false);
            ui->horizontalSlider_gainBlue->setEnabled(false);

            ui->SpinBox_gainRed->setEnabled(false);
            ui->SpinBox_gainGreen->setEnabled(false);
            ui->SpinBox_gainBlue->setEnabled(false);
        }else{
            //
            ui->horizontalSlider_gainRed->setEnabled(true);
            ui->horizontalSlider_gainGreen->setEnabled(true);
            ui->horizontalSlider_gainBlue->setEnabled(true);

            ui->SpinBox_gainRed->setEnabled(true);
            ui->SpinBox_gainGreen->setEnabled(true);
            ui->SpinBox_gainBlue->setEnabled(true);
        }

        // GUI update
        updateCameraPluginParam();
        m_capture->change_property(1, arg1);
    }
}

void MainWindow::on_comboBox_Exp_Auto_currentIndexChanged(const QString &arg1)
{
    if(m_capture!=nullptr){
        if(arg1=="Once" || arg1=="Continuous"){
            ui->horizontalSlider_exposure->setEnabled(false);
            ui->SpinBox_exposure->setEnabled(false);
        }else{
            ui->horizontalSlider_exposure->setEnabled(true);
            ui->SpinBox_exposure->setEnabled(true);
        }

        // GUI update
        updateCameraPluginParam();
        m_capture->change_property(2, arg1);
    }
}




void MainWindow::doubleSetProperty()
{
    if( m_camPluginSettingFlg == false ){ return; }

    double getVal;
    QDoubleSpinBox *spinbox = qobject_cast<QDoubleSpinBox*>(sender());
    if (spinbox!=NULL){
        qDebug("%f", spinbox->value());
        qDebug() << spinbox->objectName();
    }
    QString controlName = spinbox->objectName().split("_").last();

    m_capture->change_property(controlName, spinbox->value());
    m_capture->get_property(controlName, &getVal);
    spinbox->setValue(getVal);

    for(auto slider: list_slider){
        if(QString::compare(slider->objectName().split("_").last(), controlName, Qt::CaseSensitive)==0){
            if( controlName.compare(DEF_CAMPL_EXPOSUR) == 0 ||
                controlName.compare(DEF_CAMPL_GAMMA) == 0 ||
                controlName.compare(DEF_CAMPL_GAIN_RAW) == 0 ){
                // スピンボックスの値をそのままスライダーにセット
                slider->setValue(getVal);
            }else{
                // スピンボックスの値 * 10をスライダーにセット
                slider->setValue(getVal*100+0.9);
            }
        }
    }

}

void MainWindow::slidebar_event()
{
    if( m_camPluginSettingFlg == false ){ return; }

    QSlider * slidar = qobject_cast<QSlider*>(sender());

    QString controlName = slidar->objectName().split("_").last();
    qDebug() << controlName << " " << slidar->value();
    for(auto dspinbox: list_dspinbox){
        if(QString::compare(dspinbox->objectName().split("_").last(), controlName, Qt::CaseSensitive)==0){
            if( controlName.compare(DEF_CAMPL_EXPOSUR) == 0 ||
                controlName.compare(DEF_CAMPL_GAMMA) == 0 ||
                controlName.compare(DEF_CAMPL_GAIN_RAW) == 0 ){
                // スライダーの値をそのままスピンボックスにセット
                dspinbox->setValue((double)slidar->value());
            }else{
                // スライダーの値 / 10をスピンボックスにセット
                dspinbox->setValue((double)slidar->value()/100.0);
            }
            qDebug() << controlName << "-> "<< dspinbox->objectName()<< ": " << slidar->value();
        }

    }


}

void MainWindow::imageSetProperty()
{
    if( m_camPluginSettingFlg == false ){ return; }

    double getVal;
    QSpinBox *spinbox = qobject_cast<QSpinBox*>(sender());
    QString controlName = spinbox->objectName().split("_").last();
    getVal = spinbox->value();

    for(auto slider: list_image_slider){
        if(QString::compare(slider->objectName().split("_").last(), controlName, Qt::CaseSensitive)==0){
            slider->setValue(getVal);
        }
    }

}

void MainWindow::imageSlidebar_event()
{
    if( m_camPluginSettingFlg == false ){ return; }

    QSlider * slidar = qobject_cast<QSlider*>(sender());

    QString controlName = slidar->objectName().split("_").last();
    for(auto spinbox: list_image_spinbox){
        if(QString::compare(spinbox->objectName().split("_").last(), controlName, Qt::CaseSensitive)==0){
            int val = spinbox->value();
            slidar->setValue(val);
        }

    }


}


void MainWindow::on_pushButton_conf_cancel_clicked()
{
    if (m_capture!=nullptr){
        m_capture->event_bringup();
    }
}

void MainWindow::on_pushButton_conf_save_clicked()
{
    if(m_capture!=nullptr){

        if (m_savepath==""){
            m_savepath = QFileDialog::getSaveFileName(this, tr("save path"), QStandardPaths::writableLocation(QStandardPaths::DesktopLocation), "Config file(*.conf);; All Files(*.*)");
            if (m_savepath.isEmpty()) { return; }
            if (!m_savepath.endsWith(".conf")) {
                m_savepath += QString(".conf");
            }
        }

        // Saveボタン押下で設定保存
//        updateCameraPluginParam();    // 画像サイズがGUIで設定したのと違う値になる
        std::map<std::string, std::string> props;
        props.insert(std::make_pair("exposure", std::to_string(ui->SpinBox_exposure->value())));
        props.insert(std::make_pair("gainRaw", std::to_string(ui->SpinBox_gainRaw->value())));
        props.insert(std::make_pair("gainRed", std::to_string(ui->SpinBox_gainRed->value())));
        props.insert(std::make_pair("gainGreen", std::to_string(ui->SpinBox_gainGreen->value())));
        props.insert(std::make_pair("gainBlue", std::to_string(ui->SpinBox_gainBlue->value())));
        props.insert(std::make_pair("gamma", std::to_string(ui->SpinBox_gamma->value())));
        props.insert(std::make_pair("width", std::to_string(ui->spinBox_width->value())));
        props.insert(std::make_pair("height", std::to_string(ui->spinBox_height->value())));
        props.insert(std::make_pair("offsetX", std::to_string(ui->spinBox_offsetX->value())));
        props.insert(std::make_pair("offsetY", std::to_string(ui->spinBox_offsetY->value())));
        props.insert(std::make_pair("format", ui->comboBox_Format->currentText().toStdString()));
        props.insert(std::make_pair("autoWB", ui->comboBox_WB_Auto->currentText().toStdString()));
        props.insert(std::make_pair("autoExpo", ui->comboBox_Exp_Auto->currentText().toStdString()));

        // Get S/N
        QString str;
        m_capture->get_stringproperty("cameraSerial", str);
        p.setPropValues(props, m_camModel.toStdString(), str.toStdString());
        p.save(m_savepath.toStdString());

        // アプリ終了
        if (m_capture!=nullptr){
            m_capture->event_bringup();
        }
        close();
    }
}

//==================================================
// Toolbar
//==================================================

// Capture start / stop
// パイプラインPLAYING/PAUSEではなくタイマーStart/Stopで制御
// パイプライン制御だとOneShotができないらしい
void MainWindow::on_actionCaptureStart_triggered()
{
    if( ui->actionCaptureStart->isChecked() == true ){
        // Capture start
        startCapture();
    }else{
        // Capture stop
        stopCapture();
    }

    // ツールバーのボタン状態更新
    updateToolbarButton();
}

// One shot capture
void MainWindow::on_actionOneShot_triggered()
{

    // One frame capture
    oneShotCapture();

    // ツールバーのボタン状態更新
    updateToolbarButton();
}

// Capture image recording start / stop
void MainWindow::on_actionRecordingStart_triggered()
{
    if( ui->actionRecordingStart->isChecked() == true ){
        // Recording start
        startRec();
    }else{
        // Recording stop
        stopRec();
    }


    // ツールバーのボタン状態更新
    updateToolbarButton();
}

// Capture image (PNG) file save
void MainWindow::on_actionPngImageSave_triggered()
{
    // Image file save (PNG)
    saceImagePng();

    // ツールバーのボタン状態更新
    updateToolbarButton();
}

// View image size fit on / off
void MainWindow::on_actionFit_triggered()
{
    if( ui->actionFit->isChecked() == true ){
        // Fit on
        setFitOn();
    }else{
        // Fit off
        setFitOff();
    }

    // ツールバーのボタン状態更新
    updateToolbarButton();
}


// ツールバーのボタンの状態（有効/無効）を更新するサブルーチン
void MainWindow::updateToolbarButton()
{
    if( m_camPluginSettingFlg ){
        // カメラプラグイン設定成功
        // Enable
        ui->actionCaptureStart->setEnabled(true);
        ui->actionFit->setEnabled(true);

        if( ui->actionCaptureStart->isChecked() == true ){
            // Capture start
            // Enable
            ui->actionRecordingStart->setEnabled(true);

            // Disable
            ui->actionOneShot->setDisabled(true);
            ui->actionPngImageSave->setDisabled(true);
        }else{
            // Capture stop
            // Enable
            ui->actionOneShot->setEnabled(true);
            ui->actionPngImageSave->setEnabled(true);

            // Disable
            ui->actionRecordingStart->setDisabled(true);
        }

    }else{
        // カメラプラグイン設定失敗
        // Disable
        ui->actionCaptureStart->setDisabled(true);
        ui->actionOneShot->setDisabled(true);
        ui->actionRecordingStart->setDisabled(true);
        ui->actionPngImageSave->setDisabled(true);
        ui->actionFit->setDisabled(true);
    }

}

void MainWindow::startCapture()
{
    if( m_camPluginSettingFlg ){
        m_captureStartFlg = true;
    }
}

void MainWindow::stopCapture()
{
    if( m_camPluginSettingFlg ){
        // Stop recording
        ui->actionRecordingStart->setChecked(false);
        on_actionRecordingStart_triggered();

        // Capture stop
        m_captureStartFlg = false;
    }
}


void MainWindow::oneShotCapture()
{
    updateImage();
}


void MainWindow::startRec()
{
    QString fileName;
    bool    errFlg = false;
    int32_t ret = 0;
    int32_t storageMByte = 0;
    int32_t recMaxTime = 0;
    QMessageBox msg(this);
    msg.setStandardButtons(QMessageBox::Yes | QMessageBox::No);

    do{
        // File Select
        fileName = QFileDialog::getSaveFileName(this, "Image Recording (H.265)", "recording.mp4", "MP4(*.mp4);; All Files(*.*)");
        if (fileName.isEmpty()) { errFlg = true; break; }
        if (!fileName.endsWith(".mp4")) {
            qDebug() << "File name has no extension(.mp4)";
            fileName += QString(".mp4");
        }

        // ディスク残容量確認
        storageMByte = GetStorageSize(fileName);
        qDebug() << "storage free space(MB)=" << storageMByte;
        if( storageMByte < 0 ){
            errFlg = true;
            // Error message
            errorMsgBox(QString("Not enough disk space."));
            break;
        }
        // 最大保存時間推測＆通知
        recMaxTime = storageMByte / DEF_REC_FILE_1SEC_MBYTE;
        msg.setText(QString("Expected maximum recording time:%1 s\nDo you want to start recording?").arg(recMaxTime));
        if( msg.exec() != QMessageBox::Yes ){
            // 録画しない
            errFlg = true;
            break;
        }

        // Start recording
        int32_t width, height;
        m_capture->get_intproperty("width", width);
        m_capture->get_intproperty("height", height);
        m_record.setImageSize(width, height);
        m_capture->m_pRecord = &m_record;
        m_capture->m_setRecFrameFlg = true;
        ret = m_record.startRec(fileName.toStdString().c_str());
        if( ret != 0 ){
            errFlg = true;
            // Error message
            errorMsgBox(QString("Recording failure."));
        }

        // 表示
        ui->label_Recording_time->setText("Recording : 00h 00m 00s");
        ui->label_Recording_time->show();
        connect( &m_recTimer, SIGNAL(timeout()), this, SLOT(updateStatus()) );
        m_recTimer.start(1000);

        // Recording status display
        m_recDateTime = QDateTime::currentDateTime();
    }while(0);

    if( errFlg ){
        // Recording error
        ui->actionRecordingStart->setChecked(false);
        on_actionRecordingStart_triggered();
    }
}


void MainWindow::stopRec()
{
    m_record.stopRec();

    m_capture->m_setRecFrameFlg = false;
    m_capture->m_pRecord = NULL;

    // 非表示
    ui->label_Recording_time->hide();

    m_recTimer.stop();
    disconnect( &m_recTimer, SIGNAL(timeout()), this, SLOT(updateStatus()) );
}


void MainWindow::saceImagePng()
{
    QString         fileName;

    if( m_captureBgrMat.empty() ){ return; }

    // File Select
    fileName = QFileDialog::getSaveFileName(this, "Save Image (PNG)", "image.png", "PNG(*.png);; All Files(*.*)");
    if (fileName.isEmpty()) { return; }
    if (!fileName.endsWith(".png")) {
        qDebug() << "File name has no extension(.png)";
        fileName += QString(".png");
    }

    // Save PNG
    QImage image = QImage(m_captureBgrMat.data, m_captureBgrMat.cols, m_captureBgrMat.rows, QImage::Format_RGB888);
    image.save(fileName);
}


void MainWindow::setFitOn()
{
    m_imageSizeFit = true;
    ui->graphicsView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->graphicsView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    // Fit on
    fitResize();
}


void MainWindow::setFitOff()
{
    m_imageSizeFit = false;
    ui->graphicsView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    ui->graphicsView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);

    // 原寸大で描画
    fitCancel();
}




void MainWindow::errorMsgBox( QString errMsg )
{
//    QMessageBox msgBox(this); // thisを使うと親クラス（MainWindow）の状態を引き継ぐ。ただしWindowStaysOnTopHintを設定すると表示されない？
    QMessageBox msgBox;
    msgBox.setText(errMsg);
    msgBox.setIcon(QMessageBox::Critical);
    msgBox.setWindowFlags(Qt::WindowStaysOnTopHint);
    msgBox.exec();
}


int32_t MainWindow::GetStorageSize( QString path )
{
    QFileInfo fileInfo( path );
    QStorageInfo storageInfo( fileInfo.dir() );
    qint64  byteFree;
    int32_t MbyteAvailable;

    // Device mount check
    if( false == storageInfo.isValid() ){
        // Error : Not mount
        return -1;
    }

    // Get total byte
    byteFree = storageInfo.bytesAvailable();
    if( 0 > byteFree ){
        // Error : Get failed
        return -2;
    }
    // byte -> MByte (残容量を全体の80%にする)
    MbyteAvailable = (int32_t)(byteFree / 1000 / 1000);
    MbyteAvailable = (int32_t)(MbyteAvailable * 0.8);

    if( 500 > MbyteAvailable ){
        // Error : Small effective capacity
        return -3;
    }

    return MbyteAvailable;
}

void MainWindow::updateStatus()
{
    QDateTime dt = QDateTime::currentDateTime();
    int32_t dateSec = m_recDateTime.secsTo(dt);
    int32_t hour;
    int32_t minute;
    int32_t second;
    QString str;
    QString recStr;
    QString recTimeStr;

    second = dateSec % 60;
    minute = dateSec / 60;
    hour = minute / 60;
    minute = minute % 60;

    // Recording time
    recStr = QString("Recording : ");
    recTimeStr = QString("%1h %2m %3s").arg(hour,2,10,QChar('0')).arg(minute,2,10,QChar('0')).arg(second,2,10,QChar('0'));

    str += recStr + recTimeStr;
    ui->label_Recording_time->setText(str);

}

void MainWindow::initialize()
{
    int     width = 0;
    int     height = 0;
    int     x = 0;
    int     y = 0;
    double  exp = 0;
    double  gainR = 0;
    double  gainG = 0;
    double  gainB = 0;
    double  gainRaw = 0;
    double  gamma = 0;
    QString format;
    bool    confFileReadFlg = false;

    m_capture = nullptr;
    m_camPluginSettingFlg = false;
    m_captureStartFlg = false;

    ui->setupUi(this);

    // 引数を取得
    getArgments();

    // カメラ型番選択（設定ファイル指定済みの場合は不要）
    CameraSelectDialog camDlg;
    if( m_savepath.isEmpty() == true ){
        // 引数-fなし
        if( m_camModel.isEmpty() == true ){
            // 引数-mなし
            if( camDlg.exec() == QDialog::Accepted ){
                // OKボタン押下
                m_camModel = camDlg.m_camModel;
            }else{
                // Cancelボタン押下 : アプリ終了
                close();
                exit(0);
            }
        }
    }else{
        // 引数-fあり：設定ファイルから各パラメーターを取得
        const std::string str = m_savepath.toStdString();
        ui->statusBar->showMessage(m_savepath);
        if (QFile::exists(m_savepath)){
            p.read(str);
            std::map<std::string, std::string> props;
            if( p.getPropValues(props) == 0 ){
                exp = std::stod(props.at("exposure"));
                gainR = std::stod(props.at("gainRed"));
                gainG = std::stod(props.at("gainGreen"));
                gainB = std::stod(props.at("gainBlue"));
                gainRaw = std::stod(props.at("gainRaw"));
                gamma = std::stod(props.at("gamma"));
                format = QString::fromStdString(props.at("format"));
                width = std::stoi(props.at("width"));
                height = std::stoi(props.at("height"));
                x = std::stoi(props.at("offsetX"));
                y = std::stoi(props.at("offsetY"));
                confFileReadFlg = true;

                std::map<std::string, std::string> camInfo;
                if( p.getCameraInfo(camInfo) == 0 ){
                    std::string localstr = camInfo["name"];
                    m_camModel = QString(localstr.c_str());
                }
            }else{
                // 設定ファイルを読めない
                m_savepath.clear();
                if( camDlg.exec() == QDialog::Accepted ){
                    // OKボタン押下
                    m_camModel = camDlg.m_camModel;
                }else{
                    // Cancelボタン押下 : アプリ終了
                    close();
                    exit(0);
                }
            }
        }else{
            // no file!
            m_savepath.clear();
            if( camDlg.exec() == QDialog::Accepted ){
                // OKボタン押下
                m_camModel = camDlg.m_camModel;
            }else{
                // Cancelボタン押下 : アプリ終了
                close();
                exit(0);
            }
        }
    }

    // 画面の中央にメイン画面表示
    QDesktopWidget *desktop = QApplication::desktop();
    QRect rect = desktop->screenGeometry();
    int32_t w = this->geometry().width();
    int32_t h = this->geometry().height();
    this->setGeometry( (rect.width() - w) / 2, (rect.height() - h)/ 2, w, h );
    QString mainWinTitle = QString("Camapp_Dahua %1").arg(m_camModel); // 選択されたカメラ型番をタイトルに追加
    this->setWindowTitle(mainWinTitle);
    ui->graphicsView->setScene(&scene_);

    // 非表示
    ui->label_Recording_time->hide();

    connect(ui->pushButton_conf_cancel, &QPushButton::clicked, this, &MainWindow::close);

    // メイン画面GUI初期設定
    initCameraPluginGui();

    // 設定ファイルの内容をメイン画面に反映
    if( confFileReadFlg ){
        ui->SpinBox_exposure->setValue(exp);
        ui->SpinBox_gainRed->setValue(gainR);
        ui->SpinBox_gainGreen->setValue(gainG);
        ui->SpinBox_gainBlue->setValue(gainB);
        ui->SpinBox_gainRaw->setValue(gainRaw);
        ui->SpinBox_gamma->setValue(gamma);

        int idx = ui->comboBox_Format->findText(format);
        if (idx == -1){
            ui->comboBox_Format->addItem(format);
            ui->comboBox_Format->setCurrentText(format);
        }else{
            ui->comboBox_Format->setCurrentIndex(idx);
        }

        ui->spinBox_width->setValue(width);
        ui->spinBox_height->setValue(height);
        ui->spinBox_offsetX->setValue(x);
        ui->spinBox_offsetY->setValue(y);
    }

    // カメラ制御クラス作成 & カメラプラグイン設定
    m_capture = new capture(m_savepath, this);
    connect( m_capture, SIGNAL(errorMsgSend(QString)), this, SLOT(errorMsgBox(QString)) );
    m_camPluginSettingFlg = m_capture->cameraPluginSetting();
    if( m_camPluginSettingFlg == false ){
        // カメラプラグイン設定失敗　アプリ終了
        exit(0);
    }

    // Capture start
    if( m_camPluginSettingFlg ){
        ui->actionCaptureStart->setChecked(true);
        on_actionCaptureStart_triggered();
    }

    // Update toolbar
    updateToolbarButton();

    // Update camera plugin gui setting
    updateCameraPluginParam();

}

