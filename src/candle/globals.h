#ifndef GLOBALS_H
#define GLOBALS_H

#include <QObject>
#include <QString>

#define GRBL_LIVE_SOFT_RESET 0x18
#define GRBL_LIVE_STATUS_REPORT '?'
#define GRBL_LIVE_CYCLE_START '~'
#define GRBL_LIVE_FEED_HOLD '!'

#define GRBL_LIVE_RAPID_FULL_RATE 0x95
#define GRBL_LIVE_RAPID_HALF_RATE 0x96
#define GRBL_LIVE_RAPID_QUARTER_RATE 0x97

#define GRBL_LIVE_FEED_FULL_RATE 0x90
#define GRBL_LIVE_FEED_INCREASE_10 0x91
#define GRBL_LIVE_FEED_DECREASE_10 0x92
#define GRBL_LIVE_FEED_INCREASE_1 0x93
#define GRBL_LIVE_FEED_DECREASE_1 0x94

#define GRBL_LIVE_SPINDLE_FULL_SPEED 0x99
#define GRBL_LIVE_SPINDLE_INCREASE_10 0x9A
#define GRBL_LIVE_SPINDLE_DECREASE_10 0x9B
#define GRBL_LIVE_SPINDLE_INCREASE_1 0x9C
#define GRBL_LIVE_SPINDLE_DECREASE_1 0x9D

#define GRBL_LIVE_JOG_CANCEL 0x85

#define CONFIGURATION_FILE "settings_.ini"

// tableIndex:
// 0...n - commands from g-code program
// -1 - ui commands
// -2 - utility commands
// -3 - utility commands
#define COMMAND_TI_MIN_GCODE 0
#define COMMAND_TI_UI -1
#define COMMAND_TI_UTIL1 -2
#define COMMAND_TI_UTIL2 -3

enum GRBLCommand {
    Reset,
    Home,
    Unlock,
    JogStop,
};

enum JoggindDir {
    XPlus,
    XMinus,
    YPlus,
    YMinus,
    ZPlus,
    ZMinus
};

enum SenderState {
    SenderUnknown = -1,
    SenderTransferring = 0,
    SenderPausing = 1,
    SenderPaused = 2,
    SenderStopping = 3,
    SenderStopped = 4,
    SenderChangingTool = 5,
    SenderPausing2 = 6
};

enum DeviceState {
    DeviceUnknown = -1,
    DeviceIdle = 1,
    DeviceAlarm = 2,
    DeviceRun = 3,
    DeviceHome = 4,
    DeviceHold0 = 5,
    DeviceHold1 = 6,
    DeviceQueue = 7,
    DeviceCheck = 8,
    DeviceDoor0 = 9,
    DeviceDoor1 = 10,
    DeviceDoor2 = 11,
    DeviceDoor3 = 12,
    DeviceJog = 13,
    DeviceSleep =14
};

enum SendCommandResult {
    SendDone = 0,
    SendEmpty = 1,
    SendQueue = 2
};

struct CommandAttributes {
    int length;
    int consoleIndex;
    int tableIndex;
    QString command;

    CommandAttributes() {
    }

    CommandAttributes(int len, int consoleIdx, int tableIdx, QString cmd) {
        length = len;
        consoleIndex = consoleIdx;
        tableIndex = tableIdx;
        command = cmd;
    }
};

struct CommandQueue {
    QString command;
    int tableIndex;
    bool showInConsole;

    CommandQueue() {
    }

    CommandQueue(QString cmd, int idx, bool show) {
        command = cmd;
        tableIndex = idx;
        showInConsole = show;
    }
};

enum ConnectionMode {
    SERIAL = 0,
    RAW_TCP = 1,
    VIRTUAL = 2
};

#endif // GLOBALS_H
