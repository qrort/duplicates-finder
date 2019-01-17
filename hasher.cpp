#include "hasher.h"
#include <QHash>
#include <QDirIterator>
#include <QFileInfo>
#include <QCryptographicHash>
#include <QThread>

namespace tools {
    QCryptographicHash hasher(QCryptographicHash::Sha256);
    long long const p = 31541;
    long long const mod = 1e9 + 9;
}

long long weak_hash(const QFileInfo &file_info) {
    QFile file(file_info.absoluteFilePath());
    file.open(QIODevice::ReadOnly);
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

void Hasher::HashEntries() {
    QHash <long long, QVector <QString>> weak_hashes;
    QDirIterator it(dir, QDirIterator::Subdirectories);
    while (it.hasNext()) {
        if (QThread::currentThread()->isInterruptionRequested()) return;

        QFileInfo file_info(it.next());
        if (file_info.isFile()) {
            weak_hashes[weak_hash(file_info)].push_back(it.filePath());
            emit FileHashed();
        }
    }
    for (auto it = weak_hashes.begin(); it != weak_hashes.end(); it++) {
        if (it.value().size() > 1) {
            for (QString &i : it.value()) {
               if (QThread::currentThread()->isInterruptionRequested()) return;

               sha256_hashes[sha256(i)].push_back(i);
               emit FileHashed();
            }
        } else emit FileHashed();
    }
    emit Done(sha256_hashes);
}
