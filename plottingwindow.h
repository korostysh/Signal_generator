#ifndef PLOTTINGWINDOW_H
#define PLOTTINGWINDOW_H

#include <QMainWindow>
#include "mainwindow.h"
class QCustomPlot;
namespace Ui {
class PlottingWindow;
}

class PlottingWindow : public QMainWindow
{
    Q_OBJECT

    QVector<double> data;

    MainWindow *w;

public:
    explicit PlottingWindow(MainWindow *parent = 0);
    ~PlottingWindow();

    QCustomPlot *plot();

    void setData(QVector<double> values);

private slots:
    void on_pBSavePlot_clicked();

    void on_pBSaveData_clicked();

private:
    Ui::PlottingWindow *ui;
};

#endif // PLOTTINGWINDOW_H
