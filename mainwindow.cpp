#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QDebug>
#include <QTextStream>
#include <QVector>
#include <random>
#include <chrono>
#include "inputvalidatortableitem.h"
#include "plottingwindow.h"
#include "qcustomplot-source/qcustomplot.h"
#include "QtMath"
#include "vector"

void MainWindow::parseFile(QFile &file)
{
    bool block = ui->dataTable->blockSignals(true);
    enum class inputType{
        researchData, linesData, backgroundParams, Noise, Widening, None
    } lastType;

    while(this->ui->dataTable->rowCount()) this->ui->dataTable->removeRow(0);

    while (!file.atEnd()){
        QByteArray line = file.readLine();

        if (line.length() == 0) continue;
        if (line[0] == '/') continue;
        if (line[0] == '\n') continue; //again messege for GIT

        if (line == "[Parameters]\n"){
            lastType = inputType::researchData;
            continue;
        }

        if (line == "[Lines]\n"){
            lastType = inputType::linesData;
            continue;
        }

        if (line == "[Background]\n"){
            lastType = inputType::backgroundParams;
        }

        if (line == "[Noise]\n"){
            lastType = inputType::Noise;
        }

        if (line == "[Widening]\n"){
            lastType = inputType::Widening;
        }

        if (lastType == inputType::None) continue;

        QTextStream inStream(line);

        if (lastType == inputType::linesData){
            int N, enabled;
            double E, I, dE;
            inStream>>N>>E>>I>>dE>>enabled;

            this->on_actionAddRow_triggered();
            int row = this->ui->dataTable->rowCount()-1;

            this->ui->dataTable->item(row, 0)->setText(QString::number(E));
            this->ui->dataTable->item(row, 1)->setText(QString::number(I));
            this->ui->dataTable->item(row, 2)->setText(QString::number(dE));
            static_cast<QCheckBox*>(this->ui->dataTable->cellWidget(row, 3)->children()[1])->setChecked(enabled!=0);
        }

        if (lastType == inputType::researchData){
            double eMin, eMax;
            int channels;
            inStream>>eMin>>eMax>>channels;
            this->ui->inputMinE->setValue(eMin);
            this->ui->inputMaxE->setValue(eMax);
            this->ui->inputNoC->setValue(channels);
        }

        if (lastType == inputType::backgroundParams){
            int backEnabled;
            double A, B, C, D;
            inStream>>backEnabled>>A>>B>>C>>D;
            this->ui->inputBackground->setChecked(backEnabled != 0);
            this->ui->inputA->setValue(A);
            this->ui->inputB->setValue(B);
            this->ui->inputC->setValue(C);
            this->ui->inputD->setValue(D);
        }

        if (lastType == inputType::Noise){
            int cbStstState, rbDEState;
            inStream>>cbStstState>>rbDEState;
            ui->cbStstNoise->setChecked(cbStstState != 0);
            ui->rbP->setChecked(rbDEState != 0);
            ui->rbPG->setChecked(rbDEState == 0);
        }

        if (lastType == inputType::Widening){
            int wenabled, dEnabled;
            double a0, a1;
            inStream>>wenabled>>dEnabled>>a0>>a1;
            this->ui->cbWide->setChecked(wenabled == 1);
            this->ui->rbDE->setChecked(dEnabled != 1);
            this->ui->rbfunction->setChecked(dEnabled == 0);
            this->ui->sba0->setValue(a0);
            this->ui->sba1->setValue(a1);
        }

    }

    ui->dataTable->blockSignals(block);
    return;
}

void MainWindow::saveToFile(QFile &file) const
{
    QTextStream outFile(&file);
    outFile<<"[Parameters]"<<"\n";
    outFile<<(ui->inputMinE->value())<<" ";
    outFile<<ui->inputMaxE->value()<<" ";
    outFile<<ui->inputNoC->value()<<"\n";

    outFile<<"[Lines]"<<"\n";
    for(int i = 0; i < ui->dataTable->rowCount(); i++){
        outFile<<i+1<<" ";
        outFile<<ui->dataTable->item(i, 0)->text()<<" ";
        outFile<<ui->dataTable->item(i, 1)->text()<<" ";
        outFile<<ui->dataTable->item(i, 2)->text()<<" ";
        outFile<<static_cast<QCheckBox*>(this->ui->dataTable->cellWidget(i, 3)->children()[1])->isChecked()<<"\n";
    }

    outFile<<"[Background]"<<"\n";
    outFile<<ui->inputBackground->isChecked()<<" ";
    outFile<<ui->inputA->value()<<" ";
    outFile<<ui->inputB->value()<<" ";
    outFile<<ui->inputC->value()<<" ";
    outFile<<ui->inputD->value()<<"\n";

    outFile<<"[Noise]"<<"\n";
    outFile<<ui->cbStstNoise->isChecked()<<" ";
    outFile<<ui->rbP->isChecked()<<"\n";

    outFile<<"[Widening]"<<"\n";
    outFile<<ui->cbWide->isChecked()<<" ";
    outFile<<ui->rbDE->isChecked()<<" ";
    outFile<<ui->sba0->value()<<" ";
    outFile<<ui->sba1->value()<<"\n";
}

QVector<double> MainWindow::buildVector()
{
    int N = this->ui->inputNoC->value();
    double lowerBound = this->ui->inputMinE->value();
    double upperBound = this->ui->inputMaxE->value();
    double dev = (upperBound - lowerBound) / N;

    QVector<double> result(N, 0);
    QVector<double> wideback(N, 0);
    QVector<double> compton(N, 0);


    if (this->ui->inputBackground->isChecked()){
        double A = this->ui->inputA->value()*10000;
        double B = this->ui->inputB->value();
        double C = this->ui->inputC->value()*1000;
        double D = this->ui->inputD->value()*1000;

        for(int i = 0; i < N; i++){
            double lb = i*dev + lowerBound;
            double rb = lb+dev;
            result[i]-= (C*lb*lb)/2 + D*lb;
            result[i]+= (C*rb*rb)/2 + D*rb;
            if (result[i] < 0) result[i] = 0;
            result[i]-= (-A/B)*exp(-B*lb) ;
            result[i]+= (-A/B)*exp(-B*rb) ;

           }

        }

/*    for(int i = 0; i < this->ui->dataTable->rowCount(); i++){
        double energy = this->ui->dataTable->item(i, 0)->text() .toDouble();
        if ( !static_cast<QCheckBox*>(this->ui->dataTable->cellWidget(i, 3)->children()[1])->isChecked() )
            continue;
        if (energy < 0){
            qDebug()<<"PIME TARADOX";
            continue;
        }
        double intensity = this->ui->dataTable->item(i, 1)->text().toDouble();
            int cNumber = getChannelNumber(energy);
            if (cNumber < 0) continue;
            double pi = 3.14159;
        for(int i = 1; i < 1000*N; i++){
            int Cplus = getChannelNumber(energy-energy/(1+2*(energy/0.511)*(1-qCos(i*pi/2/(1000*N)))));

            if((Cplus>0)*(Cplus<getChannelNumber(energy-energy/(1+0.5*(energy/0.511)*(1-qCos(pi/2)))))){
            result[Cplus] += intensity/N*qCos(i*pi/2/N/1000)*qCos(i*pi/2/N/1000)/10;

        }
         }
}
*/
    if (this->ui->cbWide->isChecked())
    {
        for(int j = 0; j < N; j++){
        double lb = j*dev + lowerBound;
        double rb = lb+dev;
        double cb = (lb+rb)/2;
        double sigma = this->ui->sba0->value() + this->ui->sba1->value()*sqrt(cb);
        sigma /= sqrt(8*log(2));
        double x = lowerBound - N/10*dev;
        for(int i = -N/10; i < N; i++, x += dev)
        {
           if (i < 0){
               wideback[0] -= result[j] * 1.0/2.0 * (1 + erf((x - cb)/sqrt(2)/sigma));
               wideback[0] += result[j] * 1.0/2.0 * (1 + erf((x + dev - cb)/sqrt(2)/sigma));
           }
               else{
            wideback[i] -= result[j] * 1.0/2.0 * (1 + erf((x - cb)/sqrt(2)/sigma));
            wideback[i] += result[j] * 1.0/2.0 * (1 + erf((x + dev - cb)/sqrt(2)/sigma));
               }

        }
      }
      for(int i = 0; i < N; i++){
          result[i] = wideback[i];
      }
    }

    for(int i = 0; i < this->ui->dataTable->rowCount(); i++){
        double energy = this->ui->dataTable->item(i, 0)->text() .toDouble();
        if ( !static_cast<QCheckBox*>(this->ui->dataTable->cellWidget(i, 3)->children()[1])->isChecked() )
            continue;
        if (energy < 0){
            qDebug()<<"PIME TARADOX";
            continue;
        }
        double intensity = this->ui->dataTable->item(i, 1)->text().toDouble();
        if (!this->ui->cbWide->isChecked()){
            int cNumber = getChannelNumber(energy);
            if (cNumber < 0) continue;
            result[cNumber] += intensity;
        }
        else{
            double sigma = 1;
            if (this->ui->rbDE->isChecked()) sigma = this->ui->dataTable->item(i, 2)->text().toDouble();
            if (this->ui->rbfunction->isChecked()) sigma = this->ui->sba0->value() + this->ui->sba1->value()*sqrt(energy);
            sigma /= sqrt(8*log(2));
            double x = lowerBound;
            for(int i = 0; i < N; i++, x += dev){
                result[i] -= intensity * 1.0/2.0 * (1 + erf((x - energy)/sqrt(2)/sigma));
                result[i] += intensity * 1.0/2.0 * (1 + erf((x + dev - energy)/sqrt(2)/sigma));
            }
        }
    }


    if (this->ui->cbStstNoise->isChecked()){
        unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
        std::default_random_engine gen(seed);
        std::normal_distribution<double> dist(0, 1);
        for(int i = 0; i < N; i++){
            if (result[i] > 10 || this->ui->rbP->isChecked()) // GAUSS
                result[i] += sqrt(result[i])*dist(gen);
            else{
                std::poisson_distribution<int> dist2((int)result[i]); // AGATA POISSON
                result[i] = dist2(gen);
            }
        }
    }

    for(int i = 0; i < N; i++){
        result[i] = int(result[i]);
        if (result[i] < 0) result[i] = 0;
    }


    return result;
}

int MainWindow::getChannelNumber(double E) const
{
    double lowerBound = this->ui->inputMinE->value();
    double upperBound = this->ui->inputMaxE->value();
    int channelsN = this->ui->inputNoC->value();
    if (E < lowerBound) return -1;
    if (E > upperBound) return -1;
    double dev = (upperBound - lowerBound) / channelsN;
    double level = E-lowerBound;
    return floor(level/dev);

}


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    this->ui->dataTable->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    this->ui->dataTable->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    this->ui->dataTable->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Stretch);

    auto V = new QDoubleValidator;
    QLocale lo(QLocale::C);
    lo.setNumberOptions(QLocale::RejectGroupSeparator);
    V->setLocale(lo);

    QItemDelegate *itemDoubleDelegate = new InputValidatorTableItem(V);
    this->ui->dataTable->setItemDelegateForColumn(0, itemDoubleDelegate);
    this->ui->dataTable->setItemDelegateForColumn(1, itemDoubleDelegate);
    this->ui->dataTable->setItemDelegateForColumn(2, itemDoubleDelegate);

    this->addAction(this->ui->actionLoad);
    this->addAction(this->ui->actionSave);

    this->ui->dataTable->addAction(this->ui->actionAddRow);
    this->ui->dataTable->addAction(this->ui->actionRemoveRow);
    this->ui->actionRemoveRow->setEnabled(false);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pbLoad_clicked()
{
    QString filename = QFileDialog::getOpenFileName();
    if (filename == "") return;

    QFile F(filename, this);

    if (!F.open(QIODevice::ReadOnly | QIODevice::Text)){
        qDebug()<<"Error happend while trying to open file "<<filename;
        return;
    }

    parseFile(F);
}

void MainWindow::on_pbSave_clicked()
{
    QString filename = QFileDialog::getSaveFileName();
    if (filename == "") return;

    QFile F(filename, this);
    if (!F.open(QIODevice::WriteOnly | QIODevice::Text)){
        qDebug()<<"Error happend while trying to open file "<<filename;
        return;
    }

    saveToFile(F);
}

void MainWindow::on_actionLoad_triggered()
{
    on_pbLoad_clicked();
}

void MainWindow::on_actionAddRow_triggered()
{
    int row = this->ui->dataTable->rowCount();
    this->ui->dataTable->insertRow(row);
    this->ui->dataTable->setItem(row, 0, new QTableWidgetItem("0"));
    this->ui->dataTable->setItem(row, 1, new QTableWidgetItem("0"));
    this->ui->dataTable->setItem(row, 2, new QTableWidgetItem("0"));
    QWidget *w = new QWidget();
    QCheckBox *cb = new QCheckBox();
    QHBoxLayout *lay = new QHBoxLayout(w);
    lay->addWidget(cb);
    lay->setAlignment(Qt::AlignCenter);
    lay->setContentsMargins(0,0,0,0);
    w->setLayout(lay);
    this->ui->dataTable->setCellWidget(row, 3, w);
}

void MainWindow::on_dataTable_itemSelectionChanged()
{
    this->ui->actionRemoveRow->setText("Remove row");
    this->ui->actionRemoveRow->setEnabled(this->ui->dataTable->selectionModel()->hasSelection());
    if(this->ui->dataTable->selectionModel()->selectedRows().count() > 1)
        this->ui->actionRemoveRow->setText("Remove rows");
}

void MainWindow::on_actionRemoveRow_triggered()
{
    while(!this->ui->dataTable->selectionModel()->selectedRows().empty()){
        this->ui->dataTable->removeRow(this->ui->dataTable->selectionModel()->selectedRows().back().row());
    }
    this->ui->dataTable->clearSelection();
}

void MainWindow::on_bpRun_clicked()
{
    PlottingWindow *W = new PlottingWindow(this);

    W->plot()->addGraph();
    W->setData(buildVector());
    W->plot()->graph(0)->setLineStyle(QCPGraph::lsStepLeft);
    W->show();
}
