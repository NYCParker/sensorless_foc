NYC 2021.3

fCPU 可以等于系统时钟 fSYS 或是它的一半
外设总线时钟可能等于 fCPU 或是它的一半

模拟转换器时钟 fADCI确定转换器的性能，必须选择符合数据表中给出的规格
XMC4400 DS_v01_03EN数据表中给出的Internal ADC clock的规格是2-30Mhz 
当前程序的fADCI=15Mhz

组0，1的通道同步采样时
使用组0通道6作为采样触发位置，组0通道4结果事件作为中断标志
确保一次触发后的中断内能拿到四个需要的结果（否则可能有bug出现）
为使主从仲裁器能同步运行，要在初始化主仲裁器之前初始化从仲裁器，使能从仲裁器处于一直运行模式

上电后 1s内初始化完毕 校准结束
锁相环开始运行 得到当前角度
等待启动信号

ADC队列模式配置的顺序有关系

电流环的相关函数定义在Motor_Functions.h里的内联函数，目的还是尽量节省电流环的运算时间
电流和SIN COS的AD采样做2次~4次（对比效果和考虑时间）做平均滤波
电流环底层通道6增加请求源事件，通过RCR DRCTR设置结果缩减， 最后一次请求源事件后出结果事件（同时也要修改电流校准函数）
使能中断并且再运行过程中合理设置保证逻辑


3/11 适用于4400kit 配合 powerbord硬件
采样时间大概3us 
电流环剩余时间28us 
ccu8中断也没有问题

//ADC有个数字滤波器可以试试看（貌似要存在特定的结果寄存器里）

//可以用数据压缩配合test来看看
每62.5us 是不是只转换了一次
	WR_REG(VADC_G0->RCR[0], VADC_G_RCR_DRCTR_Msk, VADC_G_RCR_DRCTR_Pos, 1);
	WR_REG(VADC_G0->RCR[1], VADC_G_RCR_DRCTR_Msk, VADC_G_RCR_DRCTR_Pos, 1);
	WR_REG(VADC_G0->RCR[2], VADC_G_RCR_DRCTR_Msk, VADC_G_RCR_DRCTR_Pos, 1);
	WR_REG(VADC_G2->RCR[0], VADC_G_RCR_DRCTR_Msk, VADC_G_RCR_DRCTR_Pos, 1);
	WR_REG(VADC_G2->RCR[2], VADC_G_RCR_DRCTR_Msk, VADC_G_RCR_DRCTR_Pos, 1);	
	WR_REG(VADC_G3->RCR[0], VADC_G_RCR_DRCTR_Msk, VADC_G_RCR_DRCTR_Pos, 1);
	WR_REG(VADC_G3->RCR[2], VADC_G_RCR_DRCTR_Msk, VADC_G_RCR_DRCTR_Pos, 1);	
累计两个 周期加了1倍 
测试无误


//正交编码器
数据字段可由多个数据字组成，如 12 位数据可由两个 8 位数据字构成，第一个数据字包含 8
位、第二个数据字包含 4 位。必须注意：一旦开始传送数据帧，发送数据及时可用。若发送缓
存已空，将发送被动数据电平（SCTR.PDL
一个数据帧可由多个数据字串联组成。

//
启动迅速
切换转速平稳，电流稳定
输入的参数
低速时滑模增益k不需要调的很大
切换后转速再高一些再调大k
对切换的稳定性十分重要
锁相环的参数理论上需要调整，但实际上调整的作用并不大

//4.30 整理程序