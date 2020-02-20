#include "mainwindow.h"
#include "ui_mainwindow.h"

//===================================================================================================

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    QSettings  settings;
    QString imageSavePath=settings.value("SavePath").toString();
    if(!QDir(imageSavePath+"/Detected").exists())
    {
    QDir().mkdir(imageSavePath+"/Detected");
    }
    ui->LabSaveAddress->setText(imageSavePath);
    _totalFrames=0;

}
//===================================================================================================
void MainWindow::DisplayOnLable(Mat image)
{
    Mat outImg;
    cvtColor(image, outImg, CV_BGR2RGB);
    cv::resize(outImg, outImg, cv::Size(), 0.75, 0.75);
    ui->LabDisplayImage->setPixmap(QPixmap::fromImage(QImage(outImg.data,
                                                             outImg.cols,
                                                             outImg.rows,
                                                             outImg.step,
                                                             QImage::Format_RGB888)));
}
//===================================================================================================
void MainWindow::Timeout()
{
    if(_totalFrames<1){_timer.stop();ui->BtnPlay->setText("Play"); return;}
    cvSetCaptureProperty(capture, CV_CAP_PROP_POS_FRAMES, _currentFrame);
    IplImage*  ipl = cvQueryFrame(capture);
    cv::Mat frame = cv::cvarrToMat(ipl);
    if (frame.empty()){
        qDebug()<<"empty";
        return;
    }
DisplayOnLable(frame);
    //imshow("video1",frame);
    char c=waitKey(1);
    ui->TrkPosition->setValue(_currentFrame);
    ui->LabFrames->setText(QString::number(_currentFrame)+"/"+QString::number(_totalFrames));
    _currentFrame++;
}
//===================================================================================================
MainWindow::~MainWindow()
{
    delete ui;
}
//===================================================================================================
void MainWindow::on_BtnPlay_clicked()
{
    if(ui->BtnPlay->text()=="Play"){
        qDebug()<<"play";
        _timer.start(1);
        ui->BtnPlay->setText("Pause");
        return;
    }
    else{
        _timer.stop();
        ui->BtnPlay->setText("Play");
        qDebug()<<"stop";
    }
}
//===================================================================================================
void MainWindow::on_TrkPosition_valueChanged(int value)
{
    _currentFrame=value;
    if(ui->BtnPlay->text()=="Play")
        Timeout();
}
//===================================================================================================
void MainWindow::on_BtnSaveImage_clicked()
{
    QSettings settings;
    if(_totalFrames<1)return;
    if(_saveImagePath.isEmpty()){
        _saveImagePath = QFileDialog::getExistingDirectory(this, tr("Open Directory"),
                                                           settings.value("SavePath").toString(),
                                                           QFileDialog::ShowDirsOnly
                                                           | QFileDialog::DontResolveSymlinks);
        ui->LabSaveAddress->setText(_saveImagePath);
        settings.setValue("SavePath",_saveImagePath);

    }
    cvSetCaptureProperty(capture, CV_CAP_PROP_POS_FRAMES, _currentFrame);
    IplImage*  ipl = cvQueryFrame(capture);
    cv::Mat frame = cv::cvarrToMat(ipl);
    QString path=_saveImagePath+"/"+"image.jpg";
    imwrite(path.toStdString(),frame);
    // delete  ipl;
}
//===================================================================================================
void MainWindow::on_BtnOpenVideo_clicked()
{
    QSettings settings;
    QString fileName = QFileDialog::getOpenFileName(this,
                                                    tr("Open Video"),
                                                    settings.value("VideoPath").toString(),
                                                    tr("Video Files ( *.avi *.mp4)"));
    ui->LabFileUrl->setText(fileName);
    settings.setValue("VideoPath",fileName);
    _currentFrame=0;
    //QString fileName="../Sorter/test/TestVideoes/sample3.mp4";
    VideoCapture video= VideoCapture(fileName.toStdString());
    capture = cvCreateFileCapture(fileName.trimmed().toLatin1());
    _totalFrames = int(video.get(CAP_PROP_FRAME_COUNT));
    disconnect(this, SLOT(Timeout()));
    connect(&_timer,SIGNAL(timeout()),this,SLOT(Timeout()));
    ui->TrkPosition->setMaximum(_totalFrames);
    ui->LabFrames->setText("0/"+QString::number(_totalFrames));
}

//===================================================================================================
void MainWindow::on_BtnDetect_clicked()
{
    _currentFrame=0;
    cvSetCaptureProperty(capture, CV_CAP_PROP_POS_FRAMES, _currentFrame++);
    IplImage*  ipl = cvQueryFrame(capture);
    frame = cv::cvarrToMat(ipl);
    cvtColor(frame, firstFrame, COLOR_BGR2GRAY);
    GaussianBlur(firstFrame, firstFrame, Size(21, 21), 0);
    disconnect(this, SLOT(DetectMotion()));
    connect(&_detectTimer,SIGNAL(timeout()),this,SLOT(DetectMotion()));

    _detectTimer.start(1);
}
//===================================================================================================

void MainWindow::DetectMotion()
{

    cvSetCaptureProperty(capture, CV_CAP_PROP_POS_FRAMES, _currentFrame++);
    IplImage*  ipl = cvQueryFrame(capture);
    cv::Mat frame = cv::cvarrToMat(ipl);
    if (frame.empty())
    {
        _detectTimer.stop();
        qDebug()<<"empty";
        return;
    }
    ui->LabFrames->setText(QString::number(_currentFrame)+"/"+QString::number(_totalFrames));
    cvtColor(frame, gray, COLOR_BGR2GRAY);
    absdiff(firstFrame, gray, frameDelta);
    threshold(frameDelta, thresh, 25, 255, THRESH_BINARY);
    dilate(thresh, thresh, Mat(), Point(-1,-1), 2);
    findContours(thresh, cnts, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
    for(int i = 0; i< cnts.size(); i++) {
        if(contourArea(cnts[i]) < 500) {
            continue;
        }
        QString text="Motion Detected in frame:"+QString::number(_currentFrame);
        putText(frame,text.toStdString(), Point(40, 40), FONT_HERSHEY_SIMPLEX, 0.75, Scalar(0,0,255),2);
        // imshow("detect",frame);
        Mat outImg;
        cvtColor(frame, outImg, CV_BGR2RGB);
        cv::resize(outImg, outImg, cv::Size(), 0.75, 0.75);
        ui->LabDisplayImage->setPixmap(QPixmap::fromImage(QImage(outImg.data,
                                                                 outImg.cols,
                                                                 outImg.rows,
                                                                 outImg.step,
                                                                 QImage::Format_RGB888)));
        text+=".jpg";
        QSettings settings;
        QString savefile=settings.value("SavePath").toString()+"/Detected/"+text;
        imwrite(savefile.toStdString(),frame);
        waitKey(0);

    }
}
//===================================================================================================
