#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    p(),
    ui(new Ui::MainWindow)
{
    m_capture = nullptr;

    ui->setupUi(this);
    this->setWindowTitle("CamApp for TOSHIBA Teli BU1203MC Version 0.0.1");
    ui->comboBox->setEnabled(true);
    ui->comboBox->addItem("Off");
    ui->comboBox->addItem("Once");
    ui->radioButton_scale->setChecked(true);


    ui->spinBox_width->setMaximum(4000);
    ui->spinBox_height->setMaximum(3000);
    ui->spinBox_offsetx->setMaximum(4000);
    ui->spinBox_offsety->setMaximum(3000);

    ui->graphicsView->setScene(&scene_);

    ui->comboBox_3->setEnabled(true);

    ui->horizontalSlider_exposure->setMaximum(EXPO_MAX*100);
    ui->horizontalSlider_exposure->setMinimum(EXPO_MIN*100);
    ui->horizontalSlider_exposure->setValue((int)(EXPO_DEF*100));
    ui->horizontalSlider_exposure->setSingleStep(20000);
    ui->horizontalSlider_exposure->setPageStep(100000);
    ui->lineEdit_exposure->setText(QString::number(EXPO_DEF));

    ui->horizontalSlider_gamma->setRange((int)(GAMMA_MIN*100),(int)(GAMMA_MAX*100));
//    ui->horizontalSlider_gamma->setMaximum(99);
//    ui->horizontalSlider_gamma->setMinimum(0);
    ui->horizontalSlider_gamma->setValue((int)(GAMMA_DEF * 100));
    ui->horizontalSlider_gamma->setSingleStep(1);
    ui->lineEdit_gamma->setText(QString::number(GAMMA_DEF));

    ui->horizontalSlider_gainRaw->setRange((int)(GAIN_MIN*100), (int)(GAIN_MAX*100));
    ui->horizontalSlider_gainRaw->setSingleStep(1);
    ui->horizontalSlider_gainRaw->setValue((int)(GAIN_DEF*100));
    ui->lineEdit_gainRaw->setText(QString::number(GAIN_DEF));

    ui->horizontalSlider_gainRed->setMinimum((int)(WBR_MIN*100));
    ui->horizontalSlider_gainRed->setMaximum((int)(WBR_MAX*100));
    ui->horizontalSlider_gainRed->setSingleStep(1);
    ui->horizontalSlider_gainRed->setValue((int)(WBR_DEF*100));
//    ui->horizontalSlider_gainGreen->setMaximum(150);
//    ui->horizontalSlider_gainGreen->setMinimum(0);
//    ui->horizontalSlider_gainGreen->setSingleStep(1);
    ui->lineEdit_gainRed->setText(QString::number(WBR_DEF));

    ui->horizontalSlider_gainBlue->setRange((int)(WBB_MIN*100), (int)(WBB_MAX*100));
    ui->horizontalSlider_gainBlue->setValue((int)(WBB_DEF*100));
    ui->horizontalSlider_gainBlue->setSingleStep(1);
    ui->lineEdit_gainBlue->setText(QString::number(WBB_DEF));

    ui->spinBox_width->setSingleStep(8);
    ui->spinBox_width->setMinimum(0);
    ui->spinBox_height->setSingleStep(2);
    ui->spinBox_height->setMinimum(0);

//    ui->spinBox_offsetx->setSingleStep(1);
    ui->spinBox_offsetx->setMinimum(0);
    ui->spinBox_offsety->setSingleStep(16);
    ui->spinBox_offsety->setMinimum(0);
    /*DoubleSpinBox*/


    //lineEdit
    list_lineedit << ui->lineEdit_exposure;
    list_lineedit << ui->lineEdit_gamma;
    list_lineedit << ui->lineEdit_gainRaw;
    list_lineedit << ui->lineEdit_gainRed;
    list_lineedit << ui->lineEdit_gainBlue;


    for(auto const &lineedit : list_lineedit)
    {
        QDoubleValidator *doubleValidator = new QDoubleValidator();
//        ui->lineEdit_exposure->setValidator(doubleValidator);
        lineedit->setValidator(doubleValidator);
        connect(lineedit, &QLineEdit::textChanged, this, &MainWindow::doubleSetProperty);

    }



    list_slider << ui->horizontalSlider_exposure;
    list_slider << ui->horizontalSlider_gamma;
    list_slider << ui->horizontalSlider_gainRaw;
    list_slider << ui->horizontalSlider_gainRed;
//    list_slider << ui->horizontalSlider_gainGreen;
    list_slider << ui->horizontalSlider_gainBlue;


    for(auto const &horizonalslidebar: list_slider){
        connect(horizonalslidebar, &QSlider::valueChanged, this, &MainWindow::slidebar_event);
    }

    savepath = "";
    QStringList argv = QCoreApplication::arguments();
    if (argv.size()>1){
        argv.removeAt(0);

        for(QString arg: argv){
            qDebug()<<"arg" << arg;
        }
        if (argv.at(0)=="-f"){
            savepath = argv.at(1);

            const std::string str = argv.at(1).toStdString();
            ui->statusBar->showMessage(savepath);
            if (QFile::exists(argv.at(1))){
                p.read(str);
                std::map<std::string, std::string> props;
                p.getPropValues(props);
//                std::cout<< "debug   expo:" <<std::stod(props.at("exposure")) << std::endl;
                ui->lineEdit_exposure->setText(QString::fromStdString(props.at("exposure")));
                ui->lineEdit_gainRed->setText(QString::fromStdString(props.at("gainRed")));
////                ui->SpinBox_gainGreen->setValue(std::stod(props.at("gainGreen")));
                ui->lineEdit_gainBlue->setText(QString::fromStdString(props.at("gainBlue")));
                ui->lineEdit_gainRaw->setText(QString::fromStdString(props.at("gainRaw")));
                ui->lineEdit_gamma->setText(QString::fromStdString(props.at("gamma")));

                int idx = ui->comboBox_3->findText(QString::fromStdString(props.at("format")));
                if (idx == -1){
                    ui->comboBox_3->addItem(QString::fromStdString(props.at("format")));
                }else{
                    ui->comboBox_3->setCurrentIndex(idx);
                }
                ui->spinBox_width->setValue(std::stoi(props.at("width")));
                ui->spinBox_height->setValue(std::stoi(props.at("height")));
                ui->spinBox_offsetx->setValue(std::stoi(props.at("offsetX")));
                ui->spinBox_offsety->setValue(std::stoi(props.at("offsetY")));
            }else{
                // no file!

            }
        }
    }
    m_capture = new capture(savepath, this);

    connect(ui->pushButton, &QPushButton::clicked, this, &MainWindow::close);

    // get camera prop(const)
    getPropValueInt();
    QString str;
    m_capture->get_stringproperty("format", str);
    ui->comboBox->setCurrentText(str);

    QTimer *tm = new QTimer();
    tm->setInterval(100);
    tm->setSingleShot(false);
    tm->start();
    connect(tm, &QTimer::timeout, this, &MainWindow::updateImage);

    QTimer::singleShot(1000, this, [=](){if(m_capture->checkStatus()==false){
            QMessageBox msgBox(this);
            msgBox.setText("could not connect camera!");
            msgBox.exec();
        }});


}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::paintEvent(QPaintEvent *paintEvent)
{

}


void MainWindow::on_comboBox_currentIndexChanged(const QString &arg1)
{
    if (m_capture!=nullptr){
        m_capture->change_property(1, arg1);

        if(arg1=="Once" || arg1=="Continuous"){
            //do
            ui->horizontalSlider_gainRed->setEnabled(false);
//            ui->horizontalSlider_gainGreen->setEnabled(false);
            ui->horizontalSlider_gainBlue->setEnabled(false);

//            ui->SpinBox_gainRed->setEnabled(false);
//            ui->SpinBox_gainGreen->setEnabled(false);
//            ui->SpinBox_gainBlue->setEnabled(false);
            QTimer::singleShot(1000, this, [=](){
                getPropValueFloat();
                ui->comboBox->setCurrentIndex(0);
                ui->horizontalSlider_gainRed->setEnabled(true);
                ui->horizontalSlider_gainBlue->setEnabled(true);
            });
        }else{
            //
            ui->horizontalSlider_gainRed->setEnabled(true);
//            ui->horizontalSlider_gainGreen->setEnabled(true);
            ui->horizontalSlider_gainBlue->setEnabled(true);

//            ui->SpinBox_gainRed->setEnabled(true);
//            ui->SpinBox_gainGreen->setEnabled(true);
//            ui->SpinBox_gainBlue->setEnabled(true);
        }
    }
}

void MainWindow::on_comboBox_2_currentIndexChanged(const QString &arg1)
{
    if(m_capture!=nullptr){
        m_capture->change_property(2, arg1);

        if(arg1=="Once" || arg1=="Continuous"){
            ui->horizontalSlider_exposure->setEnabled(false);
//            ui->SpinBox_exposure->setEnabled(false);
        }else{
            ui->horizontalSlider_exposure->setEnabled(true);
//            ui->SpinBox_exposure->setEnabled(true);
        }
    }
}




void MainWindow::doubleSetProperty()
{
//    QDoubleSpinBox *spinbox = qobject_cast<QDoubleSpinBox*>(sender());
    QLineEdit *lineEdit = qobject_cast<QLineEdit*>(sender());

    if(m_capture!=nullptr){

    if (lineEdit!=NULL){
        qDebug() << QString("%1: %2").arg(lineEdit->objectName()).arg(lineEdit->text());
    }

    QString controlName = lineEdit->objectName().split("_").last();

    if(QString::compare(controlName, "exposure", Qt::CaseSensitive)==0){
        qDebug()<< "exposure";    
        ui->horizontalSlider_exposure->blockSignals(true);
        ui->horizontalSlider_exposure->setValue(lineEdit->text().toDouble()*100);
        ui->horizontalSlider_exposure->blockSignals(false);
        m_capture->change_property("exposure", lineEdit->text().toDouble());

    }else if(QString::compare(controlName, "gamma", Qt::CaseSensitive)==0){
        qDebug()<< "gamma";
        ui->horizontalSlider_gamma->blockSignals(true);
        ui->horizontalSlider_gamma->setValue(lineEdit->text().toDouble()*100);
        ui->horizontalSlider_gamma->blockSignals(false);
        m_capture->change_property("gamma", lineEdit->text().toDouble());

    }else if(QString::compare(controlName, "gainRaw", Qt::CaseSensitive)==0){
        qDebug()<<"gainRaw";
        ui->horizontalSlider_gainRaw->blockSignals(true);
        ui->horizontalSlider_gainRaw->setValue(lineEdit->text().toDouble()*100);
        ui->horizontalSlider_gainRaw->blockSignals(false);
        m_capture->change_property("gainRaw", lineEdit->text().toDouble());
    }else if(QString::compare(controlName, "gainRed", Qt::CaseSensitive)==0){
        qDebug() << "gainRed";
        ui->horizontalSlider_gainRed->blockSignals(true);
        ui->horizontalSlider_gainRed->setValue(lineEdit->text().toDouble()*100);
        ui->horizontalSlider_gainRed->blockSignals(false);
        m_capture->change_property("gainRed", lineEdit->text().toDouble());
    }else if(QString::compare(controlName, "gainGreen", Qt::CaseSensitive)==0){
//        qDebug() << "gainGreen";
//        m_capture->change_property("gainGreen", lineEdit->text().toDouble());
//        ui->horizontalSlider_exposure->setValue(lineEdit->text().toDouble());
    }else if(QString::compare(controlName, "gainBlue", Qt::CaseSensitive)==0){
        qDebug() << "gainBlue";
        ui->horizontalSlider_gainBlue->blockSignals(true);
        ui->horizontalSlider_gainBlue->setValue(lineEdit->text().toDouble()*100);
        ui->horizontalSlider_gainBlue->blockSignals(false);
        m_capture->change_property("gainBlue", lineEdit->text().toDouble());
    }

    this->getPropValueFloat();
//    if (m_capture!=nullptr){
//        m_capture->change_property(controlName, spinbox->value());
//    }
//    for(auto slider: list_slider){
//        if(QString::compare(slider->objectName().split("_").last(), controlName, Qt::CaseSensitive)==0){
//            slider->setValue(spinbox->value()*100);

//        }
//        ui->horizontalSlider_exposure->setValue(spinbox->value());
//    }
}
}

void MainWindow::slidebar_event()
{
    QSlider * slidar = qobject_cast<QSlider*>(sender());

    QString controlName = slidar->objectName().split("_").last();
    qDebug() << controlName << " -" << slidar->value();

    for(auto lineEdit: list_lineedit){
//        qDebug() << controlName << "-> "<< lineEdit->objectName()<< ": " << lineEdit->value();
        if(QString::compare(lineEdit->objectName().split("_").last(), controlName, Qt::CaseSensitive)==0){
            lineEdit->setText(QString::number((double)slidar->value()/100.0,'f',3));
            qDebug() << controlName << "-> "<< lineEdit->objectName()<< ": " << slidar->value();
        }

    }
    ui->lineEdit_exposure->blockSignals(false);


}


void MainWindow::on_pushButton_clicked()
{
    if (m_capture!=nullptr){
        m_capture->event_bringup();
    }
}

void MainWindow::on_pushButton_2_clicked()
{
    if (m_capture!=nullptr){
        m_capture->setStatePause();
    }
}

void MainWindow::on_pushButton_3_clicked()
{
    if(m_capture!=nullptr){
        m_capture->setStatePlaying();
    }
}

void MainWindow::on_pushButton_4_clicked()
{
//    m_capture->oneShot();
    if(m_capture!=nullptr){
        m_capture->setStatePause();
    }
}

void MainWindow::on_pushButton_5_clicked()
{
//    const QPixmap pixmap = ui->graphicsView->grab();
//    pixmap.save("out.png");
        if(m_capture!=nullptr){
        if (savepath==""){
            savepath = QFileDialog::getSaveFileName(this, tr("save path"), "");
        }else{

        }
            std::map<std::string, std::string> props;
            props.insert(std::make_pair("exposure", ui->lineEdit_exposure->text().toStdString()));
            props.insert(std::make_pair("gainRaw", ui->lineEdit_gainRaw->text().toStdString()));
            props.insert(std::make_pair("gainRed", ui->lineEdit_gainRed->text().toStdString()));
        //    props.insert(std::make_pair("gainGreen", std::to_string(ui->SpinBox_gainGreen->value())));
            props.insert(std::make_pair("gainBlue", ui->lineEdit_gainBlue->text().toStdString()));
            props.insert(std::make_pair("gamma", ui->lineEdit_gamma->text().toStdString()));
            props.insert(std::make_pair("width", std::to_string(ui->spinBox_width->value())));
            props.insert(std::make_pair("height", std::to_string(ui->spinBox_height->value())));
            props.insert(std::make_pair("offsetX", std::to_string(ui->spinBox_offsetx->value())));
            props.insert(std::make_pair("offsetY", std::to_string(ui->spinBox_offsety->value())));
            props.insert(std::make_pair("format", ui->comboBox_3->currentText().toStdString()));
            props.insert(std::make_pair("autoWB", ui->comboBox->currentText().toStdString()));
        //    props.insert(std::make_pair("autoExpo", ui->comboBox_2->currentText().toStdString()));
            QString str;
            m_capture->get_stringproperty("camid", str);
            std::map<std::string, std::string> camInfo;
            camInfo.insert(std::make_pair("serialNo", str.toStdString()));
            p.setPropValues(props);
            p.setCameraInfo(camInfo);
            p.save(savepath.toStdString());
    }
}

void MainWindow::getPropValueFloat()
{
    float value;

    m_capture->get_floatproperty("exposure", value);
    ui->lineEdit_exposure->setText(QString::number(value, 'f', 3));

    m_capture->get_floatproperty("gainRaw", value);
    ui->lineEdit_gainRaw->setText(QString::number(value, 'f', 3));

    m_capture->get_floatproperty("gainRed", value);
    ui->lineEdit_gainRed->setText(QString::number(value, 'f', 3));

    m_capture->get_floatproperty("gainBlue", value);
    ui->lineEdit_gainBlue->setText(QString::number(value, 'f', 3));

    m_capture->get_floatproperty("gamma", value);
    ui->lineEdit_gamma->setText(QString::number(value, 'f', 3));


}

void MainWindow::getPropValueInt()
{
    //
    int value;
    m_capture->get_intproperty("width", value);
    ui->spinBox_width->setValue(value);

    m_capture->get_intproperty("height", value);
    ui->spinBox_height->setValue(value);

    m_capture->get_intproperty("offsetX", value);
    ui->spinBox_offsetx->setValue(value);

    m_capture->get_intproperty("offsetY", value);
    ui->spinBox_offsety->setValue(value);
}

void MainWindow::updateImage()
{
    if (m_capture!=nullptr){
//    if(false){
       cv::Mat mat;
       cv::Mat recv;
       recv = m_capture->getFrame();
//       qDebug() <<  "mat size:" << recv.size().area();
        if((!recv.empty()) && recv.size().area()>0){
            recv.copyTo(mat);

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
//                    color = cv::COLOR_GRAY2BGR;
                }
                }else if(m_capture->m_format=="UYVY"){
                    color = cv::COLOR_YUV2RGB_UYVY;
                }else if (m_capture->m_format=="GRAY8"){
                    color = cv::COLOR_GRAY2BGR;
            }
//            qDebug() << "color: " << color;
            try{
            cv::cvtColor(mat, mat, color);
            QImage img;

            if(ui->radioButton_scale->isChecked()){
                cv::resize(mat, mat,cv::Size(640, 480),0,0,cv::INTER_CUBIC);
                img = QImage(mat.data, 640, 480, QImage::Format_RGB888);
                scene_.setSceneRect(0,0,640,480);
                ui->graphicsView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
                ui->graphicsView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
            }else{
                img = QImage(mat.data, mat.cols, mat.rows, QImage::Format_RGB888);
                scene_.setSceneRect(0,0,mat.cols, mat.rows);
                ui->graphicsView->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
                ui->graphicsView->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
            }
            scene_.clear();
            scene_.addPixmap(QPixmap::fromImage(img));
            ui->graphicsView->update();
            }catch(cv::Exception &e){
                qDebug() << "exception: "<< e.what();
            }
        }
    }
}
