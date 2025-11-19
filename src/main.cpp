#include <Arduino.h>
#include "BluetoothManager.h"
#include "CommandHandler.h"

BluetoothManager bleManager;
CommandHandler commandHandler(bleManager);

void setup() {
    Serial.begin(115200);
    bleManager.init();
}

void loop() {
    commandHandler.handle();
    bleManager.update();
    delay(100);
}
