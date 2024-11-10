// This file is a part of "G-Pilot (formerly Candle)" application.
// Copyright 2015-2021 Hayrullin Denis Ravilevich
// Copyright 2024 BTS

#ifndef CAMERAFRAMEPROCESSOR_H
#define CAMERAFRAMEPROCESSOR_H

#include <QObject>
#include <QVideoSink>
#include <QVideoFrame>

class CameraFrameProcessor : public QObject
{
 Q_OBJECT
    Q_PROPERTY(QVideoSink *videoSink READ videoSink WRITE setVideoSink NOTIFY videoSinkChanged)

    public:
        CameraFrameProcessor(QObject *parent);

        QVideoSink *videoSink() const;
        void setVideoSink(QVideoSink *sink);

    signals:
        void videoSinkChanged();

    private slots:
        void processFrame(const QVideoFrame &frame);

    private:
        QVideoSink *m_videoSink;
};

#endif // CAMERAFRAMEPROCESSOR_H
