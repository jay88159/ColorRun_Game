#include <file.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include "time.h"
/* XDCtools Header files */
#include <xdc/std.h>
#include <xdc/cfg/global.h>
#include <xdc/runtime/System.h>
#include <xdc/runtime/Error.h>
#include <xdc/runtime/Gate.h>

/* BIOS Header files */
#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Mailbox.h>
#include <ti/sysbios/knl/Task.h>
#include <ti/sysbios/fatfs/ff.h>
#include <ti/sysbios/utils/Load.h>
/* TI-RTOS Driver files */
#include <ti/drivers/GPIO.h>
#include <ti/drivers/SDSPI.h>

#include <ti/drivers/UART.h>
#include "UARTUtils.h"
/* Graphiclib Header file */
#include "driverlib/sysctl.h"
#include <grlib/grlib.h>
#include "grlib/widget.h"
#include "grlib/canvas.h"
#include "grlib/checkbox.h"
#include "grlib/container.h"
#include "grlib/pushbutton.h"
#include "grlib/radiobutton.h"
#include "grlib/slider.h"
#include "utils/ustdlib.h"
#include "Kentec320x240x16_ssd2119_8bit.h"
/* Example/Board Header file */
//#include "USBCDCD.h"
#include "Board.h"
#include "USBCDCD_LoggerIdle.h"
#include "touch.h"
#include "2048.h"
#include "driverlib/timer.h"
#include "inc/hw_memmap.h"
extern int Array2048[10][10];
/* Images */
extern const UChar image_TI_Black[];
extern const UChar image_TI_Ad[];
const UChar *image_Gallery[2] = { image_TI_Black, image_TI_Ad };

/* Screen size */
#define SCREEN_WIDTH 320
#define SCREEN_HEIGHT 240

/*!
 *  ======== DrawCommand ========
 *  Enum defines several drawing commands
 */
typedef enum DrawCommand {
	IMAGE, KeyBoadCOMMAND, ScreenCOMMAND
} DrawCommand;
typedef enum TouchCommand {
	UP, DOWN, LEFT, RIGHT
} TouchCommand;

/*
 * random background color for numbers
 */
int DrawColor[5] = { ClrGreen, ClrYellow, ClrGold, ClrPurple, ClrRed };
int sixColor[6] = { ClrGreen, ClrYellow, ClrRed, ClrBlue, ClrBrown, ClrPink };
char *sixChar[6] = { "Green", "Yellow", "Red", "Blue", "Brown", "Pink" };
char *dis_arrChar[6];
int playColor;   //the color be chosen
int scorecnt;
int MAX;
int displayTime;
int global_rand;
int dis_arrColor[6];
uint32_t ui32Period;
//**********this is for color select
int dis_array(int Color[], int num) {
	int arr_num;
	int i;
	int temp;

	for (i = 0; i < num; i++)
		dis_arrColor[i] = Color[i];

	srand(time(NULL));
	for (i = 0; i < num; i++) {
		arr_num = rand() % num;

		temp = dis_arrColor[i];
		dis_arrColor[i] = dis_arrColor[arr_num];
		dis_arrColor[arr_num] = temp;
		//	System_printf("this is arr_num is %d\n", arr_num);
	}
	return arr_num;
}
//*************this is for char select***********888
int char_dis_array(char *sixChar[6], int num) {
	int arr_num;
	int i;
	int temp;

	for (i = 0; i < num; i++)
		dis_arrChar[i] = sixChar[i];

	srand(time(NULL));
	for (i = 0; i < num; i++) {
		arr_num = rand() % num;

		temp = dis_arrChar[i];
		dis_arrChar[i] = dis_arrChar[arr_num];
		dis_arrChar[arr_num] = temp;
		//	System_printf("this is arr_num is %d\n", arr_num);
	}
	return arr_num;
}
/*!
 *  ======== DrawMessage ========
 *  Drawing message contains information needed for drawing
 *
 *  @field(drawCommand)     drawing command for current message
 *  @field(drawImageIndex)  image index in the image list to draw
 *  @field(drawString)      string needed to draw
 *  @field(drawDirCount)    count of directories displayed on the screen
 *  @field(drawDir)         array holding strings of directories
 *                          (7 strings with width 16)
 */
typedef struct DrawMessage {
	DrawCommand drawCommand;
	UInt drawImageIndex;
	UInt touchPosition[2];
	UInt draw2048Count; //
	UInt draw2048Index;
	int draw2048[10][10]; //???????
} DrawMessage;

//------------------------
// for Queue - Part B
//------------------------
typedef struct MsgObj {
	Queue_Elem elem;
	TouchCommand TouchCommand;            		// message value
} MsgObj, *Msg;				// Use Msg as pointer to MsgObj
typedef struct colorMsgObj {
	Queue_Elem elem;
	int32_t X;

	int32_t Y;
	int color;            		// message value
	int decision;
} colorMsgObj, *colorMsg;				// Use Msg as pointer to MsgObj
typedef struct timerMsgObj {
	Queue_Elem elem;
//	int32_t X;

//	int32_t Y;
//	int color;            		// message value
	int clear;
} timerMsgObj, *timerMsg;

/* Handles created dynamically in the c */
Mailbox_Handle mailboxHandle = NULL;
//Mailbox_Handle mailboxHandle1 = NULL;

/* Global context for drawing */
tContext context;
tRectangle sRect;

void output(int n, int dis_arrColor[6], int num) {  //logout

	static char pcCanvasText[10];  //what's this?

	int i, j, cnt = 0;
	int charnum = 0;
	for (i = 0; i < 4; ++i) {
		for (j = 0; j < 2; ++j) {
			sRect.i16XMin = 10 + i * 76 + 1;				//75
			sRect.i16YMin = 30 + j * 91 + 1;				//90
			sRect.i16XMax = 85 + i * 76 - 1;
			sRect.i16YMax = 120 + j * 91 - 1;
			if ((i == 0) && (j == 0)) {
				//System_printf("coming here");
				//	charnum = char_dis_array(sixChar[6],6);

				GrContextForegroundSet(&context, ClrWhite);
				GrRectFill(&context, &sRect);
				if (dis_arrColor[num] == ClrGreen)
					usprintf(pcCanvasText, "Green");
				if (dis_arrColor[num] == ClrYellow)
					usprintf(pcCanvasText, "Yellow");
				if (dis_arrColor[num] == ClrRed)
					usprintf(pcCanvasText, "Red");
				if (dis_arrColor[num] == ClrBlue)
					usprintf(pcCanvasText, "Blue");
				if (dis_arrColor[num] == ClrPink)
					usprintf(pcCanvasText, "Pink");
				if (dis_arrColor[num] == ClrBrown)
					usprintf(pcCanvasText, "Brown");
				playColor = dis_arrColor[num];

				//dis_array(sixColor,6);
				//	usprintf(pcCanvasText, "%3d", colorMsg6->decision);
				//GrContextForegroundSet(&context, ClrSkyBlue);
				GrContextForegroundSet(&context, dis_arrColor[(num + 1) % 6]); //get a random color and fill in the corner box
				GrContextFontSet(&context, &g_sFontCmss22b);
				GrStringDraw(&context, pcCanvasText, -1, 20, 70, 0);
				//	GrContextFontSet(&context, &g_sFontCm20);
				//GrStringDraw(&context, pcCanvasText, -1, 10 + i * 76 + 1,
				//	30 + j * 91 + 1, 0);
			} else if ((i == 0) && (j == 1)) {
				//System_printf("coming here");
				//usprintf(pcCanvasText, "test");
				GrContextForegroundSet(&context, ClrWhite);
				GrRectFill(&context, &sRect);
				usprintf(pcCanvasText, "Score");
				GrContextForegroundSet(&context, ClrBlack);
				GrContextFontSet(&context, &g_sFontCmss22b);
				GrStringDraw(&context, pcCanvasText, -1, 20, 132, 0);
				if (scorecnt < 1){
					scorecnt = 0;
				}
				usprintf(pcCanvasText, "%3d", scorecnt);
				GrContextForegroundSet(&context, ClrBlack);
				GrContextFontSet(&context, &g_sFontCmsc22);
				GrStringDraw(&context, pcCanvasText, -1, 24, 160, 0);
			}
			//	color = drawArray[i][j] % 5;
			else {
				if (scorecnt < 1) {
					//scorecnt= 0;
					usprintf(pcCanvasText, "END!!");
					GrContextForegroundSet(&context, ClrBlack);
					GrContextFontSet(&context, &g_sFontCmsc22);
					GrStringDraw(&context, pcCanvasText, -1, 20, 220, 0);
				}
				GrContextForegroundSet(&context, dis_arrColor[cnt]);
				GrRectFill(&context, &sRect);
				cnt++;
				if (cnt == 6) {
					cnt = 0;
				}
			}

		}

	}
	sRect.i16XMin = 0;				//16
	sRect.i16YMin = 0;				//64
	sRect.i16XMax = 320;
	sRect.i16YMax = 23;
	GrContextForegroundSet(&context, ClrSeashell);
	GrRectFill(&context, &sRect);

	GrContextForegroundSet(&context, ClrBlack);
	GrContextFontSet(&context, &g_sFontCm22b);
	usprintf(pcCanvasText, "Color Run");
	GrStringDrawCentered(&context, pcCanvasText, -1,
			GrContextDpyWidthGet(&context) / 2, 8, 0);

	GrContextForegroundSet(&context, ClrBlack);
	GrContextFontSet(&context, &g_sFontCm20);
	usprintf(pcCanvasText, "MAX:");
	GrStringDraw(&context, pcCanvasText, -1, 240, 8, 0);
	usprintf(pcCanvasText, "%3d", MAX);
	GrStringDraw(&context, pcCanvasText, -1, 285, 8, 0);

/*	usprintf(pcCanvasText, "Time:");
	GrStringDraw(&context, pcCanvasText, -1, 240, 212, 0);
	usprintf(pcCanvasText, "%3d", displayTime);
	GrStringDraw(&context, pcCanvasText, -1, 285, 212, 0);
	usprintf(pcCanvasText, "s");
	GrStringDraw(&context, pcCanvasText, -1, 290, 212, 0);*/
}

Void clearDisplay() {
	tRectangle rect = { 0, 0, SCREEN_WIDTH - 1, SCREEN_HEIGHT - 1 };
	GrContextForegroundSetTranslated(&context, 0);
	GrRectFill(&context, &rect);
}
/*
 *  ======== grlibTaskFxn ========
 *  Drawing task
 *
 *  It is pending for the message either from console task or from button ISR.
 *  Once the messages received, it draws to the screen based on information
 *  contained in the message.
 */
static int32_t TouchTestCallback(uint32_t ui32Message, int32_t lX, int32_t lY) { //TTC
	colorMsgObj msg;
	colorMsg msgp;	// Queues pass POINTERS, so we need a pointer of type Msg
	msgp = &msg;
	//
	// Check the message to determine what to do.
	//
	switch (ui32Message) {
	//
	// The screen is no longer being touched (in other words, pen/pointer
	// up).
	//
	case WIDGET_MSG_PTR_UP: {

		msgp->X = lX;
		msgp->Y = lY;

		Queue_put(LED_Queue, (Queue_Elem*) msgp);// pass pointer to Message object via LED_Queue

		Semaphore_post(QueSem);

		break;
	}
		//
		// The screen has just been touched (in other words, pen/pointer down).
		//
	case WIDGET_MSG_PTR_DOWN: {
		//
		// Handle the pointer down message if required.
		//

		break;
	}
		//
		// The location of the touch on the screen has moved (in other words,
		// the pen/pointer has moved).
		//
	case WIDGET_MSG_PTR_MOVE: {
		//
		// Handle the pointer move message if required.
		//
		break;
	}
		//
		// An unknown message was received.
		//
	default: {
		//
		// Ignore all unknown messages.
		//
		break;
	}
	}
	//
	// Success.
	//
	return (0);
}
extern void TouchScreenIntHandler(void);
/*int32_t grlibTouchTaskFxn(uint32_t ulMessage, int32_t lX, int32_t lY) {
 MsgObj msg;
 Msg msgp;		// Queues pass POINTERS, so we need a pointer of type Msg
 msgp = &msg;

 if (lX < 64)													//left
 {
 msg.TouchCommand = LEFT;
 Queue_put(LED_Queue, (Queue_Elem*) msgp);// pass pointer to Message object via LED_Queue
 Semaphore_post(QueSem);

 } else if (lX > 256)				//right
 {
 msg.TouchCommand = RIGHT;
 Queue_put(LED_Queue, (Queue_Elem*) msgp);// pass pointer to Message object via LED_Queue
 Semaphore_post(QueSem);

 } else if (lY < 36)				//up
 {
 msg.TouchCommand = UP;
 Queue_put(LED_Queue, (Queue_Elem*) msgp);// pass pointer to Message object via LED_Queue
 Semaphore_post(QueSem);

 } else if (lY > 204)				//down
 {
 msg.TouchCommand = DOWN;
 Queue_put(LED_Queue, (Queue_Elem*) msgp);// pass pointer to Message object via LED_Queue
 Semaphore_post(QueSem);

 }
 LED_OFF();
 SysCtlDelay(6700000);
 LED_ON();
 }*/
void TimeDis() {
	TimerIntClear(TIMER3_BASE, TIMER_TIMA_TIMEOUT);
	displayTime++;
	//output(4, dis_arrColor, global_rand);
}
void TimerTaskFxn() {
	TimerIntClear(TIMER2_BASE, TIMER_TIMA_TIMEOUT);
//	int global_rand = 2;
	if (scorecnt < 1) {
		//	output(4, dis_arrColor, 0);
		TimerDisable(TIMER2_BASE, TIMER_A);
		SysCtlPeripheralDisable(SYSCTL_PERIPH_TIMER2);
		TimerIntDisable(TIMER2_BASE, TIMER_TIMA_TIMEOUT);
	} else {
		scorecnt=scorecnt-2;
		Semaphore_post(AnoSem);
	}
	System_printf("this is TimerTaskFxn &************");
	//srand(5);
	//global_rand = rand() % 6;
	//global_rand = dis_array(sixColor, 6);
	//output(4, dis_arrColor, global_rand);
}
void TimerTaskDo() {
	while (TRUE) {
		Semaphore_pend(AnoSem, BIOS_WAIT_FOREVER);
		System_printf("this is TaskDo &************");
//		int global_rand = 2;
//	global_rand = rand() % 6;
		global_rand = dis_array(sixColor, 6);
		output(4, dis_arrColor, global_rand);
	}
}
Void grlibTaskFxn1(UArg arg0, UArg arg1) {
//	MsgObj msg;
//	Msg msgp;				//define pointer to MsgObj to use with queue put/get
//	msgp = &msg;
//	const UChar *pucCurImage;
	UInt key;
	int32_t lX;
	int32_t lY;
	colorMsgObj color;
	colorMsg colorp = &color;
	int random_num;
	while (TRUE) {
		Semaphore_pend(QueSem, BIOS_WAIT_FOREVER);
		colorp = Queue_get(LED_Queue);
		lX = colorp->X;
		lY = colorp->Y;
		key = Gate_enterSystem();
		if (scorecnt < 1) {
			output(4, dis_arrColor, random_num);
			TimerDisable(TIMER2_BASE, TIMER_A);
			SysCtlPeripheralDisable(SYSCTL_PERIPH_TIMER2);
			TimerIntDisable(TIMER2_BASE, TIMER_TIMA_TIMEOUT);

		} else {

			//	System_printf("xxxxxxx %d yyyyyy %d \n", lX, lY);
			if ((lX > 239) && (lY > 122))				//SIX
					{
				if (dis_arrColor[5] == playColor)
					scorecnt++;
				else
					scorecnt--;
				System_printf("5scorevs %d dis_arrColor %x, playcolor %x /n",
						scorecnt, dis_arrColor[5], playColor);
				System_printf("5555555555555xxxxxxx %d yyyyyy %d \n", lX, lY);
				//		break;
			}

			if ((lX < 160) && (lY < 119))		//ONE
					{

				if (dis_arrColor[0] == playColor)
					scorecnt++;
				else
					scorecnt--;
				System_printf("0scorevs %d dis_arrColor %x, playcolor %x /n",
						scorecnt, dis_arrColor[0], playColor);
				System_printf("00000000000xxxxxxx %d yyyyyy %d \n", lX, lY);
				//	break;
			}
			if ((163 < lX) && (lX < 236) && (lY < 119))				//TWO
					{
				if (dis_arrColor[2] == playColor)
					scorecnt++;
				else
					scorecnt--;
				System_printf("1scorevs %d dis_arrColor %x, playcolor %x /n",
						scorecnt, dis_arrColor[2], playColor);
				System_printf("1111111xxxxxxx %d yyyyyy %d \n", lX, lY);
				//	break;
			}
			if ((lX > 239) && (lY < 119))				//THREE

					{
				if (dis_arrColor[4] == playColor)
					scorecnt++;
				else
					scorecnt--;
				System_printf("2scorevs %d dis_arrColor %x, playcolor %x /n",
						scorecnt, dis_arrColor[4], playColor);
				System_printf("22222222222xxxxxxx %d yyyyyy %d \n", lX, lY);
				//	break;
			}
			if ((lX < 160) && (lY > 122))				//FOUR
					{
				if (dis_arrColor[1] == playColor)
					scorecnt++;
				else
					scorecnt--;
				System_printf("3scorevs %d dis_arrColor %x, playcolor %x /n",
						scorecnt, dis_arrColor[1], playColor);
				System_printf("333333xxxxxxx %d yyyyyy %d \n", lX, lY);
				//	break;
			}
			if ((163 < lX) && (lX < 236) && (lY > 122))				//FIVE
					{
				if (dis_arrColor[3] == playColor)
					scorecnt++;
				else
					scorecnt--;
				System_printf("4scorevs %d dis_arrColor %x, playcolor %x /n",
						scorecnt, dis_arrColor[3], playColor);
				System_printf("44444444444xxxxxxx %d yyyyyy %d \n", lX, lY);
				//	break;
			}

			System_flush();

			//	System_printf("scorevs===== %d /n", scorecnt);
			if (scorecnt > MAX) {
				MAX = scorecnt;
			}

			TimerDisable(TIMER2_BASE, TIMER_A);
			//SysCtlPeripheralDisable(SYSCTL_PERIPH_TIMER2);
			TimerIntDisable(TIMER2_BASE, TIMER_TIMA_TIMEOUT);

			SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER2);// enable Timer 2 periph clks
			TimerConfigure(TIMER2_BASE, TIMER_CFG_PERIODIC);// cfg Timer 2 mode - periodic

			ui32Period = (SysCtlClockGet() * 2);// period = CPU clk div 2 (500ms)
			TimerLoadSet(TIMER2_BASE, TIMER_A, ui32Period);	// set Timer 2 period

			TimerIntEnable(TIMER2_BASE, TIMER_TIMA_TIMEOUT);// enables Timer 2 to interrupt CPU
			TimerEnable(TIMER2_BASE, TIMER_A);				// enable Timer 2
			Gate_leaveSystem(key);
		}
		random_num = dis_array(sixColor, 6);
		output(4, dis_arrColor, random_num);
	}
}

/*
 *  ======== gpioButtonFxn0 ========
 *  Callback function for the left button
 */
Void gpioButtonFxn0(Void) {
	MsgObj msg;
	Msg msgp;		// Queues pass POINTERS, so we need a pointer of type Msg
	msgp = &msg;

	msg.TouchCommand = LEFT;
	Queue_put(LED_Queue, (Queue_Elem*) msgp);// pass pointer to Message object via LED_Queue
	Semaphore_post(QueSem);

	GPIO_clearInt(EK_TM4C123GXL_SW1);				//??
}

/*
 *  ======== gpioButtonFxn1 ========
 *  Callback function for the right button
 */
Void gpioButtonFxn1(Void) {
	MsgObj msg;
	Msg msgp;		// Queues pass POINTERS, so we need a pointer of type Msg
	msgp = &msg;

	msg.TouchCommand = RIGHT;
	Queue_put(LED_Queue, (Queue_Elem*) msgp);// pass pointer to Message object via LED_Queue
	Semaphore_post(QueSem);

	GPIO_clearInt(EK_TM4C123GXL_SW2);				//??
}

/*
 *  ======== LCD_init ========
 */
Void LCD_init() {
	/* LCD driver init */
	Kentec320x240x16_SSD2119Init();
	GrContextInit(&context, &g_sKentec320x240x16_SSD2119);
	/* Setup font */
	GrContextFontSet(&context, g_psFontFixed6x8);
}
/*
 * Bounder set
 */
void Bounder_set(void) {

	// the whole boundler
	sRect.i16XMin = 0;
	sRect.i16YMin = 0;
	sRect.i16XMax = GrContextDpyWidthGet(&context) - 1;
	sRect.i16YMax = 239;

	GrContextForegroundSet(&context, ClrSeashell);
	GrRectFill(&context, &sRect);

	int i, j;
	/*	for (i = 0; i < 4; ++i) {
	 for (j = 0; j < 2; ++j) {
	 sRect.i16XMin = 10 + i * 76 + 1;				//75
	 sRect.i16YMin = 30 + j * 91 + 1;				//90
	 sRect.i16XMax = 85 + i * 76 - 1;
	 sRect.i16YMax = 120 + j * 91 - 1;

	 GrContextForegroundSet(&context, ClrSkyBlue);
	 GrRectFill(&context, &sRect);

	 }

	 }*/

	/*GrContextForegroundSet(&context, ClrSkyBlue);

	 GrContextFontSet(&context, &g_sFontCm20);
	 GrStringDrawCentered(&context, "Project2 Jie & Gary", -1,
	 GrContextDpyWidthGet(&context) / 2, 225, 0);*/
}
//---------------------------------------------------------------------------
// Timer_ISR()
//
// Called by Hwi when timer hits zero
//
// TimerIntClear is needed here because THIS fxn is the ISR now
//---------------------------------------------------------------------------
/*void Timer_ISR(void)
 {
 timerMsgObj timerMsg1;
 timerMsg timerMsgp = &timerMsg1;
 TimerIntClear(TIMER2_BASE, TIMER_TIMA_TIMEOUT);			// must clear timer flag FROM timer
 timerMsgp->clear = 1;
 //	Queue_put(LED_Queue, (Queue_Elem*) timerMsgp);// pass pointer to Message object via LED_Queue
 //	Semaphore_post(QueSem);
 }*/
/*void TimerTaskFxn(){
 timerMsgObj timerMsg1;
 timerMsg timerMsgp = &timerMsg1;
 int random_num =0;
 while (TRUE) {
 //			Semaphore_pend(QueSem, BIOS_WAIT_FOREVER);
 //		timerMsgp = Queue_get(LED_Queue);
 if( timerMsgp->clear == 1){ //is timerMsgp clear needed to be clean?

 random_num = dis_array(sixColor, 6);
 output(4, dis_arrColor, random_num);
 }

 }
 }*/
/*
 *  ======== main ========
 */

Int main(Void) {
//	int global_rand;
	global_rand = 0;
	scorecnt = 5;
	MAX = 5;
	displayTime = 0;
//	initcolorMsg();
	Mailbox_Params mboxParams;
//	Mailbox_Params mboxParams1;
	/*    Task_Params grlibTaskParams;
	 Task_Params consoleTaskParams;
	 Task_Handle grlibTaskHandle;
	 Task_Handle consoleTaskHandle;*/

	Error_Block eb;

	/* Init board-specific functions. */
	Board_initGeneral();
	Board_initGPIO();
	Board_initUART();
	Board_initUSB(Board_USBDEVICE);
	SysCtlClockSet(
	SYSCTL_SYSDIV_5 | SYSCTL_USE_PLL | SYSCTL_XTAL_16MHZ | SYSCTL_OSC_MAIN);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER2);// enable Timer 2 periph clks
	TimerConfigure(TIMER2_BASE, TIMER_CFG_PERIODIC);// cfg Timer 2 mode - periodic

	ui32Period = (SysCtlClockGet() * 2);	// period = CPU clk div 2 (500ms)
	TimerLoadSet(TIMER2_BASE, TIMER_A, ui32Period);		// set Timer 2 period

	TimerIntEnable(TIMER2_BASE, TIMER_TIMA_TIMEOUT);// enables Timer 2 to interrupt CPU

	TimerEnable(TIMER2_BASE, TIMER_A);						// enable Timer 2

//************timer3
	SysCtlClockSet(
	SYSCTL_SYSDIV_5 | SYSCTL_USE_PLL | SYSCTL_XTAL_16MHZ | SYSCTL_OSC_MAIN);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER3);// enable Timer 3 periph clks
	TimerConfigure(TIMER3_BASE, TIMER_CFG_PERIODIC);// cfg Timer 3 mode - periodic

	ui32Period = (SysCtlClockGet());	// period = CPU clk div 2 (1s)
	TimerLoadSet(TIMER3_BASE, TIMER_A, ui32Period);		// set Timer 2 period

	TimerIntEnable(TIMER3_BASE, TIMER_TIMA_TIMEOUT);// enables Timer 2 to interrupt CPU

	TimerEnable(TIMER3_BASE, TIMER_A);						// enable Timer 2
	/* Turn on user LED */
	//    GPIO_write(DK_TM4C123G_LED, DK_TM4C123G_LED_ON);
	add_device("UART", _MSA, UARTUtils_deviceopen, UARTUtils_deviceclose,
			UARTUtils_deviceread, UARTUtils_devicewrite, UARTUtils_devicelseek,
			UARTUtils_deviceunlink, UARTUtils_devicerename);

	/* Open UART0 for writing to stdout and set buffer */
	freopen("UART:0", "w", stdout);
	setvbuf(stdout, NULL, _IOLBF, 128);

	/* Open UART0 for reading from stdin and set buffer */
	freopen("UART:0", "r", stdin);
	setvbuf(stdin, NULL, _IOLBF, 128);

	/*
	 *  Initialize UART port 0 used by SysCallback.  This and other SysCallback
	 *  UART functions are implemented in UARTUtils.c. Calls to System_printf
	 *  will go to UART0, the same as printf.
	 */
	UARTUtils_systemInit(0);

	/* Init LCD and USBCDC */
	LCD_init();
	//    USBCDCD_init();

	TouchScreenInit();

	TouchScreenCallbackSet(TouchTestCallback);

	Bounder_set();

	LED_OFF();
	/* Init and enable interrupts */
	GPIO_setupCallbacks(&EK_TM4C123GXL_gpioPortFCallbacks);

	GPIO_enableInt(EK_TM4C123GXL_SW1, GPIO_INT_RISING);
	GPIO_enableInt(EK_TM4C123GXL_SW2, GPIO_INT_RISING);

	/* SYS/BIOS Mailbox create */
	Error_init(&eb);
	Mailbox_Params_init(&mboxParams);
	mailboxHandle = Mailbox_create(sizeof(DrawMessage), 2, &mboxParams, &eb);
	if (mailboxHandle == NULL) {
		System_abort("Mailbox create failed\nAborting...");
	}

	System_printf("Starting the example\n%s, %s",
			"System provider is set to SysMin",
			"halt the target and use ROV to view output.\n");

	/* SysMin will only print to the console when you call flush or exit */
	System_flush();

	USBCDCD_init();				//?????? why not working
	fillBox(4);
	fillBox(4);
	//for(testnum=0;testnum<4;testnum++){
	global_rand = dis_array(sixColor, 6);
	//	System_printf("the size of global_rand is %d\n", global_rand);

	output(4, dis_arrColor, global_rand);
	//output(4, Array2048);
	/* Start BIOS. Will not return from this call. */
	BIOS_start();

	return (0);
}
