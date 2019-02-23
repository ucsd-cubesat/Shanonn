/*
 * lora.h
 *
 *  Created on: Feb 3, 2019
 *      Author: TritonCubed
 *
 *  The breakout uses the following pins:
 *
 *  RPI             Chip
 *  --------------------
 *  Pin 25 (GND)      0V
 *  Pin 24            CS
 *  Pin 16            RT
 *  Pin 17 (3V3)     3V3
 *  Pin 23 (SCLK)     CK
 *  Pin 21 (MISO)     SO
 *  Pin 19 (MOSI)     SI
 */

#include <sys/types.h>
#include <stdint.h>

//SX1278 device modes
#define FSK_SLEEP      0x08  //must enter before switching to lora
#define FSK_CAD        0x0F  //seems to be default startup op mode
#define LORA_SLEEP     0x88
#define LORA_STANDBY   0x89
#define LORA_TX        0x8B
#define LORA_RX_CONT   0x8D
#define LORA_RX_SINGLE 0x8E
#define LORA_CAD       0x8F

 // AND for FSK modes
#define NON_FSK 0x80

//register addresses
#define REG_FIFO                 0x00
#define REG_OP_MODE              0x01
#define REG_RF_FREQ_MSB_MSB      0x06
#define REG_RF_FREQ_MSB          0x07
#define REG_RF_FREQ_LSB          0x08
#define REG_PA_CONFIG            0x09
#define REG_PA_RAMP              0x0A
#define REG_OCP                  0x0B
#define REG_LNA                  0x0C
#define REG_FIFO_ADDR_PTR        0x0D
#define REG_FIFO_TX_BASE_ADDR    0x0E
#define REG_FIFO_RX_BASE_ADDR    0x0F
#define REG_FIFO_RX_CURRENT_ADDR 0x10
#define REG_IRQ_FLAGS_MASK       0x11
#define REG_IRQ_FLAGS            0x12
#define REG_RX_NUM_BYTES         0x13
#define REG_RX_PACKET_COUNT_MSB  0x16
#define REG_RX_PACKET_COUNT_LSB  0x17
#define REG_MODEM_STAT           0x18
#define REG_PACKET_SNR           0x19
#define REG_PACKET_RSSI          0x1A
#define REG_CURRENT_RSSI         0x1B
#define REG_HOP_CHANNEL          0x1C
#define REG_MODEM_CONFIG1        0x1D
#define REG_MODEM_CONFIG2        0x1E
#define REG_PREAMBLE_LEN_MSB     0x20
#define REG_PREAMBLE_LEN_LSB     0x21
#define REG_PAYLOAD_LEN          0x22
#define REG_MAX_PAYLOAD_LEN      0x23
#define REG_HOP_PERIOD           0x24
#define REG_MODEM_CONFIG3        0x26
#define REG_DETECT_OPTIMIZE      0x31
#define REG_DETECT_THRESH        0x37
#define REG_SYNC_WORD            0x39

//helpful values
#define CLEAR_IRQ_FLAGS   0xFF
#define FLAG_TX_DONE      0x08
#define FIFO_TX_BASE_ADDR 0x80

/*
 * Boots the lora module.
 * Requires SPI to be initialized.
 */
void lora_init( char bufferSize );

/*
 * Cleans the lora module.
 */
void lora_close();

/*
 * Reads from the specified register.
 * Returns unsigned 8-bit value.
 */
uint8_t read_reg( uint8_t reg );

/*
 * Writes to the specified register.
 * Returns unsigned 8-bit value.
 */
uint8_t write_reg( uint8_t reg, char data );
