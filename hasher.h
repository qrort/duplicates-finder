#ifndef HASHER_H
#define HASHER_H
#include <QObject>
#include <QMap>
#include <QDir>
#include <QVector>
#include <QString>

using DuplicatesMap = QMap <QByteArray, QVector <QString>>;

class Hasher : public QObject {
    Q_OBJECT

public:
    explicit Hasher(QDir &dir) : dir(dir) {}
public
    slots:
        void HashEntries();
    signals:
        void Done(DuplicatesMap sha256_hashes);

private:
    QDir &dir;
    DuplicatesMap sha256_hashes;
};

#endif
