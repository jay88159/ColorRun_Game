
/*
 *  ======== grlibdemo.c ========
 */
#include <file.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
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


#include "driverlib/fpu.h"
#include "driverlib/gpio.h"
#include "driverlib/flash.h"
#include "driverlib/sysctl.h"
#include "driverlib/systick.h"
#include "driverlib/udma.h"
/* Graphiclib Header file */
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
#include "grimages.h"
#include "touch.h"
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
    COMMAND,
    STRING
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
    UInt drawDirCount;
    Char drawDir[FILELIST_LENGTH][CHARS_ONE_LINE + 1];
} DrawMessage;

/* Holding the last command */
UChar lastCommand[CHARS_ONE_LINE + 1] = {'\0'};

/* Images */
extern const UChar image_TI_Black[];
extern const UChar image_TI_Ad[];
const UChar *image_Gallery[2] = {image_TI_Black, image_TI_Ad};

/* Up/Down arrow keys (ASCII code converted from USB keyboard) */
const UChar downArrow[4] = {0x1b, 0x5b, 0x42, 0x00};
const UChar upArrow[4] = {0x1b, 0x5b, 0x41, 0x00};

/* Handles created dynamically in the c */
Mailbox_Handle mailboxHandle = NULL;

/* Global context for drawing */
tContext context;



/**
 * Test here
 *
 */
//*****************************************************************************
//
// The DMA control structure table.
//
//*****************************************************************************
#ifdef ewarm
#pragma data_alignment=1024
tDMAControlTable sDMAControlTable[64];
#elif defined(ccs)
#pragma DATA_ALIGN(sDMAControlTable, 1024)
tDMAControlTable sDMAControlTable[64];
#else
tDMAControlTable sDMAControlTable[64] __attribute__ ((aligned(1024)));
#endif


void OnPrevious(tWidget *pWidget);
void OnNext(tWidget *pWidget);
void OnIntroPaint(tWidget *pWidget, tContext *pContext);
void OnPrimitivePaint(tWidget *pWidget, tContext *pContext);
extern tCanvasWidget g_psPanels[];

//*****************************************************************************
//
// The first panel, which contains introductory text explaining the
// application.
//
//*****************************************************************************
Canvas(g_sIntroduction, g_psPanels, 0, 0, &g_sKentec320x240x16_SSD2119, 0, 24,
       320, 166, CANVAS_STYLE_APP_DRAWN, 0, 0, 0, 0, 0, 0, OnIntroPaint);

//*****************************************************************************
//
// The second panel, which demonstrates the graphics primitives.
//
//*****************************************************************************
Canvas(g_sPrimitives, g_psPanels + 1, 0, 0, &g_sKentec320x240x16_SSD2119, 0,
       24, 320, 166, CANVAS_STYLE_APP_DRAWN, 0, 0, 0, 0, 0, 0,
       OnPrimitivePaint);

tCanvasWidget g_psPanels[] =
{
    CanvasStruct(0, 0, &g_sIntroduction, &g_sKentec320x240x16_SSD2119, 0, 24,
                 320, 166, CANVAS_STYLE_FILL, ClrBlack, 0, 0, 0, 0, 0, 0),
    CanvasStruct(0, 0, &g_sPrimitives, &g_sKentec320x240x16_SSD2119, 0, 24,
                 320, 166, CANVAS_STYLE_FILL, ClrBlack, 0, 0, 0, 0, 0, 0)
};

#define NUM_PANELS              (sizeof(g_psPanels) / sizeof(g_psPanels[0]))

char *g_pcPanelNames[] =
{
    "     Introduction     ",
    "     Primitives     "

};
//*****************************************************************************
//
// The buttons and text across the bottom of the screen.
//
//*****************************************************************************
RectangularButton(g_sPrevious, 0, 0, 0, &g_sKentec320x240x16_SSD2119, 0, 190,
                  50, 50, PB_STYLE_FILL, ClrBlack, ClrBlack, 0, ClrSilver,
                  &g_sFontCm20, "-", g_pucBlue50x50, g_pucBlue50x50Press, 0, 0,
                  OnPrevious);

Canvas(g_sTitle, 0, 0, 0, &g_sKentec320x240x16_SSD2119, 50, 190, 220, 50,
       CANVAS_STYLE_TEXT | CANVAS_STYLE_TEXT_OPAQUE, 0, 0, ClrSilver,
       &g_sFontCm20, 0, 0, 0);

RectangularButton(g_sNext, 0, 0, 0, &g_sKentec320x240x16_SSD2119, 270, 190,
                  50, 50, PB_STYLE_IMG | PB_STYLE_TEXT, ClrBlack, ClrBlack, 0,
                  ClrSilver, &g_sFontCm20, "+", g_pucBlue50x50,
                  g_pucBlue50x50Press, 0, 0, OnNext);
uint32_t g_ulPanel;
//*****************************************************************************
//
// Handles presses of the previous panel button.
//
//*****************************************************************************
void
OnPrevious(tWidget *pWidget)
{
    //
    // There is nothing to be done if the first panel is already being
    // displayed.
    //
    if(g_ulPanel == 0)
    {
        return;
    }

    //
    // Remove the current panel.
    //
    WidgetRemove((tWidget *)(g_psPanels + g_ulPanel));

    //
    // Decrement the panel index.
    //
    g_ulPanel--;

    //
    // Add and draw the new panel.
    //
    WidgetAdd(WIDGET_ROOT, (tWidget *)(g_psPanels + g_ulPanel));
    WidgetPaint((tWidget *)(g_psPanels + g_ulPanel));

    //
    // Set the title of this panel.
    //
    CanvasTextSet(&g_sTitle, g_pcPanelNames[g_ulPanel]);
    WidgetPaint((tWidget *)&g_sTitle);

    //
    // See if this is the first panel.
    //
    if(g_ulPanel == 0)
    {
        //
        // Clear the previous button from the display since the first panel is
        // being displayed.
        //
        PushButtonImageOff(&g_sPrevious);
        PushButtonTextOff(&g_sPrevious);
        PushButtonFillOn(&g_sPrevious);
        WidgetPaint((tWidget *)&g_sPrevious);
    }

    //
    // See if the previous panel was the last panel.
    //
    if(g_ulPanel == (NUM_PANELS - 2))
    {
        //
        // Display the next button.
        //
        PushButtonImageOn(&g_sNext);
        PushButtonTextOn(&g_sNext);
        PushButtonFillOff(&g_sNext);
        WidgetPaint((tWidget *)&g_sNext);
    }

}

//*****************************************************************************
//
// Handles presses of the next panel button.
//
//*****************************************************************************
void
OnNext(tWidget *pWidget)
{
    //
    // There is nothing to be done if the last panel is already being
    // displayed.
    //
    if(g_ulPanel == (NUM_PANELS - 1))
    {
        return;
    }

    //
    // Remove the current panel.
    //
    WidgetRemove((tWidget *)(g_psPanels + g_ulPanel));

    //
    // Increment the panel index.
    //
    g_ulPanel++;

    //
    // Add and draw the new panel.
    //
    WidgetAdd(WIDGET_ROOT, (tWidget *)(g_psPanels + g_ulPanel));
    WidgetPaint((tWidget *)(g_psPanels + g_ulPanel));

    //
    // Set the title of this panel.
    //
    CanvasTextSet(&g_sTitle, g_pcPanelNames[g_ulPanel]);
    WidgetPaint((tWidget *)&g_sTitle);

    //
    // See if the previous panel was the first panel.
    //
    if(g_ulPanel == 1)
    {
        //
        // Display the previous button.
        //
        PushButtonImageOn(&g_sPrevious);
        PushButtonTextOn(&g_sPrevious);
        PushButtonFillOff(&g_sPrevious);
        WidgetPaint((tWidget *)&g_sPrevious);
    }

    //
    // See if this is the last panel.
    //
    if(g_ulPanel == (NUM_PANELS - 1))
    {
        //
        // Clear the next button from the display since the last panel is being
        // displayed.
        //
        PushButtonImageOff(&g_sNext);
        PushButtonTextOff(&g_sNext);
        PushButtonFillOn(&g_sNext);
        WidgetPaint((tWidget *)&g_sNext);
    }

}
void
OnIntroPaint(tWidget *pWidget, tContext *pContext)
{
    //
    // Display the introduction text in the canvas.
    //
    GrContextFontSet(pContext, &g_sFontCm18);
    GrContextForegroundSet(pContext, ClrSilver);
    GrStringDraw(pContext, "This application demonstrates the Stellaris", -1,
                 0, 32, 0);
    GrStringDraw(pContext, "Graphics Library.", -1, 0, 50, 0);
    GrStringDraw(pContext, "Each panel shows a different feature of", -1, 0,
                 74, 0);
    GrStringDraw(pContext, "the graphics library. Widgets on the panels", -1, 0,
                 92, 0);
    GrStringDraw(pContext, "are fully operational; pressing them will", -1, 0,
                 110, 0);
    GrStringDraw(pContext, "result in visible feedback of some kind.", -1, 0,
                 128, 0);
    GrStringDraw(pContext, "Press the + and - buttons at the bottom", -1, 0,
                 146, 0);
    GrStringDraw(pContext, "of the screen to move between the panels.", -1, 0,
                 164, 0);
}

//*****************************************************************************
//
// Handles paint requests for the primitives canvas widget.
//
//*****************************************************************************
void
OnPrimitivePaint(tWidget *pWidget, tContext *pContext)
{
    uint32_t ulIdx;
    tRectangle sRect;

    //
    // Draw a vertical sweep of lines from red to green.
    //
    for(ulIdx = 0; ulIdx <= 8; ulIdx++)
    {
        GrContextForegroundSet(pContext,
                               (((((10 - ulIdx) * 255) / 10) << ClrRedShift) |
                                (((ulIdx * 255) / 10) << ClrGreenShift)));
        GrLineDraw(pContext, 115, 120, 5, 120 - (11 * ulIdx));
    }

    //
    // Draw a horizontal sweep of lines from green to blue.
    //
    for(ulIdx = 1; ulIdx <= 10; ulIdx++)
    {
        GrContextForegroundSet(pContext,
                               (((((10 - ulIdx) * 255) / 10) <<
                                 ClrGreenShift) |
                                (((ulIdx * 255) / 10) << ClrBlueShift)));
        GrLineDraw(pContext, 115, 120, 5 + (ulIdx * 11), 29);
    }

    //
    // Draw a filled circle with an overlapping circle.
    //
    GrContextForegroundSet(pContext, ClrBrown);
    GrCircleFill(pContext, 185, 69, 40);
    GrContextForegroundSet(pContext, ClrSkyBlue);
    GrCircleDraw(pContext, 205, 99, 30);

    //
    // Draw a filled rectangle with an overlapping rectangle.
    //
    GrContextForegroundSet(pContext, ClrSlateGray);
    sRect.i16XMin = 20;
    sRect.i16YMin = 100;
    sRect.i16XMax = 75;
    sRect.i16YMax = 160;
    GrRectFill(pContext, &sRect);
    GrContextForegroundSet(pContext, ClrSlateBlue);
    sRect.i16XMin += 40;
    sRect.i16YMin += 40;
    sRect.i16XMax += 30;
    sRect.i16YMax += 28;
    GrRectDraw(pContext, &sRect);

    //
    // Draw a piece of text in fonts of increasing size.
    //
    GrContextForegroundSet(pContext, ClrSilver);
    GrContextFontSet(pContext, &g_sFontCm14);
    GrStringDraw(pContext, "Strings", -1, 125, 110, 0);
    GrContextFontSet(pContext, &g_sFontCm18);
    GrStringDraw(pContext, "Strings", -1, 145, 124, 0);
    GrContextFontSet(pContext, &g_sFontCm22);
    GrStringDraw(pContext, "Strings", -1, 165, 142, 0);
    GrContextFontSet(pContext, &g_sFontCm24);
    GrStringDraw(pContext, "Strings", -1, 185, 162, 0);

    //
    // Draw an image.
    //
    GrImageDraw(pContext, g_pucLogo, 270, 80);
}

//extern volatile int16_t g_sTouchX;

//*****************************************************************************
//
// The most recent raw ADC reading for the Y position on the screen.  This
// value is not affected by the selected screen orientation.
//
//*****************************************************************************
//extern volatile int16_t g_sTouchY;
/*
 *  ======== clearDisplay ========
 *  Clear the screen with black color
 */
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
/*Void grlibTouchTaskFxn(UArg arg0, UArg arg1)
{

	System_printf("coming here");
	WidgetMessageQueueProcess();
}*/
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

        case COMMAND:
            pcCurStr = curMsg.drawString;

            /* Set foreground color white (0xFF) */
            GrContextForegroundSetTranslated(&context, 0xFF);

            /* Draw string at (0,0) */
            GrStringDraw(&context, pcCurStr, -1, 0, 0, 0);

            for (i = 0; i < curMsg.drawDirCount; i++) {
                pcCurStr = curMsg.drawDir[i];

                /* Draw string for each line */
                GrStringDraw(&context, pcCurStr, -1, 0,
                            (i + 1) * fontHeight, 0);
            }
            break;

        case STRING:
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
	    unsigned int sleepDur;
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

	        if (!strcmp(input, "w")) {
	            /* Print the CPU load */
	            cpuLoad = Load_getCPULoad();
	            printf("CPU Load: %d\n", cpuLoad);
	            drawMsg.drawCommand = IMAGE;
				drawMsg.drawImageIndex = 1;


	            Mailbox_post(mailboxHandle, &drawMsg, BIOS_WAIT_FOREVER);
	        }
	        else if (!strcmp(input, "s")) {
	            /* Put the task to sleep for X ms. */
	        	cpuLoad = Load_getCPULoad();
	        	printf("CPU Load: %d\n", cpuLoad);

				drawMsg.drawCommand = IMAGE;
				drawMsg.drawImageIndex = 0;



				/* Do not wait if there is no room for the new mail */
	            Mailbox_post(mailboxHandle, &drawMsg, BIOS_WAIT_FOREVER);
	        }else if (!strcmp(input, "a")) {
	            /* Put the task to sleep for X ms. */
	        	cpuLoad = Load_getCPULoad();
	        	printf("CPU Load: %d\n", cpuLoad);

				drawMsg.drawCommand = IMAGE;
				drawMsg.drawImageIndex = 0;

				key = Gate_enterSystem();
				/* Clear the last command */
				lastCommand[0] = 0x0;
				Gate_leaveSystem(key);

				/* Do not wait if there is no room for the new mail */
	            Mailbox_post(mailboxHandle, &drawMsg, BIOS_WAIT_FOREVER);
	        }else if (!strcmp(input, "d")) {
	            /* Put the task to sleep for X ms. */
	        	cpuLoad = Load_getCPULoad();
	        	printf("CPU Load: %d\n", cpuLoad);

				drawMsg.drawCommand = IMAGE;
				drawMsg.drawImageIndex = 0;

				key = Gate_enterSystem();
				/* Clear the last command */
				lastCommand[0] = 0x0;
				Gate_leaveSystem(key);

				/* Do not wait if there is no room for the new mail */
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
	                   "- load: Get the CPU and task load.\n"
	                   "- sleep: Put the console task to sleep.\n"
	                   "- exit: Exit the console task.\n");
	        }

	        printf("%d %% ", count++);
	        fflush(stdout);
	    }
}


/*
 *  ======== LCD_init ========
 */
Void LCD_init()
{
    /* LCD driver init */
//    CFAL96x64x16Init();
    Kentec320x240x16_SSD2119Init();

//    GrContextInit(&context, &g_sCFAL96x64x16);
    GrContextInit(&context, &g_sKentec320x240x16_SSD2119);

    /* Setup font */
    GrContextFontSet(&context, g_psFontFixed6x8);
}

/* Memory for the GPIO module to construct a Hwi
Hwi_Struct callbackHwi;

 GPIO callback structure to set callbacks for GPIO interrupts
const GPIO_Callbacks DK_TM4C123G_gpioPortMCallbacks = {
    GPIO_PORTF_BASE, INT_GPIOF, &callbackHwi,
    {NULL, NULL, gpioButtonFxn0, gpioButtonFxn1, NULL, NULL, NULL, NULL}
};*/
/*
 *  ======== main ========
 */
Int main(Void)
{

    Mailbox_Params mboxParams;
    Task_Params grlibTaskParams;
    Task_Params grlibTouchTaskParams;
    Task_Params consoleTaskParams;
    Task_Handle grlibTaskHandle;
    Task_Handle grlibTouchTaskHandle;
    Task_Handle consoleTaskHandle;

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
    USBCDCD_init();
//    TouchScreenIntHandler,                      // ADC0 Sequence 3
   TouchScreenInit();
   TouchScreenCallbackSet(WidgetPointerMessage);



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
    Error_init(&eb);
    Task_Params_init(&consoleTaskParams);
    consoleTaskParams.instance->name = "consoleTask";
    consoleTaskParams.stackSize = 550;
    consoleTaskParams.priority = 3;
    consoleTaskHandle = Task_create(consoleTaskFxn, &consoleTaskParams, &eb);
    if (consoleTaskHandle == NULL) {
        System_abort("Console task was not created\nAborting...");
    }

    /* Grlib task create */
    Error_init(&eb);
    Task_Params_init(&grlibTaskParams);
    grlibTaskParams.instance->name = "grlibTask";
    grlibTaskParams.stackSize = 550;
    grlibTaskParams.priority = 2;
    grlibTaskHandle = Task_create(grlibTaskFxn, &grlibTaskParams, &eb);
    if (grlibTaskHandle == NULL) {
        System_abort("Grlib task was not created\nAborting...");
    }

    /* Grlib touch task create */
    Error_init(&eb);
    Task_Params_init(&grlibTouchTaskParams);
    grlibTouchTaskParams.instance->name = "grlibTouchTask";
    grlibTouchTaskParams.stackSize = 550;
    grlibTouchTaskParams.priority = 1;
    grlibTouchTaskHandle = Task_create(WidgetMessageQueueProcess, &grlibTouchTaskParams, &eb);
    if (grlibTouchTaskHandle == NULL) {
        System_abort("grlibTouchTaskHandle  was not created\nAborting...");
    }

    System_printf("Starting the example\n%s, %s",
            "System provider is set to SysMin",
            "halt the target and use ROV to view output.\n");

    /* SysMin will only print to the console when you call flush or exit */
    System_flush();
    //
   // Configure and enable uDMA
   //
   SysCtlPeripheralEnable(SYSCTL_PERIPH_UDMA);
   SysCtlDelay(10);
   uDMAControlBaseSet(&sDMAControlTable[0]);
   uDMAEnable();

    WidgetAdd(WIDGET_ROOT, (tWidget *)&g_sPrevious);
   	WidgetAdd(WIDGET_ROOT, (tWidget *)&g_sTitle);
   	WidgetAdd(WIDGET_ROOT, (tWidget *)&g_sNext);
   	//
   	// Add the first panel to the widget tree.
   	//
   	g_ulPanel = 0;
   	WidgetAdd(WIDGET_ROOT, (tWidget *)g_psPanels);
   	CanvasTextSet(&g_sTitle, g_pcPanelNames[0]);

   	//
   	// Issue the initial paint request to the widgets.
   	//
   	WidgetPaint(WIDGET_ROOT);
       LED_OFF();//????
    /* Start BIOS. Will not return from this call. */
    BIOS_start();

    return (0);
}
