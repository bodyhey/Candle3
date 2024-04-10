// This file is a part of "G-Pilot (formerly Candle)" application.
// Copyright 2015-2021 Hayrullin Denis Ravilevich
// Copyright 2024 BTS

#include "configurationparser.h"

const QMap<QString,QVariant> DEFAULTS = {};

ConfigurationParser::ConfigurationParser(QObject *parent) : ConfigurationModule(parent, DEFAULTS) {}
