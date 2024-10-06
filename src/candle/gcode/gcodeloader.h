// This file is a part of "G-Pilot (formerly Candle)" application.
// Copyright 2015-2021 Hayrullin Denis Ravilevich
// Copyright 2024 BTS

#ifndef GCODELOADER_H
#define GCODELOADER_H

#include "gcode.h"
#include <QFile>

class GCodeLoader : public QObject
{
    Q_OBJECT

    public:
        GCodeLoader();
        void loadFromFile(const QString &fileName);
        void cancelLoading();

    private:
        static const int PROGRESSSTEP = 1000;
        bool m_cancelling;
        void loadFromFileObject(QFile &stream, int size);

    signals:
        void progress(int value);
        void loadingStarted();
        void loadingFinished(GCode &gcode);
        void loadingCancelled();
};

#endif // GCODELOADER_H
