/*

 * main.c
 *
 *  Created on: 2017年4月16日
 *      Author: admin
 */
/*****************************************************************
 *整体的思路是:
 * 第一步:样子FLASH的数据写入和擦除;
 * 第二步:验证CAN总线接收数据;
 * 第三步:根据前面的步骤进行最后综合
 * 第一步验证FLASH功能函数基本结束;
 * 需要添加两个功能函数:从某个地址写入和从某个地址读出的函数
 *****************************************************************/
#include "main.h"
#include "BootLoader.h"
#include "LED.h"
CanTxMsg CANA_tx_msg,CANB_tx_msg;
int main(void)
{
	DINT;
	DRTM;
	InitSysCtrl();
	CAN_GPIO_Config(CANA);
	CAN_GPIO_Config(CANB);

	LED_GPIO_Config();
	CsmUnlock();
	InitPieCtrl();
	IER = 0x0000;
	IFR = 0x0000;
	InitPieVectTable();
	EALLOW;
	PieVectTable.TINT0 = &cpu_timer0_isr;
	PieVectTable.ECAN1INTA = &Ecana_isr1;
	EDIS;
	MemCopy(&RamfuncsLoadStart, &RamfuncsLoadEnd, &RamfuncsRunStart);
	MemCopy(&Flash28_API_LoadStart, &Flash28_API_LoadEnd,&Flash28_API_RunStart);
	InitFlash();
	FlashAPI_Init();
	/*
	FLASH_ST Flash_status;
	Uint16 status = 0x0001;
	status = Flash_Verify((Uint16*)APP_INFO_ADDR,app_check,3,&Flash_status);
	if(status == STATUS_SUCCESS)
	{
		CAN_BOOT_JumpToApplication(APP_START_ADDR);
	}
	*/
	CAN_Config(CANA,250);
	CAN_Config(CANB,250);
	CAN_Rx_Config();
	CAN_Rx_IT_Concig();
	//配置LED指示灯
	LED_Timer_Config();
	//------------------------------------
	//配置中断
	PieCtrlRegs.PIEIER1.bit.INTx7 = 1;
	PieCtrlRegs.PIEIER9.bit.INTx6 = 1; //CANA的中断1
	IER |= M_INT9;
	IER |= M_INT1;
	__enable_irq();
    CANA_tx_msg.CAN_num = CANA;
    CANA_tx_msg.DLC = 8;
    CANA_tx_msg.ExtId.all = 0x4187;
    CANA_tx_msg.IDE  =CAN_ID_EXT;
    CANA_tx_msg.SAE_J1939_Flag = 0;
    CANA_tx_msg.MBox_num = 0;
    CANA_tx_msg.Tx_timeout_cnt = 1000;
    int i = 0;
    for(i = 0;i<8;i++)
    {
        CANA_tx_msg.CAN_Tx_msg_data.msg_byte.data[i] = i;
        CANB_tx_msg.CAN_Tx_msg_data.msg_byte.data[i] = i<<1;
    }
    CANB_tx_msg.CAN_num = CANB;
    CANB_tx_msg.DLC = 8;
    CANB_tx_msg.ExtId.all = 0x4189;
    CANB_tx_msg.IDE = CAN_ID_EXT;
    CANB_tx_msg.MBox_num = 0;
    CANB_tx_msg.SAE_J1939_Flag = 0;
    CANB_tx_msg.Tx_timeout_cnt = 100;
	while (1)
	{
	    if(updata_info.time_out_flag == 0)
	    {
            if(can_rx_msg.rx_update == UPDATE)
            {
                if(CpuTimer0Regs.TCR.bit.TSS == 0)
                {
                    CpuTimer0Regs.TCR.bit.TSS = 1;
                }
                updata_info.time_out_flag = 0;
                can_rx_msg.rx_update = NON_CHANGE;
                CAN_BOOT_ExecutiveCommand(&can_rx_msg);
                GpioDataRegs.GPATOGGLE.bit.GPIO23 = 1;
            }
	    }
	    else
	    {
	       CAN_BOOT_JumpToApplication(APP_START_ADDR);
	    }

	}
}
