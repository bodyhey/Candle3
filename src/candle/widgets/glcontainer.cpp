#include "glcontainer.h"
#include <QTime>

GLContainer::GLContainer(QWidget *parent) : QWidget(parent)
{
    m_glWidget = new GLWidget();

    qDebug() << width() << height();

    // createWindowContainer(
    //     m_glWidget
    // );

}

void GLContainer::addDrawable(ShaderDrawable *drawable)
{
    m_glWidget->addDrawable(drawable);
}

GLContainer& GLContainer::operator<<(ShaderDrawable *drawable)
{
    m_glWidget->addDrawable(drawable);

    return *this;
}

void GLContainer::fitDrawable(ShaderDrawable *drawable)
{
}

void GLContainer::updateExtremes(ShaderDrawable *drawable)
{
}

bool GLContainer::antialiasing() const
{
}

void GLContainer::setAntialiasing(bool antialiasing)
{
}


QString GLContainer::pinState() const
{
}

void GLContainer::setPinState(const QString &pinState)
{
}

void GLContainer::showEvent(QShowEvent *event)
{
    m_glWidget->show();

    QWidget::showEvent(event);
}

void GLContainer::resizeEvent(QResizeEvent *event)
{
    m_glWidget->setWidth(event->size().width());
    m_glWidget->setHeight(event->size().height());

    QWidget::resizeEvent(event);
}

QString GLContainer::speedState() const
{
}

void GLContainer::setSpeedState(const QString &additionalStatus)
{
}

bool GLContainer::vsync() const
{
}

void GLContainer::setVsync(bool vsync)
{
}

bool GLContainer::msaa() const
{
}

void GLContainer::setMsaa(bool msaa)
{
}

bool GLContainer::updatesEnabled() const
{
}

void GLContainer::setUpdatesEnabled(bool updatesEnabled)
{
}

bool GLContainer::zBuffer() const
{
}

void GLContainer::setZBuffer(bool zBuffer)
{
}

double GLContainer::fov() {
}

void GLContainer::setFov(double fov) {
}

double GLContainer::nearPlane() {
}

void GLContainer::setNearPlane(double plane) {
}

double GLContainer::farPlane() {
}

void GLContainer::setFarPlane(double plane) {
}

QString GLContainer::bufferState() const
{
}

void GLContainer::setBufferState(const QString &bufferState)
{
}

QString GLContainer::parserState() const
{
}

void GLContainer::setParserState(const QString &parserState)
{
}


double GLContainer::lineWidth() const
{
}

void GLContainer::setLineWidth(double lineWidth)
{
}

void GLContainer::setTopView()
{
}

void GLContainer::setBottomView()
{
}

void GLContainer::setFrontView()
{
}

void GLContainer::setBackView()
{
}

void GLContainer::setLeftView()
{
}

void GLContainer::setRightView()
{
}

int GLContainer::fps()
{
}

void GLContainer::toggleProjectionType() {
}

void GLContainer::toggleRotationCube()
{
}

void GLContainer::setIsometricView()
{
}

QColor GLContainer::colorText() const
{
}

void GLContainer::setColorText(const QColor &colorText)
{
}

QColor GLContainer::colorBackground() const
{
}

void GLContainer::setColorBackground(const QColor &colorBackground)
{
}

void GLContainer::setFps(int fps)
{
}

QTime GLContainer::estimatedTime() const
{
}

void GLContainer::setEstimatedTime(const QTime &estimatedTime)
{
}

QTime GLContainer::spendTime() const
{
}

void GLContainer::setSpendTime(const QTime &spendTime)
{
}

