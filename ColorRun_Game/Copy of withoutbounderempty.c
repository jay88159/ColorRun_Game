
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
    Char drawString[CHARS_ONE_LINE + 1];
    UInt draw2048Count;//
    UInt draw2048Index;
//    Char drawDir[FILELIST_LENGTH][CHARS_ONE_LINE + 1];
    int draw2048[10][10]//???????
} DrawMessage ;

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

/* Global context for drawing */
tContext context;
tRectangle sRect;

void output(int n,int (*drawArray)[10]){
	int i,j;
	static char pcCanvasText[5];

	GrContextForegroundSet(&context, ClrSilver);
	GrContextFontSet(&context, &g_sFontCm20);

	for ( i = 0; i < 4; ++i){
		for ( j = 0; j < 4; ++j){

//			 printf("%d",drawArray[i][j]);
			 usprintf(pcCanvasText, "%3d", drawArray[i][j]);

			 GrStringDraw(&context, pcCanvasText, -1, 64+i*48 +1, 24+j*48 +1, 0);
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
extern void TouchScreenIntHandler(void);
int32_t grlibTouchTaskFxn(uint32_t ulMessage, int32_t lX,
        int32_t lY)
{

//	 System_printf("grlibTouchTaskFxn--------");
	 LED_OFF();
	 SysCtlDelay(6700000);
	 LED_ON();

			    /* SysMin will only print to the console when you call flush or exit */
//		System_flush();
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
    const Char *pcCurStr;
    UInt i = 0;
    UInt fontHeight = GrStringHeightGet(&context);

    while (TRUE) {
        Mailbox_pend(mailboxHandle, &curMsg, BIOS_WAIT_FOREVER);

        /* Clear screen before drawing */
        clearDisplay();

        /* Parse the message and draw */
        switch (curMsg.drawCommand) {
        case IMAGE:
            pucCurImage = image_Gallery[curMsg.drawImageIndex];

            /* Draw image at (0,0) */
            GrImageDraw(&context, pucCurImage, 0, 0);
            break;

        case KeyBoadCOMMAND:

        	output(4,curMsg.draw2048);

            break;

        case ScreenCOMMAND:
            pcCurStr = curMsg.drawString;
            GrContextForegroundSetTranslated(&context, 0xFF);
            GrStringDraw(&context, pcCurStr, -1, 0, 0, 0);
            break;

        default:
            break;
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
    UInt key;
    DrawMessage drawMsg;

    drawMsg.drawCommand = IMAGE;
    drawMsg.drawImageIndex = 0;

    key = Gate_enterSystem();
    /* Clear the last command */
    lastCommand[0] = 0x0;
    Gate_leaveSystem(key);

    /* Do not wait if there is no room for the new mail */
    Mailbox_post(mailboxHandle, &drawMsg, BIOS_NO_WAIT);

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
    UInt key;
    DrawMessage drawMsg;

    drawMsg.drawCommand = IMAGE;
    drawMsg.drawImageIndex = 1;

    key = Gate_enterSystem();
    /* Clear the last command */
    lastCommand[0] = 0x0;
    Gate_leaveSystem(key);

    /* Do not wait if there is no room for the new mail */
    Mailbox_post(mailboxHandle, &drawMsg, BIOS_NO_WAIT);

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


	            up_remove_blank(4);
	            up(4);


	            // copy the data to message
	            key = Gate_enterSystem();
	            copyFrom2048ArrayToDrawMessage(4,drawMsg.draw2048 , Array2048);
	            Gate_leaveSystem(key);

	            Mailbox_post(mailboxHandle, &drawMsg, BIOS_WAIT_FOREVER);
	        }
	        else if (!strcmp(input, "d")) {
	        	 /* Print the CPU load */
				cpuLoad = Load_getCPULoad();
				printf("CPU Load: %d\n", cpuLoad);
				drawMsg.drawCommand = KeyBoadCOMMAND;

	        	down_remove_blank(4);
				down(4);


				// copy the data to message
				key = Gate_enterSystem();
				copyFrom2048ArrayToDrawMessage(4,drawMsg.draw2048 , Array2048);
				Gate_leaveSystem(key);

				Mailbox_post(mailboxHandle, &drawMsg, BIOS_WAIT_FOREVER);
	        }else if (!strcmp(input, "w")) {
	        	 /* Print the CPU load */
				cpuLoad = Load_getCPULoad();
				printf("CPU Load: %d\n", cpuLoad);
				drawMsg.drawCommand = KeyBoadCOMMAND;

				left_remove_blank(4);
				left(4);


				// copy the data to message
				key = Gate_enterSystem();
				copyFrom2048ArrayToDrawMessage(4,drawMsg.draw2048 , Array2048);
				Gate_leaveSystem(key);

				Mailbox_post(mailboxHandle, &drawMsg, BIOS_WAIT_FOREVER);
	        }else if (!strcmp(input, "s")) {
	        	 /* Print the CPU load */
				cpuLoad = Load_getCPULoad();
				printf("CPU Load: %d\n", cpuLoad);
				drawMsg.drawCommand = KeyBoadCOMMAND;

				right_remove_blank(4);
				right(4);


				// copy the data to message
				key = Gate_enterSystem();
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

    TouchScreenCallbackSet(grlibTouchTaskFxn);


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
