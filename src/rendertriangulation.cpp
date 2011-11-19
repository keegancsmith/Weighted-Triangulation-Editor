#include <QtGui>

#include "rendertriangulation.h"

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
    if (!tmap_wrapper.tmap)
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
    Face *f = tmap_wrapper.tmap->faces;
    for (int i = 0; i < tmap_wrapper.tmap->n_faces; i++, f++) {
        if (f->ignore())
            continue;

        int grey_intensity = 255 * (tmap_wrapper.max_weight - f->weight) / tmap_wrapper.max_weight;
        QColor color(grey_intensity, grey_intensity, grey_intensity);
        QBrush brush(color);
        painter.setBrush(brush);

        for (int j = 0; j < 3; j++) {
            Vertex *v = f->vertex(j);
            triangle[j].setX((v->x - tmap_wrapper.xmin) * scale + xoffset);
            triangle[j].setY((v->y - tmap_wrapper.ymin) * scale + yoffset);
        }
        painter.drawConvexPolygon(triangle, 3);
    }
}

RenderTriangulation::TMapWrapper::TMapWrapper(QString path)
{
    tmap = 0;
    setMap(path);
}

void RenderTriangulation::TMapWrapper::setMap(QString path) {
    if (tmap) {
        tmap_free(tmap);
        tmap = 0;
    }

    if (path.isEmpty())
        return;

    FILE *fin = fopen(path.toStdString().c_str(), "r");
    tmap = tmap_readin(fin);
    fclose(fin);

    Vertex *v = tmap->vertices + 1;
    xmin = xmax = v->x;
    ymin = ymax = v->y;
    for (int i = 1; i < tmap->n_vertices; i++, v++) {
        if (v->x < xmin) xmin = v->x;
        if (v->x > xmax) xmax = v->x;
        if (v->y < ymin) ymin = v->y;
        if (v->y > ymax) ymax = v->y;
    }
    xrange = xmax - xmin;
    yrange = ymax - ymin;

    // Find the maximum weight of a face
    Face *f = tmap->faces + 1;
    max_weight = std::numeric_limits<float>::min();
    for (int i = 1; i < tmap->n_faces; i++, f++)
        if (!f->ignore() && max_weight < f->weight)
            max_weight = f->weight;
}

RenderTriangulation::TMapWrapper::~TMapWrapper()
{
    setMap();
}
