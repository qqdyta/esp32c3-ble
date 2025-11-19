#include "BluetoothManager.h"
#include <Arduino.h>

#define SERVICE_UUID "49535343-FE7D-4AE5-8FA9-9FAFD205E455"
#define WRITE_UUID "49535343-8841-43F4-A8D4-ECBE34729BB3"
#define READ_UUID "49535343-1E4D-4BD9-BA61-23C647249616"

BluetoothManager* BluetoothManager::instance = nullptr;

BluetoothManager::BluetoothManager() :
    currentState(STATE_IDLE),
    pServerAddress(nullptr),
    pClient(nullptr),
    pRemoteCharacteristic(nullptr),
    pWriteRemoteCharacteristic(nullptr)
{
    instance = this;
}

void BluetoothManager::init() {
    BLEDevice::init("");
    pClient = BLEDevice::createClient();
    pClient->setClientCallbacks(this);
    setState(STATE_IDLE);
}

void BluetoothManager::update() {
    // State machine logic
    switch (currentState) {
        case STATE_IDLE:
            // Do nothing
            break;
        case STATE_SCANNING:
            if(!BLEDevice::getScan()->isScanning()){
                setState(STATE_IDLE);
                Serial.println("Scan finished. Found devices:");
                for(auto const& device : discoveredDevices) {
                    Serial.println(device->toString().c_str());
                }
            }
            break;
        case STATE_CONNECTING:
            if (pServerAddress != nullptr) {
                if (connectToServer(pServerAddress)) {
                    setState(STATE_CONNECTED);
                    Serial.println("Connected to server");
                } else {
                    Serial.println("Failed to connect to server");
                    setState(STATE_IDLE);
                }
                clearDiscoveredDevices();
                pServerAddress = nullptr;
            }
            break;
        case STATE_CONNECTED:
            // Do nothing, waiting for user commands
            break;
        case STATE_DISCONNECTING:
            if (pClient->isConnected()) {
                pClient->disconnect();
            }
            setState(STATE_IDLE);
            Serial.println("Disconnected");
            break;
    }
}

void BluetoothManager::scan(int duration) {
    if (currentState == STATE_IDLE) {
        clearDiscoveredDevices();
        setState(STATE_SCANNING);
        Serial.println("Starting scan...");
        BLEScan* pBLEScan = BLEDevice::getScan();
        pBLEScan->setAdvertisedDeviceCallbacks(this);
        pBLEScan->setActiveScan(true);
        pBLEScan->start(duration, false);
    } else {
        Serial.println("Device is busy, cannot scan");
    }
}

void BluetoothManager::connect(const std::string& address) {
    if (currentState == STATE_IDLE) {
        for(auto device : discoveredDevices) {
            if(device->getAddress().toString() == address) {
                pServerAddress = new BLEAdvertisedDevice(*device);
                setState(STATE_CONNECTING);
                return;
            }
        }
        Serial.println("Device not found. Please scan first.");
    } else {
        Serial.println("Device is busy, cannot connect");
    }
}

void BluetoothManager::disconnect() {
    if (currentState == STATE_CONNECTED) {
        setState(STATE_DISCONNECTING);
    }
}

void BluetoothManager::send(const std::string& data) {
    if (currentState == STATE_CONNECTED && pWriteRemoteCharacteristic != nullptr) {
        pWriteRemoteCharacteristic->writeValue(data.c_str(), data.length());
    } else {
        Serial.println("Not connected, cannot send data");
    }
}

void BluetoothManager::sendHex(const std::string& hexData) {
    if (currentState == STATE_CONNECTED && pWriteRemoteCharacteristic != nullptr) {
        // convert hex string to byte array
        int len = hexData.length();
        uint8_t* buf = new uint8_t[len / 2];
        for (int i = 0; i < len; i += 2) {
            std::string hex = hexData.substr(i, 2);
            buf[i / 2] = strtol(hex.c_str(), NULL, 16);
        }
        pWriteRemoteCharacteristic->writeValue(buf, len / 2);
        delete[] buf;
    } else {
        Serial.println("Not connected, cannot send hex data");
    }
}


BluetoothState BluetoothManager::getState() const {
    return currentState;
}

std::string BluetoothManager::getStateString() const {
    switch (currentState) {
        case STATE_IDLE: return "IDLE";
        case STATE_SCANNING: return "SCANNING";
        case STATE_CONNECTING: return "CONNECTING";
        case STATE_CONNECTED: return "CONNECTED";
        case STATE_DISCONNECTING: return "DISCONNECTING";
        default: return "UNKNOWN";
    }
}

void BluetoothManager::onResult(BLEAdvertisedDevice advertisedDevice) {
    discoveredDevices.push_back(new BLEAdvertisedDevice(advertisedDevice));
}

void BluetoothManager::onConnect(BLEClient* pclient) {
    setState(STATE_CONNECTED);
}

void BluetoothManager::onDisconnect(BLEClient* pclient) {
    setState(STATE_IDLE);
    if(pServerAddress != nullptr) {
        delete pServerAddress;
        pServerAddress = nullptr;
    }
    Serial.println("Disconnected from server");
}

void BluetoothManager::setState(BluetoothState newState) {
    if (currentState != newState) {
        currentState = newState;
        Serial.print("State changed to: ");
        Serial.println(getStateString().c_str());
    }
}

bool BluetoothManager::connectToServer(BLEAdvertisedDevice* pDevice) {
    if (!pClient->connect(pDevice)) {
        return false;
    }

    BLERemoteService* pRemoteService = pClient->getService(SERVICE_UUID);
    if (pRemoteService == nullptr) {
        pClient->disconnect();
        return false;
    }

    pRemoteCharacteristic = pRemoteService->getCharacteristic(READ_UUID);
    if (pRemoteCharacteristic != nullptr && pRemoteCharacteristic->canNotify()) {
        pRemoteCharacteristic->registerForNotify(notifyCallbackStatic);
    }

    pWriteRemoteCharacteristic = pRemoteService->getCharacteristic(WRITE_UUID);
    if (pWriteRemoteCharacteristic == nullptr) {
        pClient->disconnect();
        return false;
    }

    return true;
}

void BluetoothManager::notifyCallback(BLERemoteCharacteristic* pBLERemoteCharacteristic, uint8_t* pData, size_t length, bool isNotify) {
    for (int i = 0; i < length; i++) {
        Serial.write(pData[i]);
    }
}

void BluetoothManager::notifyCallbackStatic(BLERemoteCharacteristic* pBLERemoteCharacteristic, uint8_t* pData, size_t length, bool isNotify) {
    if (instance != nullptr) {
        instance->notifyCallback(pBLERemoteCharacteristic, pData, length, isNotify);
    }
}

void BluetoothManager::clearDiscoveredDevices() {
    for(auto device : discoveredDevices) {
        delete device;
    }
    discoveredDevices.clear();
}
