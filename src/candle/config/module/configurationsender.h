// This file is a part of "G-Pilot (formerly Candle)" application.
// Copyright 2015-2021 Hayrullin Denis Ravilevich
// Copyright 2024 BTS

#ifndef CONFIGURATION_SENDER_H
#define CONFIGURATION_SENDER_H

#include <QObject>
#include "module.h"

class ConfigurationSender : public ConfigurationModule
{
    Q_OBJECT
    Q_PROPERTY(QString test MEMBER m_test)
    Q_PROPERTY(int test2 MEMBER m_test2)

    public:
        explicit ConfigurationSender(QObject *parent = nullptr);
        ConfigurationSender& operator=(const ConfigurationSender&) { return *this; }
        QVariant customGet(QString) override;
        void customSet(QString, QVariant) override;
        QString getName() override { return "sender"; }

    private:
        QString m_test = "ahugu";
        int m_test2 = 421412111;
};

#endif // CONFIGURATION_SENDER_H
