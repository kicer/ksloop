# ksloop, hc32lx


## 低功耗 

`hw_bare os`模块支持SYSTICK/LPTIM驱动，采用LPTIM驱动时使用深度睡眠模式。
系统时钟默认采用RCL=32768Hz。


## GPIO接口

采用类Arduino风格的接口，支持pinMode/digitalRead/digitalWrite等，同时扩展pinFunc函数用于定义IO包括上下拉电阻、OD、端口复用等功能。


## 串口下载

支持通过P35/P36串口下载烧录，JLinkExe下载功能未完全测试。
烧录时连接串口RTS至MCU reset引脚，用于烧录前后复位。


## startup文件
通过工具`scripts/generate_vt.py`，根据`bsp/inc/*.h`文件手工生成。
```bash
python3 scripts/generate_vt.py hc32l130k8 cortex-m0plus
```
