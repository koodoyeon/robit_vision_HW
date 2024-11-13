#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <QPixmap>
#include <vector>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

struct Point {
    double x;
    double y;
};

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    QPixmap chartPixmap;
    double a, b;

    void loadData(const QString &filename);
    void calculateLeastSquares();
    void plotData();

    std::vector<Point> data;
};

#endif // MAINWINDOW_H
