#include <Arduino.h>  
#include <BLEDevice.h>  
#include <String>  
  
#define SERVICE_UUID "49535343-FE7D-4AE5-8FA9-9FAFD205E455"  
#define WRITE_UUID "49535343-8841-43F4-A8D4-ECBE34729BB3"  
#define READ_UUID "49535343-1E4D-4BD9-BA61-23C647249616"  
  
boolean doScan = true;  
boolean doConnect = false;  
boolean connected = false;  
String bluetoothDeviceName = "";  
BLEAdvertisedDevice* pServer = nullptr;  
BLERemoteCharacteristic* pRemoteCharacteristic = nullptr;  
BLERemoteCharacteristic* WriteRemoteCharacteristic = nullptr;  
  
// 串口数据处理  
void processSerialData() {  
    static String inputString = ""; // 静态变量，保留上次循环中的数据  
    while (Serial.available()) {  
        char inChar = (char)Serial.read();  
        inputString += inChar;  
        // 检查是否接收到完整的字符串  
        if (inputString.endsWith("END")) {  
            if (inputString.startsWith("START") && inputString.contains("114993073234600022-T1000")) {  
                // 提取蓝牙设备名称  
                int startIndex = inputString.indexOf("START") + 5;  
                int endIndex = inputString.lastIndexOf("END") - startIndex;  
                bluetoothDeviceName = inputString.substring(startIndex, startIndex + endIndex);  
                doScan = false;  
                doConnect = true;  
                Serial.println("Got the device we are looking for!");  
            }  
            inputString = ""; // 清空输入字符串  
        }  
    }  
}  
  
// 搜索到设备时回调功能  
class MyAdvertisedDeviceCallbacks: public BLEAdvertisedDeviceCallbacks {  
    void onResult(BLEAdvertisedDevice advertisedDevice) {  
        if (advertisedDevice.haveName() && (advertisedDevice.getName() == bluetoothDeviceName)) {  
            advertisedDevice.getScan()->stop(); // 停止当前扫描  
            pServer = new BLEAdvertisedDevice(advertisedDevice); // 暂存设备  
            doScan = false;  
        }  
    }  
};  
  
// 客户端与服务器连接与断开回调功能  
class MyClientCallback : public BLEClientCallbacks {  
    void onConnect(BLEClient* pclient) {  
        connected = true;  
    }  
    void onDisconnect(BLEClient* pclient) {  
        doScan = true;  
        connected = false;  
        pServer = nullptr; // 断开连接后，清空设备信息  
        Serial.println("Lost connection to the server!");  
    }  
};  
  
// 收到服务推送的数据时的回调函数  
void NotifyCallback(BLERemoteCharacteristic* pBLERemoteCharacteristic, uint8_t* pData, size_t length, bool isNotify) {  
    for (int i = 0; i < length; i++) {  
        if (pData[i] == '\0') continue; // 忽略空字符  
        Serial.write(pData[i]);  
        // 检查是否包含“OK”  
        if (Serial.find("OK")) {  
            Serial.flush(); // 清空串口缓冲区  
            break; // 找到后停止搜索  
        }  
    }  
}  
  
// 用来连接设备获取其中的服务与特征  
bool ConnectToServer() {  
    BLEClient* pClient = BLEDevice::createClient();  
    pClient->setClientCallbacks(new MyClientCallback());  
    if (!pClient->connect(pServer)) return false;  
  
    BLERemoteService* pRemoteService = pClient->getService(SERVICE_UUID);  
    if (pRemoteService == nullptr) {  
        pClient->disconnect();  
        return false;  
    }  
  
    pRemoteCharacteristic = pRemoteService->getCharacteristic(READ_UUID);  
    if (pRemoteCharacteristic != nullptr && pRemoteCharacteristic->canNotify()) {  
        pRemoteCharacteristic->registerForNotify(NotifyCallback);  
    }  
  
    WriteRemoteCharacteristic = pRemoteService->getCharacteristic(WRITE_UUID);  
    if (WriteRemoteCharacteristic == nullptr) {  
        pClient->disconnect();  
        return false;  
    }  
    return true;  
}  
  
void setup() {  
    Serial.begin(115200);  
    BLEDevice::init("");  
    BLEScan* pBLEScan = BLEDevice::getScan();  
    pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());  
    pBLEScan->setActiveScan(true);  
}  
  
void loop() {  
    processSerialData(); // 监听串口数据  
  
    if (doScan) {  
        BLEDevice::getScan()->clearResults();  
        BLEDevice::getScan()->start(10); // 设置扫描时间为10秒  
    }  
  
    if (doConnect && !connected) {  
        if (ConnectToServer()) {  
            delay(1000);  
            String command = "AT+CONFIG=?\r\n";  
            WriteRemoteCharacteristic->writeValue(command.c_str(), command.length());  
        } else {  
            doScan = true;  
        }  
        doConnect = false;  
    }  
  
    // 空闲时，可以适当延时，减少CPU占用  
    if (doScan && !BLEDevice::getScan()->isScanning()) {  
        delay(1000);  
    }  
}