#include "renderpointset.h"

#include <limits>

RenderPointSet::RenderPointSet(QWidget *parent)
    : QWidget(parent)
{
    setBackgroundRole(QPalette::Base);
    setAutoFillBackground(true);

    xmin = ymin = actual_xmin = actual_ymin = 0;
    xmax = ymax = actual_xmax = actual_ymax = 100;
}

QSize RenderPointSet::minimumSizeHint() const
{
    return QSize(100, 100);
}

QSize RenderPointSet::sizeHint() const
{
    return QSize(400, 200);
}

void RenderPointSet::clear()
{
    point_set.clear();
    repaint();
}

void RenderPointSet::open(QString path)
{
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return;

    QTextStream in(&file);
    int n_points, n_dimensions, unknown1, unknown2;
    in >> n_points >> n_dimensions >> unknown1 >> unknown2;

    point_set.reserve(n_points);

    for (int i = 0; i < n_points; i++) {
        int idx;
        qreal x, y, weight;
        in >> idx >> x >> y >> weight;
        point_set.append(QPointF(x, y));
    }

    update_actual_boundary();

    xmin = actual_xmin;
    xmax = actual_xmax;
    ymin = actual_ymin;
    ymax = actual_ymax;

    emit boundingBoxChanged();

    repaint();
}

void RenderPointSet::save(QString path)
{
    QFile file(path);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
        return;

    QTextStream out(&file);
    out << point_set.size() << " 2 1 0\n";

    for (int i = 0; i < point_set.size(); i++)
        out << (i + 1) << ' ' << point_set[i].x() << ' ' << point_set[i].y() << " 1\n";
}

void RenderPointSet::setXMin(qreal val)
{
    if ((point_set.empty() || val <= actual_xmin) && xmin != val) {
        xmin = val;
        emit boundingBoxChanged();
        repaint();
    }
}

void RenderPointSet::setXMax(qreal val)
{
    if ((point_set.empty() || val >= actual_xmax) && xmax != val) {
        xmax = val;
        emit boundingBoxChanged();
        repaint();
    }
}

void RenderPointSet::setYMin(qreal val)
{
    if ((point_set.empty() || val <= actual_ymin) && ymin != val) {
        ymin = val;
        emit boundingBoxChanged();
        repaint();
    }
}

void RenderPointSet::setYMax(qreal val)
{
    if ((point_set.empty() || val >= actual_ymax) && ymax != val) {
        ymax = val;
        emit boundingBoxChanged();
        repaint();
    }
}

RenderPointSet::RenderInfo RenderPointSet::calc_render_info()
{
    static const int margin = 5;
    RenderInfo ri;

    qreal xrange = xmax - xmin;
    qreal yrange = ymax - ymin;

    ri.scale = std::min(qreal(this->width()  - margin * 2) / xrange,
                        qreal(this->height() - margin * 2) / yrange);
    ri.xoffset = (this->width()  - xrange * ri.scale) / 2;
    ri.yoffset = (this->height() - yrange * ri.scale) / 2;

    return ri;
}

void RenderPointSet::paintEvent(QPaintEvent *event)
{
    if (point_set.empty())
        return;

    Q_ASSERT(xmin <= actual_xmin && actual_xmax <= xmax);
    Q_ASSERT(ymin <= actual_ymin && actual_ymax <= ymax);

    static const qreal point_radius = 3;
    RenderInfo ri = calc_render_info();

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);

    QColor color(0, 127, 0);
    QBrush brush(color);
    painter.setBrush(brush);

    foreach(const QPointF &p, point_set) {
        qreal x = (p.x() - xmin) * ri.scale + ri.xoffset;
        qreal y = (p.y() - ymin) * ri.scale + ri.yoffset;
        painter.drawEllipse(x, y, point_radius, point_radius);
    }
}

void RenderPointSet::mousePressEvent(QMouseEvent *event)
{
    RenderInfo ri = calc_render_info();
    QPointF pos = event->posF();
    qreal x = (pos.x() - ri.xoffset) / ri.scale + xmin;
    qreal y = (pos.y() - ri.yoffset) / ri.scale + ymin;

    if (event->button() == Qt::LeftButton) {
        // Add a point
        point_set.append(QPointF(x, y));
        update_actual_boundary();
        repaint();
    } else if (event->button() == Qt::RightButton) {
        // Remove a point
        if (point_set.empty())
            return;

        QPointF q(x, y);
        QPointF closest = q;
        qreal closest_dist2 = std::numeric_limits<qreal>::max();
        foreach(const QPointF &p, point_set) {
            QPointF delta = (p - q);
            qreal dist2 = (delta.x() * delta.x()) + (delta.y() * delta.y());
            if (dist2 < closest_dist2) {
                closest_dist2 = dist2;
                closest = p;
            }
        }

        int idx = point_set.indexOf(closest);
        point_set.remove(idx);
        update_actual_boundary();
        repaint();
    }
}


void RenderPointSet::update_actual_boundary()
{
    actual_xmin = actual_ymin = std::numeric_limits<qreal>::max();
    actual_xmax = actual_ymax = std::numeric_limits<qreal>::min();

    foreach(const QPointF &p, point_set) {
        if (p.x() < actual_xmin) actual_xmin = p.x();
        if (p.x() > actual_xmax) actual_xmax = p.x();
        if (p.y() < actual_ymin) actual_ymin = p.y();
        if (p.y() > actual_ymax) actual_ymax = p.y();
    }

    bool boundary_changed = false;
#define boundary_changed_check(coord, op) \
    if (coord op actual_##coord) { \
        coord = actual_##coord; \
        boundary_changed = true; \
    }

    boundary_changed_check(xmin, >);
    boundary_changed_check(xmax, <);
    boundary_changed_check(ymin, >);
    boundary_changed_check(ymax, <);

    if (boundary_changed)
        emit boundingBoxChanged();
}
