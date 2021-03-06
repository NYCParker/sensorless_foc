- [ SensorlessFOC](#head1)
	- [ 一、介绍 ](#head2)
	- [ 二、引脚定义](#head3)
	- [ 三、软件设计](#head4)
# <span id="head1"> SensorlessFOC</span>
感谢陆兵同学 @gitee.com/robbinlu 编写的上位机MotorTest
## <span id="head2"> 一、介绍 </span>
基于 XMC4400F100K512 的PMSM无位置传感器驱动控制程序  
## <span id="head3"> 二、引脚定义</span>
注：Sensorless FOC 不需要使用位置传感器接口
+ #### A/D采样输入： 

| Iu    | Iv    | Iw     | RP    | 
| :----:| :----: | :----: |:----: | 
| P14.2  | P15.2  | P14.8   | P14.1  | 

+ #### 六路PWM脉冲输出：  

| UH    | UL    | VH     | VL    | WH    | WL     |
| :----:| :----: | :----: |:----: | :----: | :----: |
| P0.5  | P0.2  | P0.4   | P0.1  | P0.3  | P0.0   |

+ #### D/A输出： 

| DAC1    |  
| :----:| 
| P14.9  | 

+ #### 通讯接口 (UART)： 

| RX    | TX    |  
| :----:| :----: | 
| P2.2  | P2.5  | 

## <span id="head4"> 三、软件设计</span>
* #### 底层驱动 ：  
1. **CCU8** :  
     - 生成3路带死区的互补的PWM信号  
     - 一路专用的PWM用于触发采样和PWM占空比更新
2. **VADC** :   
   -  VADC0 G0、G2、G3三组同步采样（G0主 G2,G3从）
3. **POSIF** :  
   - 独立多通道模式用于保护封波
4. **USIC** :  
   - ASC协议UART模式，用于和上位机通信
5. **DAC**：  
    - DAC1输出用于观测器和角度的实时Debug  

* #### 主要中断：
 1. **CCU80_0_IRQHandler**:  
     - 更新下一个62.5us内的PWM信号占空比  
     - 处理初始定位和闭环I/F启动相关流程
2. **VADC0_G0_0_IRQHandler**:   
   - 三相电流重构  
   - 观测器反电势估计
   - 锁相环角度估计与转速估计 
   - I/F-FOC切换相关流程
   - dq轴电流环运算
   - 虚拟示波器Flash存储
1. **SysTick_Handler**:   
   - 转速环运算 
   - 上位机显示
   - 虚拟示波器Flash数据显示

Control_Loop_Mode  1：电流单闭环  2：转速电流双闭环   
