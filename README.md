## 测试的环境
IDE：Clion 2023.3

PlatformIO

ESP32C3串口波特率： 115200

## 实现的功能
ESP32C3 上电后监听串口，通过不同的指令来进行连接与发送指令


### 扫描设备
命令格式： **SCANEND**
举例：SCANEND
发送命令后设备会开始进行搜索，并且在串口打印搜索到的蓝牙设备的信息

### 连接设备
命令格式： **CONNECT设备蓝牙名称END**
举例: CONNECTMiBand4END : ESP32C3会开始搜索蓝夜设备10秒，如果发现名称为MiBand4的蓝牙设备
则会自动开始连接。
发送命令进行连接，连接后可以保存实例，

### 向已经连接的设备发送指令
命令格式 ** SENT命令END**
举例：SENTAT+CONFIG=?\r\nEND
向设备发送AT+CONFIG=?\r\n指令，设备返回相关的信息会通过串口打印

### 断开连接
连接成功后可以发送DISCONNECTEND断开连接

## 标志位与流程

### doConnect 为True
上电初始化的时候doConnect为False， doScan为False，connected为False，loop函数进行不断的循环，
检查串口的信息。

如果串口接收到 CONNECT + END 的指令表示需要连接指定名称的蓝牙设备，将串口中的设备名称提取出来，设置bluetoothDeviceName，
同时将doConnect设置为True。
loop函数中检测到doConnect 为True则开始连接指定的蓝牙设备，连接成功后将connected设置为True，同时将doConnect设置为False，
表示已经连接成功，不需要再次连接，

### 指令收发
如果串口接受到的信息中是以SEND开头+END结尾的指令，表示需要发送指令，将SEND和END之间的指令提取出来，设置doSend为True，
则loop的时候会发送相关指令，发送完成后将doSend设置为False，表示不需要发送指令。

### 断开连接
将
