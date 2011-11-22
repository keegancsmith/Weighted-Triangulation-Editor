#pragma once

#include <QObject>
#include <QPointF>
#include <QVector>
#include <QTextStream>

struct TriangulatedMap
{
    struct Face {
        QPointF u, v, w;
        qreal weight;
    };

    QVector<Face> faces;
};


QTextStream &operator >> (QTextStream &, TriangulatedMap &);
