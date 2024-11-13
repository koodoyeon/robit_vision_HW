/*
a: -27.7402 , b: -141.3

반복 횟수 및 인라이어 허용 거리 설정 이유

N = log(1 - p) / log(1 - (1 - ε)^s)를 사용하여 지정.

여기서 P는 적어도 하나의 좋은 모델을 찾을 확률이고, ε 아웃라이어의 비율, S는 모델을 만들기 위한 최소 데이터 포인트 수이다.

식에 값을 대입하여 계산한 반복 횟수는:
N = log(1-0.99) / log(1-(1-0.5)^2) ≈ 16

이론값보다 안정적인 결과를 위해 반복 횟수를 더 많이 설정

데이터 특성:
- 데이터 포인트가 약 200개로 중간 크기
- 노이즈와 아웃라이어가 많음
- 특이한 0값 연속 패턴 존재
- 따라서 반복 횟수를 1000으로 설정

인라이어 허용 거리 설정 이유:
- y값의 분포:
  - 최대값 약 700, 최소값 약 -2500, 전체 범위 약 3200
  - 평균 변동폭 약 200~300
    때문에 인라이어 허용 거리를 200으로 설정하여 모델에 충분히 가까운 점을 인라이어로 간주
*/


#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <fstream>
#include <sstream>
#include <QPainter>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <limits>
#include <QDebug>

const int ITERATIONS = 1000;
const double DIST_THRESHOLD = 200.0;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    drawDataAndModel();
}

MainWindow::~MainWindow()
{
    delete ui;
}

// CSV 파일 로딩 함수
std::vector<Point> MainWindow::loadCSV(const std::string &filename) {
    std::vector<Point> data;
    std::ifstream file(filename);

    if (!file.is_open()) {
        qWarning("Failed to open file: %s", filename.c_str());
        return data;
    }

    std::string line;
    bool isFirstLine = true;

    while (std::getline(file, line)) {
        if (isFirstLine) {
            isFirstLine = false;
            continue;
        }

        std::stringstream ss(line);
        std::string x_str, y_str;

        if (std::getline(ss, x_str, ',') && std::getline(ss, y_str, ',')) {
            try {
                if (!x_str.empty() && !y_str.empty()) {
                    Point point;
                    point.x = std::stod(x_str);
                    point.y = std::stod(y_str);
                    data.push_back(point);
                }
            } catch (const std::invalid_argument& e) {
                qWarning("Invalid data found in CSV: %s", line.c_str());
            }
        }
    }
    return data;
}

// RANSAC 함수 정의
std::pair<double, double> MainWindow::ransac(const std::vector<Point>& data) {
    int maxInliers = 0;
    double bestA = 0, bestB = 0;

    for (int i = 0; i < ITERATIONS; ++i) {
        int idx1 = std::rand() % data.size();
        int idx2 = std::rand() % data.size();
        if (idx1 == idx2) continue;

        Point p1 = data[idx1];
        Point p2 = data[idx2];

        double a = (p2.y - p1.y) / (p2.x - p1.x);
        double b = p1.y - a * p1.x;

        int inliers = 0;
        for (const auto& p : data) {
            double dist = std::abs(a * p.x + b - p.y) / std::sqrt(a * a + 1);
            if (dist < DIST_THRESHOLD) {
                ++inliers;
            }
        }

        if (inliers > maxInliers) {
            maxInliers = inliers;
            bestA = a;
            bestB = b;
        }
    }

    // 구한 기울기(a)와 절편(b) 값을 출력
    qDebug() << "Calculated a:" << bestA << ", b:" << bestB;

    return {bestA, bestB};
}

// 데이터 및 모델 시각화 함수
void MainWindow::drawDataAndModel() {
    std::vector<Point> data = loadCSV(DATA_FILE_PATH);
    if (data.empty()) {
        qWarning("Data is empty, check if the file path is correct.");
        return;
    }

    auto [a, b] = ransac(data);  // RANSAC 모델 계산

    QPixmap pixmap(ui->label->size());
    pixmap.fill(Qt::white);
    QPainter painter(&pixmap);

    // 데이터 범위에 맞춰 스케일링 계산
    double minX = std::numeric_limits<double>::max();
    double maxX = std::numeric_limits<double>::min();
    double minY = std::numeric_limits<double>::max();
    double maxY = std::numeric_limits<double>::min();

    for (const auto& p : data) {
        minX = std::min(minX, p.x);
        maxX = std::max(maxX, p.x);
        minY = std::min(minY, p.y);
        maxY = std::max(maxY, p.y);
    }

    // QPixmap의 크기에 맞춰 스케일 조정
    double scaleX = pixmap.width() / (maxX - minX);
    double scaleY = pixmap.height() / (maxY - minY);

    painter.setPen(Qt::green);
    for (const auto& p : data) {
        int x = static_cast<int>((p.x - minX) * scaleX);
        int y = pixmap.height() - static_cast<int>((p.y - minY) * scaleY); // Y 방향 반전
        painter.drawEllipse(x, y, 3, 3);
    }

    // RANSAC 직선 그리기
    painter.setPen(Qt::red);
    int x1 = 0;
    int y1 = pixmap.height() - static_cast<int>((a * minX + b - minY) * scaleY);
    int x2 = pixmap.width();
    int y2 = pixmap.height() - static_cast<int>((a * maxX + b - minY) * scaleY);
    painter.drawLine(x1, y1, x2, y2);

    ui->label->setPixmap(pixmap);
}
