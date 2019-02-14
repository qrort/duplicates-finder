#include "../Headers/askwidget.h"
#include "ui_askwidget.h"
#include <QDesktopWidget>
#include <QFile>

AskWidget::AskWidget(DuplicatesVector const& _data, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AskWidget)
{
    ui->setupUi(this);
    setGeometry(QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter, 3 * size() / 2, qApp->desktop()->availableGeometry()));
    data = _data;

    setWindowTitle("Search results:");
    size_t rowcnt = 0;
    for (auto& el : data) {
        if (el.size() > 1) {
            for (QString& duplicate : el) {
                QListWidgetItem *newItem = new QListWidgetItem;
                newItem->setText(duplicate);

                ui->listWidget->insertItem(rowcnt++, newItem);
            }
            QListWidgetItem *newItem = new QListWidgetItem;
            newItem->setText("--");
            ui->listWidget->insertItem(rowcnt++, newItem);
        }
    }

    QListWidgetItem* item = 0;
    for(int i = 0; i < ui->listWidget->count(); i++){
        item = ui->listWidget->item(i);
        if (item->text() != "--") {
            item->setFlags(item->flags() | Qt::ItemIsUserCheckable);
            if (i && ui->listWidget->item(i - 1)->text() != "--") {
               item->setCheckState(Qt::Unchecked);
            } else {
                item->setCheckState(Qt::Checked);
            }
        }

    }

    connect(ui->deleteButton, &QPushButton::clicked, this, &AskWidget::deleteDuplicates);
    connect(ui->cancelButton, SIGNAL(clicked()), this, SLOT(close()));
}

void AskWidget::deleteDuplicates() {
    for (int i = 0; i < ui->listWidget->count(); i++) {
        if (ui->listWidget->item(i)->checkState() == Qt::Unchecked) {
            QFile file(ui->listWidget->item(i)->text());
            file.remove();
        }
    }/*
    for (auto it = data.begin(); it != data.end(); it++) {
        if (it.value().size() > 1) {
            for (size_t i = 1; i < it.value().size(); i++) {
                QFile file(it.value()[i]);
                file.remove();
            }
        }
    }*/
    close();
}


AskWidget::~AskWidget()
{
    delete ui;
    data.clear();
}
