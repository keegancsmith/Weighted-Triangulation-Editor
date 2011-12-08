#include <QtGui>

#include "rendertriangulation.h"
#include <limits>

RenderTriangulation::RenderTriangulation(QWidget *parent)
    : QWidget(parent)
{
    setBackgroundRole(QPalette::Base);
    setAutoFillBackground(true);
    setMouseTracking(true);
    last_tooltip_idx = -1;
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
    render(this, widget_margin);
}

void RenderTriangulation::setTriangulation(QString path)
{
    tmap_wrapper.setMap(path);
    repaint();
}

int RenderTriangulation::face_at_point(QPoint pos)
{
    if (tmap_wrapper.faces.empty())
        return -1;

    RenderInfo ri = calc_render_info(this, widget_margin);
    QPointF p((pos.x() - ri.xoffset) / ri.scale + tmap_wrapper.xmin,
              (pos.y() - ri.yoffset) / ri.scale + tmap_wrapper.ymin);

    return face_containing_point(tmap_wrapper.faces, p);
}

bool RenderTriangulation::event(QEvent *event)
{
    if (event->type() == QEvent::ToolTip) {
        QHelpEvent *helpEvent = static_cast<QHelpEvent *>(event);
        int index = face_at_point(helpEvent->pos());
        if (index == -1) {
            QToolTip::hideText();
            event->ignore();
        } else {
            if (last_tooltip_idx != -1 && last_tooltip_idx != index)
                QToolTip::hideText();
            qreal weight = tmap_wrapper.faces[index].weight;
            QToolTip::showText(helpEvent->globalPos(),
                               QString("%1").arg(weight));
        }
        last_tooltip_idx = index;
        return true;
    }
    return QWidget::event(event);
}

void RenderTriangulation::wheelEvent(QWheelEvent *event)
{
    int idx = face_at_point(event->pos());
    if (idx < 0)
        return;

    qreal weight_delta = (event->delta() / 120.0) * tmap_wrapper.max_weight / 15.0;
    qreal new_weight = tmap_wrapper.faces[idx].weight + weight_delta;
    if (new_weight < 0)
        new_weight = 0;
    else if (new_weight > tmap_wrapper.max_weight)
        new_weight = tmap_wrapper.max_weight;

    if (new_weight != tmap_wrapper.faces[idx].weight) {
        tmap_wrapper.faces[idx].weight = new_weight;
        repaint();
    }
}

void RenderTriangulation::save(QString path)
{
    QFile file(path);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
        return;

    QTextStream out(&file);
    TriangulatedMap tmap;
    tmap.faces = tmap_wrapper.faces;
    out << tmap;
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

    // Adjust paper size depending on tmap's bounding box
    QSizeF size(tmap_wrapper.xrange, tmap_wrapper.yrange);
    size.scale(500, 500, Qt::KeepAspectRatio);
    printer.setPageSize(QPrinter::Custom);
    printer.setPaperSize(size, QPrinter::Point);
    printer.setPageMargins(0, 0, 0, 0, QPrinter::Point);

    render(&printer, eps_margin);
}

RenderTriangulation::RenderInfo
RenderTriangulation::calc_render_info(QPaintDevice *device, float margin)
{
    RenderInfo ri;
    ri.scale = std::min(float(device->width()  - margin * 2) / tmap_wrapper.xrange,
                        float(device->height() - margin * 2) / tmap_wrapper.yrange);
    ri.xoffset = (device->width()  - tmap_wrapper.xrange * ri.scale) / 2;
    ri.yoffset = (device->height() - tmap_wrapper.yrange * ri.scale) / 2;
    return ri;
}

void RenderTriangulation::render(QPaintDevice *device, float margin) {
    if (tmap_wrapper.faces.empty())
        return;

    RenderInfo ri = calc_render_info(device, margin);

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
            triangle[i].setX((v.x() - tmap_wrapper.xmin) * ri.scale + ri.xoffset);
            triangle[i].setY((v.y() - tmap_wrapper.ymin) * ri.scale + ri.yoffset);
        }
        painter.drawConvexPolygon(triangle, 3);
    }
}

RenderTriangulation::TMapWrapper::TMapWrapper(QString path)
{
    setMap(path);
}

void RenderTriangulation::TMapWrapper::setMap(QString path) {
    if (path.isEmpty()) {
        faces.clear();
        return;
    }

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
