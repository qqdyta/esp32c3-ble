#ifndef COMMAND_HANDLER_H
#define COMMAND_HANDLER_H

#include <Arduino.h>
#include "BluetoothManager.h"

class CommandHandler {
public:
    CommandHandler(BluetoothManager& bleManager);
    void handle();

private:
    void processCommand(const String& command);
    BluetoothManager& bleManager;
};

#endif //COMMAND_HANDLER_H
