#include "cubedrawer.h"

#include <QOpenGLVertexArrayObject>
#include <QOpenGLBuffer>
#include <QOpenGLTexture>
#include "cube.h"
#include "utils.h"

#define DISTANCE 50.0
#define SIZE 100.0
#define MULTISAMPLE 2

CubeDrawer::CubeDrawer() : m_eye({0, 0, 1}), m_animation(this, "faceAnimation")
{
    m_fbo = nullptr;

    m_width = SIZE * MULTISAMPLE;
    m_height = SIZE * MULTISAMPLE;
    m_triangles = cube;

    m_animation.setDuration(100);
    m_animation.setStartValue(0.0);
    m_animation.setEndValue(1.0);

    const QVector3D lineColor(0.0, 0.0, 0.0);
    for (auto clickable : clickables) {
        QVector3D ofs = cube[clickable[0]].start * 1;
        m_lines
            << VertexData(cube[clickable[0]].position + ofs, lineColor, cube[clickable[0]].start)
            << VertexData(cube[clickable[1]].position + ofs, lineColor, cube[clickable[0]].start)

            << VertexData(cube[clickable[1]].position + ofs, lineColor, cube[clickable[0]].start)
            << VertexData(cube[clickable[5]].position + ofs, lineColor, cube[clickable[0]].start)

            << VertexData(cube[clickable[5]].position + ofs, lineColor, cube[clickable[0]].start)
            << VertexData(cube[clickable[2]].position + ofs, lineColor, cube[clickable[0]].start)

            << VertexData(cube[clickable[2]].position + ofs, lineColor, cube[clickable[0]].start)
            << VertexData(cube[clickable[0]].position + ofs, lineColor, cube[clickable[0]].start)
            ;
    }

    qDebug() << m_triangles.count() << m_lines.count();

    setProjection();
}

void CubeDrawer::drawBackground() {
    glClearColor(0.2, 0.2, 0.0, 0.2);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
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

    QVector<_2DTexturedVertexData> copyTriangles;
    // m_copyTriangles << _2DTexturedVertexData(QVector2D(-1.0, -1.0), QVector2D(0, 0))
    //                 << _2DTexturedVertexData(QVector2D(1.0, -1.0), QVector2D(1, 0))
    //                 << _2DTexturedVertexData(QVector2D(-1.0, 1.0), QVector2D(0, 1))
    //                 << _2DTexturedVertexData(QVector2D(-1.0, 1.0), QVector2D(0, 1))
    //                 << _2DTexturedVertexData(QVector2D(1.0, -1.0), QVector2D(1, 0))
    //                 << _2DTexturedVertexData(QVector2D(1.0, 1.0), QVector2D(1, 1));

    // 3 triangles are used to draw chamfered corner
    copyTriangles << _2DTexturedVertexData(QVector2D(-1.0, 1.0), QVector2D(0.0, 1.0))
                    << _2DTexturedVertexData(QVector2D(0.5, -1.0), QVector2D(0.75, 0.0))
                    << _2DTexturedVertexData(QVector2D(-1.0, -1.0), QVector2D(0.0, 0.0))

                    << _2DTexturedVertexData(QVector2D(-1.0, 1.0), QVector2D(0.0, 1.0))
                    << _2DTexturedVertexData(QVector2D(1.0, -0.5), QVector2D(1.0, 0.25))
                    << _2DTexturedVertexData(QVector2D(0.5, -1.0), QVector2D(0.75, 0.0))

                    << _2DTexturedVertexData(QVector2D(-1.0, 1.0), QVector2D(0.0, 1.0))
                    << _2DTexturedVertexData(QVector2D(1.0, 1.0), QVector2D(1.0, 1.0))
                    << _2DTexturedVertexData(QVector2D(1.0, -0.5), QVector2D(1.0, 0.25));

    copyVbo.allocate(copyTriangles.constData(), copyTriangles.count() * sizeof(_2DTexturedVertexData));

    glBindTexture(GL_TEXTURE_2D, m_fbo->texture());
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glDrawArrays(GL_TRIANGLES, 0, copyTriangles.count());

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

    m_viewMatrix.lookAt(eye, QVector3D(0, 0, 0), m_up);
}

void CubeDrawer::updateEyePosition(QVector3D eye, QVector3D up)
{
    m_eye = eye;
    m_up = up;
    m_needsUpdateGeometry = true;

    updateView();

    QMatrix4x4 mvp = m_projectionMatrix * m_viewMatrix;
    m_points2d.clear();
    for (auto clickable : clickables) {
        for (int i = 0; i < 6; i++) {
            QPointF mapped = (mvp.map(cube[clickable[i]].position) * 50.0).toPointF();
            m_points2d << QPoint(mapped.x() + (SIZE / 2), SIZE - (mapped.y() + 50));
        }
    }
}

CubeClickableFace CubeDrawer::faceAtPos(QPoint pos)
{
    int pi = 0;
    int ci = 0;
    for (auto clickable : clickables) {
        Q_UNUSED(clickable);

        QPoint tr1[3] = { m_points2d[pi++], m_points2d[pi++], m_points2d[pi++] };
        QPoint tr2[3] = { m_points2d[pi++], m_points2d[pi++], m_points2d[pi++] };

        if (
            Utils::triangleDir(tr1[0], tr1[1], tr1[2]) &&
            (Utils::pointInTriangle(pos, tr1[0], tr1[1], tr1[2]) || Utils::pointInTriangle(pos, tr2[0], tr2[1], tr2[2]))
        ) {
            return (CubeClickableFace)ci;
        }

        ci++;
    }

    return CubeClickableFace::None;
}

void CubeDrawer::mouseMoveEvent(QMouseEvent *event)
{
    QPoint pos = event->pos();

    CubeClickableFace lastFace = CubeClickableFace::None;
    m_faceAtCursor = faceAtPos(pos);
    if (m_faceAtCursor == lastFace) {
        return;
    }

    m_animation.stop();

    for (auto &line : m_lines) {
        line.color = QVector3D(0.0, 0.0, 0.0);
    }

    if (m_faceAtCursor == CubeClickableFace::None) {
        m_needsUpdateGeometry = true;

        return;
    }

    int fi = (int)m_faceAtCursor;
    int li = 0;

    auto clickable = clickables[fi];

    m_animation.start();

    const QVector3D white(1.0, 1.0, 1.0);
    QVector3D color = QVector3D(std::rand() % 100 / 100.0, std::rand() % 100 / 100.0, std::rand() % 100 / 100.0);
    for (int i = 0; i < 6; i++) {
        m_triangles[clickable[i]].color = color;
    }
    for (int i = 0; i < 8; i++) {
        m_lines[li + i].color = white;
    }
    m_needsUpdateGeometry = true;
}

void CubeDrawer::leaveEvent(QEvent *event)
{
    for (auto &line : m_lines) {
        line.color = QVector3D(0.0, 0.0, 0.0);
    }
}

void CubeDrawer::init()
{
    initializeOpenGLFunctions();

    m_fbo = new QOpenGLFramebufferObject(m_width, m_height, QOpenGLFramebufferObject::Depth, GL_TEXTURE_2D);
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

void CubeDrawer::initAttributes()
{
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

void CubeDrawer::updateGeometry()
{
    if (!m_vbo.isCreated())
        init();

    if (m_vao.isCreated()) {
        m_vao.bind();
    }

    m_vbo.bind();
    m_vbo.allocate((m_triangles + m_lines).constData(), (m_triangles.count() + m_lines.count()) * sizeof(VertexData));

    if (m_vao.isCreated()) {
        initAttributes();
        m_vao.release();
    }

    m_vbo.release();

    m_needsUpdateGeometry = false;
}

void CubeDrawer::drawCube()
{
    if (m_needsUpdateGeometry) {
        updateGeometry();
    }

    m_program->bind();
    m_program->setUniformValue("mvp_matrix", m_projectionMatrix * m_viewMatrix);

    QMatrix4x4 cMatrix;
    cMatrix.scale(0.9);
    m_program->setUniformValue("c_matrix", cMatrix);

    if (m_vao.isCreated()) m_vao.bind(); else {
        m_vbo.bind();
        initAttributes();
    }

    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
    glEnable(GL_LINE_SMOOTH);
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);

    glDrawArrays(GL_TRIANGLES, 0, m_triangles.count());
    glDrawArrays(GL_LINES, m_triangles.count(), m_lines.count());

    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);

    if (m_vao.isCreated()) m_vao.release(); else m_vbo.release();

    m_program->release();
    glFlush();
}

void CubeDrawer::setEye(float face)
{
    if (m_faceAtCursor == CubeClickableFace::None) {
        return;
    }

 //   qDebug() << "setEye" << face;
}
