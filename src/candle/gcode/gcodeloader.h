// This file is a part of "G-Pilot (formerly Candle)" application.
// Copyright 2015-2021 Hayrullin Denis Ravilevich
// Copyright 2024 BTS

#ifndef GCODELOADER_H
#define GCODELOADER_H

#include "gcode.h"
#include "config/module/configurationparser.h"
#include "parser/gcodeviewparser.h"
#include <QFile>
#include <QThread>

struct GCodeLoaderData {
    GCode *gcode;
    GCodeViewParser *viewParser;
};

class AbstractGCodeLoader : public QObject
{
    Q_OBJECT

    public:
        explicit AbstractGCodeLoader(QObject *parent = nullptr) : QObject(parent) {}
        virtual void loadFromFile(const QString &fileName, ConfigurationParser &configuration) = 0;
        virtual void loadFromLines(const QStringList &lines, ConfigurationParser &configuration) = 0;
        virtual void cancel() = 0;

    signals:
        void progress(int value);
        void started();
        void finished(GCodeLoaderData *result);
        void cancelled();
};

class GCodeLoader : public AbstractGCodeLoader
{
    public:
        explicit GCodeLoader(QObject *parent = nullptr);
        void loadFromFile(const QString &fileName, ConfigurationParser &configuration) override;
        void loadFromLines(const QStringList &lines, ConfigurationParser &configuration) override;
        void cancel() override;

    private:
    //    static const int PROGRESSSTEP = 1000;
        bool m_cancel;
        void loadFromFileObject(QFile &stream, int size, ConfigurationParser &configuration);
};

#endif // GCODELOADER_H
