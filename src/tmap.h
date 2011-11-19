#pragma once

#include <cassert>
#include <cstdio>
#include <limits>

struct Face;

struct Vertex {
    float x, y;
    Face * face;
};

struct Face {
    Vertex *u, *v, *w;
    Face *a, *b, *c;
    float weight;

    Vertex * vertex(int i) {
        assert(0 <= i && i < 3);
        return (&u)[i];
    }

    Face * face(int i) {
        assert(0 <= i && i < 3);
        return (&a)[i];
    }

    int vidx(Vertex * v) {
        int i;
        for (i = 0; i < 3; i++)
            if (vertex(i) == v)
                break;
        assert(i < 3);
        return i;
    }

    int fidx(Face * f) {
        int i;
        for (i = 0; i < 3; i++)
            if (face(i) == f)
                break;
        assert(i < 3);
        return i;
    }

    bool ignore() const {
        return weight == std::numeric_limits<float>::infinity();
    }
};

struct TriangulatedMap {
    int n_vertices, n_faces;

    Vertex *vertices;
    Face *faces;

    Vertex *vertex(int idx) {
        assert(0 <= idx && idx < n_vertices);
        return vertices + idx;
    }

    Face *face(int idx) {
        assert(0 <= idx && idx < n_faces);
        return faces + idx;
    }

    int vidx(Vertex *v) {
        assert(vertices <= v);
        int i = v - vertices;
        assert(i < n_vertices);
        return i;
    }

    int fidx(Face *f) {
        assert(faces <= f);
        int i = f - faces;
        assert(i < n_faces);
        return i;
    }
};


TriangulatedMap *tmap_readin(FILE *fin=NULL);
void tmap_free(TriangulatedMap *map);
