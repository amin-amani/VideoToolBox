#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QObject>
#include <QDebug>
#include <QDir>

#include <opencv2/core.hpp>
#include <opencv2/core/cuda.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc.hpp>
#include <qtimer.h>
#include <QFileDialog>
#include <QSettings>
using namespace cv;
using namespace std;
QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT
    Mat frame, gray, frameDelta, thresh, firstFrame;
      vector<vector<Point>> cnts;
  QTimer _timer;
    QTimer _detectTimer;
  int _totalFrames;
  QString _saveImagePath;

   CvCapture* capture ;
   int _currentFrame;
   void  DisplayOnLable(Mat image);
public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

public slots:
    void Timeout();
    void DetectMotion();
private slots:
    void on_BtnPlay_clicked();

    void on_TrkPosition_valueChanged(int value);

    void on_BtnSaveImage_clicked();

    void on_BtnOpenVideo_clicked();


    void on_BtnDetect_clicked();

private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
