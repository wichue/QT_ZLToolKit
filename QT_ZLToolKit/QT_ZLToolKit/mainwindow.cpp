#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "qt_test/TestClient.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    test_TcpClient();
}

MainWindow::~MainWindow()
{
    delete ui;
}

