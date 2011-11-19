#include "tmap.h"

#include <limits>

TriangulatedMap * tmap_readin(FILE *fin) {
    TriangulatedMap * m = new TriangulatedMap;
    fscanf(fin, "%d %d", &m->n_vertices, &m->n_faces);

    m->vertices = new Vertex[m->n_vertices];
    m->faces = new Face[m->n_faces];

    Vertex *v = m->vertices;
    for (int n = 0; n < m->n_vertices; n++, v++) {
        float z;
        int face_idx;
        fscanf(fin, "%f %f %f %d", &v->x, &v->y, &z, &face_idx);
        v->face = m->faces + face_idx;
    }

    Face *f = m->faces;
    for (int n = 0; n < m->n_faces; n++, f++) {
        int u_idx, v_idx, w_idx;
        int a_idx, b_idx, c_idx;
        fscanf(fin, "%d %d %d %d %d %d %f",
               &u_idx, &v_idx, &w_idx,
               &a_idx, &b_idx, &c_idx,
               &f->weight);
        f->u = m->vertices + u_idx;
        f->v = m->vertices + v_idx;
        f->w = m->vertices + w_idx;
        f->a = m->faces + a_idx;
        f->b = m->faces + b_idx;
        f->c = m->faces + c_idx;

        // If this vertex has zero area, ignore it. Some of the triangulations
        // I read in don't have enough floating point accuracy on the input,
        // which causes zero area faces.
        if ((f->u->x == f->v->x && f->u->y == f->v->y) ||
            (f->u->x == f->w->x && f->u->y == f->w->y) ||
            (f->v->x == f->w->x && f->v->y == f->w->y))
            f->weight = std::numeric_limits<float>::infinity();
    }

#ifndef NDEBUG
    Face * sink = m->faces;
    for (int i = 1; i < m->n_faces; i++) {
        Face * f = m->faces + i;
        Face * faces[3] = {f->a, f->b, f->c};
        Vertex * v[3] = {f->u, f->v, f->w};
        for (int i = 0; i < 3; i++)
            assert(faces[i] >= m->faces && faces[i] - m->faces < m->n_faces);
        for (int i = 0; i < 3; i++)
            assert(m->vertices <= v[i] && v[i] - m->vertices < m->n_vertices);
    }
    for (int i = 1; i < m->n_vertices; i++) {
        Vertex * v = m->vertices + i;
        Face * f = v->face;
        int x = f->vidx(v);
        for (int j = 1; j < 3; j++) {
            Face * h = f->face((x + j) % 3);
            if (h != sink)
                h->vidx(v);
        }
    }
#endif

    return m;
}

void tmap_free(TriangulatedMap * map) {
    delete[] map->vertices;
    delete[] map->faces;
    delete map;
}
