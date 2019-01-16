#include "askwidget.h"
#include "ui_askwidget.h"
#include <QDesktopWidget>
#include <QFile>

AskWidget::AskWidget(DuplicatesMap const& _data, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AskWidget)
{
    ui->setupUi(this);
    setGeometry(QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter, 3 * size() / 2, qApp->desktop()->availableGeometry()));
    data = _data;

    setWindowTitle("Search results:");
    size_t rowcnt = 0;
    for (auto it = data.begin(); it != data.end(); it++) {
        if (it.value().size() > 1) {
            for (QString duplicate : it.value()) {
                QListWidgetItem *newItem = new QListWidgetItem;
                newItem->setText(duplicate);
                ui->listWidget->insertItem(rowcnt++, newItem);
            }
            QListWidgetItem *newItem = new QListWidgetItem;
            newItem->setText("--");
            ui->listWidget->insertItem(rowcnt++, newItem);
        }
    }

    QObject::connect(ui->deleteButton, &QPushButton::clicked, this, &AskWidget::delete_duplicates);
    QObject::connect(ui->cancelButton, SIGNAL(clicked()), this, SLOT(close()));
}

void AskWidget::delete_duplicates() {
    for (auto it = data.begin(); it != data.end(); it++) {
        if (it.value().size() > 1) {
            for (size_t i = 1; i < it.value().size(); i++) {
                QFile file(it.value()[i]);
                file.remove();
            }
        }
    }
    close();
}


AskWidget::~AskWidget()
{
    delete ui;
    data.clear();
}
