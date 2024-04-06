// This file is a part of "G-Pilot (formerly Candle)" application.
// Copyright 2015-2021 Hayrullin Denis Ravilevich
// Copyright 2024 BTS

#ifndef TEMPCONNECTIONTOUIPROXY_H
#define TEMPCONNECTIONTOUIPROXY_H

#include "sliderbox.h"
#include "globals.h"
#include "drawers/machineboundsdrawer.h"
#include "scripting/scripting.h"
#include "tables/gcodetablemodel.h"
#include "form_partial/main/console.h"
#include <QMessageBox>

class TempConnectionToUiProxy {
    public:
        //virtual void completeTransfer() = 0;
        //virtual void jogContinuous() = 0;
        //virtual void grblReset() = 0;
        virtual MachineBoundsDrawer &machineBoundsDrawer() = 0;
        virtual GCodeTableModel &programModel() = 0;
        virtual GCodeTableModel &probeModel() = 0;
        virtual GCodeTableModel &currentModel() = 0;
        virtual bool &absoluteCoordinates() = 0;
        virtual partMainConsole &partConsole() = 0;
};

#endif // TEMPCONNECTIONTOUIPROXY_H
