//key positions
#define MATRIX_POS(\
NC,/*not connected*/\
/*0 1   2   3   4   5   6   7*/\
a1, a2, a3, a4, a5, a6, a7, a8,/*0*/\
b1, b2, b3, b4, b5, b6, b7, b8,/*1*/\
c1, c2, c3, c4, c5, c6, c7, c8,/*2*/\
d1, d2, d3, d4, d5, d6, d7, d8,/*3*/\
e1, e2, e3, e4, e5, e6, e7, e8,/*4*/\
f1, f2, f3, f4, f5, f6, f7, f8,/*5*/\
g1, g2, g3, g4, g5, g6, g7, g8,/*6*/\
h1, h2, h3, h4, h5, h6, h7, h8,/*7*/\
i1, i2, i3, i4, i5, i6, i7, i8,/*8*/\
j1, j2, j3, j4, j5, j6, j7, j8,/*9*/\
k1, k2, k3, k4, k5, k6, k7, k8,/*10*/\
l1, l2                         /*11*/\
)\
{{a1, a2, a3, a4, a5, a6, a7, a8},\
 {b1, b2, b3, b4, b5, b6, b7, b8},\
 {c1, c2, c3, c4, c5, c6, c7, c8},\
 {d1, d2, d3, d4, d5, d6, d7, d8},\
 {e1, e2, e3, e4, e5, e6, e7, e8},\
 {f1, f2, f3, f4, f5, f6, f7, f8},\
 {g1, g2, g3, g4, g5, g6, g7, g8},\
 {h1, h2, h3, h4, h5, h6, h7, h8},\
 {i1, i2, i3, i4, i5, i6, i7, i8},\
 {j1, j2, j3, j4, j5, j6, j7, j8},\
 {k1, k2, k3, k4, k5, k6, k7, k8},\
 {l1, l2, NC, NC, NC, NC, NC, NC}}
// extended layout only
// {m1, m2, m3, m4, m5, m6, m7, m8},
// {n1, n2, n3, n4, NC, NC, NC, NC}}
