#pragma once

#include <QtGui>

class PointEditor : public QWidget
{
    Q_OBJECT

public:
    PointEditor(QWidget *parent = 0);

    QSize minimumSizeHint() const;
    QSize sizeHint() const;

public slots:
    void clear();
    void open(QString path);
    void save(QString path);

protected:
    void paintEvent(QPaintEvent *event);

    QString point_set_path;
    QVector<QPointF> point_set;
    qreal xrange, xmin, xmax;
    qreal yrange, ymin, ymax;
};
