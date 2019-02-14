#ifndef ASKWIDGET_H
#define ASKWIDGET_H

#include <QWidget>
#include <QMap>

namespace Ui {
class AskWidget;
}

using DuplicatesVector = QVector <QVector <QString> >;

class AskWidget : public QWidget {
    Q_OBJECT

public:
    explicit AskWidget(DuplicatesVector const& _data, QWidget *parent = 0);
    ~AskWidget();
public slots:
    void deleteDuplicates();
private:
    DuplicatesVector data;
    Ui::AskWidget *ui;
};

#endif // ASKWIDGET_H
