// This file is a part of "G-Pilot (formerly Candle)" application.
// Copyright 2015-2021 Hayrullin Denis Ravilevich
// Copyright 2024 BTS

#ifndef CAMERAFRAMEPROCESSOR_H
#define CAMERAFRAMEPROCESSOR_H

#include <QVideoSink>
#include <QVideoFrame>

class CameraFrameProcessor : public QObject
{
    Q_OBJECT

    public:
        CameraFrameProcessor(QObject *parent);

        void setVideoSink(QVideoSink *inputSink, QVideoSink *outputSink);

    signals:
        void videoSinkChanged();

    public slots:
        void processFrame(const QVideoFrame &frame);

    private:
        QVideoSink *m_inputSink;
        QVideoSink *m_outputSink;
};

#endif // CAMERAFRAMEPROCESSOR_H
