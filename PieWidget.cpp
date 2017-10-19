#include <QPainter>
#include <QMouseEvent>
#include <QMenu>
#include <qmath.h>
#include "GoogleColors.h"
#include "PieWidget.h"

const int circleRadius = 50;
const int pieSliceRadius = 20;

PieWidget::PieWidget(QWidget *parent)
    : QWidget(parent)
    , m_rootDirInfo()
{
    setMouseTracking(true);
    setContextMenuPolicy(Qt::CustomContextMenu);
    connect(this, SIGNAL(customContextMenuRequested(const QPoint&)),
            this, SLOT(showContextMenu(const QPoint&)));
}

DirInfo *PieWidget::rootDirInfo() const
{
    return m_rootDirInfo;
}

void PieWidget::setRootDirInfo(DirInfo *rootDirInfo)
{
    m_rootDirInfo = rootDirInfo;
    update();
}

DirInfo *PieWidget::dirInfoForPoint(QPoint point) const
{
    double x = point.x() - rect().width()/2;
    double y = point.y() - rect().height()/2;
    double r = sqrt(x*x + y*y);
    if (r < circleRadius)
        return m_rootDirInfo;
    return dirInfoForPoint(point, m_rootDirInfo, circleRadius, 0, 2*M_PI);
}

DirInfo *PieWidget::mouseDirInfo() const
{
    return m_mouseDirInfo;
}

DirInfo *PieWidget::dirInfoForPoint(QPoint point, DirInfo *dirInfo, double radius, double from, double to) const
{
    double x = point.x() - rect().width()/2;
    double y = point.y() - rect().height()/2;
    double a = fmod(atan2(y,x)+M_PI*2+M_PI_2,M_PI*2);
    double r = sqrt(x*x + y*y);

    double radius2 = radius + pieSliceRadius;
    double start = from;
    foreach(DirInfo *subdirInfo, dirInfo->subdirs()) {
        double angle = subdirInfo->recursiveWeight()*(to-from)/dirInfo->recursiveWeight();
        if (angle*180/M_PI > 1) {
            if (start <= a && a <= start+angle) {
                if (radius <= r && r <= radius2)
                    return subdirInfo;
                return dirInfoForPoint(point, subdirInfo, radius2, start, start+angle);
            }
            start += angle;
        }
    }
    return 0;
}

QColor colorForDirInfo(DirInfo *dirInfo)
{
    //return googleColors[qHash(dirInfo->dir().absolutePath())%googleColorsCount];

    DirInfo *root = dirInfo->parent();
    DirInfo *preRoot = dirInfo;
    while (root->parent()) {
        root = root->parent();
        preRoot = preRoot->parent();
    }

    QStringList components = dirInfo->dir().absolutePath().split(QDir::separator());
    int colorIndex = root->subdirs().indexOf(preRoot);
    QColor color = googleColors[colorIndex%googleColorsCount];
    color.setAlphaF(pow(1.1,-(components.count()-2)));
    return color;
}

void PieWidget::drawRecursive(QPainter &painter, double radius, double from, double to, DirInfo *dirInfo)
{
    double radius2 = radius + pieSliceRadius;
    double start = from;
    foreach(DirInfo *subdirInfo, dirInfo->subdirs()) {
        double angle = subdirInfo->recursiveWeight()*(to-from)/dirInfo->recursiveWeight();
        if (angle*180/M_PI > 2) {
            QPointF p0(radius*cos(start), -radius*sin(start));
            QPointF p1(radius*cos(start-angle), -radius*sin(start-angle));
            QPointF p2(radius2*cos(start-angle), -radius2*sin(start-angle));
            QPointF p3(radius2*cos(start), -radius2*sin(start));

            /*painter.setPen(QPen(QBrush((Qt::GlobalColor)(a+7)),1));
            painter.drawArc(QRect(-radius,-radius,radius*2,radius*2), start*180*16/M_PI, -angle*180*16/M_PI);
            painter.drawLine(p1, p2);
            painter.drawArc(QRect(-radius2,-radius2,radius2*2,radius2*2), (start-angle)*180*16/M_PI, angle*180*16/M_PI);
            painter.drawLine(p3, p0);
            */

            QPainterPath path;
            path.moveTo(p0);
            path.arcTo(QRect(-radius,-radius,radius*2,radius*2), start*180/M_PI, -angle*180/M_PI);
            path.lineTo(p2);
            path.arcTo(QRect(-radius2,-radius2,radius2*2,radius2*2), (start-angle)*180/M_PI, angle*180/M_PI);
            path.lineTo(p0);

            QColor color = googleColors[qHash(subdirInfo->dir().absolutePath())%googleColorsCount];
            //QColor color = googleColors[(subdirInfo->recursiveWeight()/1024/1024/256)%googleColorsCount];
            color.setAlphaF(1.0 - qMin(10.0,(radius+1-circleRadius)*1./pieSliceRadius)/10.0);

            //QColor color = colorForDirInfo(subdirInfo);
            painter.fillPath(path, QBrush(color));
            painter.strokePath(path, QPen(QBrush(QColor("#05224e")),0.5));
            //painter.strokePath(path, QPen(QBrush(QColor("#efefef")),0.5));
            //painter.strokePath(path, QPen(QBrush(color),0.5));

            drawRecursive(painter, radius2, start, start+angle, subdirInfo);
            start -= angle;
        }
    }
}

void PieWidget::updateMouseInfo()
{
    DirInfo *prev = m_mouseDirInfo;
    m_mouseDirInfo = dirInfoForPoint(m_lastMousePoint);
    if (prev != m_mouseDirInfo)
        emit mouseDirInfoChanged(m_mouseDirInfo ? QString("%1 (%2)")
                                                  .arg(m_mouseDirInfo->dir().absolutePath())
                                                  .arg(DirInfo::formatSize(m_mouseDirInfo->recursiveWeight())) : "");
}

void PieWidget::paintEvent(QPaintEvent *event)
{
    //return;
    QWidget::paintEvent(event);

    QPainter painter(this);
    painter.setRenderHint(QPainter::TextAntialiasing, true);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setRenderHint(QPainter::HighQualityAntialiasing, true);
    painter.setPen(QPen(Qt::blue, 4));
    painter.translate(QPoint(rect().width()/2, rect().height()/2));

    QPainterPath path;
    path.addEllipse(QRect(-circleRadius,-circleRadius,circleRadius*2,circleRadius*2));
    painter.fillPath(path, QBrush(Qt::darkGray));
    drawRecursive(painter, circleRadius, M_PI_2, 2*M_PI+M_PI_2, rootDirInfo());

    painter.setPen(QPen(Qt::white, 1));
    painter.drawText(QRectF(-circleRadius,-circleRadius,circleRadius*2,circleRadius*2),
                     DirInfo::formatSize(rootDirInfo()->recursiveWeight()).replace(" ","\n"), QTextOption(Qt::AlignCenter));
    updateMouseInfo();
}

void PieWidget::mouseMoveEvent(QMouseEvent *event)
{
    if (m_rootDirInfo == NULL)
        return;
    m_lastMousePoint = event->pos();
    updateMouseInfo();
}

void PieWidget::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        if (m_mouseDirInfo == 0)
            return;
        if (m_mouseDirInfo == m_rootDirInfo) {
            if (m_rootDirInfo->parent() == 0)
                return;
            m_rootDirInfo = m_rootDirInfo->parent();
        } else
            m_rootDirInfo = m_mouseDirInfo;
        update();
        emit rootChanged(m_rootDirInfo);
    }
}

void PieWidget::showContextMenu(const QPoint &pos)
{
    DirInfo *dirInfo = m_mouseDirInfo;
    if (dirInfo == 0)
        return;

    QPoint globalPos = mapToGlobal(pos);

    QMenu myMenu;
    QAction *a1 = myMenu.addAction("Reveal in Finder");
    QAction *a2 = myMenu.addAction("Reveal in Terminal");

    QAction* selectedItem = myMenu.exec(globalPos);
    if (selectedItem == a1)
        dirInfo->showInGraphicalShell(this);
    if (selectedItem == a2)
        dirInfo->showInConsoleShell(this);
}
