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
#include <QValidator>
#include "parser.h"

#define EXPO_MIN 42
#define EXPO_MAX 16000000
#define EXPO_DEF 33000

#define GAMMA_MIN 0.45
#define GAMMA_MAX 1.00
#define GAMMA_DEF 1.00

#define GAIN_MIN 0.00
#define GAIN_MAX 18.0
#define GAIN_DEF 0.00

#define WBR_MIN 1.00
#define WBR_MAX 8.00
#define WBR_DEF 1.66

#define WBB_MIN 1.00
#define WBB_MAX 8.00
#define WBB_DEF 1.89



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
