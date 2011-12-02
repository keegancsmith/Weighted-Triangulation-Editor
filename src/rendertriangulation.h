#pragma once

#include "triangulatedmap.h"

#include <QWidget>
#include <QPaintDevice>

class RenderTriangulation : public QWidget
{
    Q_OBJECT

private:
    class TMapWrapper {
    public:
        TMapWrapper(QString path = QString());
        void setMap(QString path = QString());

        QVector<TriangulatedMap::Face> faces;
        qreal xmin, xmax, ymin, ymax;
        qreal xrange, yrange;
        qreal max_weight;
    };

    struct RenderInfo {
        qreal scale, xoffset, yoffset;
    };

public:
    RenderTriangulation(QWidget *parent = 0);

    QSize minimumSizeHint() const;
    QSize sizeHint() const;

public slots:
    void setTriangulation(QString path);
    void save(QString path);
    void renderEPS(QString path);

protected:
    bool event(QEvent *event);
    void paintEvent(QPaintEvent *event);
    void wheelEvent(QWheelEvent *event);

private:
    static const float widget_margin = 5;
    static const float eps_margin = 50;

    RenderInfo calc_render_info(QPaintDevice *device, float margin);
    void render(QPaintDevice *device, float margin);
    int face_at_point(QPoint pos);

    TMapWrapper tmap_wrapper;
    int last_tooltip_idx;
};
