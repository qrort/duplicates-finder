#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QCommonStyle>
#include <QDesktopWidget>
#include <QFileDialog>
#include <QFileInfo>
#include <QMessageBox>
#include <QTreeView>
#include <QDebug>
#include <QCryptographicHash>
#include <QFile>
#include <QListWidget>
#include <QListWidgetItem>
#include <QTextEdit>
#include <unordered_map>
#include <vector>

namespace tools {
    QString const prefix = "Selected directory: ";
    QCryptographicHash hasher(QCryptographicHash::Sha256);
    long long const p = 31541;
    long long const mod = 1e9 + 9;
    QString const search_results_title = "Search results:";
}

void MainWindow::set_selected_directory(const QDir & dir) {
    selected_directory = dir;
    ui->lineEdit->setText(tools::prefix + dir.absolutePath());
}

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setGeometry(QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter, 3 * size() / 2, qApp->desktop()->availableGeometry()));
    // model->setFilter(QDir::NoSymLinks);
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
                                                        QString(), QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);

    set_selected_directory(QDir(dir));
    ui->treeView->setRootIndex(model->index(dir));
}

long long weak_hash(const QFileInfo &file_info) {
    QFile file(file_info.absoluteFilePath());
    file.open(QIODevice::ReadOnly);
    //return file.size();
    long long res = file.size();
    int i = 0;
    for (; i < 10 && !file.atEnd(); i++) {
        char* c;
        file.read(c, sizeof(char));
        res = (res * tools::p + (*c) + 1) % tools::mod;
    }
    for (; i < 10; i++) {
        res = (res * tools::p) % tools::mod;
    }
    file.close();
    return res;
}

QByteArray sha256(const QFileInfo &file_info) {
    QFile file(file_info.absoluteFilePath());
    file.open(QIODevice::ReadOnly);
    tools::hasher.reset();
    tools::hasher.addData(&file);
    file.close();
    return tools::hasher.result();
}

void MainWindow::delete_duplicates() {
    for (auto it = sha256_hashes.begin(); it != sha256_hashes.end(); it++) {
        if (it->second.size() > 1) {
            for (size_t i = 1; i < it->second.size(); i++) {
                QFile file(it->second[i]);
                file.remove();
            }
        }
    }
    sha256_hashes.clear();
}

void MainWindow::ask() {
    using namespace std;
    QWidget* form = new QWidget;
    form->setWindowTitle(tools::search_results_title);
    form->setGeometry(QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter, 3 * form->size() / 2, qApp->desktop()->availableGeometry()));
    QVBoxLayout *layout = new QVBoxLayout();
    QListWidget *list = new QListWidget(form);
    int rowcnt = 0;
    for (auto it = sha256_hashes.begin(); it != sha256_hashes.end(); it++) {
        if (it->second.size() > 1) {
            for (QString duplicate : it->second) {
                QListWidgetItem *newItem = new QListWidgetItem;
                newItem->setText(duplicate);
                list->insertItem(rowcnt++, newItem);
            }
            QListWidgetItem *newItem = new QListWidgetItem;
            newItem->setText("--");
            list->insertItem(rowcnt++, newItem);
        }
    }
    QTextEdit *text = new QTextEdit;
    text->setPlainText("Delete duplicates? (the first file in corresponding list remains)");
    QPushButton *button = new QPushButton("Delete", form);
    layout->addWidget(list);
    layout->addWidget(text);
    layout->addWidget(button);
    form->setLayout(layout);
    QObject::connect(button, &QPushButton::clicked, this, &MainWindow::delete_duplicates);
    QObject::connect(button, SIGNAL(clicked()), form, SLOT(close()));

    form->show();
}

void MainWindow::on_scanButton_clicked()
{

    using namespace std;
    unsigned int start = clock();
    unordered_map <long long, vector <QString>> weak_hashes;
    QDirIterator it(selected_directory, QDirIterator::Subdirectories);
    while (it.hasNext()) {
        QFileInfo file_info(it.next());
        if (file_info.isFile()) {
            weak_hashes[weak_hash(file_info)].push_back(it.filePath());
         }
    }
    for (auto it = weak_hashes.begin(); it != weak_hashes.end(); it++) {
        if (it->second.size() > 1) {
            for (QString &i : it->second) sha256_hashes[sha256(i)].push_back(i);
        };
    }
    ask();
}
