#ifndef CUBEDRAWER_H
#define CUBEDRAWER_H

#include <QOpenGLFunctions_3_0>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLBuffer>
#include <QOpenGLTexture>
#include <QOpenGLFramebufferObject>
#include <QOpenGLFunctions>
#include <QMatrix4x4>
#include <QVector3D>
#include <QOpenGLShaderProgram>
#include <QPropertyAnimation>
#include <QMouseEvent>
#include "shaderdrawable.h"
#include "cube.h"
#include "../widgets/glpalette.h"

class CubeDrawer : public QObject, protected QOpenGLFunctions_3_0
{
    Q_OBJECT

    Q_PROPERTY(float faceAnimation WRITE setEye)

public:
    explicit CubeDrawer();
    void draw(QRect, GLPalette &palette);
    void setProjection();
    void updateView();
    void updateEyePosition(QVector3D eye, QVector3D up);
    CubeClickableFace faceAtPos(QPoint pos);
    CubeClickableFace mouseMoveEvent(QMouseEvent *event);
    void leaveEvent(QEvent *event);

private:
    QOpenGLVertexArrayObject m_vao;
    QOpenGLBuffer m_vbo;
    QOpenGLFramebufferObject *m_fbo;
    int m_width;
    int m_height;
    bool m_needsUpdateGeometry = true;
    QVector3D m_eye;
    QVector3D m_up;
    QVector<QPoint> m_points2d;
    QVector<VertexData> m_triangles;
    QVector<VertexData> m_lines;
    QMatrix4x4 m_projectionMatrix;
    QMatrix4x4 m_viewMatrix;
    QOpenGLShaderProgram *m_program;
    QOpenGLShaderProgram *m_copyProgram;;
    QPropertyAnimation m_animation;

    CubeClickableFace m_faceAtCursor = CubeClickableFace::None;

    void init();
    void initAttributes();
    void updateGeometry(GLPalette &palette);
    void drawBackground();
    void drawCube(GLPalette &palette);
    void setEye(float pos);
};

#endif // CUBEDRAWER_H
