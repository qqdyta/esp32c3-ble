#include "CommandHandler.h"
#include <Arduino.h>

CommandHandler::CommandHandler(BluetoothManager& bleManager) : bleManager(bleManager) {}

void CommandHandler::handle() {
    static String inputString = "";
    while (Serial.available()) {
        char inChar = (char)Serial.read();
        inputString += inChar;
        if (inChar == '\n') {
            inputString.trim();
            processCommand(inputString);
            inputString = "";
        }
    }
}

void CommandHandler::processCommand(const String& command) {
    if (command.equalsIgnoreCase("HELP")) {
        Serial.println("Available commands:");
        Serial.println("SCAN <timeout> - Scans for BLE devices");
        Serial.println("CONNECT <address> - Connects to a device by address");
        Serial.println("DISCONNECT - Disconnects from the current device");
        Serial.println("SEND <data> - Sends data to the connected device");
        Serial.println("SENDHEX <hex_data> - Sends data as a hexadecimal string");
        Serial.println("STATUS - Shows the current connection status");
        Serial.println("HELP - Shows this help message");
    } else if (command.toUpperCase().startsWith("SCAN")) {
        int timeout = 10;
        int spaceIndex = command.indexOf(' ');
        if (spaceIndex != -1) {
            timeout = command.substring(spaceIndex + 1).toInt();
        }
        bleManager.scan(timeout);
    } else if (command.toUpperCase().startsWith("CONNECT")) {
        int spaceIndex = command.indexOf(' ');
        if (spaceIndex != -1) {
            String address = command.substring(spaceIndex + 1);
            bleManager.connect(address.c_str());
        } else {
            Serial.println("Please specify an address to connect to.");
        }
    } else if (command.equalsIgnoreCase("DISCONNECT")) {
        bleManager.disconnect();
    } else if (command.toUpperCase().startsWith("SEND")) {
        int spaceIndex = command.indexOf(' ');
        if (spaceIndex != -1) {
            String data = command.substring(spaceIndex + 1);
            bleManager.send(data.c_str());
        }
    } else if (command.toUpperCase().startsWith("SENDHEX")) {
        int spaceIndex = command.indexOf(' ');
        if (spaceIndex != -1) {
            String hexData = command.substring(spaceIndex + 1);
            bleManager.sendHex(hexData.c_str());
        }
    } else if (command.equalsIgnoreCase("STATUS")) {
        Serial.print("Status: ");
        Serial.println(bleManager.getStateString().c_str());
    } else {
        Serial.println("Unknown command. Type HELP for a list of commands.");
    }
}
