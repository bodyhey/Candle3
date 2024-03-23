// This file is a part of "G-Pilot (formerly Candle)" application.
// Copyright 2015-2021 Hayrullin Denis Ravilevich
// Copyright 2024 BTS

#ifndef CONSOLE_H
#define CONSOLE_H

#include <QWidget>

namespace Ui {
class partMainConsole;
}

class partMainConsole : public QWidget
{
    Q_OBJECT

    public:
        explicit partMainConsole(QWidget *parent = nullptr);
        ~partMainConsole();
        void append(QString text);
        void clear();

    signals:
        void newCommand(QString command);
        void consoleCleared();

    private:
        Ui::partMainConsole *ui;
        void send();

    private slots:
        void onClearClicked();
        void onSendClicked();
};

#endif // CONSOLE_H
