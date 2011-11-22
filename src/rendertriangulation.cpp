#include <QtGui>

#include "rendertriangulation.h"
#include <limits>

RenderTriangulation::RenderTriangulation(QWidget *parent)
    : QWidget(parent)
{
    setBackgroundRole(QPalette::Base);
    setAutoFillBackground(true);
}

QSize RenderTriangulation::minimumSizeHint() const
{
    return QSize(100, 100);
}

QSize RenderTriangulation::sizeHint() const
{
    return QSize(400, 200);
}

void RenderTriangulation::paintEvent(QPaintEvent*)
{
    render(this, 5);
}

void RenderTriangulation::setTriangulation(QString path)
{
    tmap_wrapper.setMap(path);
    repaint();
}

void RenderTriangulation::renderEPS(QString path)
{
    if (tmap_wrapper.faces.empty())
        return;

    // Thanks to http://www.qtcentre.org/threads/23238-QPainting-to-an-eps-file
    // Setup printer
    QPrinter printer(QPrinter::HighResolution);
    printer.setOutputFormat(QPrinter::PostScriptFormat);
    printer.setOutputFileName(path);
    //printer.setFullPage(true);

    // Adjust paper size depending on tmap's bounding box
    QSizeF size(tmap_wrapper.xrange, tmap_wrapper.yrange);
    size.scale(500, 500, Qt::KeepAspectRatio);
    printer.setPageSize(QPrinter::Custom);
    printer.setPaperSize(size, QPrinter::Point);
    printer.setPageMargins(0, 0, 0, 0, QPrinter::Point);

    render(&printer, 50);
}

void RenderTriangulation::render(QPaintDevice *device, float margin) {
    if (tmap_wrapper.faces.empty())
        return;

    float scale = std::min(float(device->width()  - margin*2) / tmap_wrapper.xrange,
                           float(device->height() - margin*2) / tmap_wrapper.yrange);
    float xoffset = (device->width()  - tmap_wrapper.xrange * scale) / 2;
    float yoffset = (device->height() - tmap_wrapper.yrange * scale) / 2;

    QPainter painter(device);
    painter.setRenderHint(QPainter::Antialiasing, true);

    QColor color(0, 127, 0);
    QPen pen(color);
    painter.setPen(pen);

    QPoint triangle[3];
    
    foreach(const TriangulatedMap::Face &face, tmap_wrapper.faces) {
        int grey_intensity = 255 * (tmap_wrapper.max_weight - face.weight) / tmap_wrapper.max_weight;
        QColor color(grey_intensity, grey_intensity, grey_intensity);
        QBrush brush(color);
        painter.setBrush(brush);

        QPointF vertices[3] = { face.u, face.v, face.w };
        for (int i = 0; i < 3; i++) {
            QPointF v = vertices[i];
            triangle[i].setX((v.x() - tmap_wrapper.xmin) * scale + xoffset);
            triangle[i].setY((v.y() - tmap_wrapper.ymin) * scale + yoffset);
        }
        painter.drawConvexPolygon(triangle, 3);
    }
}

RenderTriangulation::TMapWrapper::TMapWrapper(QString path)
{
    setMap(path);
}

void RenderTriangulation::TMapWrapper::setMap(QString path) {
    if (path.isEmpty())
        return;

    QFile file(path);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return;

    QTextStream in(&file);
    TriangulatedMap tmap;
    in >> tmap;
    faces = tmap.faces;

    xmin = ymin = std::numeric_limits<qreal>::max();
    xmax = ymax = std::numeric_limits<qreal>::min();
    max_weight = std::numeric_limits<qreal>::min();
    foreach(const TriangulatedMap::Face &face, faces) {
        QPointF vertices[3] = { face.u, face.v, face.w };
        for (int i = 0; i < 3; i++) {
            qreal x = vertices[i].x();
            qreal y = vertices[i].y();
            if (x < xmin) xmin = x;
            if (x > xmax) xmax = x;
            if (y < ymin) ymin = y;
            if (y > ymax) ymax = y;
        }
        if (max_weight < face.weight)
            max_weight = face.weight;
    }
    xrange = xmax - xmin;
    yrange = ymax - ymin;
}
