# ColorRun_Game
Introduction
Using TI RTOS System to implement the Color Run Game.
Objectives
Learn to use TI RTOS
Learn to design a RTOS on Tiva C LaunchPad
Learn to interface Tiva C LaunchPad with peripherals
Description
Project requirement:
Choose a RTOS design that has at least the following threads
2 HWi (posted as SWi)
3 Tasks
   	Idle
    Identify the threads in your system design
    Develop the SYS/BIO to run the threads
   	 Demonstrate system operation on Tiva C LaunchPad with the peripherals.

Details
   We decide to design a touch game named  .

Rules of  
•	There are 6 different colors on the screen, and one word of color
•	Tap the right color according to the word
•	The color of the word is used to confuse you
•	You have 2 seconds to get the right answer
•	There are 3 conditions:
	If you tap right, you get 1 point and the screen refreshes
	If you tap wrong, you lose 1 point and the screen refreshes
	If you don’t tap, the screen refreshes after 2 seconds and you lose 2 points
•	You have 5 points at first, the game ends when your points are below 0. And the screen shows your highest score you ever got
•	Enjoy the game!!!

(1)Start
(2)End
(3)Restart
When you push the reset button, the game restarts.
 
(1) Hwi
•	ti_sysbios_family_arm_m3_Hwi0
This Hwi happens when the LCD screen is touched
•	HWI_TIMER2
A timer counting for 2 seconds until it cause the interrupt, and if the right color is touched, reset the timer immediately.

(2)Swi
•	TouchScreenSwi
Posed by Hwi ti_sysbios_family_arm_m3_Hwi0
•	Timer2Swi
Posed by Hwi HWI_TIMER2

(3)Task
•	TimerTask
Semaphore: AnoSem, posed by TimerTaskFxn()
To disorder the 6 colors ramdonly and refresh the screen if the game is not yet end
•	grlibTask1
Semaphore: QueSem, posed by TouchTestCallback()
To draw the screen: 6 colors and 1 color word
•	Timer2task
Semaphore: Timer2Sem, posed by Time2SwiFunc()
To detect if player tap the right color and calculate the score, if score decreases to 0, stop the timer, if not, pose to active TimerTask

(4)Idle
ledToggle
Toggle the LED light every 0.5 seconds

When the game is not end, the MAX shows the maximum score you have ever got. 
When the score decreases to 0, game ends. When you push the reset button, the game restarts
 
