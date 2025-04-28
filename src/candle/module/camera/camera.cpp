// This file is a part of "G-Pilot (formerly Candle)" application.
// Copyright 2015-2021 Hayrullin Denis Ravilevich
// Copyright 2024 BTS

#include "camera.h"
#include "ui_camera.h"
#include <QMediaDevices>
#include <QMessageBox>
#include <QtWidgets>

Camera::Camera(QWidget* parent) : QVideoWidget(parent), m_frameProcessor(this)
{
    QActionGroup *videoDevicesGroup = new QActionGroup(this);
    videoDevicesGroup->setExclusive(true);

    connect(videoDevicesGroup, &QActionGroup::triggered, this, &Camera::updateCameraDevice);

    // popup menu to select camera
    this->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(this, &QWidget::customContextMenuRequested, [this](const QPoint &pos){
        m_menu.exec(this->mapToGlobal(pos));
    });

    m_resizeTimer.setInterval(500);
    m_resizeTimer.setSingleShot(true);
    connect(&m_resizeTimer, &QTimer::timeout, this, [this](){
        findBestResolution(this->width(), this->height());
    });

    init();
}

Camera::~Camera()
{
    m_resizeTimer.stop();
}

void Camera::setCamera(const QCameraDevice &cameraDevice)
{
    if (!m_camera.isNull()) {
        m_camera->stop();
    }

    qDebug() << "Camera device:" << cameraDevice.description();

    m_camera.reset(new QCamera(cameraDevice));
    m_captureSession.setCamera(m_camera.data());

        // connect(m_camera.data(), &QCamera::activeChanged, this, &Camera::updateCameraActive);
    // connect(m_camera.data(), &QCamera::errorOccurred, this, &Camera::displayCameraError);

    // QVideoSink *videoSink = new QVideoSink();

    // m_captureSession.setVideoOutput(this);
    // m_captureSession.setVideoSink(videoSink);

    // m_frameProcessor.setVideoSink(
    //     m_captureSession.videoSink(),
    //     this->videoSink()
    // );

    updateCameraActive(m_camera->isActive());

    if (isVisible()) {
        m_camera->start();
    }

    findBestResolution(this->width(), this->height());
}

void Camera::hideEvent(QHideEvent *event)
{
    QVideoWidget::hideEvent(event);

    stopCamera();
}

void Camera::showEvent(QShowEvent *event)
{
    QVideoWidget::showEvent(event);

    startCamera();
}

// void Camera::paintEvent(QPaintEvent *event)
// {
//     QVideoWidget::paintEvent(event);
//     if (!m_camera.isNull()) {
//         // camera is available
//         return;
//     }

//     QPainter painter(this);
//     painter.setPen(Qt::white);
//     painter.setFont(QFont("Arial", 30));
//     QTextOption textOption;
//     textOption.setAlignment(Qt::AlignCenter | Qt::AlignVCenter);
//     textOption.setWrapMode(QTextOption::WordWrap);
//     painter.fillRect(0, 0, 100, 100, Qt::red);
//     painter.drawText(
//         QRect(0, 0, width(), height()),
//         "No camera found",
//         textOption
//     );
//     painter.beginNativePainting();
//     painter.fillRect(0, 0, 100, 100, Qt::red);
//     painter.endNativePainting();
// }

void Camera::startCamera()
{
    if (m_camera.isNull() || m_camera->isActive() || !m_camera->isAvailable()) {
        return;
    }

    m_camera->start();
}

void Camera::stopCamera()
{
    if (m_camera.isNull() || !m_camera->isActive()) {
        return;
    }

    m_camera->stop();
}

void Camera::displayCameraError()
{
    if (m_camera->error() != QCamera::NoError) {
        QMessageBox::warning(this, tr("Camera Error"), m_camera->errorString());
    }
}

void Camera::updateCameraDevice(QAction *action)
{
    if (action->data().isNull()) {
        disableCamera();

        return;
    }

    setCamera(qvariant_cast<QCameraDevice>(action->data()));
}

void Camera::updateCameraActive(bool active)
{
    qDebug() << active;
}

void Camera::resizeEvent(QResizeEvent *event)
{
    if (m_resizeTimer.isActive()) {
        m_resizeTimer.stop();
    }
    if (!m_camera.isNull() && m_camera->isActive()) {
        m_resizeTimer.start();
    }

    QVideoWidget::resizeEvent(event);
}

void Camera::showCameraDisabled()
{
    m_captureSession.setCamera(nullptr);
    QVideoFrame frame(QImage(":/images/cube_texture.png"));
    m_videoSink.setVideoFrame(frame);
}

void Camera::disableCamera()
{
    stopCamera();
    showCameraDisabled();
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

    QCameraDevice defaultCamera = QMediaDevices::defaultVideoInput();
    if (!defaultCamera.isNull()) {
        setCamera(QMediaDevices::defaultVideoInput());
    }

    m_captureSession.setVideoOutput(this);
    m_captureSession.setVideoSink(&m_videoSink);

    m_frameProcessor.setVideoSink(
        m_captureSession.videoSink(),
        this->videoSink()
    );

    if (m_camera.isNull()) {
        showCameraDisabled();
    }
}

void Camera::updateCameras()
{
    m_menu.clear();
    m_camerasCount = 0;

    QAction *videoDeviceAction = new QAction("Disable", videoDevicesGroup);
    videoDeviceAction->setCheckable(true);
    videoDeviceAction->setData(QVariant());

    m_menu.addAction(videoDeviceAction);

    const QList<QCameraDevice> availableCameras = QMediaDevices::videoInputs();
    for (const QCameraDevice &cameraDevice : availableCameras) {
        QAction *videoDeviceAction = new QAction(cameraDevice.description(), videoDevicesGroup);
        videoDeviceAction->setCheckable(true);
        videoDeviceAction->setData(QVariant::fromValue(cameraDevice));
        if (cameraDevice == QMediaDevices::defaultVideoInput()) {
            videoDeviceAction->setChecked(true);
        }

        m_menu.addAction(videoDeviceAction);
        m_camerasCount++;
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
        qDebug() << "Setting camera format to" << bestFormat.resolution()
                 << (float) bestFormat.resolution().width() / (float) bestFormat.resolution().height()
                 << bestScale;
        m_camera->setCameraFormat(bestFormat);
    }

    if (bestFormat.isNull()) {
        qDebug() << "Matching camera format not found!";
    }
}
