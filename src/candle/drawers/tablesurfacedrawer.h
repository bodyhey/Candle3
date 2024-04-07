// This file is a part of "G-Pilot (formerly Candle)" application.
// Copyright 2015-2021 Hayrullin Denis Ravilevich
// Copyright 2024 BTS

#ifndef TABLESURFACEDRAWER_H
#define TABLESURFACEDRAWER_H

#include "shaderdrawable.h"

class TableSurfaceDrawer : public ShaderDrawable
{
    public:
        TableSurfaceDrawer();

    protected:
        bool updateData();
};

#endif // TABLESURFACEDRAWER_H
