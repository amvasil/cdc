#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "general_classes/project.h"
#include "net/netclient.h"
namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_initButton_clicked();

    void on_saveButton_clicked();

    void on_openButton_clicked();

    void on_transmitButton_clicked();

    void on_pushButton_clicked();

    void on_pushButton_2_clicked();
    void slot_variables_received();
    void on_connectButton_clicked();
    void slot_connected();

    void on_pushButton_4_clicked();

    void on_pushButton_3_clicked();

private:
    Ui::MainWindow *ui;
    Project *pr;
    NetClient *client;
    int count_updated_vars;
    void repaintPlot();
    int prescaler;
};

#endif // MAINWINDOW_H
