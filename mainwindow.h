#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileSystemModel>
#include <QThread>

namespace Ui {
class MainWindow;
}

using DuplicatesMap = QMap <QByteArray, QVector <QString>>;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:

    void on_selectButton_clicked();

    void on_scanButton_clicked();

    //void delete_duplicates();

    void ask(DuplicatesMap);

private:
    Ui::MainWindow *ui;
    QFileSystemModel *model;
    QDir selected_directory;

    void show_directory();
    void set_selected_directory(const QDir & dir);
};

#endif // MAINWINDOW_H
