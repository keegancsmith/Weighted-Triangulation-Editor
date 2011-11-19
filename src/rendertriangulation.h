#pragma once

#include "tmap.h"

#include <QWidget>
#include <QPaintDevice>

class RenderTriangulation : public QWidget
{
    Q_OBJECT

private:
    class TMapWrapper {
    public:
        TMapWrapper(QString path = QString());
        ~TMapWrapper();
        void setMap(QString path = QString());

        TriangulatedMap *tmap;
        float xmin, xmax, ymin, ymax;
        float xrange, yrange;
        float max_weight;
    };

public:
    RenderTriangulation(QWidget *parent = 0);

    QSize minimumSizeHint() const;
    QSize sizeHint() const;

public slots:
    void setTriangulation(QString path);
    void renderEPS(QString path);

protected:
    void paintEvent(QPaintEvent *event);
    void render(QPaintDevice *device, float margin);

    TMapWrapper tmap_wrapper;
};
