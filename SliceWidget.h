#ifndef SLICESWIDGET_H
#define SLICESWIDGET_H

#include <QWidget>
#include "DirInfo.h"

class SliceWidget : public QWidget
{
    Q_OBJECT
public:
    explicit SliceWidget(QWidget *parent = 0);

    DirInfo *rootDirInfo() const;

    void drawRecursive(QPainter &painter);

protected:
    virtual void paintEvent(QPaintEvent *event);

signals:
    void rootChanged(DirInfo *dirInfo);

public slots:
    void setRootDirInfo(DirInfo *rootDirInfo);

private:
    DirInfo *m_rootDirInfo;
};

#endif // SLICESWIDGET_H
