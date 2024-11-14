```mermaid
stateDiagram-v2
%% Nodes
    Idle
    Running
    Pausing
    Paused
    Resuming
    Stopping
    Stopped
    Running
    Run
    Halted
    Jogging
    JoggingWaitingForIdle
    Halted
    Error
    SoftReset
    ChangingTool
    [AnyState]

    state Pause {
        Pausing --> Paused
    }

    state Jog {
        Jogging --> JoggingWaitingForIdle
        JoggingWaitingForIdle --> Jogging
    }

%% Edge connections between nodes
    [*] --> Connecting
    Connecting --> Connected
    Connecting --> ConnectingError
    ConnectingError --> Connecting : wait 3s
    Connected --> Idle : if SoftReset enabled
    Connected --> SoftReset : if SoftReset disabled
    SoftReset --> Idle
    Idle --> Run : run clicked
    Run --> Running
    Running --> Pausing : use pause or g-code
    Running --> Stopping
    Running --> Halted
    Paused --> Resuming : "on resume" g-code
    Paused --> Running
    Paused --> Stopping : stop clicked
    Halted --> Running
    Halted --> Stopping
    Resuming --> Running
    Stopping --> Stopped
    Stopped --> Idle
    Idle --> Jog
    Jog --> Idle

    [AnyState] --> Error: If error
    Error --> [*] : Error

    [AnyState] --> Disconnected
    Disconnected --> [*] : Connecting

%% You can add notes with two "%" signs in a row!
