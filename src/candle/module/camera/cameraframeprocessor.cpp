// This file is a part of "G-Pilot (formerly Candle)" application.
// Copyright 2015-2021 Hayrullin Denis Ravilevich
// Copyright 2024 BTS

#include "cameraframeprocessor.h"
#include <QPainter>

CameraFrameProcessor::CameraFrameProcessor(QObject *parent)
    : QObject{parent}
{}

QVideoSink *CameraFrameProcessor::videoSink() const
{
    return m_videoSink;
}

void CameraFrameProcessor::setVideoSink(QVideoSink *sink)
{
    if (m_videoSink != sink)
    {
        m_videoSink = sink;
        connect(m_videoSink, &QVideoSink::videoFrameChanged, this, &CameraFrameProcessor::processFrame);
    }
}

void CameraFrameProcessor::processFrame(const QVideoFrame &frame)
{
//    QPaintEngine *paintEngine = QPaintEngine::paintEngine(frame);
    // frame.
    // QPainter painter(frame);
    // frame.
}
