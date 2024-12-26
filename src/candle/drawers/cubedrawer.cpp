#include "cubedrawer.h"

#include <QOpenGLVertexArrayObject>
#include <QOpenGLBuffer>
#include <QOpenGLTexture>

#define DISTANCE 50.0

CubeDrawer::CubeDrawer() : m_eye({0, 0, 1})
{
    m_fbo = nullptr;

    m_width = 100;
    m_height = 100;

    generateCube();

    setProjection();
}

void CubeDrawer::drawBackground() {
    glClearColor(0.0, 0.0, 0.0, 0.3);
    glClear(GL_COLOR_BUFFER_BIT);
}

// dest is normalized to -1, 1
void CubeDrawer::draw(QRect dest) {
    if (!m_vbo.isCreated())
        init();

    m_fbo->bind();

    glViewport(0, 0, this->m_width, this->m_height);
    glEnable(GL_BLEND);
    drawBackground();
    drawCube();

    m_fbo->release();

    glViewport(dest.x(), dest.y(), dest.width(), dest.height());

    //
    // glBindFramebuffer(GL_READ_FRAMEBUFFER, m_fbo->handle());
    // glBlitFramebuffer(
    //     0, 0, m_width, m_height,
    //     dest.x(), dest.y(), dest.x() + m_width, dest.y() + m_height,
    //     GL_COLOR_BUFFER_BIT, GL_LINEAR
    // );

    m_copyProgram->bind();

    QOpenGLBuffer copyVbo;
    copyVbo.create();
    copyVbo.bind();

    quintptr offset = 0;

    int vertexLocation = m_copyProgram->attributeLocation("a_position");
    m_copyProgram->enableAttributeArray(vertexLocation);
    m_copyProgram->setAttributeBuffer(vertexLocation, GL_FLOAT, offset, 2, 2 * 4 * 2);

    offset += sizeof(QVector2D);

    int textureLocation = m_copyProgram->attributeLocation("a_texcoord");
    m_copyProgram->enableAttributeArray(textureLocation);
    m_copyProgram->setAttributeBuffer(textureLocation, GL_FLOAT, offset, 2, 2 * 4 * 2);

    QVector<_2DTexturedVertexData> m_copyTriangles;
    // m_copyTriangles << _2DTexturedVertexData(QVector2D(-1.0, -1.0), QVector2D(0, 0))
    //                 << _2DTexturedVertexData(QVector2D(1.0, -1.0), QVector2D(1, 0))
    //                 << _2DTexturedVertexData(QVector2D(-1.0, 1.0), QVector2D(0, 1))
    //                 << _2DTexturedVertexData(QVector2D(-1.0, 1.0), QVector2D(0, 1))
    //                 << _2DTexturedVertexData(QVector2D(1.0, -1.0), QVector2D(1, 0))
    //                 << _2DTexturedVertexData(QVector2D(1.0, 1.0), QVector2D(1, 1));

    // 3 triangles are used to draw chamfered corner
    m_copyTriangles << _2DTexturedVertexData(QVector2D(-1.0, 1.0), QVector2D(0.0, 1.0))
                    << _2DTexturedVertexData(QVector2D(0.5, -1.0), QVector2D(0.75, 0.0))
                    << _2DTexturedVertexData(QVector2D(-1.0, -1.0), QVector2D(0.0, 0.0))

                    << _2DTexturedVertexData(QVector2D(-1.0, 1.0), QVector2D(0.0, 1.0))
                    << _2DTexturedVertexData(QVector2D(1.0, -0.5), QVector2D(1.0, 0.25))
                    << _2DTexturedVertexData(QVector2D(0.5, -1.0), QVector2D(0.75, 0.0))

                    << _2DTexturedVertexData(QVector2D(-1.0, 1.0), QVector2D(0.0, 1.0))
                    << _2DTexturedVertexData(QVector2D(1.0, 1.0), QVector2D(1.0, 1.0))
                    << _2DTexturedVertexData(QVector2D(1.0, -0.5), QVector2D(1.0, 0.25));

    copyVbo.allocate(m_copyTriangles.constData(), m_copyTriangles.count() * sizeof(_2DTexturedVertexData));

    glBindTexture(GL_TEXTURE_2D, m_fbo->texture());
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glDrawArrays(GL_TRIANGLES, 0, m_copyTriangles.count());

    copyVbo.release();
    copyVbo.destroy();

    m_copyProgram->release();
}

void CubeDrawer::setProjection()
{
    m_projectionMatrix.setToIdentity();
    m_projectionMatrix.ortho(-10, 10, -10, 10, 1, 70);
}

void CubeDrawer::updateView()
{
    m_viewMatrix.setToIdentity();

    // use the eye position of main scene to rotate cube
    QVector3D normalized = m_eye.normalized();
    QVector3D eye = normalized * DISTANCE;
    qDebug() << "eye: " << m_eye << normalized << eye;

    m_viewMatrix.lookAt(eye, QVector3D(0, 0, 0), m_up);
}

void CubeDrawer::updateEyePosition(QVector3D eye, QVector3D up)
{
    m_eye = eye;
    m_up = up;
    m_needsUpdateGeometry = true;
}

void CubeDrawer::init()
{
    initializeOpenGLFunctions();

    m_fbo = new QOpenGLFramebufferObject(m_width, m_height, GL_TEXTURE_2D);
    m_vao.create();
    m_vbo.create();

    m_program = new QOpenGLShaderProgram();
    m_program->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/cube_vertex.glsl");
    m_program->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/cube_fragment.glsl");
    m_program->link();

    m_copyProgram = new QOpenGLShaderProgram();
    m_copyProgram->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/2dcopy_vertex.glsl");
    m_copyProgram->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/2dcopy_fragment.glsl");
    m_copyProgram->link();
}

void CubeDrawer::initAttributes() {
    quintptr offset = 0;

    int vertexLocation = m_program->attributeLocation("a_position");
    m_program->enableAttributeArray(vertexLocation);
    m_program->setAttributeBuffer(vertexLocation, GL_FLOAT, offset, 3,
                                  sizeof(VertexData));

    offset = sizeof(QVector3D);

    int colorLocation = m_program->attributeLocation("a_color");
    m_program->enableAttributeArray(colorLocation);
    m_program->setAttributeBuffer(colorLocation, GL_FLOAT, offset, 3,
                                  sizeof(VertexData));

    offset = sizeof(QVector3D);
}
void CubeDrawer::updateGeometry() {
    if (!m_vbo.isCreated())
        init();

    if (m_vao.isCreated()) {
        m_vao.bind();
    }

    m_vbo.bind();
    m_vbo.allocate(m_triangles.constData(),
                   m_triangles.count() * sizeof(VertexData));

    if (m_vao.isCreated()) {
        initAttributes();
        m_vao.release();
    }

    m_needsUpdateGeometry = false;

    m_vbo.release();
}

void CubeDrawer::drawCube()
{
    updateView();
    updateGeometry();

    m_program->bind();
    m_program->setUniformValue("mvp_matrix", m_projectionMatrix * m_viewMatrix);

    QMatrix4x4 cMatrix;
    cMatrix.scale(0.9);
    m_program->setUniformValue("c_matrix", cMatrix);

    if (m_vao.isCreated()) m_vao.bind(); else {
        m_vbo.bind();
        initAttributes();
    }

    glEnable(GL_CULL_FACE);
    glDrawArrays(GL_TRIANGLES, 0, m_triangles.count());
    glDisable(GL_CULL_FACE);

    if (m_vao.isCreated()) m_vao.release(); else m_vbo.release();

    m_program->release();
    glFlush();
}
