#include <QRadioButton>
#include <QTimer>
#include <QTime>
#include <QDebug>
#include "MainWindow.h"
#include "ui_MainWindow.h"
#include "DirInfo.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    updateMounts();
    startTimer(40);
}

MainWindow::~MainWindow()
{
    delete ui;

    foreach(DirInfo *dirInfo, m_dirInfos)
        delete dirInfo;
    m_dirInfos.clear();
}

DirInfo *MainWindow::currentRootDirInfo() const
{
    if (m_mounts.count() == 0 || m_dirInfos.count() == 0)
        return NULL;
    int index = ui->mounts->currentIndex();
    return m_dirInfos[m_mounts[qMax(0,index)].rootPath()];
}

void MainWindow::updateMounts()
{
    ui->mounts->clear();
    foreach (QStorageInfo storageInfo, m_mounts = QStorageInfo::mountedVolumes()) {
        ui->mounts->addItem(storageInfo.displayName(), m_mounts.indexOf(storageInfo));
        if (m_dirInfos[storageInfo.rootPath()] == NULL)
            m_dirInfos[storageInfo.rootPath()] = new DirInfo(storageInfo.rootPath());
        if (ui->pie->rootDirInfo() == NULL)
        {
            ui->pie->setRootDirInfo(currentRootDirInfo());
            ui->slice->setRootDirInfo(currentRootDirInfo());
            pathChanged(currentRootDirInfo());
        }
    }
}

void MainWindow::timerEvent(QTimerEvent *event)
{
    ui->pie->update();
    ui->slice->update();

    ui->progressBar->setValue(100*currentRootDirInfo()->recursiveWeight()/(m_mounts.first().bytesTotal() - m_mounts.first().bytesFree()));
}

void MainWindow::pathChanged(DirInfo *dirInfo)
{
    while (ui->pathComponents->count() > 2)
        ui->pathComponents->takeAt(1)->widget()->setParent(0);

    QStringList rootComponnets = currentRootDirInfo()->dir().absolutePath().replace("\\","/").split("/", QString::SkipEmptyParts);
    QStringList components = dirInfo->dir().absolutePath().replace("\\","/").split("/", QString::SkipEmptyParts).mid(rootComponnets.count());
    DirInfo *walkDirInfo = currentRootDirInfo();
    if (components.count() > 1 || components.count() == 0 || components.first().length() > 0)
        components.append("");
    foreach (QString component, components) {
        QComboBox *comboBox = new QComboBox();
        comboBox->setMaximumWidth(ui->mounts->maximumWidth());
        comboBox->setStyleSheet(ui->mounts->styleSheet());
        if (component == "")
            comboBox->insertItem(0,"");
        foreach(DirInfo *subdirInfo, walkDirInfo->subdirs()) {
            QString subdirName = subdirInfo->dir().dirName();
            comboBox->addItem(subdirName);
            if (subdirName == component) {
                walkDirInfo = subdirInfo;
                comboBox->setCurrentIndex(comboBox->count()-1);
            }
        }
        connect(comboBox, SIGNAL(currentIndexChanged(QString)), this, SLOT(dirSelected(QString)));
        ui->pathComponents->insertWidget(ui->pathComponents->count()-1, comboBox);
    }
}

void MainWindow::dirSelected(QString itemStr)
{
    if (currentRootDirInfo() == NULL)
        return;

    DirInfo *walkDirInfo = currentRootDirInfo();
    if (sender() != ui->mounts) {
        for (int i = 1; i < ui->pathComponents->count()-1; i++) {
            QComboBox *comboBox = (QComboBox *)ui->pathComponents->itemAt(i)->widget();
            QString component = comboBox->currentText();
            foreach(DirInfo *subdirInfo, walkDirInfo->subdirs()) {
                QString subdirName = subdirInfo->dir().dirName();
                if (subdirName == component) {
                    walkDirInfo = subdirInfo;
                    break;
                }
            }
            if (comboBox == sender())
                break;
        }
    }
    emit rootChanged(walkDirInfo);
    pathChanged(walkDirInfo);
}
