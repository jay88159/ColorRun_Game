
/*
 *  ======== grlibdemo.c ========
 */
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
#include <ti/sysbios/knl/Queue.h>
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
#include "inc/hw_memmap.h"
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
extern int Array2048[10][10];


/* Screen size */
#define SCREEN_WIDTH 320
#define SCREEN_HEIGHT 240

/* File-list size */
#define FILELIST_LENGTH 7

/* Characters count each line */
#define CHARS_ONE_LINE 17

/* Buffer length for the receive buffer of USBCDC */
#define RECEIVE_LENGTH 3

/* Drive number used for FatFs */
#define DRIVE_NUM 0

/*!
 *  ======== DrawCommand ========
 *  Enum defines several drawing commands
 */
typedef enum DrawCommand{
    IMAGE,
    KeyBoadCOMMAND,
    ScreenCOMMAND
} DrawCommand;
typedef enum TouchCommand{
    UP,
    DOWN,
    LEFT,
    RIGHT
} TouchCommand;

int DrawColor[5]={ClrGreen,ClrYellow,ClrGold,ClrPurple,ClrRed};

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
typedef struct DrawMessage{
    DrawCommand drawCommand;
    UInt drawImageIndex;
    UInt touchPosition[2];
    UInt draw2048Count;//
    UInt draw2048Index;
//    Char drawDir[FILELIST_LENGTH][CHARS_ONE_LINE + 1];
    int draw2048[10][10]//???????
} DrawMessage ;

//------------------------
// for Queue - Part B
//------------------------
typedef struct MsgObj {
	Queue_Elem	elem;
	TouchCommand	TouchCommand;            		// message value
} MsgObj, *Msg;				// Use Msg as pointer to MsgObj


/* Holding the last command */
UChar lastCommand[CHARS_ONE_LINE + 1] = {'\0'};

/* Images */
extern const UChar image_TI_Black[];
extern const UChar image_TI_Ad[];
const UChar *image_Gallery[2] = {image_TI_Black, image_TI_Ad};

/* Up/Down arrow keys (ASCII code converted from USB keyboard) */
/*const UChar downArrow[4] = {0x1b, 0x5b, 0x42, 0x00};
const UChar upArrow[4] = {0x1b, 0x5b, 0x41, 0x00};*/

/* Handles created dynamically in the c */
Mailbox_Handle mailboxHandle = NULL;
Mailbox_Handle mailboxHandle1 = NULL;

/* Global context for drawing */
tContext context;
tRectangle sRect;

void output(int n,int (*drawArray)[10]){

	static char pcCanvasText[5];

	int i,j,color;
	for ( i = 0; i < 4; ++i){
		for ( j = 0; j < 4; ++j){
			sRect.i16XMin = 64+i*48 +1;//16
			sRect.i16YMin = 24+j*48 +1;//64
			sRect.i16XMax = 112+i*48 -1;
			sRect.i16YMax = 72+j*48 -1;

			color = drawArray[i][j]%5;
			GrContextForegroundSet(&context, DrawColor[color]);
			GrRectFill(&context, &sRect);

			usprintf(pcCanvasText, "%3d", drawArray[i][j]);

			GrContextForegroundSet(&context, ClrBlack);
		    GrContextFontSet(&context, &g_sFontCm20);
		    GrStringDraw(&context, pcCanvasText, -1, 64+i*48 +1, 24+j*48 +16, 0);


		}

	}
}

Void clearDisplay()
{
    tRectangle rect = {0, 0, SCREEN_WIDTH - 1, SCREEN_HEIGHT - 1};
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
static long
TouchTestCallback(uint32_t ui32Message, int32_t lX, int32_t lY)
{
	MsgObj msg;
		Msg msgp;													// Queues pass POINTERS, so we need a pointer of type Msg
		msgp = &msg;



	//
	// Check the message to determine what to do.
	//
	switch(ui32Message)
	{
	//
	// The screen is no longer being touched (in other words, pen/pointer
	// up).
	//
	case WIDGET_MSG_PTR_UP:
		{
		//
		// Handle the pointer up message if required.
		//
			 if(lX < 64)//left
									 {
									 msg.TouchCommand = LEFT;
									 	Queue_put(LED_Queue, (Queue_Elem*)msgp);				// pass pointer to Message object via LED_Queue
									 	Semaphore_post(QueSem);

									 }else if(lX > 256)//right
									 {
										 msg.TouchCommand = RIGHT;
										 	Queue_put(LED_Queue, (Queue_Elem*)msgp);				// pass pointer to Message object via LED_Queue
										 	Semaphore_post(QueSem);

									 }else if(lY < 36)//up
									 {
										 msg.TouchCommand = UP;
										 	Queue_put(LED_Queue, (Queue_Elem*)msgp);				// pass pointer to Message object via LED_Queue
										 	Semaphore_post(QueSem);

									 }else if(lY > 204)//down
									 {
										 msg.TouchCommand = DOWN;
										 	Queue_put(LED_Queue, (Queue_Elem*)msgp);				// pass pointer to Message object via LED_Queue
										 	Semaphore_post(QueSem);

									 }
		break;
		}
	//
	// The screen has just been touched (in other words, pen/pointer down).
	//
	case WIDGET_MSG_PTR_DOWN:
		{
		//
		// Handle the pointer down message if required.
		//

		break;
		}
	//
	// The location of the touch on the screen has moved (in other words,
	// the pen/pointer has moved).
	//
	case WIDGET_MSG_PTR_MOVE:
		{
		//
		// Handle the pointer move message if required.
		//
		break;
		}
	//
	// An unknown message was received.
	//
	default:
		{
		//
		// Ignore all unknown messages.
		//
		break;
		}
	}
	//
	// Success.
	//
return(0);
}
extern void TouchScreenIntHandler(void);
int32_t grlibTouchTaskFxn(uint32_t ulMessage, int32_t lX,
        int32_t lY)
{
	MsgObj msg;
	Msg msgp;													// Queues pass POINTERS, so we need a pointer of type Msg
	msgp = &msg;


	 if(lX < 64)//left
		 {
		 msg.TouchCommand = LEFT;
		 	Queue_put(LED_Queue, (Queue_Elem*)msgp);				// pass pointer to Message object via LED_Queue
		 	Semaphore_post(QueSem);

		 }else if(lX > 256)//right
		 {
			 msg.TouchCommand = RIGHT;
			 	Queue_put(LED_Queue, (Queue_Elem*)msgp);				// pass pointer to Message object via LED_Queue
			 	Semaphore_post(QueSem);

		 }else if(lY < 36)//up
		 {
			 msg.TouchCommand = UP;
			 	Queue_put(LED_Queue, (Queue_Elem*)msgp);				// pass pointer to Message object via LED_Queue
			 	Semaphore_post(QueSem);

		 }else if(lY > 204)//down
		 {
			 msg.TouchCommand = DOWN;
			 	Queue_put(LED_Queue, (Queue_Elem*)msgp);				// pass pointer to Message object via LED_Queue
			 	Semaphore_post(QueSem);

		 }
	/* DrawMessage drawMsg;
	 UInt key;


	 drawMsg.drawCommand = ScreenCOMMAND;
	 drawMsg.touchPosition[0]=lX;
	 drawMsg.touchPosition[1]=lY;
	 Mailbox_post(mailboxHandle, &drawMsg, BIOS_NO_WAIT);

	 //
	     // Enable the ADC sample sequence interrupt.
	     //



	 if(lX < 64)//left
	 {
		drawMsg.drawCommand = ScreenCOMMAND;
		drawMsg.touchPosition[0]=lX;
		drawMsg.touchPosition[1]=lY;
		key = Gate_enterSystem();
		up_remove_blank(4);
		up(4);


		copyFrom2048ArrayToDrawMessage(4,drawMsg.draw2048 , Array2048);

		Gate_leaveSystem(key);

		Mailbox_post(mailboxHandle, &drawMsg, BIOS_NO_WAIT);

	 }else if(lX > 256)//right
	 {
		drawMsg.drawCommand = ScreenCOMMAND;

		 key = Gate_enterSystem();
		 down_remove_blank(4);
		 down(4);
		 copyFrom2048ArrayToDrawMessage(4,drawMsg.draw2048 , Array2048);
		 Gate_leaveSystem(key);
		 Mailbox_post(mailboxHandle, &drawMsg, BIOS_NO_WAIT);

	 }else if(lY < 36)//up
	 {
		drawMsg.drawCommand = ScreenCOMMAND;

		 key = Gate_enterSystem();
		 left_remove_blank(4);
		 left(4);
		 copyFrom2048ArrayToDrawMessage(4,drawMsg.draw2048 , Array2048);
		 Gate_leaveSystem(key);
		Mailbox_post(mailboxHandle, &drawMsg, BIOS_NO_WAIT);
	 }else if(lY > 204)//down
	 {
		drawMsg.drawCommand = ScreenCOMMAND;
		 key = Gate_enterSystem();
		 right_remove_blank(4);
		 right(4);
		 copyFrom2048ArrayToDrawMessage(4,drawMsg.draw2048 , Array2048);
		 Gate_leaveSystem(key);
		 Mailbox_post(mailboxHandle, &drawMsg, BIOS_NO_WAIT);
	 }
*/
	 LED_OFF();
	 SysCtlDelay(6700000);
	 LED_ON();
}
/*
 *  ======== grlibTaskFxn ========
 *  Drawing task
 *
 *  It is pending for the message either from console task or from button ISR.
 *  Once the messages received, it draws to the screen based on information
 *  contained in the message.
 */
Void grlibTaskFxn(UArg arg0, UArg arg1)
{
    DrawMessage curMsg;
    const UChar *pucCurImage;
    UInt key;
    UInt fontHeight = GrStringHeightGet(&context);

    while (TRUE) {
        Mailbox_pend(mailboxHandle, &curMsg, BIOS_WAIT_FOREVER);

        /* Clear screen before drawing */
//        clearDisplay();

        /* Parse the message and draw */
        switch (curMsg.drawCommand) {
        case IMAGE:
            pucCurImage = image_Gallery[curMsg.drawImageIndex];

            // Draw image at (0,0)
            GrImageDraw(&context, pucCurImage, 0, 0);
        	 // copy the data to message


            break;

        case KeyBoadCOMMAND:

        	output(4,curMsg.draw2048);

            break;

        case ScreenCOMMAND:
        	// copy the data to message
        	key = Gate_enterSystem();
        	fillBox(4);
        	Gate_leaveSystem(key);
        	output(4,curMsg.draw2048);
        	System_printf("here\n");
        	printf("data X %d , data Y %d /n",curMsg.touchPosition[0],curMsg.touchPosition[1]);
        	break;

        default:
            break;
        }
    }
}
Void grlibTaskFxn1(UArg arg0, UArg arg1)
{
	MsgObj msg;
	Msg msgp;																		//define pointer to MsgObj to use with queue put/get
	msgp = &msg;
	const UChar *pucCurImage;
	UInt key;

    while (TRUE) {
    	Semaphore_pend(QueSem, BIOS_WAIT_FOREVER);

    	//QUEUE error maybe clear the queue before using?
    	msgp = Queue_get(LED_Queue);
        /* Clear screen before drawing */
//        clearDisplay();

        /* Parse the message and draw */
        if(msgp->TouchCommand == LEFT)
        {  pucCurImage = image_Gallery[0];


//             Draw image at (0,0)
            GrImageDraw(&context, pucCurImage, 0, 0);
        	 // copy the data to message
//			key = Gate_enterSystem();
//			up_remove_blank(4);
//			up(4);
//
//
//
////			copyFrom2048ArrayToDrawMessage(4,curMsg.draw2048 , Array2048);
//			Gate_leaveSystem(key);
//
////			fillBox(4);
//			output(4,Array2048);

        }

        if(msgp->TouchCommand == RIGHT)
        {          pucCurImage = image_Gallery[1];


        //             Draw image at (0,0)
                    GrImageDraw(&context, pucCurImage, 0, 0);
                	 // copy the data to message
                    // copy the data to message
//					key = Gate_enterSystem();
//					down_remove_blank(4);
//					down(4);
//					Gate_leaveSystem(key);
//
//        //			fillBox(4);
//        			output(4,Array2048);

        }
        if(msgp->TouchCommand == UP)
                {          pucCurImage = image_Gallery[0];


                //             Draw image at (0,0)
                            GrImageDraw(&context, pucCurImage, 0, 0);
                        	 // copy the data to message
                            // copy the data to message
//                            key = Gate_enterSystem();
//							left_remove_blank(4);
//							left(4);
//
//
//							Gate_leaveSystem(key);
//
//                //			fillBox(4);
//                			output(4,Array2048);

                }
        if (msgp->TouchCommand == DOWN)
                        {          pucCurImage = image_Gallery[0];


                        //             Draw image at (0,0)
                                    GrImageDraw(&context, pucCurImage, 0, 0);
                                	 // copy the data to message
                                    // copy the data to message
//                                    key = Gate_enterSystem();
//									right_remove_blank(4);
//									right(4);
//
//									Gate_leaveSystem(key);
//
//                        //			fillBox(4);
//                        			output(4,Array2048);

                        }
    }
}

/*
 *  ======== gpioButtonFxn0 ========
 *  Callback function for the left button
 *
 *  It posts a message with image index 0 to display.
 */
Void gpioButtonFxn0(Void)
{
	MsgObj msg;
	Msg msgp;													// Queues pass POINTERS, so we need a pointer of type Msg
	msgp = &msg;
	UInt key;
	key = Gate_enterSystem();
	msg.TouchCommand = LEFT;
	Queue_put(LED_Queue, (Queue_Elem*)msgp);				// pass pointer to Message object via LED_Queue
	Semaphore_post(QueSem);
	Gate_leaveSystem(key);
    /*UInt key;
    DrawMessage drawMsg;

    drawMsg.drawCommand = IMAGE;
    drawMsg.drawImageIndex = 0;

    key = Gate_enterSystem();
     Clear the last command
    lastCommand[0] = 0x0;
    Gate_leaveSystem(key);

     Do not wait if there is no room for the new mail
    //我觉得没有ready 正在等待的task 不过 用其他的就可以啊
    Mailbox_post(mailboxHandle, &drawMsg, BIOS_NO_WAIT);*/

    GPIO_clearInt(EK_TM4C123GXL_SW1);//??
}

/*
 *  ======== gpioButtonFxn1 ========
 *  Callback function for the right button
 *
 *  It posts a message with image index 1 to display.
 */
Void gpioButtonFxn1(Void)
{
	MsgObj msg;
	Msg msgp;													// Queues pass POINTERS, so we need a pointer of type Msg
	msgp = &msg;
	UInt key;
	key = Gate_enterSystem();

	msg.TouchCommand = RIGHT;
	Queue_put(LED_Queue, (Queue_Elem*)msgp);				// pass pointer to Message object via LED_Queue
	Semaphore_post(QueSem);
	Gate_leaveSystem(key);
    /*UInt key;
    DrawMessage drawMsg;

    drawMsg.drawCommand = IMAGE;
    drawMsg.drawImageIndex = 1;

    key = Gate_enterSystem();
     Clear the last command
    lastCommand[0] = 0x0;
    Gate_leaveSystem(key);

     Do not wait if there is no room for the new mail
    Mailbox_post(mailboxHandle1, &drawMsg, BIOS_NO_WAIT);*/

    GPIO_clearInt(EK_TM4C123GXL_SW2);//??
}

/*
 *  ======== consoleTaskFxn ========
 *  Console task
 *
 *  This task listens to the key pressed in the keyboard through USBCDC.
 *  The string ended with return character '\n' will trigger the task
 *  to send this string to the mailbox.
 *  For example, when the user enter "ls\n", this task will scan all the
 *  files in the root of SD card and send the file list to the mailbox to
 *  inform the drawing task to display on the screen.
 *  The up/down arrow can be used to scroll up/down to display more files
 *  in the SD card.
 */
Void consoleTaskFxn (UArg arg0, UArg arg1)
{
	    unsigned int count;
	    unsigned int cpuLoad;
	    char input[128];
	    UInt key;
	    DrawMessage drawMsg;
	    count = 1;

	    /* printf goes to the UART com port */
	    printf("\f======== Welcome to the Console ========\n");
	    printf("Enter a command followed by return.\n"
	           "Type help for a list of commands.\n\n");

	    printf("%d %% ", count++);
	    fflush(stdout);

	    /* Loop forever receiving commands */
	    while(true) {
	        /* Get the user's input */
	        scanf("%s", input);
	        /* Flush the remaining characters from stdin since they are not used. */
	        fflush(stdin);

	        if (!strcmp(input, "a")) {
	            /* Print the CPU load */
	            cpuLoad = Load_getCPULoad();
	            printf("CPU Load: %d\n", cpuLoad);
	            drawMsg.drawCommand = KeyBoadCOMMAND;


	            // copy the data to message
	           	key = Gate_enterSystem();
	            up_remove_blank(4);
	            up(4);



	            copyFrom2048ArrayToDrawMessage(4,drawMsg.draw2048 , Array2048);
	            Gate_leaveSystem(key);

	            Mailbox_post(mailboxHandle, &drawMsg, BIOS_WAIT_FOREVER);
	        }
	        else if (!strcmp(input, "d")) {
	        	 /* Print the CPU load */
				cpuLoad = Load_getCPULoad();
				printf("CPU Load: %d\n", cpuLoad);
				drawMsg.drawCommand = KeyBoadCOMMAND;
				// copy the data to message
				key = Gate_enterSystem();
	        	down_remove_blank(4);
				down(4);



				copyFrom2048ArrayToDrawMessage(4,drawMsg.draw2048 , Array2048);
				Gate_leaveSystem(key);

				Mailbox_post(mailboxHandle, &drawMsg, BIOS_WAIT_FOREVER);
	        }else if (!strcmp(input, "w")) {
	        	 /* Print the CPU load */
				cpuLoad = Load_getCPULoad();
				printf("CPU Load: %d\n", cpuLoad);
				drawMsg.drawCommand = KeyBoadCOMMAND;
				// copy the data to message
				key = Gate_enterSystem();
				left_remove_blank(4);
				left(4);


				copyFrom2048ArrayToDrawMessage(4,drawMsg.draw2048 , Array2048);
				Gate_leaveSystem(key);

				Mailbox_post(mailboxHandle, &drawMsg, BIOS_WAIT_FOREVER);
	        }else if (!strcmp(input, "s")) {
	        	 /* Print the CPU load */
				cpuLoad = Load_getCPULoad();
				printf("CPU Load: %d\n", cpuLoad);
				drawMsg.drawCommand = KeyBoadCOMMAND;
				// copy the data to message
				key = Gate_enterSystem();
				right_remove_blank(4);
				right(4);

				copyFrom2048ArrayToDrawMessage(4,drawMsg.draw2048 , Array2048);
				Gate_leaveSystem(key);

				Mailbox_post(mailboxHandle, &drawMsg, BIOS_WAIT_FOREVER);
	        }
	        else if (!strcmp(input, "exit")) {
	            /* Exit the console task */
	            printf("Are you sure you want to exit the console? Y/N: ");
	            fflush(stdout);
	            scanf("%s", input);
	            fflush(stdin);
	            if ((input[0] == 'y' || input[0] == 'Y') && input[1] == 0x00) {
	                printf("Exiting console, goodbye.\n");
	                Task_exit();
	            }
	        }
	        else {
	            /* Print a list of valid commands. */
	            printf("Valid commands:\n"
	                   "- w: move up.\n"
	                   "- a: move left.\n"
	                   "- s: move down.\n"
	            	   "- a: move right.\n"
	            	   "- exit: Exit the console task.\n");
	        }

	        fillBox(4);
//	        printf("%d %% ", count++);
	        fflush(stdout);
	    }

}


/*
 *  ======== LCD_init ========
 */
Void LCD_init()
{
    /* LCD driver init */
    Kentec320x240x16_SSD2119Init();
    GrContextInit(&context, &g_sKentec320x240x16_SSD2119);
    /* Setup font */
    GrContextFontSet(&context, g_psFontFixed6x8);
}
/*
 * Bounder set
 */
void Bounder_set(void)
{


	    // the whole boundler
		sRect.i16XMin = 0;
		sRect.i16YMin = 0;
		sRect.i16XMax = GrContextDpyWidthGet(&context) - 1;
		sRect.i16YMax = 239;

		GrContextForegroundSet(&context, ClrSeashell);
		GrRectFill(&context, &sRect);



		int i,j;
		for ( i = 0; i < 4; ++i){
			for ( j = 0; j < 4; ++j){
				sRect.i16XMin = 64+i*48 +1;//16
				sRect.i16YMin = 24+j*48 +1;//64
				sRect.i16XMax = 112+i*48 -1;
				sRect.i16YMax = 72+j*48 -1;

				GrContextForegroundSet(&context, ClrSkyBlue);
				GrRectFill(&context, &sRect);

			}

		}

		GrContextForegroundSet(&context, ClrSkyBlue);
		GrContextFontSet(&context, &g_sFontCm20);
		GrStringDrawCentered(&context, "Total  256", -1,
							 GrContextDpyWidthGet(&context) / 2, 8, 0);

		GrContextFontSet(&context, &g_sFontCm20);
		GrStringDrawCentered(&context, "Project2 ^_^ Jack&Peter", -1,
		GrContextDpyWidthGet(&context) / 2, 225, 0);
}
/*
 *  ======== main ========
 */

Int main(Void)
{

    Mailbox_Params mboxParams;
    Mailbox_Params mboxParams1;
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
    /* Turn on user LED */
//    GPIO_write(DK_TM4C123G_LED, DK_TM4C123G_LED_ON);

     add_device("UART", _MSA, UARTUtils_deviceopen,
                UARTUtils_deviceclose, UARTUtils_deviceread,
                UARTUtils_devicewrite, UARTUtils_devicelseek,
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

    /* SYS/BIOS Mailbox create */
       Error_init(&eb);
       Mailbox_Params_init(&mboxParams1);
       mailboxHandle1 = Mailbox_create(sizeof(DrawMessage), 2, &mboxParams1, &eb);
       if (mailboxHandle == NULL) {
           System_abort("Mailbox1 create failed\nAborting...");
       }

    /* Console task create */
   /* Error_init(&eb);
    Task_Params_init(&consoleTaskParams);
    consoleTaskParams.instance->name = "consoleTask";
    consoleTaskParams.stackSize = 1024;
    consoleTaskParams.priority = 2;
    consoleTaskHandle = Task_create(consoleTaskFxn, &consoleTaskParams, &eb);
    if (consoleTaskHandle == NULL) {
        System_abort("Console task was not created\nAborting...");
    }

     Grlib task create
    Error_init(&eb);
    Task_Params_init(&grlibTaskParams);
    grlibTaskParams.instance->name = "grlibTask";
    grlibTaskParams.stackSize = 2048;
    grlibTaskParams.priority = 1;
    grlibTaskHandle = Task_create(grlibTaskFxn, &grlibTaskParams, &eb);
    if (grlibTaskHandle == NULL) {
        System_abort("Grlib task was not created\nAborting...");
    }*/

    System_printf("Starting the example\n%s, %s",
            "System provider is set to SysMin",
            "halt the target and use ROV to view output.\n");

    /* SysMin will only print to the console when you call flush or exit */
    System_flush();

    USBCDCD_init();//?????? why not working
    fillBox(4);
	fillBox(4);
	output(4,Array2048);
    /* Start BIOS. Will not return from this call. */
    BIOS_start();

    return (0);
}
