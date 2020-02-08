#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "canvas.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:

    void on_drawVertexCB_stateChanged(int arg1);

    void on_drawEdgeCB_stateChanged(int arg1);

    void on_startCB_stateChanged(int arg1);

    void on_clearButton_clicked();

    void on_drawVertexButton_clicked();

    void on_drawEdgeButton_clicked();

    void on_startButton_clicked();

    void on_showPathButton_clicked();

    void onCalculationFinished();

    void on_randButton_clicked();

    void on_stepButton_clicked();

    void onCalculationCleared();

    void onCalculationStarted();

private:
    Ui::MainWindow *ui;
    Canvas* host;
    bool djikstraDone = false;
};
#endif // MAINWINDOW_H
