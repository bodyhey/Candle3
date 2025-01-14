#ifndef CUBE_H
#define CUBE_H
#include "shaderdrawable.h"

static const QVector<VertexData> cube = {
  // f 0 Default_Generic
  VertexData(QVector3D(5.500000, 5.500000, 7.500000), 0, QVector3D(0.057735, 0.057735, 0.057735)),
  VertexData(QVector3D(7.500000, 5.500000, 5.500000), 0, QVector3D(0.057735, 0.057735, 0.057735)),
  VertexData(QVector3D(5.500000, 7.500000, 5.500000), 0, QVector3D(0.057735, 0.057735, 0.057735)),
  // f 1 Default_Generic
  VertexData(QVector3D(5.500000, 5.500000, -7.500000), 0, QVector3D(0.057735, 0.057735, -0.057735)),
  VertexData(QVector3D(5.500000, 7.500000, -5.500000), 0, QVector3D(0.057735, 0.057735, -0.057735)),
  VertexData(QVector3D(7.500000, 5.500000, -5.500000), 0, QVector3D(0.057735, 0.057735, -0.057735)),
  // f 2 BackLeft
  VertexData(QVector3D(5.500000, 7.500000, -5.500000), 7, QVector3D(0.070711, 0.070711, 0.000000)),
  VertexData(QVector3D(5.500000, 7.500000, 5.500000), 7, QVector3D(0.070711, 0.070711, 0.000000)),
  VertexData(QVector3D(7.500000, 5.500000, -5.500000), 7, QVector3D(0.070711, 0.070711, 0.000000)),
  // f 3 BackLeft
  VertexData(QVector3D(7.500000, 5.500000, -5.500000), 7, QVector3D(0.070711, 0.070711, 0.000000)),
  VertexData(QVector3D(5.500000, 7.500000, 5.500000), 7, QVector3D(0.070711, 0.070711, 0.000000)),
  VertexData(QVector3D(7.500000, 5.500000, 5.500000), 7, QVector3D(0.070711, 0.070711, 0.000000)),
  // f 4 Default_Generic
  VertexData(QVector3D(5.500000, -5.500000, 7.500000), 0, QVector3D(0.057735, -0.057735, 0.057735)),
  VertexData(QVector3D(5.500000, -7.500000, 5.500000), 0, QVector3D(0.057735, -0.057735, 0.057735)),
  VertexData(QVector3D(7.500000, -5.500000, 5.500000), 0, QVector3D(0.057735, -0.057735, 0.057735)),
  // f 5 Default_Generic
  VertexData(QVector3D(-5.500000, 5.500000, 7.500000), 0, QVector3D(-0.057735, 0.057735, 0.057735)),
  VertexData(QVector3D(-5.500000, 7.500000, 5.500000), 0, QVector3D(-0.057735, 0.057735, 0.057735)),
  VertexData(QVector3D(-7.500000, 5.500000, 5.500000), 0, QVector3D(-0.057735, 0.057735, 0.057735)),
  // f 6 Default_Generic
  VertexData(QVector3D(5.500000, -5.500000, -7.500000), 0, QVector3D(0.057735, -0.057735, -0.057735)),
  VertexData(QVector3D(7.500000, -5.500000, -5.500000), 0, QVector3D(0.057735, -0.057735, -0.057735)),
  VertexData(QVector3D(5.500000, -7.500000, -5.500000), 0, QVector3D(0.057735, -0.057735, -0.057735)),
  // f 7 BackRight
  VertexData(QVector3D(5.500000, -7.500000, 5.500000), 7, QVector3D(0.070711, -0.070711, 0.000000)),
  VertexData(QVector3D(5.500000, -7.500000, -5.500000), 7, QVector3D(0.070711, -0.070711, 0.000000)),
  VertexData(QVector3D(7.500000, -5.500000, 5.500000), 7, QVector3D(0.070711, -0.070711, 0.000000)),
  // f 8 BackRight
  VertexData(QVector3D(7.500000, -5.500000, 5.500000), 7, QVector3D(0.070711, -0.070711, 0.000000)),
  VertexData(QVector3D(5.500000, -7.500000, -5.500000), 7, QVector3D(0.070711, -0.070711, 0.000000)),
  VertexData(QVector3D(7.500000, -5.500000, -5.500000), 7, QVector3D(0.070711, -0.070711, 0.000000)),
  // f 9 Default_Generic
  VertexData(QVector3D(-5.500000, 5.500000, -7.500000), 0, QVector3D(-0.057735, 0.057735, -0.057735)),
  VertexData(QVector3D(-7.500000, 5.500000, -5.500000), 0, QVector3D(-0.057735, 0.057735, -0.057735)),
  VertexData(QVector3D(-5.500000, 7.500000, -5.500000), 0, QVector3D(-0.057735, 0.057735, -0.057735)),
  // f 10 FrontLeft
  VertexData(QVector3D(-5.500000, 7.500000, 5.500000), 7, QVector3D(-0.070711, 0.070711, 0.000000)),
  VertexData(QVector3D(-5.500000, 7.500000, -5.500000), 7, QVector3D(-0.070711, 0.070711, 0.000000)),
  VertexData(QVector3D(-7.500000, 5.500000, 5.500000), 7, QVector3D(-0.070711, 0.070711, 0.000000)),
  // f 11 FrontLeft
  VertexData(QVector3D(-7.500000, 5.500000, 5.500000), 7, QVector3D(-0.070711, 0.070711, 0.000000)),
  VertexData(QVector3D(-5.500000, 7.500000, -5.500000), 7, QVector3D(-0.070711, 0.070711, 0.000000)),
  VertexData(QVector3D(-7.500000, 5.500000, -5.500000), 7, QVector3D(-0.070711, 0.070711, 0.000000)),
  // f 12 LeftBottom
  VertexData(QVector3D(5.500000, 5.500000, -7.500000), 7, QVector3D(0.000000, 0.070711, -0.070711)),
  VertexData(QVector3D(-5.500000, 5.500000, -7.500000), 7, QVector3D(0.000000, 0.070711, -0.070711)),
  VertexData(QVector3D(5.500000, 7.500000, -5.500000), 7, QVector3D(0.000000, 0.070711, -0.070711)),
  // f 13 LeftBottom
  VertexData(QVector3D(5.500000, 7.500000, -5.500000), 7, QVector3D(0.000000, 0.070711, -0.070711)),
  VertexData(QVector3D(-5.500000, 5.500000, -7.500000), 7, QVector3D(0.000000, 0.070711, -0.070711)),
  VertexData(QVector3D(-5.500000, 7.500000, -5.500000), 7, QVector3D(0.000000, 0.070711, -0.070711)),
  // f 14 Default_Generic
  VertexData(QVector3D(-5.500000, -5.500000, 7.500000), 0, QVector3D(-0.057735, -0.057735, 0.057735)),
  VertexData(QVector3D(-7.500000, -5.500000, 5.500000), 0, QVector3D(-0.057735, -0.057735, 0.057735)),
  VertexData(QVector3D(-5.500000, -7.500000, 5.500000), 0, QVector3D(-0.057735, -0.057735, 0.057735)),
  // f 15 Default_Generic
  VertexData(QVector3D(-5.500000, -5.500000, -7.500000), 0, QVector3D(-0.057735, -0.057735, -0.057735)),
  VertexData(QVector3D(-5.500000, -7.500000, -5.500000), 0, QVector3D(-0.057735, -0.057735, -0.057735)),
  VertexData(QVector3D(-7.500000, -5.500000, -5.500000), 0, QVector3D(-0.057735, -0.057735, -0.057735)),
  // f 16 FrontRight
  VertexData(QVector3D(-7.500000, -5.500000, 5.500000), 7, QVector3D(-0.070711, -0.070711, 0.000000)),
  VertexData(QVector3D(-7.500000, -5.500000, -5.500000), 7, QVector3D(-0.070711, -0.070711, 0.000000)),
  VertexData(QVector3D(-5.500000, -7.500000, 5.500000), 7, QVector3D(-0.070711, -0.070711, 0.000000)),
  // f 17 FrontRight
  VertexData(QVector3D(-5.500000, -7.500000, 5.500000), 7, QVector3D(-0.070711, -0.070711, 0.000000)),
  VertexData(QVector3D(-7.500000, -5.500000, -5.500000), 7, QVector3D(-0.070711, -0.070711, 0.000000)),
  VertexData(QVector3D(-5.500000, -7.500000, -5.500000), 7, QVector3D(-0.070711, -0.070711, 0.000000)),
  // f 18 FrontBottom
  VertexData(QVector3D(-7.500000, 5.500000, -5.500000), 7, QVector3D(-0.070711, 0.000000, -0.070711)),
  VertexData(QVector3D(-5.500000, 5.500000, -7.500000), 7, QVector3D(-0.070711, 0.000000, -0.070711)),
  VertexData(QVector3D(-7.500000, -5.500000, -5.500000), 7, QVector3D(-0.070711, 0.000000, -0.070711)),
  // f 19 FrontBottom
  VertexData(QVector3D(-7.500000, -5.500000, -5.500000), 7, QVector3D(-0.070711, 0.000000, -0.070711)),
  VertexData(QVector3D(-5.500000, 5.500000, -7.500000), 7, QVector3D(-0.070711, 0.000000, -0.070711)),
  VertexData(QVector3D(-5.500000, -5.500000, -7.500000), 7, QVector3D(-0.070711, 0.000000, -0.070711)),
  // f 20 RightBottom
  VertexData(QVector3D(-5.500000, -5.500000, -7.500000), 7, QVector3D(0.000000, -0.070711, -0.070711)),
  VertexData(QVector3D(5.500000, -5.500000, -7.500000), 7, QVector3D(0.000000, -0.070711, -0.070711)),
  VertexData(QVector3D(-5.500000, -7.500000, -5.500000), 7, QVector3D(0.000000, -0.070711, -0.070711)),
  // f 21 RightBottom
  VertexData(QVector3D(-5.500000, -7.500000, -5.500000), 7, QVector3D(0.000000, -0.070711, -0.070711)),
  VertexData(QVector3D(5.500000, -5.500000, -7.500000), 7, QVector3D(0.000000, -0.070711, -0.070711)),
  VertexData(QVector3D(5.500000, -7.500000, -5.500000), 7, QVector3D(0.000000, -0.070711, -0.070711)),
  // f 22 RightTop
  VertexData(QVector3D(5.500000, -5.500000, 7.500000), 7, QVector3D(0.000000, -0.070711, 0.070711)),
  VertexData(QVector3D(-5.500000, -5.500000, 7.500000), 7, QVector3D(0.000000, -0.070711, 0.070711)),
  VertexData(QVector3D(5.500000, -7.500000, 5.500000), 7, QVector3D(0.000000, -0.070711, 0.070711)),
  // f 23 RightTop
  VertexData(QVector3D(5.500000, -7.500000, 5.500000), 7, QVector3D(0.000000, -0.070711, 0.070711)),
  VertexData(QVector3D(-5.500000, -5.500000, 7.500000), 7, QVector3D(0.000000, -0.070711, 0.070711)),
  VertexData(QVector3D(-5.500000, -7.500000, 5.500000), 7, QVector3D(0.000000, -0.070711, 0.070711)),
  // f 24 FrontTop
  VertexData(QVector3D(-7.500000, -5.500000, 5.500000), 7, QVector3D(-0.070711, 0.000000, 0.070711)),
  VertexData(QVector3D(-5.500000, -5.500000, 7.500000), 7, QVector3D(-0.070711, 0.000000, 0.070711)),
  VertexData(QVector3D(-7.500000, 5.500000, 5.500000), 7, QVector3D(-0.070711, 0.000000, 0.070711)),
  // f 25 FrontTop
  VertexData(QVector3D(-7.500000, 5.500000, 5.500000), 7, QVector3D(-0.070711, 0.000000, 0.070711)),
  VertexData(QVector3D(-5.500000, -5.500000, 7.500000), 7, QVector3D(-0.070711, 0.000000, 0.070711)),
  VertexData(QVector3D(-5.500000, 5.500000, 7.500000), 7, QVector3D(-0.070711, 0.000000, 0.070711)),
  // f 26 BackBottom
  VertexData(QVector3D(7.500000, -5.500000, -5.500000), 7, QVector3D(0.070711, 0.000000, -0.070711)),
  VertexData(QVector3D(5.500000, -5.500000, -7.500000), 7, QVector3D(0.070711, 0.000000, -0.070711)),
  VertexData(QVector3D(7.500000, 5.500000, -5.500000), 7, QVector3D(0.070711, 0.000000, -0.070711)),
  // f 27 BackBottom
  VertexData(QVector3D(7.500000, 5.500000, -5.500000), 7, QVector3D(0.070711, 0.000000, -0.070711)),
  VertexData(QVector3D(5.500000, -5.500000, -7.500000), 7, QVector3D(0.070711, 0.000000, -0.070711)),
  VertexData(QVector3D(5.500000, 5.500000, -7.500000), 7, QVector3D(0.070711, 0.000000, -0.070711)),
  // f 28 LeftTop
  VertexData(QVector3D(-5.500000, 5.500000, 7.500000), 7, QVector3D(0.000000, 0.070711, 0.070711)),
  VertexData(QVector3D(5.500000, 5.500000, 7.500000), 7, QVector3D(0.000000, 0.070711, 0.070711)),
  VertexData(QVector3D(-5.500000, 7.500000, 5.500000), 7, QVector3D(0.000000, 0.070711, 0.070711)),
  // f 29 LeftTop
  VertexData(QVector3D(-5.500000, 7.500000, 5.500000), 7, QVector3D(0.000000, 0.070711, 0.070711)),
  VertexData(QVector3D(5.500000, 5.500000, 7.500000), 7, QVector3D(0.000000, 0.070711, 0.070711)),
  VertexData(QVector3D(5.500000, 7.500000, 5.500000), 7, QVector3D(0.000000, 0.070711, 0.070711)),
  // f 30 BackTop
  VertexData(QVector3D(7.500000, 5.500000, 5.500000), 7, QVector3D(0.070711, 0.000000, 0.070711)),
  VertexData(QVector3D(5.500000, 5.500000, 7.500000), 7, QVector3D(0.070711, 0.000000, 0.070711)),
  VertexData(QVector3D(7.500000, -5.500000, 5.500000), 7, QVector3D(0.070711, 0.000000, 0.070711)),
  // f 31 BackTop
  VertexData(QVector3D(7.500000, -5.500000, 5.500000), 7, QVector3D(0.070711, 0.000000, 0.070711)),
  VertexData(QVector3D(5.500000, 5.500000, 7.500000), 7, QVector3D(0.070711, 0.000000, 0.070711)),
  VertexData(QVector3D(5.500000, -5.500000, 7.500000), 7, QVector3D(0.070711, 0.000000, 0.070711)),
  // f 32 Back
  VertexData(QVector3D(7.500000, -5.500000, -5.500000), 2, QVector3D(0.100000, 0.000000, 0.000000)),
  VertexData(QVector3D(7.500000, 5.500000, -5.500000), 2, QVector3D(0.100000, 0.000000, 0.000000)),
  VertexData(QVector3D(7.500000, -5.500000, 5.500000), 2, QVector3D(0.100000, 0.000000, 0.000000)),
  // f 33 Back
  VertexData(QVector3D(7.500000, -5.500000, 5.500000), 2, QVector3D(0.100000, 0.000000, 0.000000)),
  VertexData(QVector3D(7.500000, 5.500000, -5.500000), 2, QVector3D(0.100000, 0.000000, 0.000000)),
  VertexData(QVector3D(7.500000, 5.500000, 5.500000), 2, QVector3D(0.100000, 0.000000, 0.000000)),
  // f 34 Left
  VertexData(QVector3D(-5.500000, 7.500000, 5.500000), 5, QVector3D(0.000000, 0.100000, 0.000000)),
  VertexData(QVector3D(5.500000, 7.500000, 5.500000), 5, QVector3D(0.000000, 0.100000, 0.000000)),
  VertexData(QVector3D(-5.500000, 7.500000, -5.500000), 5, QVector3D(0.000000, 0.100000, 0.000000)),
  // f 35 Left
  VertexData(QVector3D(-5.500000, 7.500000, -5.500000), 5, QVector3D(0.000000, 0.100000, 0.000000)),
  VertexData(QVector3D(5.500000, 7.500000, 5.500000), 5, QVector3D(0.000000, 0.100000, 0.000000)),
  VertexData(QVector3D(5.500000, 7.500000, -5.500000), 5, QVector3D(0.000000, 0.100000, 0.000000)),
  // f 36 Bottom
  VertexData(QVector3D(-5.500000, 5.500000, -7.500000), 4, QVector3D(0.000000, 0.000000, -0.100000)),
  VertexData(QVector3D(5.500000, 5.500000, -7.500000), 4, QVector3D(0.000000, 0.000000, -0.100000)),
  VertexData(QVector3D(-5.500000, -5.500000, -7.500000), 4, QVector3D(0.000000, 0.000000, -0.100000)),
  // f 37 Bottom
  VertexData(QVector3D(-5.500000, -5.500000, -7.500000), 4, QVector3D(0.000000, 0.000000, -0.100000)),
  VertexData(QVector3D(5.500000, 5.500000, -7.500000), 4, QVector3D(0.000000, 0.000000, -0.100000)),
  VertexData(QVector3D(5.500000, -5.500000, -7.500000), 4, QVector3D(0.000000, 0.000000, -0.100000)),
  // f 38 Right
  VertexData(QVector3D(-5.500000, -7.500000, -5.500000), 6, QVector3D(0.000000, -0.100000, 0.000000)),
  VertexData(QVector3D(5.500000, -7.500000, -5.500000), 6, QVector3D(0.000000, -0.100000, 0.000000)),
  VertexData(QVector3D(-5.500000, -7.500000, 5.500000), 6, QVector3D(0.000000, -0.100000, 0.000000)),
  // f 39 Right
  VertexData(QVector3D(-5.500000, -7.500000, 5.500000), 6, QVector3D(0.000000, -0.100000, 0.000000)),
  VertexData(QVector3D(5.500000, -7.500000, -5.500000), 6, QVector3D(0.000000, -0.100000, 0.000000)),
  VertexData(QVector3D(5.500000, -7.500000, 5.500000), 6, QVector3D(0.000000, -0.100000, 0.000000)),
  // f 40 Front
  VertexData(QVector3D(-7.500000, -5.500000, 5.500000), 1, QVector3D(-0.100000, 0.000000, 0.000000)),
  VertexData(QVector3D(-7.500000, 5.500000, 5.500000), 1, QVector3D(-0.100000, 0.000000, 0.000000)),
  VertexData(QVector3D(-7.500000, -5.500000, -5.500000), 1, QVector3D(-0.100000, 0.000000, 0.000000)),
  // f 41 Front
  VertexData(QVector3D(-7.500000, -5.500000, -5.500000), 1, QVector3D(-0.100000, 0.000000, 0.000000)),
  VertexData(QVector3D(-7.500000, 5.500000, 5.500000), 1, QVector3D(-0.100000, 0.000000, 0.000000)),
  VertexData(QVector3D(-7.500000, 5.500000, -5.500000), 1, QVector3D(-0.100000, 0.000000, 0.000000)),
  // f 42 Top
  VertexData(QVector3D(-5.500000, -5.500000, 7.500000), 3, QVector3D(0.000000, 0.000000, 0.100000)),
  VertexData(QVector3D(5.500000, -5.500000, 7.500000), 3, QVector3D(0.000000, 0.000000, 0.100000)),
  VertexData(QVector3D(-5.500000, 5.500000, 7.500000), 3, QVector3D(0.000000, 0.000000, 0.100000)),
  // f 43 Top
  VertexData(QVector3D(-5.500000, 5.500000, 7.500000), 3, QVector3D(0.000000, 0.000000, 0.100000)),
  VertexData(QVector3D(5.500000, -5.500000, 7.500000), 3, QVector3D(0.000000, 0.000000, 0.100000)),
  VertexData(QVector3D(5.500000, 5.500000, 7.500000), 3, QVector3D(0.000000, 0.000000, 0.100000)),
};

enum class CubeClickableFace {
    None = -1,
    BackLeft = 0,
    BackRight = 1,
    FrontLeft = 2,
    LeftBottom = 3,
    FrontRight = 4,
    FrontBottom = 5,
    RightBottom = 6,
    RightTop = 7,
    FrontTop = 8,
    BackBottom = 9,
    LeftTop = 10,
    BackTop = 11,
    Back = 12,
    Left = 13,
    Bottom = 14,
    Right = 15,
    Front = 16,
    Top = 17,
};

static const int clickables[][6] = {
    {6, 7, 8, 9, 10, 11},
    {21, 22, 23, 24, 25, 26},
    {30, 31, 32, 33, 34, 35},
    {36, 37, 38, 39, 40, 41},
    {48, 49, 50, 51, 52, 53},
    {54, 55, 56, 57, 58, 59},
    {60, 61, 62, 63, 64, 65},
    {66, 67, 68, 69, 70, 71},
    {72, 73, 74, 75, 76, 77},
    {78, 79, 80, 81, 82, 83},
    {84, 85, 86, 87, 88, 89},
    {90, 91, 92, 93, 94, 95},
    {96, 97, 98, 99, 100, 101},
    {102, 103, 104, 105, 106, 107},
    {108, 109, 110, 111, 112, 113},
    {114, 115, 116, 117, 118, 119},
    {120, 121, 122, 123, 124, 125},
    {126, 127, 128, 129, 130, 131},
};

#endif // CUBE_H

