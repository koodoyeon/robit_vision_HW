#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <opencv2/opencv.hpp>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;

    std::vector<cv::Point2f> loadData(const std::string &filePath);
    std::vector<int> kMeansClustering(const std::vector<cv::Point2f> &data, int k, cv::Mat &outputImage);
    void runClusteringAndDisplay();
};

#endif // MAINWINDOW_H
