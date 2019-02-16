#include "../Headers/hasher.h"
#include <QHash>
#include <QFile>
#include <QDirIterator>
#include <QFileInfo>
#include <QCryptographicHash>
#include <QThread>
#include <QDebug>

Hasher::Hasher(QDir _dir) {
    dir = _dir;
    qRegisterMetaType<QString>("QString");
}

bool Hasher::isOpenable(const QFileInfo & file) {
    if (!(QFile::permissions(file.filePath()) & QFile::ReadUser)) {
       emit log("Do not have permission for opening" + file.filePath());
       return false;
    }
    if (!file.isReadable()) {
       emit log(file.filePath() + "is not readable");
       return false;
    }
    return true;
}

namespace {
    QCryptographicHash hasher(QCryptographicHash::Sha256);
    long long const p = 31541;
    long long const mod = 1e9 + 9;
    int const BUFFER_SIZE = 1 << 12;
    long long weak_hash(const QFileInfo &file_info) {
        QFile file(file_info.absoluteFilePath());
        file.open(QIODevice::ReadOnly);
        long long res = file.size();
        int i = 0;
        char c;
        for (; i < 10 && !file.atEnd(); i++) {
            file.read(&c, sizeof(char));
            res = (res * p + c + 1) % mod;
        }
        for (; i < 10; i++) {
            res = (res * p) % mod;
        }
        file.close();
        return res;
    }

    bool _equal(QString a, QString b) {
        QFile p(a), q(b);
        if (p.open(QIODevice::ReadOnly) && q.open(QIODevice::ReadOnly)) {
            char b1[BUFFER_SIZE], b2[BUFFER_SIZE];
            while (true)
            {
                int l1 = static_cast<int>(p.read(b1, BUFFER_SIZE)),
                l2 = static_cast<int>(q.read(b2, BUFFER_SIZE));
                if (!l1) break;
                if (!(l1 == l2 && !memcmp(b1, b2, l1))) return false;
            }
            return true;
        }
        return false;
    }

    void push_to_container(DuplicatesVector & duplicates, QString file_path) {
        for (int i = 0; i < duplicates.size(); i++) {
            if (_equal(duplicates[i].back(), file_path)) {
                duplicates[i].push_back(file_path);
                return;
            }
        }
        duplicates.push_back({file_path});
    }

    void append(DuplicatesVector & a, DuplicatesVector & b) {
        for (auto el : b) a.push_back(el);
    }

    void reduce(DuplicatesVector & v) {
        qSort(v.begin(), v.end(), [](QVector <QString> const& a, QVector <QString> const& b)
            {
                return a.size() > b.size();
            }
        );
        int ones = 0;
        for (int i = 0; i < v.size(); i++) ones += (v[i].size() < 2);
        v.resize(v.size() - ones);
    }
}
void Hasher::HashEntries() {
    DuplicatesVector duplicates;
    QHash <long long, QVector <QString>> weak_hashes;
    QDirIterator it(dir.absolutePath(), QDir::Dirs | QDir::Files | QDir::Hidden | QDir::NoDotAndDotDot | QDir::NoSymLinks, QDirIterator::Subdirectories);

    while (it.hasNext()) {
        if (QThread::currentThread()->isInterruptionRequested()) return;
        QFileInfo file_info(it.next());
        if (file_info.isFile()) {
            if (isOpenable(file_info)) {
                weak_hashes[weak_hash(file_info)].push_back(file_info.absoluteFilePath());
            } else {
                emit FileHashed();
            }
            emit FileHashed();
        }
    }

    for (auto it = weak_hashes.begin(); it != weak_hashes.end(); it++) {
        DuplicatesVector resolved;
        resolved.clear();
        if (it.value().size() > 1) {
            for (QString &i : it.value()) {
                if (QThread::currentThread()->isInterruptionRequested()) return;
                push_to_container(resolved, i);
                emit FileHashed();
            }
        } else {
            emit FileHashed();
        }

        append(duplicates, resolved);
    }
    reduce(duplicates);
    emit Done(duplicates);
}
