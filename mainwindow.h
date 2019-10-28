#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "capture.h"
#include <QLabel>
#include <QTimer>
#include <QtGui>
#include <QFileDialog>
#include <QGraphicsScene>
#include <QtGlobal>
#include <QDoubleSpinBox>
#include <QSlider>
#include "parser.h"

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
    void paintEvent(QPaintEvent* paintEvent);

private slots:
    void updateImage();
    void on_comboBox_currentIndexChanged(const QString &arg1);

    void on_comboBox_2_currentIndexChanged(const QString &arg1);

    void doubleSetProperty();
    void slidebar_event();

    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

    void on_pushButton_3_clicked();

    void on_pushButton_4_clicked();

    void on_pushButton_5_clicked();

private:
    Ui::MainWindow *ui;
    QString savepath;
     capture* m_capture;
     //QStandardItemModel itemModel;
     parser p;
     QGraphicsScene scene_;
     QTimer *timer;
     QList<QDoubleSpinBox *> list_dspinbox;
     QList<QSlider *> list_slider;




};

#endif // MAINWINDOW_H
