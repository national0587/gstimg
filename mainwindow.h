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
#include <QMessageBox>
#include <QDesktopWidget>
#include "parser.h"

#define EXPO_MIN 45
#define EXPO_MAX 1000000
#define EXPO_DEF 33000

#define GAMMA_MIN 0.45
#define GAMMA_MAX 1.00
#define GAMMA_DEF 1.00

#define GAIN_MIN 0.00
#define GAIN_MAX 18.0
#define GAIN_DEF 0.00

#define WBR_MIN 1.00
#define WBR_MAX 8.00
#define WBR_DEF 1.855

#define WBB_MIN 1.00
#define WBB_MAX 8.00
#define WBB_DEF 2.633



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
    void on_comboBox_currentIndexChanged(const QString &arg1);

#if 0   // 2020/03/27 使っていないボタンを無効化
    void on_comboBox_2_currentIndexChanged(const QString &arg1);
#endif

    void doubleSetProperty();
    void slidebar_event();

    void on_pushButton_clicked();
#if 0   // 2020/03/27 使っていないボタンを無効化
    void on_pushButton_2_clicked();
    void on_pushButton_3_clicked();
    void on_pushButton_4_clicked();
#endif
    void on_pushButton_5_clicked();
    void getPropValueFloat();
    void getPropValueInt();
    void updateImage();


private:
    Ui::MainWindow *ui;
    QString savepath;
     capture* m_capture;
     //QStandardItemModel itemModel;
     parser p;
     QGraphicsScene scene_;
     QTimer *timer;
     QList<QLineEdit *> list_lineedit;
     QList<QSlider *> list_slider;




};

#endif // MAINWINDOW_H
