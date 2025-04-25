// This file is a part of "G-Pilot (formerly Candle)" application.
// Copyright 2015-2021 Hayrullin Denis Ravilevich
// Copyright 2024 BTS

#ifndef CAMERA_H
#define CAMERA_H

#include <QMediaDevices>
#include <QWidget>
#include <QCamera>
#include <QMediaCaptureSession>
#include <QMediaRecorder>
#include <QScopedPointer>
#include <QVideoWidget>
#include <QVideoSink>
#include <QMenu>
#include <QTimer>
#include "cameraframeprocessor.h"

QT_BEGIN_NAMESPACE
namespace Ui { class Camera; }
QT_END_NAMESPACE

class Camera : public QVideoWidget
{
    Q_OBJECT

public:
    Camera(QWidget* parent = nullptr);
    ~Camera() override;

private slots:
    void setCamera(const QCameraDevice &cameraDevice);
    void startCamera();
    void stopCamera();
    void displayCameraError();
    void updateCameraDevice(QAction *action);
    void updateCameraActive(bool active);

protected:
    void resizeEvent(QResizeEvent *event) override;
    void hideEvent(QHideEvent *event) override;
    void showEvent(QShowEvent *event) override;

private:
    Ui::Camera *ui;

    QActionGroup *videoDevicesGroup = nullptr;
    QMediaDevices m_devices;
    QScopedPointer<QCamera> m_camera;
    QMediaCaptureSession m_captureSession;
    QTimer m_resizeTimer;

    QString m_videoContainerFormat;
    bool m_applicationExiting = false;
    QMenu m_menu;
    CameraFrameProcessor m_frameProcessor;

    void init();
    void updateCameras();
    void findBestResolution(int, int);
};

#endif
