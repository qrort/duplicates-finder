#ifndef HASHER_H
#define HASHER_H
#include <QObject>
#include <QMap>
#include <QDir>
#include <QVector>
#include <QString>

using DuplicatesVector = QVector <QVector <QString> >;

class Hasher : public QObject {
    Q_OBJECT

public:
    explicit Hasher(QDir _dir);
public
    slots:
        void HashEntries();
    signals:
        void FileHashed();
        void Done(DuplicatesVector);
        void log(QString);
private:
    bool isOpenable(const QFileInfo & file_info);
    QDir dir;
};

#endif
