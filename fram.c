#include "fram.h"
#include "gpio.h"

/* PIN configuration */
#define FRAM_HOLD_PORT GPIOA
#define FRAM_HOLD_PIN  4
#define FRAM_SCK_PORT  GPIOA
#define FRAM_SCK_PIN   5
#define FRAM_MISO_PORT GPIOA
#define FRAM_MISO_PIN  6
#define FRAM_MOSI_PORT GPIOA
#define FRAM_MOSI_PIN  7
#define FRAM_WP_PORT   GPIOB
#define FRAM_WP_PIN    0
#define FRAM_CS_PORT   GPIOB
#define FRAM_CS_PIN    1

/* FRAM operation codes */
#define FRAM_WREN 0x6 // Write enable
#define FRAM_WRDI 0x4 // Write disable
#define FRAM_RDSR 0x5 // Read status register
#define FRAM_WRSR 0x1 // Write status register
#define FRAM_READ 0x3 // Read data
#define FRAM_WRIT 0x2 // Write data

/* PIN management */
#define FRAM_CS_SET     GPIO_SET  (FRAM_CS_PORT,   FRAM_CS_PIN  )
#define FRAM_CS_RESET   GPIO_RESET(FRAM_CS_PORT,   FRAM_CS_PIN  )
#define FRAM_WP_SET     GPIO_SET  (FRAM_WP_PORT,   FRAM_WP_PIN  )
#define FRAM_WP_RESET   GPIO_RESET(FRAM_WP_PORT,   FRAM_WP_PIN  )
#define FRAM_HOLD_SET   GPIO_SET  (FRAM_HOLD_PORT, FRAM_HOLD_PIN)
#define FRAM_HOLD_RESET GPIO_RESET(FRAM_HOLD_PORT, FRAM_HOLD_PIN)
#define FRAM_SCK_SET    GPIO_SET  (FRAM_SCK_PORT,  FRAM_SCK_PIN )
#define FRAM_SCK_RESET  GPIO_RESET(FRAM_SCK_PORT,  FRAM_SCK_PIN )
#define FRAM_MOSI_SET   GPIO_SET  (FRAM_MOSI_PORT, FRAM_MOSI_PIN)
#define FRAM_MOSI_RESET GPIO_RESET(FRAM_MOSI_PORT, FRAM_MOSI_PIN)
#define FRAM_MISO_GET   GPIO_GET  (FRAM_MISO_PORT, FRAM_MISO_PIN)

#define FRAM_GET_ADDR_H(_code, _addr) ((_code) | ((uint8_t) (_addr >> 5) & UINT8_BIT(3)))

static void FramSend(uint8_t data)
{
  int i = 8;
  while(i--)
  {
    if(data & UINT8_BIT(7))
    {
      FRAM_MOSI_SET;
    }
    else
    {
      FRAM_MOSI_RESET;
    }
    data <<= 1;
    FRAM_SCK_SET;
    FRAM_SCK_RESET;
  }
}

static uint8_t FramRecv(void)
{
  uint8_t data;
  
  int i = 8;
  while(i--)
  {
    data <<= 1;
    if(FRAM_MISO_GET)
    {
      data |= UINT8_BIT(0);
    }
    else
    {
      data &= ~UINT8_BIT(0);
    }
    FRAM_SCK_SET;
    FRAM_SCK_RESET;
  }

  return data;
}

void FramWrite(uint16_t addr, const void* buff, size_t size)
{
  uint8_t addr_h, addr_l;
  const uint8_t* mark = (const uint8_t*) buff;

  addr_h = FRAM_GET_ADDR_H(FRAM_WRIT, addr);
  addr_l = (uint8_t) addr;

  FRAM_WP_SET;

  FRAM_CS_RESET;
  FramSend(FRAM_WREN);
  FRAM_CS_SET;

  FRAM_CS_RESET;
  FramSend(addr_h);
  FramSend(addr_l);
  while (size--)
  {
    FramSend(*mark++);
  };
  FRAM_CS_SET;

  FRAM_WP_RESET;
}

void FramRead(uint16_t addr, void* buff, size_t size)
{
  uint8_t addr_h, addr_l;
  uint8_t* mark = (uint8_t*) buff;

  addr_h = FRAM_GET_ADDR_H(FRAM_READ, addr);
  addr_l = (uint8_t) addr;

  FRAM_CS_RESET;
  FramSend(addr_h);
  FramSend(addr_l);
  while (size--)
  {
    *mark++ = FramRecv();
  };
  FRAM_CS_SET;
}
