# 蓝牙调试神器：你的专属探险工具 🧭

欢迎来到蓝牙世界的探险！你手中的这块小小的 **XIAO ESP32C3** 开发板，现在已经变身为一个强大而灵活的蓝牙调试神器。忘掉那些复杂、繁琐的调试过程吧，从现在起，你将像一位经验丰富的冒险家，用简单、直观的“咒语”（指令）去探索、连接并与未知的蓝牙设备进行“心灵沟通”。

## ⚔️ 英雄的装备：开箱指南

在开始你的蓝牙探险之前，请确保你的“装备”已经准备就绪：

1.  **硬件核心**：一块 XIAO ESP32C3 开发板。
2.  **通信桥梁**：一根 Type-C 数据线，连接你的电脑和开发板。
3.  **魔法终端**：一个串口监视器工具（例如 PlatformIO 的 Serial Monitor, Arduino IDE 的串口监视器, 或者 CoolTerm 等）。
4.  **波特率设置**：请将你的串口工具波特率设置为 `115200`，这是我们与神器沟通的“频率”。

一切就绪后，将代码烧录到你的 ESP32C3 中，激动人心的冒险即将开始！

## 📖 魔法书：指令大全

现在，翻开这本古老的魔法书，学习那些能让你驰骋蓝牙世界的强大咒语吧！在你的串口终端中输入这些指令，然后按下回车，即可施放魔法。

---

### ✨ **HELP** - 引路明灯

当你迷失在指令的森林中时，念出这个咒语，它会为你照亮前行的道路，展示所有可用的魔法。

-   **效果**：列出所有可用的指令及其简要说明。
-   **示例**：
    ```
    HELP
    ```
-   **回响**：
    ```
    Available commands:
    SCAN <timeout> - Scans for BLE devices
    CONNECT <address> - Connects to a device by address
    DISCONNECT - Disconnects from the current device
    SEND <data> - Sends data to the connected device
    SENDHEX <hex_data> - Sends data as a hexadecimal string
    STATUS - Shows the current connection status
    HELP - Shows this help message
    ```

---

### 👁️ **SCAN** - 千里眼

施放这个咒语，你的设备会化身“千里眼”，侦查周围所有正在广播的蓝牙设备，无论它们隐藏得多深。

-   **参数**：`<timeout>` (可选) - 扫描的持续时间（秒），默认为10秒。
-   **示例**：
    -   进行一次10秒的默认扫描：
        ```
        SCAN
        ```
    -   进行一次30秒的深度扫描：
        ```
        SCAN 30
        ```
-   **回响**：
    ```
    Starting scan...
    State changed to: SCANNING
    (扫描结束后)
    State changed to: IDLE
    Scan finished. Found devices:
    Device found: Name: MiBand4, Address: a1:b2:c3:d4:e5:f6, RSSI: -55
    Device found: Name: Bluetooth Speaker, Address: 1a:2b:3c:4d:5e:6f, RSSI: -78
    ...
    ```

---

### 🤝 **CONNECT** - 联盟之握

当你发现目标设备后，使用这个咒语，与之建立牢不可破的“联盟”。你需要提供目标的“身份符文”（MAC地址）。

-   **参数**：`<address>` - 你想要连接的设备的MAC地址。
-   **示例**：
    ```
    CONNECT a1:b2:c3:d4:e5:f6
    ```
-   **回响**：
    ```
    State changed to: CONNECTING
    (连接成功后)
    Connected to server
    State changed to: CONNECTED
    ```

---

### 🔮 **STATUS** - 神谕之镜

想知道你当前的状态吗？是孤身一人，还是已与盟友同行？这个咒语会像一面镜子，映照出你当前最真实的状态。

-   **示例**：
    ```
    STATUS
    ```
-   **回响** (已连接时)：
    ```
    Status: CONNECTED
    ```
-   **回响** (未连接时):
    ```
    Status: IDLE
    ```

---

### 💨 **SEND** - 风之密语

与已连接的设备进行对话，发送你的“密语”。

-   **参数**：`<data>` - 你想发送的文本信息。
-   **示例**：
    ```
    SEND Hello, World!
    ```
-   **回响**：无直接回响，但你的消息已随风送达。设备返回的信息会直接显示在串口上。

---

### 📜 **SENDHEX** - 符文信使

当你需要发送更底层的、由十六进制“符文”组成的指令时，这个咒语将是你的最佳选择。

-   **参数**：`<hex_data>` - 十六进制字符串，无需`0x`前缀，也无需空格。
-   **示例**：
    ```
    SENDHEX 0102030405
    ```
-   **回响**：同样，消息已通过“符文”传递，静待对方的回应。

---

### 🚪 **DISCONNECT** - 断开链接

当你需要结束与设备的连接，和平地“分道扬镳”时，使用此咒语。

-   **示例**：
    ```
    DISCONNECT
    ```
-   **回响**：
    ```
    State changed to: DISCONNECTING
    Disconnected
    State changed to: IDLE
    ```

## 🗺️ 未来的藏宝图

这次冒险才刚刚开始！在未来的更新中，我们计划探索更多激动人心的功能：

-   **自动重连**：当与盟友意外失联时，能够自动尝试重新建立连接。
-   **服务与特征扫描**：不仅是发现设备，更能深入探索其内部的“结构”（服务与特征）。
-   **多设备管理**：或许有一天，你可以同时与多个设备建立联盟！

祝你在这片广阔的蓝牙世界中，探险愉快！
