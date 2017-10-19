#include <QThreadPool>
#include <QProcess>
#include <QMessageBox>
#include "math.h"
#include "DirInfo.h"

volatile bool DirInfo::m_stop = false;

DirInfo::DirInfo(const QString &path, DirInfo *parent)
    : QRunnable()
    , m_parent(parent)
    , m_dir(path)
    , m_weight()
    , m_recursiveWeight()
    , m_subdirs()
{
    setAutoDelete(false);

    QStringList excludes;
    excludes << "/mnt";

    if (excludes.contains(path))
        return;

    foreach (QFileInfo fileInfo, m_dir.entryInfoList(QDir::Files))
        m_weight += fileInfo.size();

    if (m_dir.dirName() == "")
        run();
    else
        QThreadPool::globalInstance()->start(this);
}

DirInfo::~DirInfo()
{
    m_parent = NULL;
    foreach (DirInfo *dirInfo, m_subdirs)
        dirInfo->setStop(true);
    QThreadPool::globalInstance()->waitForDone();
    qDeleteAll(m_subdirs);
}

void DirInfo::run()
{
    QList<QFileInfo> dirs = m_dir.entryInfoList(QDir::Dirs|QDir::NoDotAndDotDot|QDir::NoSymLinks);
    m_subdirs.reserve(dirs.count());
    foreach (QFileInfo dirInfo, dirs) {
        if (!m_stop)
            m_subdirs.append(new DirInfo(dirInfo.absoluteFilePath(), this));
    }
    if (!m_stop)
        updateRecursiveWeight(m_weight);
}

void DirInfo::updateRecursiveWeight(qint64 w)
{
    m_recursiveWeight.fetchAndAddRelaxed(w);

    if (m_parent)
        m_parent->updateRecursiveWeight(w);
    //qSort(m_subdirs.begin(), m_subdirs.end(),
    //      [](DirInfo *a, DirInfo *b){return a->recursiveWeight() > b->recursiveWeight();});
}

DirInfo *DirInfo::parent() const
{
    return m_parent;
}

QDir DirInfo::dir() const
{
    return m_dir;
}

qint64 DirInfo::weight() const
{
    return m_weight;
}

qint64 DirInfo::recursiveWeight() const
{
    return m_recursiveWeight;
}

const QList<DirInfo*>& DirInfo::subdirs() const
{
    return m_subdirs;
}

void DirInfo::showInGraphicalShell(QWidget *parent)
{
    QString pathIn = m_dir.absolutePath();
    // Mac, Windows support folder or file.
#if defined(Q_OS_WIN)
    const QString explorer = QLatin1String("explorer.exe");
    if (explorer.isEmpty()) {
        QMessageBox::warning(parent,
                             QObject::tr("Launching Windows Explorer failed"),
                             QObject::tr("Could not find explorer.exe in path to launch Windows Explorer."));
        return;
    }
    QString param;
    if (!QFileInfo(pathIn).isDir())
        param = QLatin1String("/select,");
    param += QDir::toNativeSeparators(pathIn);
    QString command = explorer + " " + param;
    QProcess::startDetached(command);
#elif defined(Q_OS_MAC)
    Q_UNUSED(parent)
    QStringList scriptArgs;
    scriptArgs << QLatin1String("-e")
               << QString::fromLatin1("tell application \"Finder\" to reveal POSIX file \"%1\"")
                                     .arg(pathIn);
    QProcess::execute(QLatin1String("/usr/bin/osascript"), scriptArgs);
    scriptArgs.clear();
    scriptArgs << QLatin1String("-e")
               << QLatin1String("tell application \"Finder\" to activate");
    QProcess::execute("/usr/bin/osascript", scriptArgs);
#else
    // we cannot select a file here, because no file browser really supports it...
    const QFileInfo fileInfo(pathIn);
    const QString folder = fileInfo.absoluteFilePath();
    const QString app = Utils::UnixUtils::fileBrowser(Core::ICore::instance()->settings());
    QProcess browserProc;
    const QString browserArgs = Utils::UnixUtils::substituteFileBrowserParameters(app, folder);
    if (debug)
        qDebug() <<  browserArgs;
    bool success = browserProc.startDetached(browserArgs);
    const QString error = QString::fromLocal8Bit(browserProc.readAllStandardError());
    success = success && error.isEmpty();
    if (!success)
        showGraphicalShellError(parent, app, error);
#endif
}

void DirInfo::showInConsoleShell(QWidget *parent)
{
    QString pathIn = m_dir.absolutePath();
    // Mac, Windows support folder or file.
#if defined(Q_OS_WIN)
    const QString cmd = QLatin1String("cmd.exe");
    if (cmd.isEmpty()) {
        QMessageBox::warning(parent,
                             QObject::tr("Launching Command Line failed"),
                             QObject::tr("Could not find cmd.exe in path to launch Command Line."));
        return;
    }
    QString command = cmd + " /K \"cd /d " + QDir::toNativeSeparators(pathIn) + "\"";
    QProcess::startDetached(command);
#elif defined(Q_OS_MAC)
    Q_UNUSED(parent)
    QStringList scriptArgs;
    scriptArgs << QLatin1String("-e")
               << QString::fromLatin1("tell application \"Terminal\" to do script \"cd %1\"")
                                     .arg(QDir::toNativeSeparators(pathIn));
    QProcess::execute(QLatin1String("/usr/bin/osascript"), scriptArgs);
    scriptArgs.clear();
    scriptArgs << QLatin1String("-e")
               << QLatin1String("tell application \"Terminal\" to activate");
    QProcess::execute("/usr/bin/osascript", scriptArgs);
#else
    // we cannot select a file here, because no file browser really supports it...
    const QFileInfo fileInfo(pathIn);
    const QString folder = fileInfo.absoluteFilePath();
    const QString app = Utils::UnixUtils::fileBrowser(Core::ICore::instance()->settings());
    QProcess browserProc;
    const QString browserArgs = Utils::UnixUtils::substituteFileBrowserParameters(app, folder);
    if (debug)
        qDebug() <<  browserArgs;
    bool success = browserProc.startDetached(browserArgs);
    const QString error = QString::fromLocal8Bit(browserProc.readAllStandardError());
    success = success && error.isEmpty();
    if (!success)
        showGraphicalShellError(parent, app, error);
#endif
}
void DirInfo::setStop(volatile bool stop)
{
    m_stop = stop;
    foreach (DirInfo *dirInfo, m_subdirs)
        dirInfo->setStop(stop);
}

QString DirInfo::formatSize(qint64 size, int len)
{
    if (size <= 0)
        return "";
    static const char *sizeNames[] = {"B","KB","MB","GB","TB","PB","EB"};
    int p = log(size)/log(1024);
    QString sz = QString("%1").arg(size/pow(1024,p));
    return QString("%1 %2").arg(sz.mid(0,qMin(sz.length(), len+1))).arg(sizeNames[p]);
}

