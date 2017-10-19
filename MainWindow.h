#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStorageInfo>
#include <QMap>

class DirInfo;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    DirInfo *currentRootDirInfo() const;
    void updateMounts();

protected:
    virtual void timerEvent(QTimerEvent *event);

signals:
    void rootChanged(DirInfo *dirInfo);

public slots:
    void pathChanged(DirInfo *dirInfo);

private slots:
    void dirSelected(QString itemStr);

private:
    Ui::MainWindow *ui;
    QList<QStorageInfo> m_mounts;
    QMap<QString,DirInfo*> m_dirInfos;
};

#endif // MAINWINDOW_H
