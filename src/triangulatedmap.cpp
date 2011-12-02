#include "triangulatedmap.h"

#include <QtGlobal>
#include <limits>
#include <complex>

QTextStream &operator >> (QTextStream & in, TriangulatedMap & tmap) {
    const qreal infinity = std::numeric_limits<qreal>::infinity();

    int n_vertices, n_faces;
    in >> n_vertices >> n_faces;

    QVector<QPointF> vertices;
    vertices.reserve(n_vertices);

    for (int n = 0; n < n_vertices; n++) {
        qreal x, y, z;
        int face_idx;
        in >> x >> y >> z >> face_idx;
        vertices.push_back(QPointF(x, y));
    }

    for (int n = 0; n < n_faces; n++) {
        TriangulatedMap::Face f;
        int u_idx, v_idx, w_idx;
        int a_idx, b_idx, c_idx;
        in >> u_idx >> v_idx >> w_idx
           >> a_idx >> b_idx >> c_idx
           >> f.weight;

        Q_ASSERT(u_idx < vertices.size());
        Q_ASSERT(v_idx < vertices.size());
        Q_ASSERT(w_idx < vertices.size());

        f.u = vertices[u_idx];
        f.v = vertices[v_idx];
        f.w = vertices[w_idx];

        // If this vertex has zero area, ignore it. Some of the triangulations
        // I read in don't have enough floating point accuracy on the input,
        // which causes zero area faces.
        if ((f.u.x() == f.v.x() && f.u.y() == f.v.y()) ||
            (f.u.x() == f.w.x() && f.u.y() == f.w.y()) ||
            (f.v.x() == f.w.x() && f.v.y() == f.w.y()) ||
            n == 0)
            f.weight == infinity;

        if (f.weight != infinity)
            tmap.faces.push_back(f);
    }
}

namespace CompGeom {
    typedef std::complex<qreal> point;
    const qreal EPS = 1e-15;

    qreal cross(point a, point b) {
        return std::imag(a * std::conj(b));
    }

    int ccw(point a, point b, point c) {
        qreal cr = cross(b - a, c - a);
        if(std::abs(cr) <= EPS)
            return 0;

        return cr > 0 ? 1 : -1;
    }

    bool point_in_face(const TriangulatedMap::Face &f, const QPointF &p_) {
        point a(f.u.x(), f.u.y());
        point b(f.v.x(), f.v.y());
        point c(f.w.x(), f.w.y());
        point p(p_.x(), p_.y());

        int side1 = ccw(p, a, b);
        int side2 = ccw(p, b, c);
        int side3 = ccw(p, c, a);

        return side1 == side2 && side2 == side3;
    }
}

int face_containing_point(const QVector<TriangulatedMap::Face> &faces, QPointF p) {
    for (int i = 0; i < faces.size(); i++) {
        if (CompGeom::point_in_face(faces[i], p))
            return i;
    }
    return -1;
}
