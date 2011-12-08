#include "pointeditor.h"

#include <limits>

PointEditor::PointEditor(QWidget *parent)
    : QWidget(parent)
{
    setBackgroundRole(QPalette::Base);
    setAutoFillBackground(true);
}

QSize PointEditor::minimumSizeHint() const
{
    return QSize(100, 100);
}

QSize PointEditor::sizeHint() const
{
    return QSize(400, 200);
}

void PointEditor::clear()
{
    point_set.clear();
}

void PointEditor::open(QString path)
{
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return;

    QTextStream in(&file);
    int n_points, n_dimensions, unknown1, unknown2;
    in >> n_points >> n_dimensions >> unknown1 >> unknown2;

    point_set.reserve(n_points);

    xmin = ymin = std::numeric_limits<qreal>::max();
    xmax = ymax = std::numeric_limits<qreal>::min();

    for (int i = 0; i < n_points; i++) {
        int idx;
        qreal x, y, weight;
        in >> idx >> x >> y >> weight;
        point_set.append(QPointF(x, y));

        if (x < xmin) xmin = x;
        if (x > xmax) xmax = x;
        if (y < ymin) ymin = y;
        if (y > ymax) ymax = y;
    }

    xrange = xmax - xmin;
    yrange = ymax - ymin;

    repaint();
}

void PointEditor::save(QString path)
{
    QFile file(path);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
        return;

    QTextStream out(&file);
    out << point_set.size() << " 2 1 0\n";

    for (int i = 0; i < point_set.size(); i++)
        out << (i + 1) << point_set[i].x() << ' ' << point_set[i].y() << " 1\n";
}

void PointEditor::paintEvent(QPaintEvent *event)
{
    const int margin = 5;
    const qreal point_radius = 3;

    if (point_set.empty())
        return;

    qreal scale = std::min(qreal(this->width()  - margin * 2) / xrange,
                           qreal(this->height() - margin * 2) / yrange);
    float xoffset = (this->width()  - xrange * scale) / 2;
    float yoffset = (this->height() - yrange * scale) / 2;

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);

    QColor color(0, 127, 0);
    QBrush brush(color);
    painter.setBrush(brush);

    foreach(const QPointF &p, point_set) {
        qreal x = (p.x() - xmin) * scale + xoffset;
        qreal y = (p.y() - ymin) * scale + yoffset;
        painter.drawEllipse(x, y, point_radius, point_radius);
    }
}
