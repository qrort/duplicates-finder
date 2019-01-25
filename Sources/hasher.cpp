#include "../Headers/hasher.h"
#include <QHash>
#include <QFile>
#include <QDirIterator>
#include <QFileInfo>
#include <QCryptographicHash>
#include <QThread>
#include <QDebug>

namespace tools {
    QCryptographicHash hasher(QCryptographicHash::Sha256);
    long long const p = 31541;
    long long const mod = 1e9 + 9;
}

Hasher::Hasher(QDir _dir) {
    dir = _dir;
    qRegisterMetaType<QString>("QString");
}

bool Hasher::isOpenable(const QFileInfo & file) {
    if (file.isFile()) {
        if (!(QFile::permissions(file.filePath()) & QFile::ReadUser)) {
           emit log("Do not have permission for opening" + file.filePath());
           return false;
        }
        if (!file.isReadable()) {
           emit log(file.filePath() + "is not readable");
           return false;
        }
        return true;
    } else return false;
}


long long weak_hash(char *&c, const QFileInfo &file_info) {
    QFile file(file_info.absoluteFilePath());
    file.open(QIODevice::ReadOnly);
    long long res = file.size();
    int i = 0;
    for (; i < 10 && !file.atEnd(); i++) {
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

void Hasher::HashEntries() {
    DuplicatesMap sha256_hashes;
    QHash <long long, QVector <QString>> weak_hashes;
    QDirIterator it(dir.absolutePath(), QDir::Dirs | QDir::Files | QDir::Hidden | QDir::NoDotAndDotDot | QDir::NoSymLinks, QDirIterator::Subdirectories);
    char *c = new char;
    int cnt = 0;
    while (it.hasNext()) {
        if (QThread::currentThread()->isInterruptionRequested()) return;
        QFileInfo file_info(it.next());
        if (isOpenable(file_info)) {
            weak_hashes[weak_hash(c, file_info)].push_back(file_info.absoluteFilePath());
        } else {
            emit FileHashed();
            cnt++;
        }
        if (file_info.isFile()) {
           emit FileHashed();
           cnt++;
        }
    }
    delete c;
    for (auto it = weak_hashes.begin(); it != weak_hashes.end(); it++) {
        if (it.value().size() > 1) {
            for (QString &i : it.value()) {
               // qDebug() << it.value().size() << endl;
                if (QThread::currentThread()->isInterruptionRequested()) return;
                sha256_hashes[sha256(i)].push_back(i);
                emit FileHashed();
                cnt++;
            }
        } else {
            emit FileHashed();
            cnt++;
        }
    }
    qDebug() << cnt;
    emit Done(sha256_hashes);
}
