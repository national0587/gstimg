#ifndef CAMERASELECTDIALOG_H
#define CAMERASELECTDIALOG_H

#include <QDialog>

#include "cameraModel.h"

namespace Ui {
class CameraSelectDialog;
}

class CameraSelectDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CameraSelectDialog(QWidget *parent = 0);
    ~CameraSelectDialog();

    QString    m_camModel;

private slots:
    void on_buttonBox_accepted();

    void on_buttonBox_rejected();

private:
    Ui::CameraSelectDialog *ui;
};

#endif // CAMERASELECTDIALOG_H
