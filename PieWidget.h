#ifndef PIEWIDGET_H
#define PIEWIDGET_H

#include <QWidget>
#include "DirInfo.h"

class PieWidget : public QWidget
{
    Q_OBJECT
public:
    explicit PieWidget(QWidget *parent = 0);

    DirInfo *rootDirInfo() const;
    DirInfo *mouseDirInfo() const;
    DirInfo *dirInfoForPoint(QPoint point) const;

private:
    DirInfo *dirInfoForPoint(QPoint point, DirInfo *dirInfo, double radius, double from, double to) const;
    void drawRecursive(QPainter &painter, double radius, double from, double to, DirInfo *dirInfo);
    void updateMouseInfo();

protected:
    virtual void paintEvent(QPaintEvent *event);
    virtual void mouseMoveEvent(QMouseEvent *event);
    virtual void mouseReleaseEvent(QMouseEvent *event);

signals:
    void mouseDirInfoChanged(QString);
    void rootChanged(DirInfo *dirInfo);

public slots:
    void showContextMenu(const QPoint& pos);
    void setRootDirInfo(DirInfo *rootDirInfo);

private:
    DirInfo *m_rootDirInfo;
    DirInfo *m_mouseDirInfo;
    QPoint m_lastMousePoint;
};

QColor colorForDirInfo(DirInfo *dirInfo);

#endif // PIEWIDGET_H
