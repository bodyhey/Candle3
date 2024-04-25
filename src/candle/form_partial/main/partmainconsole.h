// This file is a part of "G-Pilot (formerly Candle)" application.
// Copyright 2015-2021 Hayrullin Denis Ravilevich
// Copyright 2024 BTS

#ifndef PARTMAINCONSOLE_H
#define PARTMAINCONSOLE_H

#include <QWidget>
#include <QTextBlock>
#include "../../config/module/configurationconsole.h"
#include "globals.h"

namespace Ui {
class partMainConsole;
}

class partMainConsole : public QWidget
{
    Q_OBJECT

    public:
        explicit partMainConsole(QWidget *parent);
        void initialize(ConfigurationConsole &configurationConsole);
        ~partMainConsole();
        int append(QString text);
        int append(CommandAttributes commandAttributes);
        void appendResponse(int consoleIndex, QString command, QString response);
        void appendResponse(CommandAttributes commandAttributes);
        void clear();

    signals:
        void newCommand(QString command);
        void consoleCleared();

    private:
        Ui::partMainConsole *ui;
        ConfigurationConsole *m_configurationConsole;
        void send();
        bool isScrolledToEnd();
        void scrollToEnd();
        QTextBlock lastBlock();

        class BlockData : public QTextBlockUserData
        {
            public:
                BlockData(int consoleIndex, int commandIndex) : m_consoleIndex(consoleIndex), m_commandIndex(commandIndex) {}
                int consoleIndex() const { return m_consoleIndex; }
                int commandIndex() const { return m_commandIndex; }

            private:
                int m_consoleIndex;
                int m_commandIndex;
        };

        void applyDarkBackgroundMode();

    private slots:
        void onClearClicked();
        void onSendClicked();
};

#endif // PARTMAINCONSOLE_H
