#include <QPainter>
#include "GoogleColors.h"
#include "PieWidget.h"
#include "SliceWidget.h"

SliceWidget::SliceWidget(QWidget *parent)
    : QWidget(parent)
{

}

DirInfo *SliceWidget::rootDirInfo() const
{
    return m_rootDirInfo;
}

void SliceWidget::setRootDirInfo(DirInfo *rootDirInfo)
{
    m_rootDirInfo = rootDirInfo;
    update();
}

void SliceWidget::drawRecursive(QPainter &painter)
{
    double from = 0;
    double to = rect().height();
    double start = 0;
    foreach(DirInfo *subdirInfo, m_rootDirInfo->subdirs()) {
        double height = subdirInfo->recursiveWeight()*(to-from)/m_rootDirInfo->recursiveWeight();
        if (height < 5)
            continue;

        QColor color = googleColors[qHash(subdirInfo->dir().absolutePath())%googleColorsCount];
        //QColor color = googleColors[(subdirInfo->recursiveWeight()/1024/1024/256)%googleColorsCount];
        //color.setAlpha(1.0 - qMin(10.0,(radius+1-circleRadius)*1./pieSliceRadius)/10.0);

        //QColor color = colorForDirInfo(subdirInfo);
        painter.fillRect(QRect(0,start,rect().width(),height), QBrush(color));
        painter.setPen(QPen(QBrush(QColor("#202020")),0.5));
        painter.drawRect(QRect(0,start,rect().width(),height));
        if (height > 20) {
            QString name = QString("%1").arg(subdirInfo->dir().dirName());
            painter.drawText(QRectF(4,start+4,rect().width()-8,height-8), name, QTextOption(Qt::AlignTop|Qt::AlignLeft));
        }
        if (height > 40) {
            QString size = DirInfo::formatSize(subdirInfo->recursiveWeight());
            painter.drawText(QRectF(4,start+4,rect().width()-8,height-8), size, QTextOption(Qt::AlignBottom|Qt::AlignRight));
        }
        start += height;
    }
}

void SliceWidget::paintEvent(QPaintEvent *event)
{
    QWidget::paintEvent(event);

    QPainter painter(this);
    painter.setRenderHint(QPainter::TextAntialiasing, true);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setRenderHint(QPainter::HighQualityAntialiasing, true);

    drawRecursive(painter);
}
