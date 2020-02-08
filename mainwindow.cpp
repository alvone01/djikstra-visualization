#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "canvas.h"
#include <QDebug>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    host = new Canvas(this);
    connect(host, &Canvas::calculationFinished, this, &MainWindow::onCalculationFinished);
    connect(host, &Canvas::calculationCleared, this, &MainWindow::onCalculationCleared);
    connect(host, &Canvas::calculationStarted, this, &MainWindow::onCalculationStarted);
    ui->drawVertexButton->setCheckable(true);
    ui->drawEdgeButton->setCheckable(true);
    ui->startButton->setCheckable(true);
    ui->showPathButton->setCheckable(true);
}

MainWindow::~MainWindow()
{
    delete ui;
    delete host;
}

void MainWindow::on_drawVertexCB_stateChanged(int arg1)
{
    if (arg1) {
        ui->drawEdgeCB->setCheckState(Qt::CheckState::Unchecked);
        ui->startCB->setCheckState(Qt::CheckState::Unchecked);
        Canvas::actionType = ActionType::DrawVertex;

    } else {
        if (ui->drawEdgeCB->checkState() == Qt::CheckState::Unchecked) {
            Canvas::actionType = ActionType::MoveVertex;
        }
    }
}

void MainWindow::on_drawEdgeCB_stateChanged(int arg1)
{
    if (arg1) {
        ui->drawVertexCB->setCheckState(Qt::CheckState::Unchecked);
        ui->startCB->setCheckState(Qt::CheckState::Unchecked);
        Canvas::actionType = ActionType::DrawEdge;
    } else {
        if (ui->drawVertexCB->checkState() == Qt::CheckState::Unchecked) {
            Canvas::actionType = ActionType::MoveVertex;
        }
    }
}

void MainWindow::on_startCB_stateChanged(int arg1)
{
    if (arg1) {
        ui->drawVertexCB->setCheckState(Qt::CheckState::Unchecked);
        ui->drawEdgeCB->setCheckState(Qt::CheckState::Unchecked);
        Canvas::actionType = ActionType::PickSource;
    } else {
        if (ui->drawVertexCB->checkState() == Qt::CheckState::Unchecked) {
            Canvas::actionType = ActionType::MoveVertex;
        }
    }
}

void MainWindow::on_clearButton_clicked()
{
    host->clearDjikstra();
    Canvas::actionType = ActionType::MoveVertex;
    ui->showPathButton->setChecked(false);
}

void MainWindow::on_drawVertexButton_clicked()
{
    if (djikstraDone) {
        ui->drawVertexButton->setChecked(false);
        return;
    }
    if (ui->drawVertexButton->isChecked()) {

        ui->drawEdgeButton->setChecked(false);
        ui->startButton->setChecked(false);
        ui->showPathButton->setChecked(false);
        Canvas::actionType = ActionType::DrawVertex;
        host->resetCreateEdge();

    } else {
        Canvas::actionType = ActionType::MoveVertex;
    }
}


void MainWindow::on_drawEdgeButton_clicked()
{
    if (djikstraDone) {
        ui->drawEdgeButton->setChecked(false);
        return;
    }
    if (ui->drawEdgeButton->isChecked()) {

        ui->drawVertexButton->setChecked(false);
        ui->startButton->setChecked(false);
        ui->showPathButton->setChecked(false);
        Canvas::actionType = ActionType::DrawEdge;

    } else {
        Canvas::actionType = ActionType::MoveVertex;
        host->resetCreateEdge();
    }
}

void MainWindow::on_startButton_clicked()
{
    if (djikstraDone) {
        ui->startButton->setChecked(!ui->startButton->isChecked());
        return;
    }
    if (ui->startButton->isChecked()) {

        ui->drawVertexButton->setChecked(false);
        ui->drawEdgeButton->setChecked(false);
        ui->showPathButton->setChecked(false);
        Canvas::actionType = ActionType::PickSource;
    } else {
        Canvas::actionType = ActionType::MoveVertex;
    }
}

void MainWindow::on_showPathButton_clicked()
{
    if (ui->showPathButton->isChecked()) {

        ui->drawVertexButton->setChecked(false);
        ui->drawEdgeButton->setChecked(false);
        ui->startButton->setChecked(false);
        Canvas::actionType = ActionType::PickDestination;
    } else {
        Canvas::actionType = ActionType::MoveVertex;
        host->cleanPath(Qt::gray);
    }
}

void MainWindow::onCalculationFinished() {
    ui->startButton->setChecked(false);
    ui->showPathButton->setChecked(true);
    Canvas::actionType = ActionType::PickDestination;
    djikstraDone = true;
}

void MainWindow::on_randButton_clicked()
{
    host->randGenerate();
}

void MainWindow::on_stepButton_clicked()
{
    host->stepCalculation();
}

void MainWindow::onCalculationCleared() {
    djikstraDone = false;
}

void MainWindow::onCalculationStarted() {
    djikstraDone = true;
}
