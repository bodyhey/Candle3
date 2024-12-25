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
#include "shaderdrawable.h"

class CubeDrawer : protected QOpenGLFunctions_3_0
{
public:
    CubeDrawer();
    void draw(QRect);
    void setProjection();
    void updateView();
    void updateEyePosition(QVector3D eye);
private:
    QOpenGLVertexArrayObject m_vao;
    QOpenGLBuffer m_vbo;
    QOpenGLFramebufferObject *m_fbo;
    int m_width;
    int m_height;
    bool m_needsUpdateGeometry = true;
    QVector3D m_eye;
    QVector<VertexData> m_triangles;

    void init();
    void updateGeometry();
    void drawBackground();
    void drawCube();
    void generateCube();

    QMatrix4x4 m_projectionMatrix;
    QMatrix4x4 m_viewMatrix;
    QOpenGLShaderProgram *m_program;
    QOpenGLShaderProgram *m_copyProgram;;
};

#endif // CUBEDRAWER_H
