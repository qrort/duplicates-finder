#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "hasher.h"
#include "askwidget.h"
#include "ui_askwidget.h"
#include <QCommonStyle>
#include <QDesktopWidget>
#include <QFileDialog>
#include <QTreeView>
#include <QDebug>

void MainWindow::set_selected_directory(const QDir & dir) {
    selected_directory = dir;
    ui->lineEdit->setText("Selected directory: " + dir.absolutePath());
}

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setGeometry(QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter, 3 * size() / 2, qApp->desktop()->availableGeometry()));
    model = new QFileSystemModel(this);
    model->setRootPath(QDir::homePath());

    ui->treeView->setModel(model);
    ui->treeView->setColumnWidth(0, 320);
    ui->treeView->setRootIndex(model->index(QDir::homePath()));
    set_selected_directory(QDir::home());
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_selectButton_clicked()
{

    QString dir = QFileDialog::getExistingDirectory(this, "Select Directory for Scanning",
                                                        selected_directory.absolutePath(), QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);

    if (!dir.isEmpty()) {
        set_selected_directory(QDir(dir));
         ui->treeView->setRootIndex(model->index(dir));
    }
}

void MainWindow::ask(DuplicatesMap sha256_hashes) {
    AskWidget *askWidget = new AskWidget(sha256_hashes);
    askWidget->show();
}

void MainWindow::on_scanButton_clicked()
{
    Hasher *hasher = new Hasher(selected_directory);
    QObject::connect(hasher, &Hasher::Done, this, &MainWindow::ask);
    hasher->HashEntries();
}
