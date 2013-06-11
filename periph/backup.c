#include "backup.h"
#include "stm32f10x.h"
#include "stm32f10x_conf.h"

uint16_t bcpRegTable[] =
{
	BKP_DR1,
	BKP_DR2,
	BKP_DR3,
	BKP_DR4,
	BKP_DR5,
	BKP_DR6,
	BKP_DR7,
	BKP_DR8,
	BKP_DR9,
	BKP_DR10
};

void BcpWrite(int index, int value)
{
	PWR_BackupAccessCmd(ENABLE);
	BKP_WriteBackupRegister(bcpRegTable[index], value);
	PWR_BackupAccessCmd(DISABLE);
}

int BcpRead(int index)
{
	return BKP_ReadBackupRegister(bcpRegTable[index]);
}

void BcpInit()
{
}

