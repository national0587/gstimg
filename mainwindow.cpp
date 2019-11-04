#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    p(),
    ui(new Ui::MainWindow)
{
    m_capture = nullptr;

    ui->setupUi(this);
    this->setWindowTitle("Camapp for Dahua Version 0.2.2");
    ui->comboBox->setEnabled(true);
    ui->comboBox->addItem("Off");
    ui->comboBox->addItem("Once");
    ui->comboBox->addItem("Continuous");
    ui->comboBox_2->setEnabled(true);
    ui->comboBox_2->addItem("Off");
    ui->comboBox_2->addItem("Once");
    ui->comboBox_2->addItem("Continuous");



    ui->graphicsView->setScene(&scene_);
    //pause playing null...
    ui->pushButton_2->setVisible(false);
    ui->pushButton_3->setVisible(false);
    ui->pushButton_4->setVisible(false);

    ui->comboBox_3->setEnabled(false);

    ui->horizontalSlider_exposure->setMaximum(1000000);
    ui->horizontalSlider_exposure->setMinimum(10);
    ui->horizontalSlider_exposure->setSingleStep(100);
    ui->horizontalSlider_gamma->setMaximum(400);
    ui->horizontalSlider_gamma->setMinimum(0);
    ui->horizontalSlider_gamma->setSingleStep(1);
    ui->horizontalSlider_gainRaw->setMaximum(320);
    ui->horizontalSlider_gainRaw->setMinimum(10);
    ui->horizontalSlider_gainRaw->setSingleStep(1);
    ui->horizontalSlider_gainRed->setMaximum(150);
    ui->horizontalSlider_gainRed->setMinimum(0);
    ui->horizontalSlider_gainRed->setSingleStep(1);
    ui->horizontalSlider_gainGreen->setMaximum(150);
    ui->horizontalSlider_gainGreen->setMinimum(0);
    ui->horizontalSlider_gainGreen->setSingleStep(1);
    ui->horizontalSlider_gainBlue->setMaximum(150);
    ui->horizontalSlider_gainBlue->setMinimum(0);
    ui->horizontalSlider_gainBlue->setSingleStep(1);

    ui->spinBox_width->setSingleStep(16);
    ui->spinBox_width->setMinimum(0);
    ui->spinBox_height->setSingleStep(16);
    ui->spinBox_height->setMinimum(0);

    ui->spinBox_offsetx->setSingleStep(16);
    ui->spinBox_offsetx->setMinimum(0);
    ui->spinBox_offsety->setSingleStep(16);
    ui->spinBox_offsety->setMinimum(0);
    /*DoubleSpinBox*/

    list_dspinbox << ui->SpinBox_exposure;
    list_dspinbox << ui->SpinBox_gamma;
    list_dspinbox << ui->SpinBox_gainRaw;
    list_dspinbox << ui->SpinBox_gainRed;
    list_dspinbox << ui->SpinBox_gainGreen;
    list_dspinbox << ui->SpinBox_gainBlue;

//    connect(ui->exposureSpinBox, SIGNAL(valueChanged(double)), ui->horizontalSlider_exposure, SLOT(setValue(int)));
//    connect(ui->horizontalSlider_exposure, SIGNAL(valueChanged(int)), ui->exposureSpinBox, SLOT(setValue(double)));
    for(auto const &doublespinbox : list_dspinbox)
    {
         connect(doublespinbox, static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), this, static_cast<void (MainWindow::*)(void)>(&MainWindow::doubleSetProperty));
         //    connect(ui->doubleSpinBox, &QAbstractSpinBox::editingFinished, [=] {qDebug("hoge");});
    }



    list_slider << ui->horizontalSlider_exposure;
    list_slider << ui->horizontalSlider_gamma;
    list_slider << ui->horizontalSlider_gainRaw;
    list_slider << ui->horizontalSlider_gainRed;
    list_slider << ui->horizontalSlider_gainGreen;
    list_slider << ui->horizontalSlider_gainBlue;

    for(auto const &horizonalslidebar: list_slider){
        connect(horizonalslidebar, &QSlider::actionTriggered, this, &MainWindow::slidebar_event);

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
                ui->SpinBox_exposure->setValue(std::stod(props.at("exposure")));
                ui->SpinBox_gainRed->setValue(std::stod(props.at("gainRed")));
                ui->SpinBox_gainGreen->setValue(std::stod(props.at("gainGreen")));
                ui->SpinBox_gainBlue->setValue(std::stod(props.at("gainBlue")));
                ui->SpinBox_gainRaw->setValue(std::stod(props.at("gainRaw")));
                ui->SpinBox_gamma->setValue(std::stod(props.at("gamma")));

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


    timer = new QTimer();
    timer->setSingleShot(false);
    timer->setInterval(1000);
    timer->setTimerType(Qt::PreciseTimer);
    timer->start();

    connect(timer, SIGNAL(timeout()), this, SLOT(updateImage()));
    connect(ui->pushButton, &QPushButton::clicked, this, &MainWindow::close);

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::paintEvent(QPaintEvent *paintEvent)
{
    if (m_capture!=nullptr){
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
                    color = cv::COLOR_GRAY2BGR;
                }
                }else if(m_capture->m_format=="UYVY"){
                    color = cv::COLOR_YUV2RGB_UYVY;
                }else if (m_capture->m_format=="GRAY8"){
            }
//            qDebug() << "color: " << color;
            try{
            cv::cvtColor(mat, mat, color);
            cv::resize(mat, mat,cv::Size(640, 480),0,0,cv::INTER_CUBIC);

            QImage img = QImage(mat.data, 640, 480, QImage::Format_RGB888);
            scene_.clear();
            scene_.addPixmap(QPixmap::fromImage(img));
            ui->graphicsView->update();
            }catch(cv::Exception &e){
                qDebug() << "exception: "<< e.what();
            }
        }
    }
}

void MainWindow::updateImage()
{
        //qDebug()<<"call";
    if(m_capture!=nullptr){
        // double propertt
        float temp;
        //m_capture->get_floatproperty("exposure", temp);

    }


}

void MainWindow::on_comboBox_currentIndexChanged(const QString &arg1)
{
    if (m_capture!=nullptr){
        m_capture->change_property(1, arg1);

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
    }
}

void MainWindow::on_comboBox_2_currentIndexChanged(const QString &arg1)
{
    if(m_capture!=nullptr){
        m_capture->change_property(2, arg1);

        if(arg1=="Once" || arg1=="Continuous"){
            ui->horizontalSlider_exposure->setEnabled(false);
            ui->SpinBox_exposure->setEnabled(false);
        }else{
            ui->horizontalSlider_exposure->setEnabled(true);
            ui->SpinBox_exposure->setEnabled(true);
        }
    }
}




void MainWindow::doubleSetProperty()
{
    QDoubleSpinBox *spinbox = qobject_cast<QDoubleSpinBox*>(sender());
    if (spinbox!=NULL){
        qDebug("%f", spinbox->value());
        qDebug() << spinbox->objectName();
    }
    QString controlName = spinbox->objectName().split("_").last();

//    if(QString::compare(), "exposureSpinBox", Qt::CaseSensitive)==0){
//        qDebug()<< "exposure";
//        m_capture->change_property("exposure", spinbox->value());
//    }else if(QString::compare(spinbox->objectName(), "gammaSpinBox", Qt::CaseSensitive)==0){
//        qDebug()<< "gamma";
//        m_capture->change_property("gamma", spinbox->value());
//    }else if(QString::compare(spinbox->objectName(), "gainSpinBox", Qt::CaseSensitive)==0){
//        qDebug()<<"gain";
//        m_capture->change_property("gainRaw", spinbox->value());
//    }else if(QString::compare(spinbox->objectName(), "balanceRatioRed", Qt::CaseSensitive)==0){
//        qDebug() << "gainRed";
//        m_capture->change_property("gainRed", spinbox->value());
//    }else if(QString::compare(spinbox->objectName(), "balanceRatioGreen", Qt::CaseSensitive)==0){
//    qDebug() << "gainGreen";
//    m_capture->change_property("gainGreen", spinbox->value());
//    }else if(QString::compare(spinbox->objectName(), "balanceRatioBlue", Qt::CaseSensitive)==0){
//    qDebug() << "gainBlue";
//    m_capture->change_property("gainBlue", spinbox->value());
//    }
    if (m_capture!=nullptr){
        m_capture->change_property(controlName, spinbox->value());
    }
    for(auto slider: list_slider){
        if(QString::compare(slider->objectName().split("_").last(), controlName, Qt::CaseSensitive)==0){
            slider->setValue(spinbox->value()*10);

        }
//        ui->horizontalSlider_exposure->setValue(spinbox->value());
    }

}

void MainWindow::slidebar_event()
{
    QSlider * slidar = qobject_cast<QSlider*>(sender());

    QString controlName = slidar->objectName().split("_").last();
    qDebug() << controlName << " " << slidar->value();
    for(auto dspinbox: list_dspinbox){
        qDebug() << controlName << "-> "<< dspinbox->objectName()<< ": " << slidar->value();
        if(QString::compare(dspinbox->objectName().split("_").last(), controlName, Qt::CaseSensitive)==0){
            dspinbox->setValue((double)slidar->value()/10);
            qDebug() << controlName << "-> "<< dspinbox->objectName()<< ": " << slidar->value();
        }

    }


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
    if (savepath==""){
        savepath = QFileDialog::getSaveFileName(this, tr("save path"), QStandardPaths::writableLocation(QStandardPaths::DesktopLocation));
    }else{

    }
    std::map<std::string, std::string> props;
    props.insert(std::make_pair("exposure", std::to_string(ui->SpinBox_exposure->value())));
    props.insert(std::make_pair("gainRaw", std::to_string(ui->SpinBox_gainRaw->value())));
    props.insert(std::make_pair("gainRed", std::to_string(ui->SpinBox_gainRed->value())));
    props.insert(std::make_pair("gainGreen", std::to_string(ui->SpinBox_gainGreen->value())));
    props.insert(std::make_pair("gainBlue", std::to_string(ui->SpinBox_gainBlue->value())));
    props.insert(std::make_pair("gamma", std::to_string(ui->SpinBox_gamma->value())));
    props.insert(std::make_pair("width", std::to_string(ui->spinBox_width->value())));
    props.insert(std::make_pair("height", std::to_string(ui->spinBox_height->value())));
    props.insert(std::make_pair("offsetX", std::to_string(ui->spinBox_offsetx->value())));
    props.insert(std::make_pair("offsetY", std::to_string(ui->spinBox_offsety->value())));
    props.insert(std::make_pair("format", ui->comboBox_3->currentText().toStdString()));
    props.insert(std::make_pair("autoWB", ui->comboBox->currentText().toStdString()));
    props.insert(std::make_pair("autoExpo", ui->comboBox_2->currentText().toStdString()));
    p.setPropValues(props);
    p.save(savepath.toStdString());
}
