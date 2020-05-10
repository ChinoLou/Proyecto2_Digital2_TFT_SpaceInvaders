//***************************************************************************************************************************************
/* Librería para el uso de la pantalla ILI9341 en modo 8 bits
 * Basado en el código de martinayotte - https://www.stm32duino.com/viewtopic.php?t=637
 * Adaptación, migración y creación de nuevas funciones: Pablo Mazariegos y José Morales
 * IE3027: Electrónica Digital 2 - 2019
 * Steven Josue Castillo Lou - 17169
 * Vincenzo Musella 
 * (solo falta sd y comparacion balas 2)
 * FINAL!
 */
//***************************************************************************************************************************************
#include <stdint.h>
#include <stdbool.h>
#include <TM4C123GH6PM.h>
#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/debug.h"
#include "driverlib/gpio.h"
#include "driverlib/interrupt.h"
#include "driverlib/rom_map.h"
#include "driverlib/rom.h"
#include "driverlib/sysctl.h"
#include "driverlib/timer.h"

#include "bitmaps.h"
#include "font.h"
#include "lcd_registers.h"


#define LCD_RST PA_6
#define LCD_CS  PA_7   //Se utilizaron los pines PA_6 & PA_7 para no desoldar las resistencias R9 y R10 de la Tiva_C 
#define LCD_RS  PD_2
#define LCD_WR  PD_3
#define LCD_RD  PE_1
int DPINS[] = {PB_0, PB_1, PB_2, PB_3, PB_4, PB_5, PB_6, PB_7};  


#define PUSH1 PF_4
#define PUSH2 PF_0
#define LED_R PF_1
#define LED_B PF_2
#define LED_G PF_3

#define BOTON1 PD_7   //BOTONES PARA CONTROL DE LAS NAVES
#define BOTON2 PD_6
#define BOTON3 PC_7
#define BOTON4 PC_6
#define BOTON5 PC_5
#define BOTON6 PC_4

#define Music1_ON PA_5  //Boton para musica --> salida para conectar a arduino y activas o cambiar de melodia 
#define Music2_ON PE_2  //Boton para musica --> salida para conectar a arduino y activas o cambiar de melodia 

//***************************************************************************************************************************************
// Functions Prototypes
//***************************************************************************************************************************************
void LCD_Init(void);
void LCD_CMD(uint8_t cmd);
void LCD_DATA(uint8_t data);
void SetWindows(unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2);
void LCD_Clear(unsigned int c);
void H_line(unsigned int x, unsigned int y, unsigned int l, unsigned int c);
void V_line(unsigned int x, unsigned int y, unsigned int l, unsigned int c);
void Rect(unsigned int x, unsigned int y, unsigned int w, unsigned int h, unsigned int c);
void FillRect(unsigned int x, unsigned int y, unsigned int w, unsigned int h, unsigned int c);
void LCD_Print(String text, int x, int y, int fontSize, int color, int background);
uint8_t collider(uint16_t objeto1[], uint16_t objeto2[], uint8_t d1[] , uint8_t d2[]); 
void LCD_Bitmap(unsigned int x, unsigned int y, unsigned int width, unsigned int height, unsigned char bitmap[]);
void LCD_Sprite(int x, int y, int width, int height, unsigned char bitmap[],int columns, int index, char flip, char offset);
uint8_t naves_choque(uint8_t posicion);

void SD_Bitmap(int x, int y, int width, int height, char *filename);
unsigned char Char_to_uChar(char letra); 

extern uint8_t Fondo_UNO_X2 [];
extern uint8_t ALIEN_uno    [];
extern uint8_t ALIEN_dos    [];
extern uint8_t ALIEN_tres   [];
extern uint8_t GALAGA_uno   [];
extern uint8_t GALAGA_dos   [];
extern uint8_t GALAGA_tres  [];

//***************************************************************************************************************************************
// Variables Globales & Variables Botones
//***************************************************************************************************************************************
uint8_t  x1_i = 0; //Variables para indicies del control de movimiento naves jugadores 
uint8_t  x3_i = 0;
uint16_t x2_i = 0;
uint16_t y1_mi = 0;

uint16_t  xi_bala   = 0;          //variables para bala Nave UNO
uint16_t  y1_atack1 = 0;
uint8_t   t         = 0; 
bool flag_balas     = 0;             //Bandera para bala 1                   
bool habilitar_disparo = true; 


 

uint16_t  xi_bala2  = 0;           //variables para bala Nave DOS
uint16_t  y2_atack2 = 0;
bool flag_balas2    = 0;             //Bandera para bala 1    
bool habilitar_disparo2 = true; 

uint8_t flag_juego = 0;
int ESTADO1    = 0; //variable para antirebote principal
int var        = 0;


//**********************  Variables Botones ******************************
// set pin numbers:
const int START = PUSH1;          // the number of the pushbutton pin
const int NEXT  = PUSH2;          // the number of the pushbutton pin

const int SW1 = BOTON1;           //Botones para el control
const int SW2 = BOTON2;      
const int SW3 = BOTON3;          
const int SW4 = BOTON4;    
const int SW5 = BOTON5;          
const int SW6 = BOTON6;  

// Variables para el cambio de estado de los botones
bool Estado_START = 0;            // variable for reading the pushbutton status
bool Estado_NEXT = 0;    

bool Estado_SW1 = 0;
bool Estado_SW2 = 0;
bool Estado_SW3 = 0;
bool Estado_SW4 = 0;
bool Estado_SW5 = 0;
bool Estado_SW6 = 0;


bool flag_start   = 0;              //bandera para escoger modo de juego
bool flag_music1  = 0;              //Bandera para melodia 1 --> Star Wars
bool flag_music2  = 0;


unsigned long previousMillis  = 0;       // will store last time 
unsigned long previousMillis2 = 0;       // will store last time2

uint16_t posicion_bala[]={0,0};

                                      

uint16_t objeto1[] = {30,0};             //objetos para comparar coordenadas x,y de los 21 aliens
uint16_t objeto2[] = {30,0};             
uint16_t objeto3[] = {30,0};
uint16_t objeto4[] = {70,0};
uint16_t objeto5[] = {70,0};
uint16_t objeto6[] = {70,0};
uint16_t objeto7[] = {110,0};
uint16_t objeto8[] = {110,0};
uint16_t objeto9[] = {110,0};
uint16_t objeto10[]= {150,0};
uint16_t objeto11[]= {150,0};
uint16_t objeto12[]= {150,0};
uint16_t objeto13[]= {190,0};
uint16_t objeto14[]= {190,0};
uint16_t objeto15[]= {190,0};
uint16_t objeto16[]= {230,0};
uint16_t objeto17[]= {230,0};
uint16_t objeto18[]= {230,0};
uint16_t objeto19[]= {270,0};
uint16_t objeto20[]= {270,0};
uint16_t objeto21[]= {270,0};
//-------------------------------------------------------------------------
uint8_t  choque[] = {};
int  n = 0; 
int  n2= 0;
uint8_t d1[]={2,8};   //para la funcion collier
uint8_t d2[]={10,10};

// ------------ Variables SD ---------------------------------------------
/*
File root;
File myFile;
Sd2Card card;
char I;
char J;
uint8_t V = 0;
char H;
int cont = 2;
int defnum = 1;
*/

//***************************************************************************************************************************************
// Inicialización
//***************************************************************************************************************************************
void setup() {
  SysCtlClockSet(SYSCTL_SYSDIV_2_5|SYSCTL_USE_PLL|SYSCTL_OSC_MAIN|SYSCTL_XTAL_16MHZ);
  Serial.begin(9600);
  GPIOPadConfigSet(GPIO_PORTB_BASE, 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7, GPIO_STRENGTH_8MA, GPIO_PIN_TYPE_STD_WPU);
  Serial.println("Inicio");
  LCD_Init();
  LCD_Clear(0x00);
  
  // Entradas digitales para controlar el juego
  pinMode(START, INPUT_PULLUP);
  pinMode(NEXT,  INPUT_PULLUP);  

  pinMode(BOTON1, INPUT_PULLUP);
  pinMode(BOTON2, INPUT_PULLUP);
  pinMode(BOTON3, INPUT_PULLUP);
  pinMode(BOTON4, INPUT_PULLUP);
  pinMode(BOTON5, INPUT_PULLUP);
  pinMode(BOTON6, INPUT_PULLUP);

  // Salidias digitales para controlar la musica reproducida desde el arduino
  pinMode(LED_R, OUTPUT);  
  pinMode(LED_G, OUTPUT);  
  pinMode(LED_B, OUTPUT);
  pinMode(Music1_ON,  OUTPUT); //Pin PA_5 --> a entrada1 arduino
  pinMode(Music2_ON,  OUTPUT); //Pin PE_2 --> a entrada2 arduino


  //%%%%%%%%%%%%% SD %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
 /* 
  // Open serial communications and wait for port to open:
  SPI.setModule(0);

 while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }
 
  Serial.print("Initializing Vinni´s SD card");
  // On the Ethernet Shield, CS is pin 4. It's set as an output by default.
  // Note that even if it's not used as the CS pin, the hardware SS pin
  // (10 on most Arduino boards, 53 on the Mega) must be left as an output
  // or the SD library functions will not work.
  pinMode(10, OUTPUT);

  if (!SD.begin(32)) {
    Serial.println("initialization failed!");
    return;
  }
  Serial.println("...WAITING...");
  Serial.println("");
  Serial.println("Initialization done.");
  Serial.println("");
  //root = SD.open("/");

  //printDirectory(root, 0);
  //showfile();

  Serial.println("Listado de archivos finalizado!");
  Serial.println("");
  */
  


  // ------------------------------------------
  delay(1000);

  //SD_Bitmap(97, 100, 126, 40, "FD.txt");
  
  //LCD_Bitmap(97, 100, 126, 40, Fondo_UNO); 
  FillRect(97, 100, 126, 40,0x00);
  LCD_Bitmap(34, 80, 252, 80, Fondo_UNO_X2);
  digitalWrite(Music1_ON, HIGH);

  x2_i      = 301; //posx final para nave 2
  y1_atack1 = 200; //posy inicial para las balas de la nave 1 
  y2_atack2 = 200; //posy inicial para las balas de la nave 2 

 // H_line( 10, 50+15, 20, 0xfff);


  
}

//LCD_Bitmap(unsigned int x, unsigned int y, unsigned int width, unsigned int height, unsigned char bitmap[]);
//LCD_Sprite(int x, int y, int width, int height, unsigned char bitmap[],int columns, int index, char flip, char offset);      

//***************************************************************************************************************************************
// Loop Infinito
//***************************************************************************************************************************************
void loop() {
  Estado_START = digitalRead(START);  //START --> se configura con PULL-UP (lógica inversa si se presiona cambia a 0 sino se presiona esta en 1 lógico) 
  Estado_NEXT  = digitalRead(NEXT);  //NEXT --> se configura con PULL-UP
  Estado_SW1   = digitalRead(SW1);
  Estado_SW2   = digitalRead(SW2);
  Estado_SW3   = digitalRead(SW3);
  Estado_SW4   = digitalRead(SW4);
  Estado_SW6   = digitalRead(SW6);  //PC4 -->Atacke Nave 1
  Estado_SW5   = digitalRead(SW5);  //PC5 -->Atacke Nave 2


//------------------- Pantalla de Inicio y Start -------------------------------

  if(Estado_START == LOW){
    digitalWrite(LED_R, HIGH); 
    FillRect(0, 0, 319, 239,0x00);
    
    
    String text1 = "SPACE INVADERS!";
    LCD_Print(text1, 45, 10, 2, 0xffff, 0x00); //0x421b);

    for(int x = 0; x <320-18; x++){
      delay(15);
      int anim2 = (x/15)%3; 
      int anim_uno = (x/15)%1; 
      int anim_DOS = (x/15)%2; 

      LCD_Sprite(x,60,18,21,GALAGA_uno,1,anim_uno,0,1);  
      V_line(x-2, 60, 21, 0x00); 
      
      LCD_Sprite(x,90,18,21,GALAGA_dos,1,anim_uno,0,1);  
      V_line(x-2, 90, 21, 0x00); 

      LCD_Sprite(x,120,18,21,GALAGA_tres,1,anim_uno,0,1);  
      V_line(x-2, 120, 21, 0x00); 
      
      LCD_Sprite(x,150,20,20,ALIEN_UNO,4,anim2,0,1); 
      V_line(x-2, 150, 20, 0x00);   // los parametros de V_line debe coincidir con y=y_imagen & largo linea=altura_imagen

      LCD_Sprite(x,180,20,20,ALIEN_DOS,4,anim2,0,1); 
      V_line(x-2, 180, 20, 0x00);  

      LCD_Sprite(x,210,20,20,ALIEN_TRES,2,anim2,0,1); 
      V_line(x-2, 210, 20, 0x00);  
      }
    }

    if (Estado_NEXT == LOW){ 
    var = (var + 1)%3;// modulo de la var del switch case
    digitalWrite(Music1_ON, LOW);
    delay(250);
    digitalWrite(Music2_ON, HIGH);
    }
    
    switch (var) {  
      case 1:           //para 2 jugadores 
        flag_juego = 1;  
      break;
      
      case 2:           //para 1 jugador
        flag_juego = 2;
      break;
      
    }

//------------------------  Single Game  ------------------------------------

  
  if(flag_juego == 2){
    digitalWrite(LED_R, LOW);
    digitalWrite(LED_G, HIGH);  
    FillRect(0, 0, 319, 239,0xf0);
    //flag_start = 1;
    }


//----------------------  Multiplayer Mode -------------------------------------

    
  if(flag_juego == 1){
    digitalWrite(LED_G, LOW); 
    digitalWrite(LED_B, HIGH);  
  //FillRect(0, 0, 319, 239,0x00);
    LCD_Clear(0x00);
    flag_start = 1;
    } 

    //goto juego1;
    //juego1:;  // continúa el código


//--------------------------------------------------------------------------------

//---------------------------------------------------------------

  while (flag_start) {

  Estado_SW1   = digitalRead(SW1);
  Estado_SW2   = digitalRead(SW2);
  Estado_SW3   = digitalRead(SW3);
  Estado_SW4   = digitalRead(SW4);
  Estado_SW6   = digitalRead(SW6);  //PC4 -->Atacke Nave 1
  Estado_SW5   = digitalRead(SW5);  //PC5 -->Atacke Nave 2

  int anim1 = (x1_i/5)%2;
  //x2_i = 160;

  //x1_atack1 = x1_i+9 ;     //La coordenada de la nave del jugador 1 es igual a la coord. x de la bala + 9 para estar centrada



// ---------------- Mov. Aliens -----------------------
//Funcion para Movimiento Aliens automático!!!

  unsigned long currentMillis = millis();         
  if(currentMillis - previousMillis > 500) {
    y1_mi = y1_mi + 1; 
    previousMillis  = currentMillis; 
    previousMillis2 = previousMillis+5; 

  if (y1_mi == 319){
      y1_mi = 0; 
  }


//------------------------------------------------
  LCD_Bitmap(30, y1_mi, 20, 20, ALIEN_uno);
    V_line( 30-2,y1_mi, 20, 0x00);   

  LCD_Bitmap(30, y1_mi+20, 20, 20, ALIEN_dos);
    V_line( 30-2,y1_mi+20, 20, 0x00);  

  LCD_Bitmap(30, y1_mi+40, 20, 20, ALIEN_tres);
    V_line( 30-2,y1_mi+40, 20, 0x00);      
//------------------------------------------------
  LCD_Bitmap(70, y1_mi, 20, 20, ALIEN_uno);
    V_line( 70-2,y1_mi, 20, 0x00);   

  LCD_Bitmap(70, y1_mi+20, 20, 20, ALIEN_dos);
    V_line( 70-2,y1_mi+20, 20, 0x00);  

  LCD_Bitmap(70, y1_mi+40, 20, 20, ALIEN_tres);
    V_line( 70-2,y1_mi+40, 20, 0x00);        
//------------------------------------------------
  LCD_Bitmap(110, y1_mi, 20, 20, ALIEN_uno);
    V_line( 110-2,y1_mi, 20, 0x00);   

  LCD_Bitmap(110, y1_mi+20, 20, 20, ALIEN_dos);
    V_line( 110-2,y1_mi+20, 20, 0x00);   

  LCD_Bitmap(110, y1_mi+40, 20, 20, ALIEN_tres);
    V_line( 110-2,y1_mi+40, 20, 0x00);                 
//-------------- ************** ---------------------

  LCD_Bitmap(150, y1_mi, 20, 20, ALIEN_uno);
    V_line(150-2,y1_mi, 20, 0x00);   

  LCD_Bitmap(150, y1_mi+20, 20, 20, ALIEN_dos);
    V_line( 150-2,y1_mi+20, 20, 0x00);     

  LCD_Bitmap(150, y1_mi+40, 20, 20, ALIEN_tres);
    V_line( 150-2,y1_mi+40, 20, 0x00);      
//------------------------------------------------
  LCD_Bitmap(190, y1_mi, 20, 20, ALIEN_uno);
    V_line( 190-2,y1_mi, 20, 0x00);   

  LCD_Bitmap(190, y1_mi+20, 20, 20, ALIEN_dos);
    V_line( 190-2,y1_mi+20, 20, 0x00);  

  LCD_Bitmap(190, y1_mi+40, 20, 20, ALIEN_tres);
    V_line( 190-2,y1_mi+40, 20, 0x00);      

//------------------------------------------------
  LCD_Bitmap(230, y1_mi, 20, 20, ALIEN_uno);
    V_line( 230-2,y1_mi, 20, 0x00);   

  LCD_Bitmap(230, y1_mi+20, 20, 20, ALIEN_dos);
    V_line( 230-2,y1_mi+20, 20, 0x00);  

  LCD_Bitmap(230, y1_mi+40, 20, 20, ALIEN_tres);
    V_line( 230-2,y1_mi+40, 20, 0x00);      
//------------------------------------------------
  LCD_Bitmap(270, y1_mi, 20, 20, ALIEN_uno);
    V_line( 270-2,y1_mi, 20, 0x00);   

  LCD_Bitmap(270, y1_mi+20, 20, 20, ALIEN_dos);
    V_line( 270-2,y1_mi+20, 20, 0x00); 

  LCD_Bitmap(270, y1_mi+40, 20, 20, ALIEN_tres);
    V_line( 270-2,y1_mi+40, 20, 0x00); 

//----------------- MOV. BALAS --------------------    

  //LCD_Bitmap(0, y1_mi+40,  3, 15, BALA_uno);
     //H_line( 0 ,y1_mi+40,  3, 0x0000);
      
         

//-------------- ************** -----------------------
}

objeto1[1] ={y1_mi};      // Definicion de los objetos de las coord.y(coordenada variable) de LOS 21 ALIENS
objeto2[1] ={y1_mi+20};
objeto3[1] ={y1_mi+40};

objeto4[1] ={y1_mi};
objeto5[1] ={y1_mi+20};
objeto6[1] ={y1_mi+40};

objeto7[1] ={y1_mi};
objeto8[1] ={y1_mi+20};
objeto9[1] ={y1_mi+40};

objeto10[1]={y1_mi};
objeto11[1]={y1_mi+20};
objeto12[1]={y1_mi+40};

objeto13[1]={y1_mi};
objeto14[1]={y1_mi+20};
objeto15[1]={y1_mi+40};

objeto16[1]={y1_mi};
objeto17[1]={y1_mi+20};
objeto18[1]={y1_mi+40};

objeto19[1]={y1_mi};
objeto20[1]={y1_mi+20};
objeto21[1]={y1_mi+40};

posicion_bala[0]=xi_bala;   //Definicion de los objetos de las coordenadas de las balas de la nave 1
posicion_bala[1]=y1_atack1;


/*for(uint16_t x=30;x<=270;x=+30){
   for(uint16_t y = y1_mi; y>=(y1_mi-40);y=-20){
    uint16_t objeto[]={};
    objeto[0]={x};
    objeto[1]={y};
    uint8_t d1[]={2,8};
    uint8_t d2[]={10,10};
    if(collider(posicion_bala,objeto,d1,d2)){
      habilitar_disparo=true;
      break;
    }
   }
   if(habilitar_disparo){
    break;
   }
}*/
/*uint16_t objeto[]={};
if(objeto[0]>270){
  objeto[0]=30;
  objeto[1]=+20;
  if(objeto[1]>(y1_mi+40)){
    objeto[1]=y1_mi;
  }
}else{
  objeto[0]=+40;
}*/

//%%%%%%%%%%%%%%%%%%% Ataque Nave 1 %%%%%%%%%%%%%%%%%%

  if((habilitar_disparo == true)&&(Estado_SW5 == LOW )){      //Ataque 1
    habilitar_disparo = false;
    flag_balas = 1;
    xi_bala = x1_i+9; //guarda la coord. x actual de la nave en movimiento!
    
  //FillRect(x1_atack1,   y1_atack1, 3, 15,0xffff);            //Fillrect para borrar disparo finan donde choca con el alien
  //H_line( x1_atack1, y1_atack1+15, 3, 0x0000);               //coord.y + alto imagen
  
  //LCD_Bitmap(x1_atack1, y1_atack1, 3, 15, BALA_dos);
    // H_line( x1_atack1, y1_atack1+16, 3, 0x0000);             //coord.y + alto imagen+1
  
  }

//----------------- MOV. BALAS Nave 1 --------------------   
if((habilitar_disparo == false)) {
  y1_atack1 = y1_atack1 - 1; 
  if(y1_atack1==0){
  habilitar_disparo=true;
  y1_atack1=200;
  flag_balas=0;
}
 
  if (flag_balas){
  LCD_Bitmap(xi_bala, y1_atack1, 3, 15, BALA_uno);
     H_line( xi_bala, y1_atack1+16, 3, 0x0000);               //coord.y + alto imagen+1     
     V_line(xi_bala+2,y1_atack1, 15, 0x00);
  }  
  
}

n++;
n=n%21;
if(naves_choque(n)==1){
  habilitar_disparo=true;
  y1_atack1=200;
  flag_balas=0;
  
}


//%%%%%%%%%%%%%%%%%%% Ataque Nave 2 %%%%%%%%%%%%%%%%%%

  if((habilitar_disparo2 == true)&&(Estado_SW6 == LOW )){      //Ataque 2
    habilitar_disparo2 = false;
    flag_balas2 = 1;
    xi_bala2 = x2_i+9; //guarda la coord. x actual de la nave en movimiento!
    
  
  }

//----------------- MOV. BALAS Nave 2 --------------------   
if((habilitar_disparo2 == false)) {
  y2_atack2 = y2_atack2 - 1; 
  if(y2_atack2==0){
  habilitar_disparo=true;
  y2_atack2=200;
  flag_balas2=0;
}
 
  if (flag_balas2){
  LCD_Bitmap(xi_bala2, y2_atack2, 3, 15, BALA_dos);
     H_line( xi_bala2, y2_atack2+16, 3, 0x0000);               //coord.y + alto imagen+1     
     V_line(xi_bala2+2,y2_atack2, 15, 0x00);
  }  
  
}  

n2++;
n2=n2%21;
if(naves_choque(n2)==1){
  habilitar_disparo2=true;
  y2_atack2=200;
  flag_balas2=0;
  
}
  


 
// ---------------- Jugador 1 -----------------------

  if(Estado_SW1 == LOW && x1_i > 0){ //Izquierda
      x1_i = x1_i-1; 
      }
      LCD_Bitmap(x1_i, 220, 18, 21, GALAGA_uno);
       V_line( x1_i-2, 220, 21, 0x00);

   if(Estado_SW2 == LOW && x1_i < 160-18){ //Derecha
      x1_i = x1_i+1;   
      }
      LCD_Bitmap(x1_i, 220, 18, 21, GALAGA_uno);
       V_line( x1_i+1, 220, 21, 0x00);

// --------------  Jugador 2  ---------------------



  if(Estado_SW3 == LOW && x2_i > 160){ //der
      x2_i = x2_i-1; 
      //delay(2);
      }
      // LCD_Sprite(x2_i,175,18,24,GALAGA_dos,2,0,0,1);
      LCD_Bitmap(x2_i, 220, 18, 21, GALAGA_dos);
        V_line( x2_i-1,220,   21, 0x00);

   if(Estado_SW4 == LOW && x2_i < 320-18){ //Derecha
      x2_i = x2_i+1;
      //delay(2);
      }
      //LCD_Sprite(x2_i,136,18,24,planta,2,anim1,0,1);
      LCD_Bitmap(x2_i, 220, 18, 21, GALAGA_dos);
       V_line( x2_i+1, 220, 21, 0x00);     
      }
      
//-------------------------------------------------------------------
} //Llave loop principal

//***************************************************************************************************************************************
// Función para inicializar LCD
//***************************************************************************************************************************************
void LCD_Init(void) {
  pinMode(LCD_RST, OUTPUT);
  pinMode(LCD_CS, OUTPUT);
  pinMode(LCD_RS, OUTPUT);
  pinMode(LCD_WR, OUTPUT);
  pinMode(LCD_RD, OUTPUT);
  for (uint8_t i = 0; i < 8; i++){
    pinMode(DPINS[i], OUTPUT);
  }
  //****************************************
  // Secuencia de Inicialización
  //****************************************
  digitalWrite(LCD_CS, HIGH);
  digitalWrite(LCD_RS, HIGH);
  digitalWrite(LCD_WR, HIGH);
  digitalWrite(LCD_RD, HIGH);
  digitalWrite(LCD_RST, HIGH);
  delay(5);
  digitalWrite(LCD_RST, LOW);
  delay(20);
  digitalWrite(LCD_RST, HIGH);
  delay(150);
  digitalWrite(LCD_CS, LOW);
  //****************************************
  LCD_CMD(0xE9);  // SETPANELRELATED
  LCD_DATA(0x20);
  //****************************************
  LCD_CMD(0x11); // Exit Sleep SLEEP OUT (SLPOUT)
  delay(100);
  //****************************************
  LCD_CMD(0xD1);    // (SETVCOM)
  LCD_DATA(0x00);
  LCD_DATA(0x71);
  LCD_DATA(0x19);
  //****************************************
  LCD_CMD(0xD0);   // (SETPOWER) 
  LCD_DATA(0x07);
  LCD_DATA(0x01);
  LCD_DATA(0x08);
  //****************************************
  LCD_CMD(0x36);  // (MEMORYACCESS)
  LCD_DATA(0x40|0x80|0x20|0x08); // LCD_DATA(0x19);
  //****************************************
  LCD_CMD(0x3A); // Set_pixel_format (PIXELFORMAT)
  LCD_DATA(0x05); // color setings, 05h - 16bit pixel, 11h - 3bit pixel
  //****************************************
  LCD_CMD(0xC1);    // (POWERCONTROL2)
  LCD_DATA(0x10);
  LCD_DATA(0x10);
  LCD_DATA(0x02);
  LCD_DATA(0x02);
  //****************************************
  LCD_CMD(0xC0); // Set Default Gamma (POWERCONTROL1)
  LCD_DATA(0x00);
  LCD_DATA(0x35);
  LCD_DATA(0x00);
  LCD_DATA(0x00);
  LCD_DATA(0x01);
  LCD_DATA(0x02);
  //****************************************
  LCD_CMD(0xC5); // Set Frame Rate (VCOMCONTROL1)
  LCD_DATA(0x04); // 72Hz
  //****************************************
  LCD_CMD(0xD2); // Power Settings  (SETPWRNORMAL)
  LCD_DATA(0x01);
  LCD_DATA(0x44);
  //****************************************
  LCD_CMD(0xC8); //Set Gamma  (GAMMASET)
  LCD_DATA(0x04);
  LCD_DATA(0x67);
  LCD_DATA(0x35);
  LCD_DATA(0x04);
  LCD_DATA(0x08);
  LCD_DATA(0x06);
  LCD_DATA(0x24);
  LCD_DATA(0x01);
  LCD_DATA(0x37);
  LCD_DATA(0x40);
  LCD_DATA(0x03);
  LCD_DATA(0x10);
  LCD_DATA(0x08);
  LCD_DATA(0x80);
  LCD_DATA(0x00);
  //****************************************
  LCD_CMD(0x2A); // Set_column_address 320px (CASET)
  LCD_DATA(0x00);
  LCD_DATA(0x00);
  LCD_DATA(0x01);
  LCD_DATA(0x3F);
  //****************************************
  LCD_CMD(0x2B); // Set_page_address 480px (PASET)
  LCD_DATA(0x00);
  LCD_DATA(0x00);
  LCD_DATA(0x01);
  LCD_DATA(0xE0);
//  LCD_DATA(0x8F);
  LCD_CMD(0x29); //display on 
  LCD_CMD(0x2C); //display on

  LCD_CMD(ILI9341_INVOFF); //Invert Off
  delay(120);
  LCD_CMD(ILI9341_SLPOUT);    //Exit Sleep
  delay(120);
  LCD_CMD(ILI9341_DISPON);    //Display on
  digitalWrite(LCD_CS, HIGH);
}
//***************************************************************************************************************************************
// Función para enviar comandos a la LCD - parámetro (comando)
//***************************************************************************************************************************************
void LCD_CMD(uint8_t cmd) {
  digitalWrite(LCD_RS, LOW);
  digitalWrite(LCD_WR, LOW);
  GPIO_PORTB_DATA_R = cmd;
  digitalWrite(LCD_WR, HIGH);
}
//***************************************************************************************************************************************
// Función para enviar datos a la LCD - parámetro (dato)
//***************************************************************************************************************************************
void LCD_DATA(uint8_t data) {
  digitalWrite(LCD_RS, HIGH);
  digitalWrite(LCD_WR, LOW);
  GPIO_PORTB_DATA_R = data;
  digitalWrite(LCD_WR, HIGH);
}
//***************************************************************************************************************************************
// Función para definir rango de direcciones de memoria con las cuales se trabajara (se define una ventana)
//***************************************************************************************************************************************
void SetWindows(unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2) {
  LCD_CMD(0x2a); // Set_column_address 4 parameters
  LCD_DATA(x1 >> 8);
  LCD_DATA(x1);   
  LCD_DATA(x2 >> 8);
  LCD_DATA(x2);   
  LCD_CMD(0x2b); // Set_page_address 4 parameters
  LCD_DATA(y1 >> 8);
  LCD_DATA(y1);   
  LCD_DATA(y2 >> 8);
  LCD_DATA(y2);   
  LCD_CMD(0x2c); // Write_memory_start
}
//***************************************************************************************************************************************
// Función para borrar la pantalla - parámetros (color)
//***************************************************************************************************************************************
void LCD_Clear(unsigned int c){  
  unsigned int x, y;
  LCD_CMD(0x02c); // write_memory_start
  digitalWrite(LCD_RS, HIGH);
  digitalWrite(LCD_CS, LOW);   
  SetWindows(0, 0, 319, 239); // 479, 319);
  for (x = 0; x < 320; x++)
    for (y = 0; y < 240; y++) {
      LCD_DATA(c >> 8); 
      LCD_DATA(c); 
    }
  digitalWrite(LCD_CS, HIGH);
} 
//***************************************************************************************************************************************
// Función para dibujar una línea horizontal - parámetros ( coordenada x, cordenada y, longitud, color)
//*************************************************************************************************************************************** 
void H_line(unsigned int x, unsigned int y, unsigned int l, unsigned int c) {  
  unsigned int i, j;
  LCD_CMD(0x02c); //write_memory_start
  digitalWrite(LCD_RS, HIGH);
  digitalWrite(LCD_CS, LOW);
  l = l + x;
  SetWindows(x, y, l, y);
  j = l;// * 2;
  for (i = 0; i < l; i++) {
      LCD_DATA(c >> 8); 
      LCD_DATA(c); 
  }
  digitalWrite(LCD_CS, HIGH);
}
//***************************************************************************************************************************************
// Función para dibujar una línea vertical - parámetros ( coordenada x, cordenada y, longitud, color)
//*************************************************************************************************************************************** 
void V_line(unsigned int x, unsigned int y, unsigned int l, unsigned int c) {  
  unsigned int i,j;
  LCD_CMD(0x02c); //write_memory_start
  digitalWrite(LCD_RS, HIGH);
  digitalWrite(LCD_CS, LOW);
  l = l + y;
  SetWindows(x, y, x, l);
  j = l; //* 2;
  for (i = 1; i <= j; i++) {
    LCD_DATA(c >> 8); 
    LCD_DATA(c);
  }
  digitalWrite(LCD_CS, HIGH);  
}
//***************************************************************************************************************************************
// Función para dibujar un rectángulo - parámetros ( coordenada x, cordenada y, ancho, alto, color)
//***************************************************************************************************************************************
void Rect(unsigned int x, unsigned int y, unsigned int w, unsigned int h, unsigned int c) {
  H_line(x  , y  , w, c);
  H_line(x  , y+h, w, c);
  V_line(x  , y  , h, c);
  V_line(x+w, y  , h, c);
}
//***************************************************************************************************************************************
// Función para dibujar un rectángulo relleno - parámetros ( coordenada x, cordenada y, ancho, alto, color)
//***************************************************************************************************************************************
void FillRect(unsigned int x, unsigned int y, unsigned int w, unsigned int h, unsigned int c) {
  unsigned int i;
  for (i = 0; i < h; i++) {
    H_line(x  , y  , w, c);
    H_line(x  , y+i, w, c);
  }
}
//***************************************************************************************************************************************
// Función para dibujar texto - parámetros ( texto, coordenada x, cordenada y, color, background) 
//***************************************************************************************************************************************
void LCD_Print(String text, int x, int y, int fontSize, int color, int background) {
  int fontXSize ;
  int fontYSize ;
  
  if(fontSize == 1){
    fontXSize = fontXSizeSmal ;
    fontYSize = fontYSizeSmal ;
  }
  if(fontSize == 2){
    fontXSize = fontXSizeBig ;
    fontYSize = fontYSizeBig ;
  }
  
  char charInput ;
  int cLength = text.length();
  Serial.println(cLength,DEC);
  int charDec ;
  int c ;
  int charHex ;
  char char_array[cLength+1];
  text.toCharArray(char_array, cLength+1) ;
  for (int i = 0; i < cLength ; i++) {
    charInput = char_array[i];
    Serial.println(char_array[i]);
    charDec = int(charInput);
    digitalWrite(LCD_CS, LOW);
    SetWindows(x + (i * fontXSize), y, x + (i * fontXSize) + fontXSize - 1, y + fontYSize );
    long charHex1 ;
    for ( int n = 0 ; n < fontYSize ; n++ ) {
      if (fontSize == 1){
        charHex1 = pgm_read_word_near(smallFont + ((charDec - 32) * fontYSize) + n);
      }
      if (fontSize == 2){
        charHex1 = pgm_read_word_near(bigFont + ((charDec - 32) * fontYSize) + n);
      }
      for (int t = 1; t < fontXSize + 1 ; t++) {
        if (( charHex1 & (1 << (fontXSize - t))) > 0 ) {
          c = color ;
        } else {
          c = background ;
        }
        LCD_DATA(c >> 8);
        LCD_DATA(c);
      }
    }
    digitalWrite(LCD_CS, HIGH);
  }
}
//***************************************************************************************************************************************
// Función para dibujar una imagen a partir de un arreglo de colores (Bitmap) Formato (Color 16bit R 5bits G 6bits B 5bits)
//***************************************************************************************************************************************
void LCD_Bitmap(unsigned int x, unsigned int y, unsigned int width, unsigned int height, unsigned char bitmap[]){  
  LCD_CMD(0x02c); // write_memory_start
  digitalWrite(LCD_RS, HIGH);
  digitalWrite(LCD_CS, LOW); 
  
  unsigned int x2, y2;
  x2 = x+width;
  y2 = y+height;
  SetWindows(x, y, x2-1, y2-1);
  unsigned int k = 0;
  unsigned int i, j;

  for (int i = 0; i < width; i++) {
    for (int j = 0; j < height; j++) {
      LCD_DATA(bitmap[k]);
      LCD_DATA(bitmap[k+1]);
      //LCD_DATA(bitmap[k]);    
      k = k + 2;
     } 
  }
  digitalWrite(LCD_CS, HIGH);
}
//***************************************************************************************************************************************
// Función para dibujar una imagen sprite - los parámetros columns = número de imagenes en el sprite, index = cual desplegar, flip = darle vuelta
//***************************************************************************************************************************************
void LCD_Sprite(int x, int y, int width, int height, unsigned char bitmap[],int columns, int index, char flip, char offset){
  LCD_CMD(0x02c); // write_memory_start
  digitalWrite(LCD_RS, HIGH);
  digitalWrite(LCD_CS, LOW); 

  unsigned int x2, y2;
  x2 =   x+width;
  y2=    y+height;
  SetWindows(x, y, x2-1, y2-1);
  int k = 0;
  int ancho = ((width*columns));
  if(flip){
  for (int j = 0; j < height; j++){
      k = (j*(ancho) + index*width -1 - offset)*2;
      k = k+width*2;
     for (int i = 0; i < width; i++){
      LCD_DATA(bitmap[k]);
      LCD_DATA(bitmap[k+1]);
      k = k - 2;
     } 
  }
  }else{
     for (int j = 0; j < height; j++){
      k = (j*(ancho) + index*width + 1 + offset)*2;
     for (int i = 0; i < width; i++){
      LCD_DATA(bitmap[k]);
      LCD_DATA(bitmap[k+1]);
      k = k + 2;
     } 
  }
    
    
    }
  digitalWrite(LCD_CS, HIGH);
}


// NUEVAS FUNCIONES ---------------------------------------------------------------------------

uint8_t collider(uint16_t objeto1[], uint16_t objeto2[], uint8_t d1[] , uint8_t d2[]) {     //FUNCION PARA COMPAR CHOQUE BALA VS ALIEN
  uint16_t dx;
  uint16_t dy;
  uint16_t objeto_x = abs((objeto1[0] + d1[0]) - (objeto2[0] + d2[0]));
  uint16_t objeto_y = abs((objeto1[1] + d1[1]) - (objeto2[1] + d2[1]));
  dx = abs(d1[0] + d2[0]);
  dy = abs(d1[1] + d2[1]);
  if (((objeto_x - dx) < 0) && ((objeto_y - dy) < 0)) {
    return 1; //chocaron ambos ejes
  } else {
    return 0; //no choco
  }
}

uint8_t naves_choque(uint8_t posicion){                         //FUNCION PARA CHOQUE C/NAVE INDIVIDUAL 
  uint8_t temp;
  switch(posicion){
    
    case 0:
    temp=collider(posicion_bala,objeto1,d1,d2);
    return temp;
    break;
    
    case 1:
    temp=collider(posicion_bala,objeto2,d1,d2);
    return temp;
    break;
    
    case 2:
    temp=collider(posicion_bala,objeto3,d1,d2);
    if(temp){
      FillRect(objeto3[0],objeto3[1],20,20,0xFF); //------------------- alien de la fila de hasta abajo
    }
    return temp;
    break;

    case 3:
    temp=collider(posicion_bala,objeto4,d1,d2);
    return temp;
    break;

    case 4:
    temp =collider(posicion_bala,objeto5,d1,d2);
    return temp;
    break;

    case 5:
    temp=collider(posicion_bala,objeto6,d1,d2);
    if(temp){
      FillRect(objeto6[0],objeto6[1],20,20,0x00); //------------------- alien de la fila de hasta abajo
    }
    return temp;
    break;

    case 6:
    temp=collider(posicion_bala,objeto7,d1,d2);
    return temp;
    break;

    case 7:
    temp=collider(posicion_bala,objeto8,d1,d2);
    return temp;
    break;

    case 8:
    temp=collider(posicion_bala,objeto9,d1,d2);
    return temp;
    break;

    case 9:
    temp=collider(posicion_bala,objeto10,d1,d2);
    return temp;
    break;

    case 10:
    temp=collider(posicion_bala,objeto11,d1,d2);
    return temp;
    break;

    case 11:
    temp=collider(posicion_bala,objeto12,d1,d2);
    return temp;
    break;

    case 12:
    temp=collider(posicion_bala,objeto13,d1,d2);
    return temp;
    break;

    case 13:
    temp=collider(posicion_bala,objeto14,d1,d2);
    return temp;
    break;

    case 14:
    temp=collider(posicion_bala,objeto15,d1,d2);
    return temp;
    break;

    case 15:
    temp=collider(posicion_bala,objeto16,d1,d2);
    return temp;
    break;

    case 16:
    temp=collider(posicion_bala,objeto17,d1,d2);
    return temp;
    break;

    case 17:
    temp=collider(posicion_bala,objeto18,d1,d2);
    return temp;
    break;

    case 18:
    temp=collider(posicion_bala,objeto19,d1,d2);
    return temp;
    break;

    case 19:
    temp=collider(posicion_bala,objeto20,d1,d2);
    return temp;
    break;

    case 20:
    temp=collider(posicion_bala,objeto21,d1,d2);
    return temp;
    break;


  }
}


//**********************  Funciones SD 
//**********************
/*

unsigned char Char_to_uChar(char letra){
  unsigned char num;
  if(letra>=48 && letra <=57){
    num = letra - 48;
  }
  else if (letra >= 97 && letra <=102){
    num = letra -87;
  }
  return num;
}
void SD_Bitmap(int x, int y, int width, int height, char *filename){
  uint16_t SD_x;
  uint16_t SD_y;
  uint16_t ent;
  uint16_t contador = 1;
  uint16_t temp;
  uint16_t temp2;
  uint16_t linea_salto = 1;
  myFile = SD.open(filename);
  LCD_CMD(0x02c); //write_memory_start
  digitalWrite(LCD_RS, HIGH);
  digitalWrite(LCD_CS, LOW);
  SetWindows(x,y,x+width-1,y+height-1);
  if (myFile){
    SD_x = x;
    SD_y = y;
    while (myFile.available()) {
      uint16_t leer = myFile.read();
      leer = Char_to_uChar(leer);
        if (contador == 1){
          temp2 = leer << 12;
          temp = temp2;
          contador++;
        }
        else if (contador == 2){
          temp2 = leer << 8;
          temp = temp+temp2;
          contador++;
        }
        else if (contador == 3){
          temp2 = leer<<4;
          temp = temp+temp2;
          contador++;
        }
        else if (contador == 4){
          linea_salto++;
          contador = 1;
          temp = temp+leer;
          LCD_DATA(temp>>8);
          LCD_DATA(temp);
          SD_x = SD_x+1;
          if(SD_x==((x+width))){
            SD_x = x;
            SD_y = SD_y+1;
          }
        }
        
          else;
        }
      
      digitalWrite(LCD_CS, HIGH);
      myFile.close();
  }
  
  else{
    Serial.print("error opening ");
  }
} */
