// This file is a part of "G-Pilot (formerly Candle)" application.
// Copyright 2015-2021 Hayrullin Denis Ravilevich
// Copyright 2024 BTS

#include "cameraframeprocessor.h"
#include <QPainter>
#include <QDebug>

CameraFrameProcessor::CameraFrameProcessor(QObject *parent)
    : QObject{parent}
{}

void CameraFrameProcessor::setVideoSink(QVideoSink *inputSink, QVideoSink *outputSink)
{
    if (m_inputSink != inputSink)
    {
        m_inputSink = inputSink;
        connect(m_inputSink, &QVideoSink::videoFrameChanged, this, &CameraFrameProcessor::processFrame);
    }

    m_outputSink = outputSink;
}

void CameraFrameProcessor::processFrame(const QVideoFrame &frame)
{
    QImage image = frame.toImage();

    QPainter painter(&image);
    if (!painter.isActive()) {
        return;
    }

    QFont font = painter.font();
    font.setPixelSize(100);
    font.setBold(true);
    painter.setFont(font);

    painter.drawText(50, 50, "Example text");

    QVideoFrame frameCopy(image);

    m_outputSink->setVideoFrame(frameCopy);
}
