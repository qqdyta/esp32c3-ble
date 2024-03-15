#include <Arduino.h> // 引入Arduino库
#include <BLEDevice.h> // 引入蓝牙库
#include <String> // 引入字符串库

// 定义服务和特征的UUID
#define SERVICE_UUID "49535343-FE7D-4AE5-8FA9-9FAFD205E455"
#define WRITE_UUID "49535343-8841-43F4-A8D4-ECBE34729BB3"
#define READ_UUID "49535343-1E4D-4BD9-BA61-23C647249616"

// 定义一些全局变量，用于控制程序的状态
boolean doScan = false; // 是否进行扫描
boolean doConnect = false; // 是否进行连接
boolean connected = false; // 是否已连接
boolean doSend = false; // 是否发送指令

std:: string bluetoothDeviceName = ""; // 蓝牙设备名称
std:: string sendCommand = ""; // 要发送的指令
unsigned long startSearchTime = 0; // 开始搜索的时间

// 定义一些全局的蓝牙设备和特征对象
BLEAdvertisedDevice* pServer; // 广告设备
BLERemoteCharacteristic* pRemoteCharacteristic; // 远程特征
BLERemoteCharacteristic* WriteRemoteCharacteristic; // 写特征
BLEClient* pClient; // 客户端

// 定义一个类，用于处理搜索到设备时的回调
class MyAdvertisedDeviceCallbacks: public BLEAdvertisedDeviceCallbacks {
    void onResult(BLEAdvertisedDevice advertisedDevice) {
        // 如果搜索到的设备名称与我们要连接的设备名称一致，则停止搜索，并设置doConnect为true
        if (advertisedDevice.haveName() && (advertisedDevice.getName() == bluetoothDeviceName)) {
            advertisedDevice.getScan()->stop(); // 停止当前扫描
            pServer = new BLEAdvertisedDevice(advertisedDevice); // 暂存设备
            doScan = false;
            doConnect = true;
            Serial.println("Got the device we are looking for!");
            // 如果搜索时间超过10秒则停止搜索
        }else if( millis() - startSearchTime > 10000){
            advertisedDevice.getScan()->stop(); // 停止当前扫描
            doScan = false;
            Serial.println("Stop Scanning");
        }
    }
};

// 定义一个类，用于处理客户端与服务器连接与断开的回调
class MyClientCallback : public BLEClientCallbacks {
    void onConnect(BLEClient* pclient) {} // 连接时的回调，这里没有做任何处理
    void onDisconnect(BLEClient* pclient) { // 断开连接时的回调，这里将doScan设置为true，connected设置为false
        doScan = false;
        connected = false;
        Serial.println("End connection to the server!");
    }
};

// 定义一个函数，用于处理收到服务推送的数据
void NotifyCallback(BLERemoteCharacteristic* pBLERemoteCharacteristic, uint8_t* pData, size_t length, bool isNotify) {
    char buf[length + 1];
    for (size_t i = 0; i < length; i++) {
        buf[i] = pData[i];
    }
    buf[length] = 0;
    Serial.printf(buf); // 将收到的数据打印出来
}

// 定义一个函数，用于连接设备并获取其中的服务与特征
bool ConnectToServer(void) {
    // 创建客户端，并设置客户端的回调
    pClient  = BLEDevice::createClient(); // 创建客户端
    pClient->setClientCallbacks(new MyClientCallback()); // 添加客户端与服务器连接与断开回调功能
    if (!pClient->connect(pServer)) { // 尝试连接设备
        delete pServer; // 释放暂存的设备
        pServer = nullptr; // 释放暂存的设备
        return false;
    }
    Serial.println("Connect to Device success!");

    // 尝试获取设备中的服务
    BLERemoteService* pRemoteService = pClient->getService(SERVICE_UUID); // 尝试获取设备中的服务
    if (pRemoteService == nullptr) {
        Serial.println("Get Service Failed");
        delete pServer;
        pServer = nullptr;
        pClient->disconnect();
        return false;
    }
    Serial.println("Got Service successfully");

    // 尝试获取服务中的特征
    pRemoteCharacteristic = pRemoteService->getCharacteristic(READ_UUID); // 尝试获取服务中的特征
    if (pRemoteCharacteristic == nullptr) {
        Serial.println("Got characteristic failed");
        pClient->disconnect();
        delete pServer;  // 释放暂存的设备
        pServer = nullptr; // 释放暂存的设备
        return false;
    }
    Serial.println("Got Read Characteristic successfully");

    // 将蓝牙接受的信息通过串口打印出来
    if(pRemoteCharacteristic->canRead()) { // 如果特征值可以读取则读取数据
        Serial.printf(pRemoteCharacteristic->readValue().c_str());
    }
    if(pRemoteCharacteristic->canNotify()) { // 如果特征值启用了推送则添加推送接收处理
        pRemoteCharacteristic->registerForNotify(NotifyCallback);
    }

    // 尝试获取服务中的写特征
    WriteRemoteCharacteristic = pRemoteService->getCharacteristic(WRITE_UUID); // 尝试获取服务中的特征
    if (WriteRemoteCharacteristic == nullptr) {
        Serial.println("Got characteristic failed");
        pClient->disconnect();
        delete pServer;  // 释放暂存的设备
        pServer = nullptr; // 释放暂存的设备
        return false;
    }
    Serial.println("CONSUCCESS");

    delete pServer;
    pServer = nullptr;
    return true;
};

// 定义一个函数，用于处理串口数据
void processSerialData() {
    static String inputString = ""; // 静态变量，保留上次循环中的数据
    while (Serial.available()) {
        char inChar = (char)Serial.read();
        inputString += inChar;
        // 检查是否接收到完整的字符串
        if (inputString.endsWith("END")) {

            //连接新设备
            if (inputString.startsWith("CONNECT")) {
                // 提取蓝牙设备名称
                int startIndex = inputString.indexOf("START") + 8;
                int endIndex = inputString.lastIndexOf("END") - startIndex;
                bluetoothDeviceName = inputString.substring(startIndex, startIndex + endIndex).c_str();
                doScan = true;
                Serial.print("Got the device Name: ");
                Serial.print(bluetoothDeviceName.c_str());

                // 发送指令
            }else if(inputString.startsWith("SENT")){
                // 提取指令
                int startIndex = inputString.indexOf("SENT") + 4;
                int endIndex = inputString.lastIndexOf("END") - startIndex;
                Serial.print("GotCMD");
                Serial.print(inputString.substring(startIndex, startIndex + endIndex).c_str());
                Serial.println("END");
                sendCommand = inputString.substring(startIndex, startIndex + endIndex).c_str();
                doSend = true;
                // 断开连接
            }else if(inputString.startsWith("DISCONNECT")){
                // 断开连接
                BLEDevice::getScan()->clearResults();
                pClient->disconnect();
                doScan = false;
                doConnect = false;
                connected = false;
                Serial.println("DisconnectSuccess!");

                // 扫描设备
            }else if(inputString.endsWith("SCAN")){

                doScan = true;
                Serial.println("Start Scanning");
            }
            inputString = ""; // 清空输入字符串
        }
    }
}

// 定义setup函数，用于初始化
void setup() {
    Serial.begin(115200); // 初始化串口
    BLEDevice::init(""); // 初始化蓝牙设备
    BLEScan* pBLEScan = BLEDevice::getScan(); // 获取蓝牙扫描对象
    pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks()); // 设置扫描回调
    pBLEScan->setActiveScan(true); // 设置为主动扫描
    pBLEScan->setInterval(100); // 设置扫描间隔
    pBLEScan->setWindow(80); // 设置扫描窗口
}

// 定义loop函数，用于循环执行
void loop() {

    processSerialData(); // 处理串口数据
    // 如果需要扫描则进行扫描
    if (doScan) {
        startSearchTime = millis();
        Serial.println("Start Scanning");
        BLEDevice::getScan()->clearResults();
        BLEDevice::getScan()->start(0); // 持续搜索设备
        doScan = false;
    }
    // 如果找到设备就尝试连接设备
    if (doConnect) {
        if (ConnectToServer()) {
            connected = true;
        }
        else {
            doScan = true;
        }
        doConnect = false;
    }
    // 如果已经连接就可以向设备发送数据
    if (connected && doSend) {
        if(WriteRemoteCharacteristic->canWrite()) { // 如果可以向特征值写数据
            WriteRemoteCharacteristic->writeValue(sendCommand.c_str(), sendCommand.length());
            doSend = false;
        }
    }
}