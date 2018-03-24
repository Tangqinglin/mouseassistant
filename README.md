# mouseassistant
windows鼠标驱动助理
1. 有两个项目 DTMouseDriver 和 mouseassistant；
2. 项目DTMouseDriver基于kmdf虚拟鼠标驱动程序；
3. 项目mouseassistant基于qt5.6.3的应用程序，作为蓝牙服务端，从客户端接收鼠标数据；
4. DTMouseDriver 和 mouseassistant 通过rawpdo传递数据;
5. 用命令 "devcon.exe -r install DTMouseDirver.inf  *PNP6677" 安装 DTMouseDriver 驱动.
