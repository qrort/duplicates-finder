#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileSystemModel>
#include <QDir>
#include <QMap>
#include <QVector>
#include <QThread>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void show_directory();

private slots:

    void on_selectButton_clicked();

    void on_scanButton_clicked();

    void delete_duplicates();

private:
    Ui::MainWindow *ui;
    QFileSystemModel *model;
    QDir selected_directory;
    QMap <QByteArray, QVector <QString>>  sha256_hashes;
    void set_selected_directory(const QDir & dir);
    void ask();
    QThread *thread;
};

#endif // MAINWINDOW_H
