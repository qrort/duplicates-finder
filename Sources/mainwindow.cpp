#include "../Headers/mainwindow.h"
#include "ui_mainwindow.h"
#include "../Headers/hasher.h"
#include "../Headers/askwidget.h"
#include "ui_askwidget.h"
#include <QCommonStyle>
#include <QDesktopWidget>
#include <QFileDialog>
#include <QProgressDialog>
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

    progress = 0;
    ui->progressBar->setValue(0);
    ui->treeView->setModel(model);
    ui->treeView->setColumnWidth(0, 320);
    ui->treeView->setRootIndex(model->index(QDir::homePath()));
    set_selected_directory(QDir::home());

    qRegisterMetaType<DuplicatesMap>("DuplicatesMap");
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

void MainWindow::update_progress() {
    ui->progressBar->setValue(++progress);
}

void MainWindow::reset_progress() {
    hashing_thread->quit();
    hashing_thread->wait();
    progress = 0;
    ui->progressBar->setValue(0);
    delete hashing_thread;
    hashing_thread = nullptr;
}

void MainWindow::list_error(QString message) {
    QListWidgetItem *newItem = new QListWidgetItem;
    newItem->setText(message);
    ui->errorList->insertItem(errors++, newItem);
}

void MainWindow::ask(DuplicatesMap sha256_hashes) {
    AskWidget *askWidget = new AskWidget(sha256_hashes);
    reset_progress();
    askWidget->show();
}

int MainWindow::count() {
    int result = 0;
    QDirIterator it(selected_directory.absolutePath(), QDir::Dirs | QDir::Files | QDir::Hidden | QDir::NoDotAndDotDot | QDir::NoSymLinks, QDirIterator::Subdirectories);
    while (it.hasNext()) {
        QFileInfo file_info(it.next());
        if (file_info.isFile()) result++;
    }
    return result;
}

void MainWindow::on_scanButton_clicked()
{
    if (hashing_thread == nullptr) {
        hashing_thread = new QThread;
        Hasher *hasher = new Hasher(selected_directory);
        int files_count = count();
        progress = errors = 0;
        ui->progressBar->setValue(0);
        ui->progressBar->setMaximum(2 * files_count);
        hasher->moveToThread(hashing_thread);
        connect(hasher, &Hasher::Done, this, &MainWindow::ask);
        connect(hashing_thread, &QThread::started, hasher, &Hasher::HashEntries);
        connect(hasher, &Hasher::FileHashed, this, &MainWindow::update_progress);
        hashing_thread->start();
    }
}

void MainWindow::on_cancelButton_clicked()
{
    if (hashing_thread != nullptr && hashing_thread->isRunning()) {
        hashing_thread->requestInterruption();
        reset_progress();
    }
}
