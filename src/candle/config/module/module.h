// This file is a part of "G-Pilot (formerly Candle)" application.
// Copyright 2015-2021 Hayrullin Denis Ravilevich
// Copyright 2024 BTS

#ifndef CONFIGURATION_MODULE_H
#define CONFIGURATION_MODULE_H

class ConfigurationModule
{
    public:
        virtual void load() = 0;
        virtual void save() = 0;
};

#endif // CONFIGURATION_MODULE_H
