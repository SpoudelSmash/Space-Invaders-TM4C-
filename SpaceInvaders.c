// SpaceInvaders.c
// Runs on LM4F120/TM4C123
// Jonathan Valvano and Daniel Valvano
// This is a starter project for the EE319K Lab 10

// Last Modified: 1/17/2020 
// http://www.spaceinvaders.de/
// sounds at http://www.classicgaming.cc/classics/spaceinvaders/sounds.php
// http://www.classicgaming.cc/classics/spaceinvaders/playguide.php
/* This example accompanies the books
   "Embedded Systems: Real Time Interfacing to Arm Cortex M Microcontrollers",
   ISBN: 978-1463590154, Jonathan Valvano, copyright (c) 2019

   "Embedded Systems: Introduction to Arm Cortex M Microcontrollers",
   ISBN: 978-1469998749, Jonathan Valvano, copyright (c) 2019

 Copyright 2019 by Jonathan W. Valvano, valvano@mail.utexas.edu
    You may use, edit, run or distribute this file
    as long as the above copyright notice remains
 THIS SOFTWARE IS PROVIDED "AS IS".  NO WARRANTIES, WHETHER EXPRESS, IMPLIED
 OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
 MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
 VALVANO SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL,
 OR CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 For more information about my classes, my research, and my books, see
 http://users.ece.utexas.edu/~valvano/
 */
// ******* Possible Hardware I/O connections*******************
// Slide pot pin 1 connected to ground
// Slide pot pin 2 connected to PD2/AIN5
// Slide pot pin 3 connected to +3.3V 
// fire button connected to PE0
// special weapon fire button connected to PE1
// 8*R resistor DAC bit 0 on PB0 (least significant bit)
// 4*R resistor DAC bit 1 on PB1
// 2*R resistor DAC bit 2 on PB2
// 1*R resistor DAC bit 3 on PB3 (most significant bit)
// LED on PB4
// LED on PB5

// Backlight (pin 10) connected to +3.3 V
// MISO (pin 9) unconnected
// SCK (pin 8) connected to PA2 (SSI0Clk)
// MOSI (pin 7) connected to PA5 (SSI0Tx)
// TFT_CS (pin 6) connected to PA3 (SSI0Fss)
// CARD_CS (pin 5) unconnected
// Data/Command (pin 4) connected to PA6 (GPIO), high for data, low for command
// RESET (pin 3) connected to PA7 (GPIO)
// VCC (pin 2) connected to +3.3 V
// Gnd (pin 1) connected to ground

#include <stdint.h>
#include "../inc/tm4c123gh6pm.h"
#include "ST7735.h"
#include "Print.h"
#include "Random.h"
#include "PLL.h"
#include "ADC.h"
#include "Images.h"
#include "Sound.h"
#include "Timer0.h"
#include "Timer1.h"
//Variables
int test1;
int test2;
int lang = -1, mode = 0, lvl =1, score=0;  // Lang: -1 = undecided, 0 = Eng, 1 = Esp; Mode: 0 = menu, 1= game; lvl(level)= 1-5
uint32_t timer1 = 0, timer2 = 0;
volatile uint32_t buttonin = 0;
int bullets = 0; //Bullets on screen

//Prototypes
void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts
void Delay10ms(uint32_t count); // time delay in 0.01 seconds (for enemy)
void Clock10ms(void);
//void Delay5ms(uint32_t count); // time delay in 0.005 seconds (for projectile)
//void Clock5ms(void);
void GPIO_Init(void);
void Sprite_Move(void);
void Win_screen(void);
void Pew_Move(void);

//Sprites
struct State {
	signed long x;
	signed long y;
	unsigned long hp;
	const unsigned short *image;
	long life;
	
	
};

typedef struct State Sprite;
Sprite Nyan;
Sprite Wave1[4];
Sprite Wave2[8];
Sprite Wave3[16];
Sprite Wave4[32];
Sprite Wave5[64];

struct projectile {
	signed long x;
	signed long y;
	const unsigned short *image;
	long life;
};
typedef struct projectile Bullet; 
Bullet pews[5];										// 5 pews at one time



void Sprite_Init(void){
	int i;
	for(i = 0; i<4; i++){
		Wave1[i].x = 20*i;
		Wave1[i].y = 10;
		Wave1[i].hp = 1;
		Wave1[i].image = SmallEnemy30pointA;
		Wave1[i].life = 1;
	}
	
	for(i = 0; i<8; i++){								// Each lvl has enemy= 2<<lvl 
		Wave2[i].x = 0;
		Wave2[i].y = 10;
		Wave2[i].hp = 1;
		Wave2[i].image = SmallEnemy30pointA;
		Wave2[i].life = 0;
	}
		Wave2[0].life = 1;
	for(i = 0; i<16; i++){
		Wave3[i].x = 0;
		Wave3[i].y = 10;
		Wave3[i].hp = 1;
		Wave3[i].image = SmallEnemy30pointA;
		Wave3[i].life = 0;
	}
	Wave3[0].life = 1;
	for(i = 0; i<32; i++){
		Wave4[i].x = 0;
		Wave4[i].y = 10;
		Wave4[i].hp = 1;
		Wave4[i].image = SmallEnemy30pointA;
		Wave4[i].life = 0;
	}
	Wave4[0].life = 1;
		Wave5[0].x = 40;
		Wave5[0].y = 70;
		Wave5[0].hp = 1;
		//Wave5[0].image = ratking2;
		Wave5[0].life = 50;
	
	
	//ST7735_DrawBitmap(0, 9, SmallEnemy10pointA, 16,10);
  //ST7735_DrawBitmap(20,9, SmallEnemy10pointB, 16,10);
  //ST7735_DrawBitmap(40, 9, SmallEnemy20pointA, 16,10);
  //ST7735_DrawBitmap(60, 9, SmallEnemy20pointB, 16,10);
  //ST7735_DrawBitmap(80, 9, SmallEnemy30pointA, 16,10);
  //ST7735_DrawBitmap(100, 9, SmallEnemy30pointB, 16,10);
	
	Timer0_Init(Pew_Move, 850000);					// Called every .01 sec (10 ms)
	
}

void Nyan_Init(void){
	Nyan.x = (ADC_In()/32);
	Nyan.y = 159;
	Nyan.image = Nyanim;
	Nyan.life = 1;
	ST7735_DrawBitmap(Nyan.x, Nyan.y, Nyan.image, 15,21); // player ship bottom
	
	int i;
	for(i = 0; i<5; i++){
		pews[i].image = pew;
		pews[i].life = 0;
	}
}

//Update Enemy sprite position and check for Nyan Death
void Sprite_Move(void){
	int i,j;
	if(mode == 1){
		if (lvl == 1){
			Delay10ms(4);
			for(i=0; i<4; i++){
			if(Wave1[i].life==1){
				if (Wave1[i].x > 130){
					Wave1[i].x = 0;
					Wave1[i].y += 10;
					
				}
				Wave1[i].x ++;
				
				ST7735_DrawBitmap(Wave1[i].x, Wave1[i].y, Wave1[i].image, 16,10);
				
			//Check for Death
			
			if((Nyan.x >= Wave1[i].x)&&(Nyan.x <= (Wave1[i].x+10))){			// If wave.x<=nyan.x<=wave.x+15 (inside entitiy bounds x)
				if((Nyan.y >= (Wave1[i].y-21))&&(Nyan.y <= Wave1[i].y)){			// If wave.y-8<=neyan.y<=wave.y (inside entitiy bounds y
						mode=-1;
				}
			}	
	}
	
}
//Check for Enemy dead
			for(i=bullets-1; i>=0; i--){
			for(j=(2<<lvl)-1; j>=0; j--){
					if(Wave1[j].life ==1){
					if((pews[i].x >= Wave1[j].x)&&(pews[i].x <= (Wave1[j].x+10))){			// If wave.x<=pew.x<=wave.x+15 (inside entitiy bounds x)
						if((pews[i].y >= (Wave1[j].y-8))&&(pews[i].y <= Wave1[j].y)){			// If wave.y-8<=pew.y<=wave.y (inside entitiy bounds y)
							Wave1[j].life =0;
							Sound_Explosion();
							ST7735_FillRect(pews[i].x,pews[i].y-11,4,11,0x0000);
							ST7735_DrawBitmap(Wave1[j].x, Wave1[j].y, Death, 16, 10);
							pews[i].y=0;
							score +=50*(1<<lvl);
						}
					}
				}
			}
		}
}
		if (lvl == 2){
			Delay10ms(4);
			for(i=(2<<lvl)-1; i>=0; i--){
				if(Wave2[i].life==1){
					Wave2[i].x ++;
					
					if ((Wave2[i].x == 20)&&(Wave2[i].y == 10)){
						Wave2[i+1].life =1;
					}
					if (Wave2[i].x > 160){
						Wave2[i].x = 0;
						Wave2[i].y += 10;
					}
					
				
				ST7735_DrawBitmap(Wave2[i].x, Wave2[i].y, Wave2[i].image, 16,10);
				
			//Check for Death
			
			if((Nyan.x >= Wave2[i].x)&&(Nyan.x <= (Wave2[i].x+10))){			// If wave.x<=nyan.x<=wave.x+15 (inside entitiy bounds x)
				if((Nyan.y >= (Wave2[i].y-21))&&(Nyan.y <= Wave2[i].y+10)){			// If wave.y-8<=neyan.y<=wave.y (inside entitiy bounds y
						mode=-1;
				}
			}
		
		}
	}
			//Check for Enemy dead
			for(i=bullets-1; i>=0; i--){
			for(j=(2<<lvl)-1; j>=0; j--){
					if(Wave2[j].life ==1){
					if((pews[i].x >= Wave2[j].x)&&(pews[i].x <= (Wave2[j].x+10))){			// If wave.x<=pew.x<=wave.x+15 (inside entitiy bounds x)
						if((pews[i].y >= (Wave2[j].y-8))&&(pews[i].y <= Wave2[j].y)){			// If wave.y-8<=pew.y<=wave.y (inside entitiy bounds y)
							Wave2[j].life =0;
							Sound_Explosion();
							ST7735_FillRect(pews[i].x,pews[i].y-11,4,11,0x0000);
							ST7735_DrawBitmap(Wave2[j].x, Wave2[j].y, Death, 16, 10);
							pews[i].y=0;
							score +=50*(1<<lvl);
						}
					}
				}
			}
		}
	}
		if (lvl == 3){
			Delay10ms(3);
			for(i=(2<<lvl)-1; i>=0; i--){
				if(Wave3[i].life==1){
					Wave3[i].x ++;
					if ((Wave3[i].x == 20)&&(Wave3[i].y == 10)){
						Wave3[i+1].life =1;
					}
					if (Wave3[i].x > 160){
						Wave3[i].x = 0;
						Wave3[i].y += 10;
					}
				
				ST7735_DrawBitmap(Wave3[i].x, Wave3[i].y, Wave3[i].image, 16,10);
				
			//Check for Death
			
			if((Nyan.x >= Wave3[i].x)&&(Nyan.x <= (Wave3[i].x+10))){			// If wave.x<=nyan.x<=wave.x+15 (inside entitiy bounds x)
				if((Nyan.y >= (Wave3[i].y-21))&&(Nyan.y <= Wave3[i].y)){			// If wave.y-8<=neyan.y<=wave.y (inside entitiy bounds y
						mode=-1;
				}
			}
		
		}
	}
			//Check for Enemy dead
			for(i=bullets-1; i>=0; i--){
			for(j=(2<<lvl)-1; j>=0; j--){
					if(Wave3[j].life ==1){
					if((pews[i].x >= Wave3[j].x)&&(pews[i].x <= (Wave3[j].x+10))){			// If wave.x<=pew.x<=wave.x+15 (inside entitiy bounds x)
						if((pews[i].y >= (Wave3[j].y-8))&&(pews[i].y <= Wave3[j].y)){			// If wave.y-8<=pew.y<=wave.y (inside entitiy bounds y)
							Wave3[j].life =0;
							Sound_Explosion();
							ST7735_DrawBitmap(Wave3[j].x, Wave3[j].y, Death, 16, 10);
							pews[i].y=0;
							score +=50*(1<<lvl);
						}
					}
				}
			}
		}
}
		if (lvl == 4){
			Delay10ms(2);
			for(i=(2<<lvl)-1; i>=0; i--){
				if(Wave4[i].life==1){
					Wave4[i].x ++;
					if ((Wave4[i].x == 20)&&(Wave4[i].y == 10)){
						Wave4[i+1].life =1;
					}
					if (Wave4[i].x > 160){
						Wave4[i].x = 0;
						Wave4[i].y += 10;
					}
				
				ST7735_DrawBitmap(Wave4[i].x, Wave4[i].y, Wave4[i].image, 16,10);
				
			//Check for Death
			
			if((Nyan.x >= Wave4[i].x)&&(Nyan.x <= (Wave4[i].x+10))){			// If wave.x<=nyan.x<=wave.x+15 (inside entitiy bounds x)
				if((Nyan.y >= (Wave4[i].y-21))&&(Nyan.y <= Wave4[i].y)){			// If wave.y-8<=neyan.y<=wave.y (inside entitiy bounds y
						mode=-1;
				}
			}
		
		}
	}
			//Check for Enemy dead
			for(i=bullets-1; i>=0; i--){
			for(j=(2<<lvl)-1; j>=0; j--){
					if(Wave4[j].life ==1){
					if((pews[i].x >= Wave4[j].x)&&(pews[i].x <= (Wave4[j].x+10))){			// If wave.x<=pew.x<=wave.x+15 (inside entitiy bounds x)
						if((pews[i].y >= (Wave4[j].y-8))&&(pews[i].y <= Wave4[j].y)){			// If wave.y-8<=pew.y<=wave.y (inside entitiy bounds y)
							Wave4[j].life =0;
							Sound_Explosion();
							ST7735_DrawBitmap(Wave4[j].x, Wave4[j].y, Death, 16, 10);
							pews[i].y=0;
							score +=50*(1<<lvl);
						}
					}
				}
			}
		}
}
		if (lvl == 5){
			Delay10ms(1);
			if (Wave5[0].x >70){
				Wave5[0].hp =0;
			}else if (Wave5[0].x < 10){
				Wave5[0].hp =1;
			}
			if(Wave5[0].hp){
					Wave5[0].x ++;
			}else{
					Wave5[0].x --;
			}
						
				ST7735_DrawBitmap(Wave5[0].x, Wave5[0].y, Wave5[0].image, 53,66);
				
			
			for(i=bullets-1; i>=0; i--){
				if((pews[i].x >= (Wave5[0].x+2))&&(pews[i].x <= (Wave5[0].x+45))){			// If wave.x<=pew.x<=wave.x+w (inside entitiy bounds x)
						if((pews[i].y >= (Wave5[0].y-60))&&(pews[i].y <= (Wave5[0].y-5))){			// If wave.y-h<=pew.y<=wave.y (inside entitiy bounds y)
							Wave5[0].life --;
							Sound_Explosion();
							//ST7735_DrawBitmap(Wave4[j].x, Wave4[j].y, ratkinghit, 53, 66);
							pews[i].y=0;
						}
					}	
			}
			
		}
	}
}

//Update Nyan position (interrupt)
void Nyan_Move(uint32_t slidepot){					// values range from 0 to 4000
	if(mode > 0){
		uint32_t newx = slidepot/32;
		if(newx > 112){
			newx= 112;
	}
		while (Nyan.x > (newx)){
			Nyan.x--;
			ST7735_DrawBitmap(Nyan.x, Nyan.y, Nyan.image, 15,21);
		}
		while (Nyan.x < (newx)){
		Nyan.x++;
		ST7735_DrawBitmap(Nyan.x, Nyan.y, Nyan.image, 15,21);
		}
	}
}

//Interrupt from timer0
void Pew_Move(){														// Update projectile values and add new ones from fire
	static int oldpews;
	
	if(mode >0){
	int i;
	
	if (bullets>=1){
		if(oldpews<bullets){
			Sound_Shoot();
		}
		oldpews=bullets;
		for(i=bullets-1; i>=0; i--){
			
			pews[i].y--;
			ST7735_DrawBitmap(pews[i].x, pews[i].y, pew, 4,11);
			
	}
		if(pews[0].y <= 0){
			bullets--;
			pews[0]=pews[1];
			pews[1]=pews[2];
			pews[2]=pews[3];
			pews[3]=pews[4];
		}
		}else{
			bullets=0;
		}
		
	}
}

void Fire(){													// short interrupt for fire
	
	if (mode == 1){
		if(bullets < 5){
		pews[bullets].x = Nyan.x+7;
		pews[bullets].y = Nyan.y-21;
		pews[bullets].life =1;
		bullets++;
		}
	}
}

void Pause_menu(){
	int selection = 7;	// sel=7 correspond to continue
	if (lang == 0){
		ST7735_SetCursor(8, 3);
		ST7735_OutString("PAUSED");
		ST7735_DrawString(10, 7, "Continue", 0xFFFF);						// x:0 to 20; y: 0 to 15 on DRAWSTRING function
		ST7735_DrawString(10, 8, "Quit", 0xFFFF);
	}else{
		ST7735_SetCursor(8, 3);
		ST7735_OutString("PAUSADO");
		ST7735_DrawString(10, 7, "Seguir", 0xFFFF);						// x:0 to 20; y: 0 to 15 on DRAWSTRING function
		ST7735_DrawString(10, 8, "Salir", 0xFFFF);
		buttonin=0;
	}
	while(1){
		ST7735_DrawString(9, selection, ">", 0x20FD);
		if (buttonin == 0x02){ // Next option
			buttonin=0;
			ST7735_DrawString(9, selection, ">", 0x0000);
			if (selection == 7){
				selection++;
			}else{
				selection=7;
			}
			
		}
		if (buttonin == 0x01){													//Start/End game
			buttonin=0;
			if(selection == 7){
				if (lang == 0){
					ST7735_DrawString(10, selection, "Continue", 0x0000);						// x:0 to 20; y: 0 to 15 on DRAWSTRING function
					ST7735_DrawString(10, selection+1, "Quit", 0x0000);
					ST7735_DrawString(9, selection, ">", 0x0000);
					ST7735_DrawString(8, 3, "PAUSED", 0x0000);
				}else{
					ST7735_DrawString(10, selection, "Seguir", 0x0000);						// x:0 to 20; y: 0 to 15 on DRAWSTRING function
					ST7735_DrawString(10, selection+1, "Salir", 0x0000);
					ST7735_DrawString(9, selection, ">", 0x0000);
					ST7735_DrawString(8, 3, "PAUSADO", 0x0000);
				}
				mode=1;
				return;
			}else{
				mode=-1;
				return;
			}
			
		}
	}
	}


void Main_menu(){
	ST7735_FillScreen(0x0000);            // set screen to black (0x8200 = dark blue)
	ST7735_SetCursor(1, 0);
  ST7735_OutString("English");	
	ST7735_SetCursor(1, 1);
	ST7735_OutString("Espa\xA4ol");
	
	//Selection
	int selection = 0;
	
	while(mode == 0){
		ST7735_DrawString(10, selection, "<", 0xFFFF);
		if (buttonin == 0x02){													// Next option
			ST7735_DrawString(10, selection, "<", 0x0000);
			selection = (selection + 1) % 2;
			Sound_Shoot();
			buttonin=0;
		}
		if (buttonin == 0x01){													//Start game
			lang = selection;
			mode=1;
			ST7735_DrawString(10, selection, "<", 0x20FD);
			buttonin=0;
			DisableInterrupts();
		}
	}
}

void Gametime(){																		// In-game functions
	int wavestat = 0;
	
	ST7735_FillScreen(0x0000);
	Nyan_Init();
	Sprite_Init();
	mode = 1;
	EnableInterrupts();
	
	int i=0;
	while (mode >= 0){						// Game loop
		wavestat=0;
		if (lvl == 1){
			for (i=0;i<4 ; i++){
				if(Wave1[i].life == 1){
				wavestat++;
				}
			}
		}
		if (lvl == 2){
			for(i=(2<<lvl)-1; i>=0; i--){
				if(Wave2[i].life == 1){
				wavestat++;
				}
			}
		}
		if (lvl == 3){
			for(i=(2<<lvl)-1; i>=0; i--){
				if(Wave3[i].life == 1){
				wavestat++;
				}
			}
		}
		if (lvl == 4){
			for(i=(2<<lvl)-1; i>=0; i--){
				if(Wave4[i].life == 1){
				wavestat++;
				}
			}
		}
		if (lvl == 5){
			Win_screen();
			for(i=(2<<lvl)-1; i>=0; i--){
				if(Wave2[i].life == 1){
				wavestat++;
				}
			}
		}
		if(lvl == 6){
			Win_screen();
		}
		
		if (wavestat==0){								// Next wave
			lvl++;
			
		}
		
		
		Sprite_Move();
		if(mode == 0){
			Pause_menu();
		}
	}
}

void Win_screen(){
	DisableInterrupts();
	ST7735_FillScreen(0x0000);   // set screen to black
	if (lang == 0){
		ST7735_SetCursor(6, 1);
		ST7735_OutString("Congrats!");
		ST7735_SetCursor(4, 2);
		ST7735_OutString("You Saved the");
		ST7735_SetCursor(2, 3);
		ST7735_OutString("Land of Pastries!");
		ST7735_SetCursor(2, 5);
		ST7735_OutString("Level:");
		ST7735_SetCursor(2, 6);
		ST7735_OutString("Score:");
	}else{
		ST7735_SetCursor(5, 1);
		ST7735_OutString("Felicidades!");
		ST7735_SetCursor(1, 2);
		ST7735_OutString("Has salvado");
		ST7735_SetCursor(3, 3);
		ST7735_OutString("la tierra de los");
		ST7735_SetCursor(6, 4);
		ST7735_OutString("pasteles");
		
		ST7735_SetCursor(0, 5);
		ST7735_OutString("Nivel:");
		ST7735_SetCursor(0, 6);
		ST7735_OutString("Puntuacion:");
	}
	ST7735_SetCursor(10, 5);
  LCD_OutDec(lvl);
	ST7735_SetCursor(10, 6);
	LCD_OutDec(score);
	while(1){};
}

//test main
int maint(){
	DisableInterrupts();
  PLL_Init(Bus80MHz);       // Bus clock is 80 MHz 
  Random_Init(1);
	Timer1_Init(Clock10ms, 850000);		//interrupt at .01 sec
  Output_Init();
	GPIO_Init();							// I/o and Systick
	Sound_Init();							// Sound and DAC
	ADC_Init(); 							// ADC
	ST7735_FillRect(0,0,16,20,0xFFFF);
	return 1;
}

int main(void){
  DisableInterrupts();
  PLL_Init(Bus80MHz);       // Bus clock is 80 MHz 
  Random_Init(1);
	//Timer0_Init(Pew_Move, 850000);					// Called every .01 sec (10 ms) now in Sprite_init
	Timer1_Init(Clock10ms, 850000);		//interrupt at .01 sec
	
  Output_Init();
	GPIO_Init();							// I/o and Systick
	Sound_Init();							// Sound and DAC
	ADC_Init(); 							// ADC
	EnableInterrupts();
	

	Main_menu();
	
	
	//Delay100ms(50);              // delay 5 sec at 80 MHz
  //ST7735_DrawBitmap(22, 159, PlayerShip0, 18,8); // player ship bottom
  //ST7735_DrawBitmap(42, 159, PlayerShip1, 18,8); // player ship bottom
  //ST7735_DrawBitmap(62, 159, pew, 4,11); // player ship bottom
	while(mode >= 0){
		Gametime();
  }
	
	//Delay10ms(500);              // delay 5 sec at 80 MHz
	Sound_Explosion();
	DisableInterrupts();
	
	
  ST7735_FillScreen(0x0000);   // set screen to black
	if (lang == 0){
		ST7735_SetCursor(1, 1);
		ST7735_OutString("GAME OVER");
		ST7735_SetCursor(1, 2);
		ST7735_OutString("Nice try,");
		ST7735_SetCursor(1, 3);
		ST7735_OutString("Nyan!");
		ST7735_SetCursor(2, 4);
		ST7735_OutString("Level:");
		ST7735_SetCursor(2, 5);
		ST7735_OutString("Score:");
	}
	else{
		ST7735_SetCursor(1, 1);
		ST7735_OutString("JUEGO TERMINADO");
		ST7735_SetCursor(1, 2);
		ST7735_OutString("Buen intento,");
		ST7735_SetCursor(1, 3);
		ST7735_OutString("Nyan!");
		ST7735_SetCursor(2, 4);
		ST7735_OutString("Nivel:");
		ST7735_SetCursor(2, 5);
		ST7735_OutString("Conseguir:");
		//ST7735_SetCursor(2, 6);
		//ST7735_OutString("Nyan!");
	}
  ST7735_SetCursor(10, 4);
  LCD_OutDec(lvl);
	ST7735_SetCursor(10, 5);
	LCD_OutDec(score);
	//ST7735_SetCursor(5, 6);
	//LCD_OutDec(test2);
  while(1){
  }

}

void tester(){
	test1 ++;
}

void Clock10ms(){
	timer1 ++;
}
// You can't use this timer, it is here for starter code only 
// you must use interrupts to perform delays
void Delay10ms(uint32_t count){
	timer1 = 0;
	while (count > timer1){}
  
}


void GPIO_Init(){
	
	SYSCTL_RCGCGPIO_R |= 0x10;
	while (!(SYSCTL_RCGCGPIO_R & 0x10));
	
	NVIC_ST_CTRL_R = 0;
	NVIC_ST_RELOAD_R = 8500000 - 1;				// 10Hz 
	NVIC_ST_CURRENT_R = 0;
	NVIC_ST_CTRL_R |= 0x07;
	
	GPIO_PORTE_DIR_R &= ~(0x1F);
	GPIO_PORTE_AFSEL_R &= ~0x03;
	GPIO_PORTE_PCTL_R &= ~0x000F0000;
	GPIO_PORTE_AMSEL_R = 0;
	GPIO_PORTE_PDR_R |= 0x1F;
	GPIO_PORTE_IS_R &= ~0x03;		// Edge-sensitive
	GPIO_PORTE_IBE_R &= ~0x03;		// Not both edges
	GPIO_PORTE_IEV_R |= 0x03;			// Rising
	GPIO_PORTE_ICR_R = 0x03;
	GPIO_PORTE_IM_R |= 0x03;		// Arm interrupts
	NVIC_EN0_R = 0x10;					// Port E interrupts
	GPIO_PORTE_DEN_R |= 0x1F;
}


void SysTick_Handler(void){
	Nyan_Move(ADC_In());
	
}

void GPIOPortE_Handler(void){
	
	if ((GPIO_PORTE_RIS_R & 0x02) == 0x02){				// Play/Pause button
		GPIO_PORTE_ICR_R = 0x03;
		buttonin = 0x02;
		mode =0;
	}
	else {		// Fire/Select button	
		GPIO_PORTE_ICR_R = 0x01;
		buttonin = 0x01;
		
		//Fire
		Fire();
	}
}



