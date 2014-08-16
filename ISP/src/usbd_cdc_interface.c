/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "usbd_cdc_interface.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define APP_RX_DATA_SIZE  2048
#define APP_TX_DATA_SIZE  2048

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
USBD_CDC_LineCodingTypeDef LineCoding =
{ 115200, /* baud rate */
0x00, /* stop bits-1 */
0x00, /* parity - none */
0x08 /* nb. of bits 8 */
};

uint8_t UserRxBuffer[APP_RX_DATA_SIZE];/* Received Data over USB are stored in this buffer */
uint8_t UserTxBuffer[APP_TX_DATA_SIZE];/* Received Data over UART (CDC interface) are stored in this buffer */

uint32_t UserTxBufPtrIn = 0;/* Increment this pointer or roll it back to
 start address when data are received over USART */
uint32_t UserTxBufPtrOut = 0; /* Increment this pointer or roll it back to
 start address when data are sent over USB */

uint32_t UserRxBufPtrIn = 0; /* Increment this pointer or roll it back to
 start address when writing received data
 in the buffer APP_Rx_Buffer. */

uint32_t UserRxBufPtrOut = 0; /* Increment this pointer or roll it back to
 start address when data are sent over USB */

/* UART handler declaration */
UART_HandleTypeDef UartHandle;
/* TIM handler declaration */
TIM_HandleTypeDef TimHandle;
/* USB handler declaration */
extern USBD_HandleTypeDef hUSBDDevice;

static struct
{
    uint8_t Buffer[CDC_DATA_FS_OUT_PACKET_SIZE];
    int Position, Size;
    char ReadDone;
} s_RxBuffer;

char g_VCPInitialized;
char ComPortClosed = 1;

/* Private function prototypes -----------------------------------------------*/
static int8_t CDC_Itf_Init(void);
static int8_t CDC_Itf_DeInit(void);
static int8_t CDC_Itf_Control(uint8_t cmd, uint8_t* pbuf, uint16_t length);
static int8_t CDC_Itf_Receive(uint8_t* pbuf, uint32_t *Len);
static void CDC_Error_Handler(void);

USBD_CDC_ItfTypeDef USBD_CDC_fops =
{ CDC_Itf_Init, CDC_Itf_DeInit, CDC_Itf_Control, CDC_Itf_Receive };

/* Private functions ---------------------------------------------------------*/

/**
 * @brief  Initializes the CDC media low layer      
 * @param  None
 * @retval Result of the operation: USBD_OK if all operations are OK else USBD_FAIL
 */
static int8_t CDC_Itf_Init(void)
{

	USBD_CDC_SetTxBuffer(&hUSBDDevice, UserTxBuffer, 0);
	USBD_CDC_SetRxBuffer(&hUSBDDevice, s_RxBuffer.Buffer);
	g_VCPInitialized = 1;
	return (USBD_OK);
}

/**
 * @brief  CDC_Itf_DeInit
 *         DeInitializes the CDC media low layer
 * @param  None
 * @retval Result of the operation: USBD_OK if all operations are OK else USBD_FAIL
 */
static int8_t CDC_Itf_DeInit(void)
{
	return (USBD_OK);
}

/**
 * @brief  CDC_Itf_Control
 *         Manage the CDC class requests
 * @param  Cmd: Command code            
 * @param  Buf: Buffer containing command data (request parameters)
 * @param  Len: Number of data to be sent (in bytes)
 * @retval Result of the operation: USBD_OK if all operations are OK else USBD_FAIL
 */
static int8_t CDC_Itf_Control(uint8_t cmd, uint8_t* pbuf, uint16_t length)
{
	switch (cmd)
	  {
	  case CDC_SEND_ENCAPSULATED_COMMAND:
	    /* Add your code here */
	    break;

	  case CDC_GET_ENCAPSULATED_RESPONSE:
	    /* Add your code here */
	    break;

	  case CDC_SET_COMM_FEATURE:
	    /* Add your code here */
	    break;

	  case CDC_GET_COMM_FEATURE:
	    /* Add your code here */
	    break;

	  case CDC_CLEAR_COMM_FEATURE:
	    /* Add your code here */
	    break;

	  case CDC_SET_LINE_CODING:
	    LineCoding.bitrate    = (uint32_t)(pbuf[0] | (pbuf[1] << 8) |\
	                            (pbuf[2] << 16) | (pbuf[3] << 24));
	    LineCoding.format     = pbuf[4];
	    LineCoding.paritytype = pbuf[5];
	    LineCoding.datatype   = pbuf[6];

	    /* Set the new configuration */
	    ComPortClosed = 0;
	    break;

	  case CDC_GET_LINE_CODING:
	    pbuf[0] = (uint8_t)(LineCoding.bitrate);
	    pbuf[1] = (uint8_t)(LineCoding.bitrate >> 8);
	    pbuf[2] = (uint8_t)(LineCoding.bitrate >> 16);
	    pbuf[3] = (uint8_t)(LineCoding.bitrate >> 24);
	    pbuf[4] = LineCoding.format;
	    pbuf[5] = LineCoding.paritytype;
	    pbuf[6] = LineCoding.datatype;
	    break;

	  case CDC_SET_CONTROL_LINE_STATE:
	    /* Add your code here */
	    break;

	  case CDC_SEND_BREAK:
	     /* Add your code here */
	    break;

	  default:
	    break;
	  }

	return (USBD_OK);
}

/**
 * @brief  CDC_Itf_DataRx
 *         Data received over USB OUT endpoint are sent over CDC interface 
 *         through this function.
 * @param  Buf: Buffer of data to be transmitted
 * @param  Len: Number of data received (in bytes)
 * @retval Result of the operation: USBD_OK if all operations are OK else USBD_FAIL
 */
int8_t CDC_Itf_Receive(uint8_t* Buf, uint32_t *Len)
{
	//HAL_UART_Transmit_DMA(&UartHandle, Buf, *Len);
	s_RxBuffer.Position = 0;
	s_RxBuffer.Size = *Len;
	s_RxBuffer.ReadDone = 1;
	return (USBD_OK);

}

int VCP_read(void *pBuffer, int size)
{
    if (!s_RxBuffer.ReadDone)
        return 0;

    int remaining = s_RxBuffer.Size - s_RxBuffer.Position;
    int todo = MIN(remaining, size);
    if (todo <= 0)
        return 0;

    memcpy(pBuffer, s_RxBuffer.Buffer + s_RxBuffer.Position, todo);
    s_RxBuffer.Position += todo;
    if (s_RxBuffer.Position >= s_RxBuffer.Size)
    {
        s_RxBuffer.ReadDone = 0;
        USBD_CDC_ReceivePacket(&hUSBDDevice);
    }

    return todo;
}

int VCP_write(const void *pBuffer, int size)
{
    if (size > CDC_DATA_HS_OUT_PACKET_SIZE)
    {
        int offset;
        for (offset = 0; offset < size; offset++)
        {
            int todo = MIN(CDC_DATA_HS_OUT_PACKET_SIZE,
                           size - offset);
            int done = VCP_write(((char *)pBuffer) + offset, todo);
            if (done != todo)
                return offset + done;
        }

        return size;
    }

    USBD_CDC_HandleTypeDef *pCDC =
            (USBD_CDC_HandleTypeDef *)hUSBDDevice.pClassData;
    while(pCDC->TxState) { } //Wait for previous transfer

    USBD_CDC_SetTxBuffer(&hUSBDDevice, (uint8_t *)pBuffer, size);
    if (USBD_CDC_TransmitPacket(&hUSBDDevice) != USBD_OK)
        return 0;

    while(pCDC->TxState) { } //Wait until transfer is done
    return size;
}

/**
 * @brief  Tx Transfer completed callback
 * @param  huart: UART handle
 * @retval None
 */
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
	/* Initiate next USB packet transfer once UART completes transfer (transmitting data over Tx line) */
	USBD_CDC_ReceivePacket(&hUSBDDevice);
}

/**
 * @brief  This function is executed in case of error occurrence.
 * @param  None
 * @retval None
 */
static void CDC_Error_Handler(void)
{
	/* Add your own code here */
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
