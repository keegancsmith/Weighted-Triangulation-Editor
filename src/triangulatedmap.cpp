#include "triangulatedmap.h"

#include <QtAlgorithms>
#include <QtGlobal>
#include <QMap>
#include <QPair>
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

bool operator<(const QPointF & a, const QPointF & b) {
    if (a.x() != b.x())
        return a.x() < b.x();
    else
        return a.y() < b.y();
}

QTextStream &operator << (QTextStream & out, TriangulatedMap & tmap) {
    const qreal infinity = std::numeric_limits<qreal>::infinity();

    // Insert a dummy face
    QPointF dummy_vertex(0, 0);
    TriangulatedMap::Face dummy_face = { dummy_vertex, dummy_vertex,
                                         dummy_vertex, infinity };
    tmap.faces.prepend(dummy_face);

    // Create a mapping from vertices to indexes. Since the dummy_face is at
    // the front of the faces list, the dummy_vertex should also end up in the
    // front which is what we need.
    QMap<QPointF, int> vertices;
    foreach(const TriangulatedMap::Face &face, tmap.faces) {
        QPointF vs[3] = { face.u, face.v, face.w };
        for (int i = 0; i < 3; i++)
            if (!vertices.contains(vs[i]))
                vertices.insert(vs[i], vertices.size());
    }

    out << vertices.size() << ' ' << tmap.faces.size() << '\n';

    {
        // We need to know a face for each vertex when we output the list.
        QVector<int> vertex_face_idx(vertices.size());
        for (int i = 0; i < tmap.faces.size(); i++) {
            TriangulatedMap::Face face = tmap.faces[i];
            QPointF vs[3] = { face.u, face.v, face.w };
            for (int j = 0; j < 3; j++)
                vertex_face_idx[vertices[vs[j]]] = i;
        }

        // Create a list of the vertices and sort by index so we can output
        // the vertices in the index order.
        QVector< QPair<int, QPointF> > points;
        QMapIterator<QPointF, int> it(vertices);
        while (it.hasNext()) {
            it.next();
            points.append(qMakePair(it.value(), it.key()));
        }
        qSort(points.begin(), points.end());

        for (int i = 0; i < points.size(); i++) {
            QPointF p = points[i].second;
            out << p.x() << ' ' << p.y() << " 0 "
                << vertex_face_idx[i] << '\n';
        }
    }

    {
        // We need to know the adjacent faces for each face. We can find this
        // by creating a mapping from edges to adjacent faces. Every face
        // should have three adjacent faces, but if it doesnt we use the dummy
        // face as the adjacent face.
        typedef QPair<int, int> Edge;
        QMap<Edge, QPair<int, int> > edges;
        for (int i = 1; i < tmap.faces.size(); i++) {
            TriangulatedMap::Face face = tmap.faces[i];
            QPointF vs[3] = { face.u, face.v, face.w };
            for (int j = 0; j < 3; j++) {
                int a = vertices[vs[j]];
                int b = vertices[vs[(j + 1) % 3]];
                // If we force a <= b order then the edge (a, b) is the same
                // as (b, a) since it will be stored as (a, b)
                if (b < a)
                    qSwap(a, b);

                Edge edge(a, b);
                if (edges[edge].first == 0) {
                    edges[edge].first = i;
                } else {
                    Q_ASSERT(edges[edge].second == 0);
                    edges[edge].second = i;
                }
            }
        }

        out << "0 0 0 0 0 0 inf\n";
        for (int i = 1; i < tmap.faces.size(); i++) {
            TriangulatedMap::Face face = tmap.faces[i];
            int vs[3] = { vertices[face.u], vertices[face.v],
                          vertices[face.w] };
            out << vs[0] << ' ' << vs[1] << ' ' << vs[2] << ' ';
            for (int j = 0; j < 3; j++) {
                int a = vs[j];
                int b = vs[(j + 1) % 3];
                // If we force a <= b order then the edge (a, b) is the same
                // as (b, a) since it will be stored as (a, b)
                if (b < a)
                    qSwap(a, b);

                Edge edge(a, b);

                int adjacent_idx = edges[edge].first;
                if (adjacent_idx == i)
                    adjacent_idx = edges[edge].second;

                out << adjacent_idx << ' ';
            }

            out << face.weight << '\n';
        }
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
