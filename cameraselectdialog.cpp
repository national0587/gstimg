#include "cameraselectdialog.h"
#include "ui_cameraselectdialog.h"

CameraSelectDialog::CameraSelectDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CameraSelectDialog)
{
    ui->setupUi(this);

    // Set camera model
    QStringList modellist =
                  {
                    QString(NAME_A5501CG20E_MODEL),
                    QString(NAME_A3131CG000E_MODEL),
                    QString(NAME_A7500MG20E_MODEL),
                    QString(NAME_A3200CU000E_MODEL),
                    QString(NAME_A3600MU60E_MODEL),
                    QString(NAME_A5131CU210E_MODEL)
                   }; // ConboBox(CameraModel)
    ui->comboBox_Camera_Model->clear();
    ui->comboBox_Camera_Model->addItems( modellist );
    ui->comboBox_Camera_Model->setCurrentText(NAME_A5501CG20E_MODEL);
    m_camModel = ui->comboBox_Camera_Model->currentText();

}

CameraSelectDialog::~CameraSelectDialog()
{
    delete ui;
}

// Ok button
void CameraSelectDialog::on_buttonBox_accepted()
{
    m_camModel = ui->comboBox_Camera_Model->currentText();
}

// Cancel button
void CameraSelectDialog::on_buttonBox_rejected()
{

}
