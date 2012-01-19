#pragma once

#include <QtGui>

class RenderPointSet : public QWidget
{
    Q_OBJECT

public:
    RenderPointSet(QWidget *parent = 0);

    QSize minimumSizeHint() const;
    QSize sizeHint() const;

    qreal xMin() const { return xmin; }
    qreal xMax() const { return xmax; }
    qreal yMin() const { return ymin; }
    qreal yMax() const { return ymax; }

public slots:
    void clear();
    void open(QString path);
    void save(QString path);

    void setXMin(qreal val);
    void setXMax(qreal val);
    void setYMin(qreal val);
    void setYMax(qreal val);

signals:
    void boundingBoxChanged();

protected:
    void paintEvent(QPaintEvent *event);

    QString point_set_path;
    QVector<QPointF> point_set;
    qreal xmin, xmax, ymin, ymax;
    qreal actual_xmin, actual_xmax;
    qreal actual_ymin, actual_ymax;
};
