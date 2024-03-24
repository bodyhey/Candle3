// This file is a part of "G-Pilot (formerly Candle)" application.
// Copyright 2015-2021 Hayrullin Denis Ravilevich
// Copyright 2024 BTS

#ifndef CONSOLE_H
#define CONSOLE_H

#include <QWidget>
#include "../../config/module/configurationconsole.h"

namespace Ui {
class partMainConsole;
}

class partMainConsole : public QWidget
{
    Q_OBJECT

    public:
        explicit partMainConsole(QWidget *parent, const ConfigurationConsole &configurationConsole);
        ~partMainConsole();
        int append(QString text);
        void appendResponse(int consoleIndex, QString command, QString response);
        void clear();

    signals:
        void newCommand(QString command);
        void consoleCleared();

    private:
        Ui::partMainConsole *ui;
        const ConfigurationConsole &m_configurationConsole;
        void send();
        bool isScrolledToEnd();
        void scrollToEnd();

        int m_commandIndex = 0; // won't be cleared even if the console is cleared
        // int m_minIndex = 0;
        // int m_maxIndex = 0;


        void applyDarkBackgroundMode();

        private slots:
        void onClearClicked();
        void onSendClicked();
};

#endif // CONSOLE_H
