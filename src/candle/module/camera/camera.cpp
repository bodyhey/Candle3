// This file is a part of "G-Pilot (formerly Candle)" application.
// Copyright 2015-2021 Hayrullin Denis Ravilevich
// Copyright 2024 BTS

#include "camera.h"
#include "ui_camera.h"
#include <QMediaDevices>
#include <QVideoSink>
#include <QMessageBox>
#include <QtWidgets>

Camera::Camera(QWidget* parent) : QVideoWidget(parent), ui(new Ui::Camera), m_frameProcessor(this)
{
    ui->setupUi(this);

    QActionGroup *videoDevicesGroup = new QActionGroup(this);
    videoDevicesGroup->setExclusive(true);

    connect(videoDevicesGroup, &QActionGroup::triggered, this, &Camera::updateCameraDevice);

    // popup menu to select camera
    this->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(this, &QWidget::customContextMenuRequested, [=](const QPoint &pos){
        m_menu.exec(this->mapToGlobal(pos));
    });

    init();
}

void Camera::setCamera(const QCameraDevice &cameraDevice)
{
    if (!m_camera.isNull()) {
        m_camera->stop();
    }

    qDebug() << cameraDevice.description();

    m_camera.reset(new QCamera(cameraDevice));
    m_captureSession.setCamera(m_camera.data());

        // connect(m_camera.data(), &QCamera::activeChanged, this, &Camera::updateCameraActive);
    // connect(m_camera.data(), &QCamera::errorOccurred, this, &Camera::displayCameraError);

    QVideoSink *videoSink = new QVideoSink();

    m_captureSession.setVideoOutput(this);
    m_captureSession.setVideoSink(videoSink);

    m_frameProcessor.setVideoSink(
        m_captureSession.videoSink(),
        this->videoSink()
    );

    updateCameraActive(m_camera->isActive());

    if (isVisible()) {
        m_camera->start();
    }

    findBestResolution(this->width(), this->height());
}

void Camera::hideEvent(QHideEvent *event)
{
    QWidget::hideEvent(event);

    stopCamera();
}

void Camera::showEvent(QShowEvent *event)
{
    QWidget::showEvent(event);

    startCamera();
}

void Camera::startCamera()
{
    if (m_camera->isActive() || !m_camera->isAvailable())
        return;

    m_camera->start();
}

void Camera::stopCamera()
{
    if (!m_camera->isActive())
        return

    m_camera->stop();
}

void Camera::displayCameraError()
{
    if (m_camera->error() != QCamera::NoError)
        QMessageBox::warning(this, tr("Camera Error"), m_camera->errorString());
}

void Camera::updateCameraDevice(QAction *action)
{
    setCamera(qvariant_cast<QCameraDevice>(action->data()));
}

void Camera::updateCameraActive(bool active)
{
    qDebug() << active;
}

void Camera::resizeEvent(QResizeEvent *event)
{
    findBestResolution(event->size().width(), event->size().height());

    QVideoWidget::resizeEvent(event);
}

void Camera::init()
{
    #if QT_CONFIG(permissions)
        // camera
        QCameraPermission cameraPermission;
        switch (qApp->checkPermission(cameraPermission)) {
            case Qt::PermissionStatus::Undetermined:
                qApp->requestPermission(cameraPermission, this, &Camera::init);
                return;
            case Qt::PermissionStatus::Denied:
                qWarning("Camera permission is not granted!");
                return;
            case Qt::PermissionStatus::Granted:
                break;
        }
    #endif

    videoDevicesGroup = new QActionGroup(this);
    videoDevicesGroup->setExclusive(true);
    updateCameras();
    connect(&m_devices, &QMediaDevices::videoInputsChanged, this, &Camera::updateCameras);

    connect(videoDevicesGroup, &QActionGroup::triggered, this, &Camera::updateCameraDevice);

    setCamera(QMediaDevices::defaultVideoInput());
}

void Camera::updateCameras()
{
    m_menu.clear();

    const QList<QCameraDevice> availableCameras = QMediaDevices::videoInputs();
    for (const QCameraDevice &cameraDevice : availableCameras) {
        QAction *videoDeviceAction = new QAction(cameraDevice.description(), videoDevicesGroup);
        videoDeviceAction->setCheckable(true);
        videoDeviceAction->setData(QVariant::fromValue(cameraDevice));
        if (cameraDevice == QMediaDevices::defaultVideoInput())
            videoDeviceAction->setChecked(true);

        m_menu.addAction(videoDeviceAction);
    }
}

void Camera::findBestResolution(int w, int h)
{
    QCameraFormat bestFormat;
    float bestScale = 1000.0;

    for (const QCameraFormat &format : m_camera->cameraDevice().videoFormats()) {
        if (format.pixelFormat() != QVideoFrameFormat::Format_NV12 && format.pixelFormat() != QVideoFrameFormat::Format_NV21) {
            continue;
        }

        QSize resolution = format.resolution();
        float scaleX = resolution.width() / (float)w;
        float scaleY = resolution.height() / (float)h;
        float maxScale = std::max(scaleX, scaleY);

        if (maxScale >= 1 && maxScale < bestScale) {
            bestScale = maxScale;
            bestFormat = format;
        }
    }

    if (!bestFormat.isNull() && m_camera->cameraFormat() != bestFormat) {
        qDebug() << "Setting camera format to" << bestFormat.resolution() << bestScale;
        m_camera->setCameraFormat(bestFormat);
    }

    if (bestFormat.isNull()) {
        qDebug() << "Matching camera format not found!";
    }
}
