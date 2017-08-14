#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFile>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

private:

    void parseFile(QFile &file);

    QVector<double> buildVector();

    int getChannelNumber(double E) const;

public:
    void saveToFile(QFile &file) const;

    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_pbLoad_clicked();

    void on_actionLoad_triggered();

    void on_pbSave_clicked();

    void on_actionAddRow_triggered();

    void on_dataTable_itemSelectionChanged();

    void on_actionRemoveRow_triggered();

    void on_bpRun_clicked();

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
