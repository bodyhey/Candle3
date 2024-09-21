#ifndef GLOBALS_H
#define GLOBALS_H

#include <QObject>
#include <QDebug>
#include <QVector3D>
#include <QString>
#include "config/registry.h"

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
#define TABLE_INDEX_MIN_GCODE 0
#define TABLE_INDEX_UI -1
#define TABLE_INDEX_UTIL1 -2
#define TABLE_INDEX_UTIL2 -3

enum Units {
    Inches,
    Millimeters
};

enum GRBLCommand {
    Reset,
    Home,
    Unlock,
    JogStop,
};

enum class JoggindDir {
    None,
    XPlus,
    XMinus,
    YPlus,
    YMinus,
    ZPlus,
    ZMinus
};

typedef QVector3D JoggingVector;

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
    DeviceUnknown = 0,
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
    DeviceSleep = 14
};

enum SendCommandResult {
    SendDone = 0,
    SendEmpty = 1,
    SendQueue = 2
};

enum CommandSource : uint8_t {
    Console,
    GeneralUI,
    Program,
    // Commands like "send before/after pause" confugured by user
    ProgramAdditionalCommands,
    System,
};

struct CommandAttributes {
    CommandSource source;
    int length;
    int commandIndex;
    int tableIndex;
    QString command;
    QString response = "";    

    CommandAttributes() {
    }

    CommandAttributes(const CommandAttributes& other) {
        source = other.source;
        length = other.length;
        commandIndex = other.commandIndex;
        tableIndex = other.tableIndex;
        command = other.command;
        response = other.response;
    }

    CommandAttributes(CommandSource source, int commandIndex, int tableIndex, QString command) {
        this->source = source;
        this->length = command.length() + 1;
        this->commandIndex = commandIndex;
        this->tableIndex = tableIndex;
        this->command = command;
    }
};

struct CommandQueue {
    QString command;
    int tableIndex;
    CommandSource source;

    CommandQueue() {
    }

    CommandQueue(CommandSource source, QString command, int tableIndex) {
        this->command = command;
        this->tableIndex = tableIndex;
        this->source = source;
    }
};

enum ConnectionMode {
    SERIAL = 0,
    RAW_TCP = 1,
    VIRTUAL = 2
};

#endif // GLOBALS_H
