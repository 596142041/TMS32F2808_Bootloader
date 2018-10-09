/********************************************************
 *文件名:main.c
 *创建时间:2018-10-09
 *作者: 皇甫仁和
 ********************************************************/
#include "main.h"
#include "BootLoader.h"
#include "LED.h"
int main(void)
{
	DINT;
	DRTM;
	InitSysCtrl();
	CAN_GPIO_Config(CANA);
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
	//此处用于判断当前是否存在app应用程序,如果存在就直接跳转至应用层
	FLASH_ST Flash_status;
	Uint16 status = 0x0001;
	status = Flash_Verify((Uint16*)APP_INFO_ADDR,app_check,2,&Flash_status);
	if(status == STATUS_SUCCESS)
	{
		CAN_BOOT_JumpToApplication(APP_START_ADDR);
	}
	CAN_Config(CANA,250);
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
