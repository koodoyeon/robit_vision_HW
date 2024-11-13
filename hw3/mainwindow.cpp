

/*
K = 3

K 값 선정 이유

   - SSD(클러스터 내 분산) 감소 그래프에서 K = 3에서 감소폭이 둔해지는 지점.
   - 초기 시각화 분석에서 데이터가 세 개의 군집으로 나뉨.
   - K = 3으로 클러스터링 시 각 군집이 명확히 구분됨.

*/


#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <opencv2/opencv.hpp>
#include <QImage>
#include <QPixmap>
#include <QDebug>
#include <fstream>
#include <sstream>
#include <algorithm>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    runClusteringAndDisplay();
}

MainWindow::~MainWindow()
{
    delete ui;
}

std::vector<cv::Point2f> MainWindow::loadData(const std::string &filePath) {
    std::vector<cv::Point2f> data;
    std::ifstream file(filePath);

    // 파일 열림 여부 확인
    if (!file.is_open()) {
        qDebug() << "파일을 열 수 없습니다. 경로를 확인하세요:" << QString::fromStdString(filePath);
        return data;
    }

    std::string line;
    while (std::getline(file, line)) {
        // 쉼표를 공백으로 대체하여 데이터를 읽음
        std::replace(line.begin(), line.end(), ',', ' ');
        std::stringstream lineStream(line);
        float x, y;
        if (lineStream >> x >> y) {
            data.emplace_back(x, y);
        }
    }

    // 데이터가 제대로 로드되었는지 확인
    if (data.empty()) {
        qDebug() << "데이터 파일이 비어 있거나 로드에 실패했습니다.";
    } else {
        qDebug() << "데이터가 성공적으로 로드되었습니다. 데이터 개수:" << data.size();
    }

    return data;
}

std::vector<int> MainWindow::kMeansClustering(const std::vector<cv::Point2f> &data, int k, cv::Mat &outputImage) {

    // 데이터를 kmeans 함수에 맞는 cv::Mat 형식으로 변환
    cv::Mat points(data.size(), 2, CV_32F);
    for (size_t i = 0; i < data.size(); ++i) {
        points.at<float>(i, 0) = data[i].x;
        points.at<float>(i, 1) = data[i].y;
    }

    cv::Mat labels, centers;
    cv::kmeans(points, k, labels,
               cv::TermCriteria(cv::TermCriteria::EPS + cv::TermCriteria::COUNT, 10, 1.0),
               3, cv::KMEANS_PP_CENTERS, centers);

    // 클러스터링 결과를 시각화할 이미지 생성
    outputImage = cv::Mat::zeros(400, 400, CV_8UC3);
    outputImage.setTo(cv::Scalar(255, 255, 255));

    cv::line(outputImage, cv::Point(outputImage.cols / 2, 0), cv::Point(outputImage.cols / 2, outputImage.rows), cv::Scalar(0, 0, 0), 2);
    cv::line(outputImage, cv::Point(0, outputImage.rows / 2), cv::Point(outputImage.cols, outputImage.rows / 2), cv::Scalar(0, 0, 0), 2);

    // 클러스터 색상 설정
    std::vector<cv::Scalar> colors = {cv::Scalar(255, 0, 0), cv::Scalar(0, 255, 0), cv::Scalar(0, 0, 255),
                                      cv::Scalar(255, 255, 0), cv::Scalar(255, 0, 255), cv::Scalar(0, 255, 255)};

    // 각 데이터 포인트를 시각화, 좌표를 화면 크기에 맞게 스케일 조정
    for (size_t i = 0; i < data.size(); ++i) {
        int clusterIdx = labels.at<int>(i);
        cv::Point2f point = data[i] * 10;
        point.x += outputImage.cols / 2;
        point.y = outputImage.rows / 2 - point.y;

        cv::circle(outputImage, point, 3, colors[clusterIdx % colors.size()], cv::FILLED);
    }

    return labels;
}

void MainWindow::runClusteringAndDisplay() {
    auto data = loadData("/home/dy/K-meansclustering/cluster_data_.csv");
    int k = 3;
    cv::Mat clusterImage;
    auto labels = kMeansClustering(data, k, clusterImage);

    QImage qImg(clusterImage.data, clusterImage.cols, clusterImage.rows, clusterImage.step, QImage::Format_RGB888);
    QPixmap pixmap = QPixmap::fromImage(qImg.rgbSwapped());

    ui->resultLabel->setAlignment(Qt::AlignCenter);
    ui->resultLabel->setPixmap(pixmap.scaled(ui->resultLabel->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
}
