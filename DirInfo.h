#ifndef DIRINFO_H
#define DIRINFO_H

#include <QDir>
#include <QMutex>
#include <QRunnable>

class DirInfo : QRunnable
{
public:
    DirInfo(const QString & path, DirInfo *m_parent = 0);
    ~DirInfo();

    virtual void run();

    void updateRecursiveWeight(qint64 w);

    DirInfo *parent() const;
    QDir dir() const;
    qint64 weight() const;
    qint64 recursiveWeight() const;
    const QList<DirInfo*>& subdirs() const;

    void showInGraphicalShell(QWidget *parent);
    void showInConsoleShell(QWidget *parent);

    void setStop(volatile bool stop);

    static QString formatSize(qint64 size, int len = 4);

private:
    static volatile bool m_stop;
    DirInfo *m_parent;
    QDir m_dir;
    qint64 m_weight;
    QAtomicInteger<qint64> m_recursiveWeight;
    QList<DirInfo*> m_subdirs;
};

#endif // DIRINFO_H
