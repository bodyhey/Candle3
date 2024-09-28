// This file is a part of "G-Pilot (formerly Candle)" application.
// Copyright 2015-2021 Hayrullin Denis Ravilevich
// Copyright 2024 BTS

#ifndef GCODE_H
#define GCODE_H

#include <QList>
#include <QTextStream>
#include <QObject>

struct GCodeLine
{
    QString command;
    QString comment;
    QString response;
    int lineNumber;
    int state;
    QStringList args;

    enum States { InQueue, Sent, Processed, Skipped };
};

class GCode : public QObject, public QList<GCodeLine>
{
    Q_OBJECT

    static const int PROGRESSSTEP = 1000;

    public:
        GCode();
        bool isModified();
        void saveToFile(const QString &fileName);
        void loadFromFile(const QString &fileName);
        //void loadFromString(const QString &data);
        //void loadFromList(QList<std::string> data);
        void loadFromFileObject(QFile &stream, int size);
        void cancelLoading();

    private:
        bool m_cancelling;
        void clear();

    signals:
        void progress(int value);
        void loadingStarted();
        void loadingFinished();
        void loadingCancelled();
};

#endif // GCODE_H
