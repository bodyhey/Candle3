// This file is a part of "Candle" application.
// Copyright 2015-2021 Hayrullin Denis Ravilevich

#include "glwidget.h"
#include <QDebug>
#include <QtWidgets>
#include <QPainter>
#include <QEasingCurve>
#include <QOpenGLDebugLogger>

#ifdef GLES
//#include <GLES/gl.h>
#endif

#define ZOOMSTEP 1.1
#define DEFAULT_ZOOM 200
#define MIN_ZOOM  0.2
#define ONE_DEG_IN_RAD 0.0174533
// what is this value? oryginally was 1.25
#define MAGIC_ZOOM_MULTIPLIER 1.9

#ifdef GLES
GLWidget::GLWidget(QWidget *parent) : QOpenGLWidget(parent), m_shaderProgram(0)
#else
GLWidget::GLWidget(QWidget *parent) : QGLWidget(parent), m_shaderProgram(0)
#endif

{   
    m_frames = 0;
    m_fps = 0;

    m_animateView = false;
    m_updatesEnabled = false;

    m_xRot = m_xRotTarget = 35.264;
    m_yRot = m_yRotTarget = m_yRot > 180 ? 405 : 45;

    m_zoomDistance = DEFAULT_ZOOM;

    m_lookAt = QVector3D(0,0,0);

    m_perspective = false;

    m_fov = 30;
    m_near = 0.5;
    m_far = 5000.0;

    m_xMin = 0;
    m_xMax = 0;
    m_yMin = 0;
    m_yMax = 0;
    m_zMin = 0;
    m_zMax = 0;
    m_xSize = 0;
    m_ySize = 0;
    m_zSize = 0;

    updateProjection();
    updateView();

    m_spendTime.setHMS(0, 0, 0);
    m_estimatedTime.setHMS(0, 0, 0);

    m_vsync = false;
    m_targetFps = 60;

    QTimer::singleShot(1000, this, SLOT(onFramesTimer()));

    setMouseTracking(true);

    // enable antialiasing
    QSurfaceFormat sf = format();
    sf.setSamples(8);
    setFormat(sf);
}

GLWidget::~GLWidget()
{
    if (m_shaderProgram) {
        delete m_shaderProgram;
    }
}

double GLWidget::calculateVolume(QVector3D size) {
    return size.x() * size.y() * size.z();
}

void GLWidget::addDrawable(ShaderDrawable *drawable)
{
    m_shaderDrawables.append(drawable);
}

GLWidget& GLWidget::operator<<(ShaderDrawable *drawable)
{
    addDrawable(drawable);

    return *this;
}

void GLWidget::fitDrawable(ShaderDrawable *drawable)
{
    stopViewAnimation();

    m_zoomDistance = DEFAULT_ZOOM;

    if (drawable != NULL) {
        updateExtremes(drawable);

        double largestSize = qMax(qMax(m_xSize, m_ySize), m_zSize);

        double newZoom = largestSize / (MAGIC_ZOOM_MULTIPLIER * tan((m_fov * ONE_DEG_IN_RAD) / 2.0));
        m_zoomDistance = newZoom > 0 ? qMax(newZoom, MIN_ZOOM) : DEFAULT_ZOOM;

        m_lookAt = QVector3D(
            m_xSize / 2 + m_xMin,
            m_ySize / 2 + m_yMin,
            m_zSize / 2 + m_zMin
        );
    } else {
        m_lookAt = QVector3D(0, 0, 0);

        m_xMin = 0;
        m_xMax = 0;
        m_yMin = 0;
        m_yMax = 0;
        m_zMin = 0;
        m_zMax = 0;

        m_xSize = 0;
        m_ySize = 0;
        m_zSize = 0;
    }

    updateProjection();
    updateView();
}

void GLWidget::updateExtremes(ShaderDrawable *drawable)
{
    QVector3D minExtremes = drawable->getMinimumExtremes();
    QVector3D maxExtremes = drawable->getMaximumExtremes();

    m_xMin = !qIsNaN(minExtremes.x()) ? minExtremes.x() : 0;
    m_xMax = !qIsNaN(maxExtremes.x()) ? maxExtremes.x() : 0;
    m_yMin = !qIsNaN(minExtremes.y()) ? minExtremes.y() : 0;
    m_yMax = !qIsNaN(maxExtremes.y()) ? maxExtremes.y() : 0;
    m_zMin = !qIsNaN(minExtremes.z()) ? minExtremes.z() : 0;
    m_zMax = !qIsNaN(maxExtremes.z()) ? maxExtremes.z() : 0;

    m_xSize = m_xMax - m_xMin;
    m_ySize = m_yMax - m_yMin;
    m_zSize = m_zMax - m_zMin;
}

bool GLWidget::antialiasing() const
{
    return m_antialiasing;
}

void GLWidget::setAntialiasing(bool antialiasing)
{
    m_antialiasing = antialiasing;
}

void GLWidget::onFramesTimer()
{
    m_fps = m_frames;
    m_frames = 0;

    QTimer::singleShot(1000, this, SLOT(onFramesTimer()));
}

void GLWidget::viewAnimation()
{
    double t = (double)m_animationFrame++ / (m_fps * 0.2);

    if (t >= 1) stopViewAnimation();

    QEasingCurve ec(QEasingCurve::OutExpo);
    double val = ec.valueForProgress(t);

    m_xRot = m_xRotStored + double(m_xRotTarget - m_xRotStored) * val;
    m_yRot = m_yRotStored + double(m_yRotTarget - m_yRotStored) * val;

    updateView();
}

QString GLWidget::pinState() const
{
    return m_pinState;
}

void GLWidget::setPinState(const QString &pinState)
{
    m_pinState = pinState;
}

QString GLWidget::speedState() const
{
    return m_speedState;
}

void GLWidget::setSpeedState(const QString &additionalStatus)
{
    m_speedState = additionalStatus;
}

bool GLWidget::vsync() const
{
    return m_vsync;
}

void GLWidget::setVsync(bool vsync)
{
    m_vsync = vsync;
}

bool GLWidget::msaa() const
{
    return m_msaa;
}

void GLWidget::setMsaa(bool msaa)
{
    m_msaa = msaa;
}

bool GLWidget::updatesEnabled() const
{
    return m_updatesEnabled;
}

void GLWidget::setUpdatesEnabled(bool updatesEnabled)
{
    m_updatesEnabled = updatesEnabled;

    if (updatesEnabled) {
        m_timerPaint.start(m_vsync ? 0 : 1000 / m_targetFps, Qt::PreciseTimer, this);
    } else {
        m_timerPaint.stop();
    }
}

bool GLWidget::zBuffer() const
{
    return m_zBuffer;
}

void GLWidget::setZBuffer(bool zBuffer)
{
    m_zBuffer = zBuffer;
}

double GLWidget::fov() {
    return m_fov;
}

void GLWidget::setFov(double fov) {
    m_fov = fov;
    updateProjection();
}

double GLWidget::nearPlane() {
    return m_near;
}

void GLWidget::setNearPlane(double plane) {
    m_near = plane;
    updateProjection();
}

double GLWidget::farPlane() {
    return m_far;
}

void GLWidget::setFarPlane(double plane) {
    m_far = plane;
    updateProjection();
}

QString GLWidget::bufferState() const
{
    return m_bufferState;
}

void GLWidget::setBufferState(const QString &bufferState)
{
    m_bufferState = bufferState;
}

QString GLWidget::parserState() const
{
    return m_parserState;
}

void GLWidget::setParserState(const QString &parserState)
{
    m_parserState = parserState;
}


double GLWidget::lineWidth() const
{
    return m_lineWidth;
}

void GLWidget::setLineWidth(double lineWidth)
{
    m_lineWidth = lineWidth;
}

void GLWidget::setTopView()
{
    m_xRotTarget = 90;
    m_yRotTarget = m_yRot > 180 ? 360 : 0;
    beginViewAnimation();
}

void GLWidget::setFrontView()
{
    m_xRotTarget = 0;
    m_yRotTarget = m_yRot > 180 ? 360 : 0;
    beginViewAnimation();
}

void GLWidget::setLeftView()
{
    m_xRotTarget = 0;
    m_yRotTarget = m_yRot > 270 ? 450 : 90;
    beginViewAnimation();
}

int GLWidget::fps()
{
    return m_targetFps;
}

void GLWidget::toggleProjectionType() {
    m_perspective = !m_perspective;
    updateProjection();
    updateView();
}

void GLWidget::toggleRotationCube()
{
    m_rotationCube = !m_rotationCube;
}

void GLWidget::setIsometricView()
{
    m_perspective = false;
    updateProjection();
    m_xRotTarget = 35.264;
    m_yRotTarget = m_yRot > 180 ? 405 : 45;
    beginViewAnimation();
}

void GLWidget::beginViewAnimation() {
    m_xRotStored = m_xRot;
    m_yRotStored = m_yRot;
    m_animationFrame = 0;
    m_animateView = true;
}

void GLWidget::stopViewAnimation() {
    m_animateView = false;
}
QColor GLWidget::colorText() const
{
    return m_colorText;
}

void GLWidget::setColorText(const QColor &colorText)
{
    m_colorText = colorText;
}

QColor GLWidget::colorBackground() const
{
    return m_colorBackground;
}

void GLWidget::setColorBackground(const QColor &colorBackground)
{
    m_colorBackground = colorBackground;
}

void GLWidget::setFps(int fps)
{
    if (fps <= 0) return;
    m_targetFps = fps;
    setUpdatesEnabled(m_updatesEnabled);
}

QTime GLWidget::estimatedTime() const
{
    return m_estimatedTime;
}

void GLWidget::setEstimatedTime(const QTime &estimatedTime)
{
    m_estimatedTime = estimatedTime;
}

QTime GLWidget::spendTime() const
{
    return m_spendTime;
}

void GLWidget::setSpendTime(const QTime &spendTime)
{
    m_spendTime = spendTime;
}

void GLWidget::initializeGL()
{
    // Initialize functions
    initializeOpenGLFunctions();

    // Create shader program
    m_shaderProgram = new QOpenGLShaderProgram();

    if (m_shaderProgram) {
        m_shaderProgram->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/base_vertex.glsl");
        m_shaderProgram->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/base_fragment.glsl");
        m_shaderProgram->link();
        qDebug() << "shader program created";
    }

    qDebug() << "Initialize debug logger";
    QString glVersion = QString::fromUtf8((const char *)glGetString(GL_VERSION));
    QString glslVersion = QString::fromUtf8((const char *)glGetString(GL_SHADING_LANGUAGE_VERSION));
    QString glVendor = QString::fromUtf8((const char *)glGetString(GL_VENDOR));
    QString glRenderer = QString::fromUtf8((const char *)glGetString(GL_RENDERER));
    qDebug() << "OpenGL version: " << glVersion << "GLSL version: " << glslVersion << "Vendor: " << glVendor << "Renderer: " << glRenderer;
    QOpenGLDebugLogger *logger = new QOpenGLDebugLogger(this);
    logger->initialize();
}

void GLWidget::resizeGL(int width, int height)
{
    glViewport(0, 0, width, height);
    updateProjection();
    emit resized();
}

void GLWidget::updateProjection()
{
    // Reset projection
    m_projectionMatrix.setToIdentity();

    double aspectRatio = (double)width() / height();    
    double orthoSize = m_zoomDistance * tan((m_fov * 0.0174533) / 2.0);

    // perspective / orthographic projection
    if (m_perspective)
        m_projectionMatrix.perspective(m_fov, aspectRatio, m_near, m_far);
    else
        m_projectionMatrix.ortho(-orthoSize * aspectRatio, orthoSize * aspectRatio, -orthoSize, orthoSize, -m_far/2.0, m_far/2.0);
}

void GLWidget::updateView()
{
    // Set view matrix
    m_viewMatrix.setToIdentity();

    double angY = M_PI / 180 * m_yRot;
    double angX = M_PI / 180 * m_xRot;

    QVector3D eye(cos(angX) * sin(angY), sin(angX), cos(angX) * cos(angY));
    QVector3D up(fabs(m_xRot) == 90 ? -sin(angY + (m_xRot < 0 ? M_PI : 0)) : 0, cos(angX), fabs(m_xRot) == 90 ? -cos(angY + (m_xRot < 0 ? M_PI : 0)) : 0);

    m_cubeDrawer.updateEyePosition(eye);

    m_viewMatrix.lookAt(eye * m_zoomDistance, QVector3D(0,0,0), up.normalized());
    m_viewMatrix.rotate(-90, 1.0, 0.0, 0.0);
    m_viewMatrix.translate(-m_lookAt);
}

void GLWidget::drawText(QPainter &painter, QPoint &pos, QString text, int lineHeight, Qt::AlignmentFlag align)
{
    int x = pos.x();
    if (align == Qt::AlignRight) {
        pos.setX(x - painter.fontMetrics().horizontalAdvance(text));
    }
    painter.drawText(pos, text);
    // revert X, advance Y
    pos.setX(x);
    pos.setY(pos.y() + lineHeight);
}

void GLWidget::drawTexts(QPainter &painter, QPoint &pos, QStringList texts, int lineHeight)
{
    foreach (QString text, texts) {
        drawText(painter, pos, text, lineHeight);
    }
}


#ifdef GLES
void GLWidget::paintGL() {
#else

void GLWidget::paintEvent(QPaintEvent *pe) {
    Q_UNUSED(pe)
#endif
    QPainter painter(this);

    // Segment counter
    int vertices = 0;

    painter.beginNativePainting();

    // Clear viewport
    glClearColor(m_colorBackground.redF(), m_colorBackground.greenF(), m_colorBackground.blueF(), 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Shader drawable points
    glEnable(GL_PROGRAM_POINT_SIZE);

    // Update settings
    if (m_antialiasing) {
        if (m_msaa) glEnable(GL_MULTISAMPLE); else {
            glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
            glEnable(GL_LINE_SMOOTH);
            glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);
            glEnable(GL_POINT_SMOOTH);

            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            glEnable(GL_BLEND);
        }
    }
    if (m_zBuffer) glEnable(GL_DEPTH_TEST);

    if (m_shaderProgram) {
        // Draw 3d
        m_shaderProgram->bind();

        // Set modelview-projection matrix
        m_shaderProgram->setUniformValue("mvp_matrix", m_projectionMatrix * m_viewMatrix);
        m_shaderProgram->setUniformValue("mv_matrix", m_viewMatrix);

        // Update geometries in current opengl context
        foreach (ShaderDrawable *drawable, m_shaderDrawables)
            if (drawable->needsUpdateGeometry()) drawable->updateGeometry(m_shaderProgram);

        // Draw geometries
        foreach (ShaderDrawable *drawable, m_shaderDrawables) {
            drawable->draw(m_shaderProgram);
            if (drawable->visible()) vertices += drawable->getVertexCount();
        }

        m_shaderProgram->release();
    }

    if (m_rotationCube) {
        m_cubeDrawer.draw(QRect(0, height() - 100, 100, 100));

        // viewport was changed by cube drawer
        glViewport(0, 0, this->width(), this->height());
    }

    // Draw 2D
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_MULTISAMPLE);
    glDisable(GL_LINE_SMOOTH);
    glDisable(GL_BLEND);

    painter.endNativePainting();

    QPoint pos;

    QPen pen(m_colorText);
    painter.setPen(pen);

    // left side
    pos = QPoint(10, this->height() - 80);

    if (!qIsNaN(m_bottomSurfaceCursorPos.x())) {
        drawText(painter, pos, QString("Cursor: %1, %2").arg(m_bottomSurfaceCursorPos.x(), 0, 'f', 2).arg(m_bottomSurfaceCursorPos.y(), 0, 'f', 2), 15);
    } else {
        drawText(painter, pos, "Cursor: ??", 15);
    }
    drawText(painter, pos, QString("X: %1 ... %2").arg(m_xMin, 0, 'f', 3).arg(m_xMax, 0, 'f', 3), 15);
    drawText(painter, pos, QString("Y: %1 ... %2").arg(m_yMin, 0, 'f', 3).arg(m_yMax, 0, 'f', 3), 15);
    drawText(painter, pos, QString("Z: %1 ... %2").arg(m_zMin, 0, 'f', 3).arg(m_zMax, 0, 'f', 3), 15);
    drawText(painter, pos, QString("%1 / %2 / %3").arg(m_xSize, 0, 'f', 3).arg(m_ySize, 0, 'f', 3).arg(m_zSize, 0, 'f', 3), 15);

    QFontMetrics fm(painter.font());

    pos.setY(fm.height() + 10);

    drawText(painter, pos, m_parserState, 10);
    drawText(painter, pos, m_speedState, 10);
    drawText(painter, pos, m_pinState, 10);

    // right side
    pos = QPoint(this->width() - 10, this->height() - 60);

    drawText(painter, pos, m_spendTime.toString("hh:mm:ss") + " / " + m_estimatedTime.toString("hh:mm:ss"), 15, Qt::AlignRight);
    drawText(painter, pos, m_bufferState, 15, Qt::AlignRight);
    drawText(painter, pos, QString(tr("Vertices: %1")).arg(vertices), 15, Qt::AlignRight);
    drawText(painter, pos, QString("FPS: %1").arg(m_fps), 15, Qt::AlignRight);

    m_frames++;

#ifdef GLES
    update();
#endif
}

void GLWidget::mousePressEvent(QMouseEvent *event)
{
    m_lastPos = event->pos();
    m_xLastRot = m_xRot;
    m_yLastRot = m_yRot;
}

QPointF GLWidget::getClickPositionOnXYPlane(QVector2D mouseClickPosition, QMatrix4x4 projectionMatrix, QMatrix4x4 viewMatrix)
{
    // Invert the matrices
    QMatrix4x4 invertedProjection = projectionMatrix.inverted();
    QMatrix4x4 invertedView = viewMatrix.inverted();

    // Convert 2D mouse position to 3D position with Z = -1 (near plane)
    QVector3D nearPlanePosition(mouseClickPosition, -1.0f);

    // Unproject the 3D position on the near plane to the world space
    QVector3D nearPlaneWorldPosition = invertedProjection.map(nearPlanePosition);
    nearPlaneWorldPosition = invertedView.map(nearPlaneWorldPosition);

    // Convert 2D mouse position to 3D position with Z = 1 (far plane)
    QVector3D farPlanePosition(mouseClickPosition, 1.0f);

    // Unproject the 3D position on the far plane to the world space
    QVector3D farPlaneWorldPosition = invertedProjection.map(farPlanePosition);
    farPlaneWorldPosition = invertedView.map(farPlaneWorldPosition);

    // Calculate the direction from the near plane to the far plane
    QVector3D direction = farPlaneWorldPosition - nearPlaneWorldPosition;

    // If the direction is parallel to the XY plane, then the click is not on the XY plane
    if (direction.z() == 0)
    {
        return QPointF(NAN, NAN);
    }

    // Calculate the intersection of the line with the XY plane (Z = 0)
    float t = -nearPlaneWorldPosition.z() / direction.z();
    QVector3D intersection = nearPlaneWorldPosition + direction * t;

    // Limit XY range
    if (abs(intersection.x()) > 3000 || abs(intersection.y()) > 3000) {
        return QPointF(NAN, NAN);
    }

    return intersection.toPointF();
}

void GLWidget::mouseMoveEvent(QMouseEvent *event)
{
    QPoint pos = event->pos();
    QVector2D normalizedPos(
        pos.x() / (width()  * 0.5f) - 1.0f,
        -(pos.y() / (height() * 0.5f) - 1.0f)
    );

    m_bottomSurfaceCursorPos = getClickPositionOnXYPlane(normalizedPos, m_projectionMatrix, m_viewMatrix);
    if (!qIsNaN(m_bottomSurfaceCursorPos.x())) {
        emit cursorPosChanged(m_bottomSurfaceCursorPos);
    }

    if ((event->buttons() & Qt::MiddleButton && !(event->modifiers() & Qt::ShiftModifier)) 
        || (event->buttons() & Qt::LeftButton && !(event->modifiers() & Qt::ShiftModifier))) {

        stopViewAnimation();

        m_yRot = normalizeAngle(m_yLastRot - (pos.x() - m_lastPos.x()) * 0.5);
        m_xRot = m_xLastRot + (pos.y() - m_lastPos.y()) * 0.5;

        if (m_xRot < -90) m_xRot = -90;
        if (m_xRot > 90) m_xRot = 90;

        updateView();
        emit rotationChanged();
    }

    if ((event->buttons() & Qt::MiddleButton && event->modifiers() & Qt::ShiftModifier) 
        || event->buttons() & Qt::RightButton
        || (event->buttons() & Qt::LeftButton && (event->modifiers() & Qt::ShiftModifier)))
    {
        // Get world to clip
        QMatrix4x4 mvp(m_projectionMatrix * m_viewMatrix);
        // Get clip to world
        QMatrix4x4 mvpi(mvp.inverted());

        QVector4D centerVector(mvp * QVector4D(m_lookAt.x(),m_lookAt.y(),m_lookAt.z(), 1.0));

        // Get last mouse XY in clip
        QVector4D lastMouseInWorld(
            (m_lastPos.x() / (double)width()) * 2.0 - 1.0,
            -((m_lastPos.y() / (double)height()) * 2.0 - 1.0),
            0,
            1.0
        );
        // Project last mouse pos to world
        lastMouseInWorld = mvpi * lastMouseInWorld * centerVector.w();

        // Get current mouse XY in clip
        QVector4D currentMouseInWorld(
            (pos.x() / (double)width()) * 2.0 - 1.0,
            -((pos.y() / (double)height()) * 2.0 - 1.0),
            0,
            1.0
        );
        // Project current mouse pos to world
        currentMouseInWorld = mvpi * currentMouseInWorld * centerVector.w();

        //currentMouseInWorld /= currentMouseInWorld.w();

        // Get difference
        QVector4D difference = currentMouseInWorld - lastMouseInWorld;

        // Subtract difference from center point
        m_lookAt -= QVector3D(difference.x(), difference.y(), difference.z());

        m_lastPos = pos;

        updateView();
    }
}

void GLWidget::wheelEvent(QWheelEvent *we)
{
    int delta = we->angleDelta().y();
    if (m_zoomDistance > MIN_ZOOM && delta < 0)
        m_zoomDistance /= ZOOMSTEP;
    else if (delta > 0)
        m_zoomDistance *= ZOOMSTEP;

    if (!m_perspective) updateProjection();
    else updateView();
}

void GLWidget::timerEvent(QTimerEvent *te)
{
    if (te->timerId() == m_timerPaint.timerId()) {
        if (m_animateView) viewAnimation();
#ifndef GLES
        update();
#endif
    } else {
#ifdef GLES
        QOpenGLWidget::timerEvent(te);
#else
        QGLWidget::timerEvent(te);
#endif
    }
}

double GLWidget::normalizeAngle(double angle)
{
    while (angle < 0) angle += 360;
    while (angle > 360) angle -= 360;

    return angle;
}
