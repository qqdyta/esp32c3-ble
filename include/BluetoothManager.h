#ifndef BLUETOOTH_MANAGER_H
#define BLUETOOTH_MANAGER_H

#include <BLEDevice.h>
#include <vector>

enum BluetoothState {
    STATE_IDLE,
    STATE_SCANNING,
    STATE_CONNECTING,
    STATE_CONNECTED,
    STATE_DISCONNECTING
};

class BluetoothManager : public BLEAdvertisedDeviceCallbacks, public BLEClientCallbacks {
public:
    BluetoothManager();

    void init();
    void update();

    void scan(int duration);
    void connect(const std::string& address);
    void disconnect();
    void send(const std::string& data);
    void sendHex(const std::string& hexData);
    BluetoothState getState() const;
    std::string getStateString() const;

private:
    // BLE Callbacks
    void onResult(BLEAdvertisedDevice advertisedDevice) override;
    void onConnect(BLEClient* pclient) override;
    void onDisconnect(BLEClient* pclient) override;

    void setState(BluetoothState newState);
    bool connectToServer(BLEAdvertisedDevice* pDevice);
    void notifyCallback(BLERemoteCharacteristic* pBLERemoteCharacteristic, uint8_t* pData, size_t length, bool isNotify);
    static void notifyCallbackStatic(BLERemoteCharacteristic* pBLERemoteCharacteristic, uint8_t* pData, size_t length, bool isNotify);
    void clearDiscoveredDevices();

    BluetoothState currentState;
    BLEAdvertisedDevice* pServerAddress;
    BLEClient* pClient;
    BLERemoteCharacteristic* pRemoteCharacteristic;
    BLERemoteCharacteristic* pWriteRemoteCharacteristic;

    std::vector<BLEAdvertisedDevice*> discoveredDevices;
    static BluetoothManager* instance;
};

#endif //BLUETOOTH_MANAGER_H
