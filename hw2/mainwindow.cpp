// a = -1.23206 , b = 44.1683

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFile>
#include <QTextStream>
#include <QPainter>
#include <QMessageBox>
#include <QDebug>
#include <cmath>

const double DIST_THRESHOLD = 200.0;  // 인라이어 허용 거리

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);

    chartPixmap = QPixmap(ui->chartLabel->size());
    chartPixmap.fill(Qt::white);

    QString filename = "/home/dy/leastsquaresmethod/First-order function.csv";

    if (!QFile::exists(filename)) {
        QMessageBox::warning(this, "파일 오류", "지정한 경로에 파일이 존재하지 않습니다:\n" + filename);
        return;
    }

    loadData(filename);
    calculateLeastSquares();
    plotData();

    ui->chartLabel->setPixmap(chartPixmap);
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::loadData(const QString &filename) {
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning("Could not open file");
        return;
    }

    QTextStream in(&file);
    while (!in.atEnd()) {
        QString line = in.readLine();
        QStringList values = line.split(",");
        if (values.size() == 2) {
            Point point;
            point.x = values[0].toDouble();
            point.y = values[1].toDouble();
            data.push_back(point);
        }
    }
    file.close();
}

void MainWindow::calculateLeastSquares() {
    double sumX = 0, sumY = 0, sumXY = 0, sumX2 = 0;
    int inlierCount = 0;

    // 인라이어 데이터만을 사용하여 최소제곱법 계산
    for (const auto &point : data) {
        // y값이 허용 거리 내에 있는 데이터만 사용
        if (std::abs(point.y) <= DIST_THRESHOLD) {
            sumX += point.x;
            sumY += point.y;
            sumXY += point.x * point.y;
            sumX2 += point.x * point.x;
            ++inlierCount;
        }
    }

    // 최소제곱법 공식에 따라 a와 b를 계산
    a = (inlierCount * sumXY - sumX * sumY) / (inlierCount * sumX2 - sumX * sumX);
    b = (sumY - a * sumX) / inlierCount;

    qDebug() << "Calculated values:" << "a =" << a << ", b =" << b;
    ui->resultLabel->setText(QString("a = %1, b = %2").arg(a).arg(b));
}

void MainWindow::plotData() {
    QPainter painter(&chartPixmap);
    painter.setRenderHint(QPainter::Antialiasing);

    const int scaleX = 10;
    const int scaleY = 10;

    painter.setPen(Qt::black);
    int centerX = ui->chartLabel->width() / 2;
    int centerY = ui->chartLabel->height() / 2;
    painter.drawLine(centerX, 0, centerX, ui->chartLabel->height());
    painter.drawLine(0, centerY, ui->chartLabel->width(), centerY);

    painter.setPen(Qt::blue);
    for (const auto &point : data) {
        int x = centerX + static_cast<int>(point.x * scaleX);
        int y = centerY - static_cast<int>(point.y * scaleY);
        painter.drawEllipse(QPoint(x, y), 3, 3);
    }

    painter.setPen(Qt::red);
    int x_min = -centerX / scaleX;
    int x_max = centerX / scaleX;
    int y_min = static_cast<int>(a * x_min + b);
    int y_max = static_cast<int>(a * x_max + b);

    painter.drawLine(QPoint(centerX + x_min * scaleX, centerY - y_min * scaleY),
                     QPoint(centerX + x_max * scaleX, centerY - y_max * scaleY));

    ui->chartLabel->setPixmap(chartPixmap);
}
