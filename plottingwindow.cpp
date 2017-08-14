#include "plottingwindow.h"
#include "ui_plottingwindow.h"
#include <QFileDialog>
#include <QTextStream>
#include "mainwindow.h"

PlottingWindow::PlottingWindow(MainWindow *parent) :
    QMainWindow(parent), w(parent),
    ui(new Ui::PlottingWindow)
{
    ui->setupUi(this);
}

PlottingWindow::~PlottingWindow()
{
    delete ui;
}

QCustomPlot *PlottingWindow::plot()
{
    return this->ui->widget;  //change for a Git
}

void PlottingWindow::setData(QVector<double> values)
{
    data = values;
    QVector<double> xtics(values.size(), 0);
    for(int i = 0; i < xtics.size(); i++) xtics[i] = i;
    ui->widget->graph(0)->setData(xtics, values);
    plot()->xAxis->setRange(0, values.size() + 1);
    plot()->yAxis->setRange(0, (*std::max_element(values.begin(), values.end()))*1.1);
}

void PlottingWindow::on_pBSavePlot_clicked()
{
    this->ui->widget->savePng(QFileDialog::getSaveFileName(this, "PNG file"));
}

void PlottingWindow::on_pBSaveData_clicked()
{
    QFile F(QFileDialog::getSaveFileName(this, "Data file"));
    if (!F.open(QIODevice::WriteOnly | QIODevice::Text)){
        qDebug()<<"Error happend while trying to open file";
        return;
    }

    w->saveToFile(F);

    QTextStream outStream(&F);
    outStream<<"[Data]\n";
    for(int i = 0; i < data.size()-1; i++){
        outStream<<i<<" "<<data[i]<<"\n";
    }
    F.close();
}
