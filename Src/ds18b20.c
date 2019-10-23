#include "ds18b20.h"
#include "defines.h"

//--------------------------------------------------
void DelayMicro( uint32_t micros)
{
micros *= (SystemCoreClock / 1000000) / 9;
/* Wait till done */
while (micros--) ;
}
//--------------------------------------------------
void port_init(void)
{
	RCC->APB2ENR |= RCC_APB2ENR_IOPBEN;
	GPIOB->CRH |= GPIO_CRH_MODE11;
	GPIOB->CRH |= GPIO_CRH_CNF11_0;
	GPIOB->CRH &= ~GPIO_CRH_CNF11_1;
}
//--------------------------------------------------
uint8_t ds18b20_Reset(void)
{
  uint16_t status;
  GPIOB->ODR &= ~GPIO_ODR_ODR11;//������ �������
  DelayMicro(485);//�������� ��� ������� �� 480 �����������
  GPIOB->ODR |= GPIO_ODR_ODR11;//������� �������
  DelayMicro(65);//�������� ��� ������� �� 60 �����������
  status = GPIOB->IDR & GPIO_IDR_IDR11;//��������� �������
  DelayMicro(500);//�������� ��� ������� �� 480 �����������
  //(�� ������ ������ ������� ��������, ��� ��� ����� ���� ���������� � ��������)
  return (status ? 1 : 0);//����� ���������
}
//----------------------------------------------------------
uint8_t ds18b20_ReadBit(void)
{
  uint8_t bit = 0;
  GPIOB->ODR &= ~GPIO_ODR_ODR11;//������ �������
  DelayMicro(2);
	GPIOB->ODR |= GPIO_ODR_ODR11;//������� �������
	DelayMicro(13);
	bit = (GPIOB->IDR & GPIO_IDR_IDR11 ? 1 : 0);//��������� �������	
	DelayMicro(45);
  return bit;
}
//-----------------------------------------------
uint8_t ds18b20_ReadByte(void)
{
  uint8_t data = 0;
  for (uint8_t i = 0; i <= 7; i++)
  data += ds18b20_ReadBit() << i;
  return data;
}
//-----------------------------------------------
void ds18b20_WriteBit(uint8_t bit)
{
  GPIOB->ODR &= ~GPIO_ODR_ODR11;
  DelayMicro(bit ? 3 : 65);
  GPIOB->ODR |= GPIO_ODR_ODR11;
  DelayMicro(bit ? 65 : 3);
}
//-----------------------------------------------
void ds18b20_WriteByte(uint8_t dt)
{
  for (uint8_t i = 0; i < 8; i++)
  {
    ds18b20_WriteBit(dt >> i & 1);
    DelayMicro(5);
  }
}
//-----------------------------------------------
uint8_t ds18b20_init(uint8_t mode, uint8_t *ROM)
{
	if(ds18b20_Reset()) return 1;

	if(mode==SKIP_ROM)
		ds18b20_WriteByte(0xCC);//SKIP ROM
	else{
		ds18b20_WriteByte(0x55); // MATCH ROM
		vTaskDelay(1);
		for(int j=0;j<8;j++)
			ds18b20_WriteByte(&ROM[j]); //SKIP ROM

		vTaskDelay(1);
	}
	ds18b20_WriteByte(0x4E);//WRITE SCRATCHPAD
	ds18b20_WriteByte(0x64);//TH REGISTER 100C
	ds18b20_WriteByte(0x1E);//TL REGISTER - 30C
	ds18b20_WriteByte(RESOLUTION_12BIT);//Resolution 12 bit
  return 0;
}
//----------------------------------------------------------
void ds18b20_MeasureTemperCmd(uint8_t mode, uint8_t *ROM)
{
  ds18b20_Reset();
  if(mode==SKIP_ROM)
	  ds18b20_WriteByte(0xCC);//SKIP ROM
  else {
	  ds18b20_WriteByte(0x55); // MATCH ROM
	  for(int j=0;j<8;j++)
		  ds18b20_WriteByte(ROM[j]); //SKIP ROM
  }
  //CONVERT T
  ds18b20_WriteByte(0x44);
}
//----------------------------------------------------------
void ds18b20_ReadStratcpad(uint8_t mode, uint8_t *Data, uint8_t *ROM)
{
  uint8_t i;
  ds18b20_Reset();

  if(mode==SKIP_ROM)
	  ds18b20_WriteByte(0xCC); //SKIP ROM
  else{
	  ds18b20_WriteByte(0x55); // MATCH ROM
	  vTaskDelay(1);
	  for(int j=0;j<8;j++)
		  ds18b20_WriteByte(ROM[j]); //SKIP ROM
	  vTaskDelay(1);
  }
  //READ SCRATCHPAD
  ds18b20_WriteByte(0xBE);
  for(i=0;i<8;i++)
  {
    Data[i] = ds18b20_ReadByte();
  }
}
//----------------------------------------------------------
uint8_t ds18b20_GetSign(uint16_t dt)
{
  if (dt&(1<<11)) return 1;
  else return 0;
}
//----------------------------------------------------------
float ds18b20_Convert(uint16_t dt)
{
  float t;
  t = (float) ((dt&0x07FF)>>4);
  t += (float)(dt&0x000F) / 16.0f;
  return t;
}
//----------------------------------------------------------

void ds18b20_ReadROM(uint8_t *Data)
{
  uint8_t i;
  ds18b20_Reset();
  ds18b20_WriteByte(0x33);  // Read ROM  1 slave on the bus
  for(i=0;i<8;i++)
  {
    Data[i] = ds18b20_ReadByte();
  }

}


