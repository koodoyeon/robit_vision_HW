// mainwindow.h

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <vector>

struct Point {
    double x;
    double y;
};

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;

    std::vector<Point> loadCSV(const std::string &filename);
    std::pair<double, double> ransac(const std::vector<Point>& data);
    void drawDataAndModel();
};

#endif // MAINWINDOW_H
