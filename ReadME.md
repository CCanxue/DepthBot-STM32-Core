# DepthBot-STM32-Core
STM32 Motor Control with PWM &amp; RealSense &amp; QT 

### 文件结构

- USB转串口CH340驱动：电脑与单片机串口通信时要安装该驱动
- robot_control_new: 包含了单片机控制代码，其中.ioc文件是STM2CubeMX生成的初始化文件
- Project：包含了Qt中的相关代码
- Openblas：相机运行必需库
- Related_project：CSDN上一个类似项目

### 工具

- STM32CubeMX：生成单片机初始化代码
- STM32CubeProgrammer：下载代码到单片机
- Keil：编码方式为GB2312
- QT 6.6.2
- Realsense and OpenCV lib 