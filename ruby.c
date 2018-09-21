/* 
 * File:   varun_4_1.c
 * Author: VARUN SAHNI
 *
 * Created on changed 24 April, 2018, 8:40 PM
 * this is test code for 6 switch 1 fan for black touch panel
 * status: good workingg but problem with harware:
 * do changes in action panel function
 */

#include <stdio.h>
#include <stdlib.h>
#include <pic16f1526.h>
// 'C' source line config statements

// CONFIG1
#pragma config FOSC = HS        // Oscillator Selection (HS Oscillator, High-speed crystal/resonator connected between OSC1 and OSC2 pins)
#pragma config WDTE = OFF       // Watchdog Timer Enable (WDT disabled)
#pragma config PWRTE = OFF      // Power-up Timer Enable (PWRT disabled)
#pragma config MCLRE = OFF      // MCLR Pin Function Select (MCLR/VPP pin function is digital input)
#pragma config CP = OFF         // Flash Program Memory Code Protection (Program memory code protection is disabled)
#pragma config BOREN = OFF      // Brown-out Reset Enable (Brown-out Reset disabled)
#pragma config CLKOUTEN = OFF   // Clock Out Enable (CLKOUT function is disabled. I/O or oscillator function on the CLKOUT pin)
#pragma config IESO = OFF       // Internal/External Switchover (Internal/External Switchover mode is disabled)
#pragma config FCMEN = OFF      // Fail-Safe Clock Monitor Enable (Fail-Safe Clock Monitor is disabled)

// CONFIG2
#pragma config WRT = OFF        // Flash Memory Self-Write Protection (Write protection off)
#pragma config VCAPEN = OFF     // Voltage Regulator Capacitor Enable bit (VCAP pin function disabled)
#pragma config STVREN = OFF     // Stack Overflow/Underflow Reset Enable (Stack Overflow or Underflow will not cause a Reset)
#pragma config BORV = LO        // Brown-out Reset Voltage Selection (Brown-out Reset Voltage (Vbor), low trip point selected.)
#pragma config LPBOR = OFF      // Low-Power Brown Out Reset (Low-Power BOR is disabled)
#pragma config LVP = OFF        // Low-Voltage Programming Enable (High-voltage on MCLR/VPP must be used for programming)

// #pragma config statements should precede project file includes.
// Use project enums instead of #define for ON and OFF.

#include <xc.h>
// Since we have used 16 MHz crystal
#define _XTAL_FREQ 16000000  

// Pin MACROS
#define OUTPUT_RELAY1 RB1
#define OUTPUT_RELAY2 RC1
#define OUTPUT_RELAY3 RA0
#define OUTPUT_RELAY4 RF1
#define OUTPUT_RELAY5 RA3
#define OUTPUT_RELAY6 RA1
//#define OUTPUT_REGULATOR RA2
//#define OUTPUT_FAN_1 RB3
//#define OUTPUT_FAN_2 RA5
//#define OUTPUT_FAN_3 RC0

#define OUTPUT_REGULATOR RE1
#define OUTPUT_FAN_1 RB0
#define OUTPUT_FAN_2 RB4
#define OUTPUT_FAN_3 RB2

//#define OUTPUT_RELAY7 RA2
//#define OUTPUT_RELAY8 RB3

#define OUTPUT_RELAY_DIR_1 TRISBbits.TRISB1
#define OUTPUT_RELAY_DIR_2 TRISCbits.TRISC1
#define OUTPUT_RELAY_DIR_3 TRISAbits.TRISA0
#define OUTPUT_RELAY_DIR_4 TRISFbits.TRISF1
#define OUTPUT_RELAY_DIR_5 TRISAbits.TRISA3        
#define OUTPUT_RELAY_DIR_6 TRISAbits.TRISA1 
//#define OUTPUT_REGULATOR_DIR TRISAbits.TRISA2
//#define OUTPUT_FAN_1_DIR TRISBbits.TRISB3
//#define OUTPUT_FAN_2_DIR TRISAbits.TRISA5
//#define OUTPUR_FAN_3_DIR TRISCbits.TRISC0

//*********FAN PINS************//
#define OUTPUT_REGULATOR_DIR TRISEbits.TRISE1
#define OUTPUT_FAN_1_DIR TRISBbits.TRISB0
#define OUTPUT_FAN_2_DIR TRISBbits.TRISB4
#define OUTPUR_FAN_3_DIR TRISBbits.TRISB2

//#define OUTPUT_RELAY_DIR_7 TRISAbits.TRISA2
//#define OUTPUT_RELAY_DIR_8 TRISbbits.TRISB3





/*
 * Extra Periferals Direction and PORT
 */
//#define ZCD_CCP9_DIR TRISEbits.TRISE3
// USART Directions
#define USART_1_TRANSMIT_OUTPUT_DIR TRISCbits.TRISC6
#define USART_1_RECIEVE_INPUT_DIR TRISCbits.TRISC7

#define USART_2_TRANSMIT_OUTPUT_DIR TRISGbits.TRISG1
#define USART_2_RECIEVE_INPUT_DIR TRISGbits.TRISG2

#define RECIEVED_DATA_LENGTH (16*2)
#define TOTAL_NUMBER_OF_SWITCH (16*2)


#define TOUCHPANEL_DATA_LENGTH (16*2)
#define TRUE 1
#define FALSE 0
#define ON 1
#define OFF 0

#define CHAR_ON '1'
#define CHAR_OFF '0'

// Conditional compilation
//#define DEBUG
//#define RELEASE


//#define SWITCH_7_RELAY
//#define SWITCH_8_RELAY


// ALL error Definitions
/* 
 * #define WRONG_DATA_RECIEVED_ERROR_CODE ERRX
 * #define RECIVING_OVERRUN_ERROR EROV
 * #define RECEIVING_DATA_LOST_IN_MAIN ERLS
 */
/* DATA USED IN MANUAL  STARTS HERE*/
unsigned int M1;unsigned int M2;unsigned int M3;unsigned int M4;unsigned int M5;unsigned int M6;unsigned int M7;unsigned int M8;


#define ON 1
#define OFF 0
#define CHAR_OFF '0'
#define CHAR_ON '1'
        
/* DATA USED IN MANUAL END HERE*/




unsigned char ErrorNames[5]="####";

int mainReceivedDataPosition=0, mainDataReceived=FALSE;
unsigned char mainReceivedDataBuffer[RECIEVED_DATA_LENGTH]="#"; 
unsigned char tempReceivedDataBuffer[RECIEVED_DATA_LENGTH-8]="#";
unsigned char parentalLockBuffer[TOTAL_NUMBER_OF_SWITCH]="00000000000000000000000000000000";
unsigned char copy_parentalLockBuffer[TOTAL_NUMBER_OF_SWITCH]="00000000000000000000000000000000";
unsigned char currentStateBuffer[(TOTAL_NUMBER_OF_SWITCH*4)+2]="#";



int touchpanelReceivedataPosition = 0; 
int touchPanelDataReceived = FALSE;
unsigned char touchpanleReceivedDatabuffer[TOUCHPANEL_DATA_LENGTH]="#";
unsigned char tempReceiveTouchpanelDataBuffer[TOUCHPANEL_DATA_LENGTH-8]="#";


#define TouchMatikBoardAddress 'f'



int start_PWM_Generation_in_ISR_FLAG=FALSE;
char levelofDimmer_MSB='0',levelofDimmer_LSB='0';

void errorsISR(char* errNum);
void errorsMain(char* errNum);
void sendAcknowledgment(char* currentStateBuffer);
void sendFeedback_TO_Gateway(char sw_status, char Switch_Num);
void sendFeedback_TO_Touch(char Switch_Num_1s, char sw_status);

void clearAllPorts();
void pinINIT_extra();
void GPIO_pin_Initialize();

void AllInterruptEnable();
void EUSART_Initialize();
void EUSART2_Initialize();

void TMR3_Initialize();
void TMR1_Initialize();
void CCP9_Initialize();
void allPeripheralInit();

void copyReceivedDataBuffer();
void copyTouchpanelReceiveDataBuffer();
void applianceControl(char switchMSB, char switchLSB, char switchSTATE, char dimmerSpeedMSB, char dimmerSpeedLSB, char parentalControl, char finalFrameState);

void actiontouchPanel(char Switch_Num, char sw_status, char speeds );//, char speeds



interrupt void isr(){
   
    
    
    // ************************************* XbEE UART INTERRUPT *********************************************** //
    if(RC1IF){        
        if(RC1STAbits.OERR){    // If over run error, then reset the receiver
            RC1STAbits.CREN = 0; // countinuous Recieve Disable
            RC1STAbits.CREN = 1; // countinuous Recieve Enable
            
            ErrorNames[0]='E';      ErrorNames[1]='R';      ErrorNames[2]='O';      ErrorNames[3]='V';
            errorsISR(ErrorNames); 
        } 
        mainReceivedDataBuffer[mainReceivedDataPosition]=RC1REG;
        #ifdef DEBUG
        TX1REG=mainReceivedDataBuffer[mainReceivedDataPosition];
        #endif
        if(mainReceivedDataBuffer[0]=='%'){
            mainReceivedDataPosition++;
            if(mainReceivedDataPosition>15){
                mainDataReceived=TRUE;
                mainReceivedDataPosition=0;                
                RC1IF=0;                
            }
        }
        else{
            RC1STAbits.CREN = 0; // countinuous Recieve Disable
            RC1STAbits.CREN = 1; // countinuous Recieve Enable
            mainReceivedDataPosition=0; // Reinitiate buffer counter
            
            ErrorNames[0]='E';      ErrorNames[1]='R';      ErrorNames[2]='R';      ErrorNames[3]='X';
            errorsISR(ErrorNames);            
        }
    }// End of RC1IF
     
     /**************************************TOUCH_PANEL INTERRUPT*******************************************/
    if(RC2IF){        
        if(RC2STAbits.OERR){    // If over run error, then reset the receiver
            RC2STAbits.CREN = 0; // countinuous Recieve Disable
            RC2STAbits.CREN = 1; // countinuous Recieve Enable
            
            ErrorNames[0]='E';      ErrorNames[1]='R';      ErrorNames[2]='O';      ErrorNames[3]='V';
            errorsISR(ErrorNames); 
        }   
        
        touchpanleReceivedDatabuffer[touchpanelReceivedataPosition] = RC2REG;
        if(touchpanleReceivedDatabuffer[0] == '(')
        {
            touchpanelReceivedataPosition++;
            if(touchpanelReceivedataPosition > 7)
            {
                touchPanelDataReceived = TRUE;
            
                touchpanelReceivedataPosition=0;
                 RC2IF = 0;
            }
        }
        else{
            RC2STAbits.CREN = 0; // countinuous Recieve Disable
            RC2STAbits.CREN = 1; // countinuous Recieve Enable
            touchpanelReceivedataPosition=0; // Reinitiate buffer counter
            
            ErrorNames[0]='E';      ErrorNames[1]='R';      ErrorNames[2]='R';      ErrorNames[3]='T';
            errorsISR(ErrorNames);            
        }
    }//End of RC2IF
}




/*
 * Alfaone Main code starts here
 * 
 */
int main() {

                OUTPUT_REGULATOR = ON;
             
                OUTPUT_FAN_1=OFF;
                
                OUTPUT_FAN_2=OFF;
                
                OUTPUT_FAN_3=OFF;
       
    GPIO_pin_Initialize();
    allPeripheralInit();
   // AllInterruptEnable();

    while(1){
         ///STARTING OF MOBILE APP DATA RECEIVE
        if(mainDataReceived==TRUE){
            mainDataReceived=FALSE;
            int start_flag = 0;
            int end_flag = 0;
            if(mainReceivedDataBuffer[0]=='%' && mainReceivedDataBuffer[1]=='%' && mainReceivedDataBuffer[14]=='@' && mainReceivedDataBuffer[15]=='@'){
                if(mainReceivedDataBuffer[0] == '%' && mainReceivedDataBuffer[1]=='%' && start_flag == 0)
                {
                    end_flag = 1;
                }
                if(mainReceivedDataBuffer[14]=='@' && mainReceivedDataBuffer[15]=='@' && end_flag ==1)
                {
                    copyReceivedDataBuffer();
                                 start_flag = 0;
                                   end_flag = 0;
                }
                
                
                
                
                
                applianceControl(tempReceivedDataBuffer[0],
                        tempReceivedDataBuffer[1],
                        tempReceivedDataBuffer[2],
                        tempReceivedDataBuffer[3],
                        tempReceivedDataBuffer[4],
                        tempReceivedDataBuffer[5],
                        tempReceivedDataBuffer[6]);
                                
            }   // End of all buffer data check
            else{
                ErrorNames[0]='E';      ErrorNames[1]='R';      ErrorNames[2]='L';      ErrorNames[3]='S';
                errorsMain(ErrorNames);
                RC1STAbits.SPEN=0;  // Serial port disabled 
                RC1STAbits.CREN = 0; // countinuous Recieve Disable                
                for(int dataBufferCounter = 0; dataBufferCounter< 15; dataBufferCounter++)
                {
                    mainReceivedDataBuffer[dataBufferCounter] = '#'; // clean received data buffer
                }
                RC1STAbits.CREN = 1; // countinuous Recieve Enable
                RC1STAbits.SPEN=1;  // Serial port enabled (configures RXx/DTx and TXx/CKx pins as serial port pins)
            }
        } // End of mainDataReceived condition
        
        ///STARTING OF TOUCHPANEL DATA RECEIVE
        if(touchPanelDataReceived == TRUE)
        {
          //  TX1REG = 'R';
            touchPanelDataReceived = FALSE;
            int start_flag = 0;
            int end_flag = 0;
            if(touchpanleReceivedDatabuffer[0] == '(' && touchpanleReceivedDatabuffer[7] == ')')
            {
                
                if(touchpanleReceivedDatabuffer[0] == '('  && start_flag == 0)
                {
                    end_flag =1;

                }
                if(touchpanleReceivedDatabuffer[7] == ')' && end_flag ==1)
                {
                copyTouchpanelReceiveDataBuffer();
                if(tempReceiveTouchpanelDataBuffer[0] != '@'){
                  //  TX1REG= tempReceiveTouchpanelDataBuffer[0];
                 //   __delay_ms(10);
                 //   TX1REG= tempReceiveTouchpanelDataBuffer[1];__delay_ms(10);
                 //   TX1REG= tempReceiveTouchpanelDataBuffer[2];__delay_ms(10);
                   actiontouchPanel(tempReceiveTouchpanelDataBuffer[0],tempReceiveTouchpanelDataBuffer[1],tempReceiveTouchpanelDataBuffer[2]); //,tempReceiveTouchpanelDataBuffer[2]
                    start_flag = 0;
                    end_flag = 0;
                }
                                
                }
               
            }
                else
                {
                ErrorNames[0]='E';      ErrorNames[1]='R';      ErrorNames[2]='L';      ErrorNames[3]='S';
                errorsMain(ErrorNames);
                RC2STAbits.SPEN = 0;  // Serial port disabled  
                RC2STAbits.CREN = 0; // countinuous Recieve Disable                
                for(int dataBufferCounter = 0; dataBufferCounter< 8; dataBufferCounter++)
                {
                    touchpanleReceivedDatabuffer[dataBufferCounter] = '#'; // clean received data buffer
                }
                RC2STAbits.CREN = 1; // countinuous Recieve Enable
                RC2STAbits.SPEN=1;  // Serial port enabled (configures RXx/DTx and TXx/CKx pins as serial port pins)
            }
            
        }
        
       
}
}

void applianceControl(char charSwitchMSB, char charSwitchLSB, char charSwitchSTATE, char chDimmerSpeedMSB, char chDimmerSpeedLSB,
        char charParentalControl, char charFinalFrameState){
    
    //define used variables and initilize it with zero
    int integerSwitchNumber = 0;
    int integerSwitchState = 0;
    int integerSpeed = 0;
    int currentStateBufferPositions=0;
   // TX1REG = charParentalControl;
    // Get switch Number in Integer format 
    //define all used character data types and initlize it with "#"
    char switchNumberStringBuffer[2]="#";
    char dimmerSpeedStringBuffer[2]="#";
    
    switchNumberStringBuffer[0]=charSwitchMSB;
    switchNumberStringBuffer[1]=charSwitchLSB;    
    integerSwitchNumber = atoi(switchNumberStringBuffer);//convert string into integer
    
    // Get switch State in Integer Format
    
    integerSwitchState = charSwitchSTATE-'0';
    
    // Get speed of Fan or level of dimmer    
    dimmerSpeedStringBuffer[0]=chDimmerSpeedMSB;
    dimmerSpeedStringBuffer[1]=chDimmerSpeedLSB;    
    integerSpeed = atoi(dimmerSpeedStringBuffer);
    
    // save Parental lock state of each switch into parental lock buffer
//    int integerParentalControl=charParentalControl-'0';
    parentalLockBuffer[integerSwitchNumber] = charParentalControl;
   
   
    copy_parentalLockBuffer[integerSwitchNumber]=parentalLockBuffer[integerSwitchNumber];
  //   TX1REG = parentalLockBuffer[integerSwitchNumber]; //ok same
  //   TX1REG = copy_parentalLockBuffer[integerSwitchNumber];
    // ACKNOWLEDGMENT data Format :->> (Gateway+SwitchState+SwitchMSB+SwitchLSB)
    
    currentStateBufferPositions = ((1+4*(integerSwitchNumber))-5);
    currentStateBuffer[currentStateBufferPositions++] = 'G';
    currentStateBuffer[currentStateBufferPositions++] = charSwitchSTATE;
    currentStateBuffer[currentStateBufferPositions++] = charSwitchMSB;
    currentStateBuffer[currentStateBufferPositions] = charSwitchLSB;    
    
    currentStateBufferPositions-=3;     // since we have come forward by 3 address in current state buffer
    if(charFinalFrameState=='1')    // until 
    {
        sendAcknowledgment(currentStateBuffer+currentStateBufferPositions);  
        if(integerSwitchNumber != 7){
        __delay_ms(5);
        TX2REG = '(' ;
        __delay_ms(1);
        TX2REG = 'f' ;//touchmatoc address
        __delay_ms(1);
        TX2REG =charSwitchLSB + 16 ;
        __delay_ms(1);
        TX2REG=charSwitchSTATE;
        __delay_ms(1);
        TX2REG='0';
        __delay_ms(1);
        TX2REG='0';
        __delay_ms(1);
        TX2REG='0';
        __delay_ms(1);
        TX2REG=')';
        }
        if(integerSwitchNumber == 7)  {
           // TX1REG ='7';
                switch(integerSwitchState){
                case 0:
                {
                // TX1REG = '0';
                __delay_ms(5);
                TX2REG = '(' ;
                __delay_ms(1);
                TX2REG = 'f' ;//touchmatoc address
                __delay_ms(1);
                TX2REG ='P' ;
                __delay_ms(1);
                TX2REG='0';
                __delay_ms(1);
                TX2REG='0';//fan speed
                __delay_ms(1);
                TX2REG='0';
                __delay_ms(1);
                TX2REG='0';
                __delay_ms(1);
                TX2REG=')';
                }
                break;
                case 1: {
                    if(chDimmerSpeedMSB == '0'){
                                                                
                  //           TX1REG = '0';
                            __delay_ms(5);
                            TX2REG = '(' ;
                            __delay_ms(1);
                            TX2REG = 'f' ;//touchmatoc address
                            __delay_ms(1);
                            TX2REG ='P';
                            __delay_ms(1);
                            TX2REG='1';
                            __delay_ms(1);
                            TX2REG='1';//fan speed
                            __delay_ms(1);
                            TX2REG='0';
                            __delay_ms(1);
                            TX2REG='0';
                            __delay_ms(1);
                            TX2REG=')';
                        }
                        
                      if(chDimmerSpeedMSB == '2')
                        {                                          
                    //         TX1REG = '2';
                            __delay_ms(5);
                            TX2REG = '(' ;
                            __delay_ms(1);
                            TX2REG = 'f' ;//touchmatik address
                            __delay_ms(1);
                            TX2REG ='P';
                            __delay_ms(1);
                            TX2REG='1';
                            __delay_ms(1);
                            TX2REG='1';//fan speed
                            __delay_ms(1);
                            TX2REG='0';
                            __delay_ms(1);
                            TX2REG='0';
                            __delay_ms(1);
                            TX2REG=')';
                        }
                        
                       if(chDimmerSpeedMSB == '5')
                        {                                          
                     //        TX1REG = '5';
                            __delay_ms(5);
                            TX2REG = '(' ;
                            __delay_ms(1);
                            TX2REG = 'f' ;//touchmatoc address
                            __delay_ms(1);
                            TX2REG ='P' ;
                            __delay_ms(1);
                            TX2REG='1';
                            __delay_ms(1);
                            TX2REG='2';//fan speed
                            __delay_ms(1);
                            TX2REG='0';
                            __delay_ms(1);
                            TX2REG='0';
                            __delay_ms(1);
                            TX2REG=')';
                        }
                        if(chDimmerSpeedMSB == '7')
                        {                                          
                       //      TX1REG = '7';
                            __delay_ms(5);
                            TX2REG = '(' ;
                            __delay_ms(1);
                            TX2REG = 'f' ;//touchmatoc address
                            __delay_ms(1);
                            TX2REG ='P' ;
                            __delay_ms(1);
                            TX2REG='1';
                            __delay_ms(1);
                            TX2REG='3';//fan speed
                            __delay_ms(1);
                            TX2REG='0';
                            __delay_ms(1);
                            TX2REG='0';
                            __delay_ms(1);
                            TX2REG=')';
                        }
                       if(chDimmerSpeedMSB == '9')
                        {                                          
                      //       TX1REG = '9';
                            __delay_ms(5);
                            TX2REG = '(' ;
                            __delay_ms(1);
                            TX2REG = 'f' ;//touchmatoc address
                            __delay_ms(1);
                            TX2REG ='P';
                            __delay_ms(1);
                            TX2REG=charSwitchSTATE;
                            __delay_ms(1);
                            TX2REG='4';//fan speed
                            __delay_ms(1);
                            TX2REG='0';
                            __delay_ms(1);
                            TX2REG='0';
                            __delay_ms(1);
                            TX2REG=')';
                        }
                        }
                break;
                 default:
                break;
                    }

                }

            }
        
        
    
    
    switch(integerSwitchNumber){
  case 1:
        {


             OUTPUT_RELAY1 = integerSwitchState;


        }
            break;
        case 2:
            {

//            TX1REG='2';

              OUTPUT_RELAY2 = integerSwitchState;

            break;
            }
        case 3:
        {
        
//            TX1REG='3';
           
            OUTPUT_RELAY3 = integerSwitchState;


        }
            break;
        case 4:
        {
//            TX1REG='4';
          
            OUTPUT_RELAY4 = integerSwitchState;

        }
            break;
        case 5:
        {
            
                OUTPUT_RELAY5 = integerSwitchState;
        }
            break;
            
        case 6:
        {
                OUTPUT_RELAY6 = integerSwitchState;
        }
            break;
        case 7:
        {
            if(integerSwitchState == 0)
            {
               
                OUTPUT_FAN_1=OFF;
                __delay_ms(500);
                OUTPUT_FAN_2=OFF;
                __delay_ms(500);
                OUTPUT_FAN_3=OFF;
                __delay_ms(500);
                 OUTPUT_REGULATOR = ON;
                __delay_ms(500);
            }
            else if(integerSwitchState == 1)
            {
                if(chDimmerSpeedMSB == '0')
                {
                 //    TX1REG='0'; 
           //      OUTPUT_REGULATOR = ON;
           //     __delay_ms(500);
                OUTPUT_FAN_1=OFF;
                __delay_ms(500);
                OUTPUT_FAN_2=OFF;
                __delay_ms(500);
                OUTPUT_FAN_3=OFF;
                __delay_ms(500); 
                 OUTPUT_REGULATOR = ON;
                __delay_ms(500);
                }
                else if(chDimmerSpeedMSB == '2')
                {
               //      TX1REG='2'; 
            //   OUTPUT_REGULATOR = ON;
             //   __delay_ms(500);

                OUTPUT_FAN_2=OFF;
                __delay_ms(500);
                OUTPUT_FAN_3=OFF;
                __delay_ms(500); 
                 OUTPUT_REGULATOR = ON;
                __delay_ms(500);
                  OUTPUT_FAN_1=ON;
                __delay_ms(500);
                }
                else  if(chDimmerSpeedMSB == '5')
                {
               //     TX1REG='5';
              //   OUTPUT_REGULATOR = ON;
               // __delay_ms(500);                  
                OUTPUT_FAN_1=OFF;
                __delay_ms(500);
                 OUTPUT_FAN_3=OFF;
                __delay_ms(500);
                OUTPUT_FAN_2=ON;
                __delay_ms(500);
              OUTPUT_REGULATOR = ON;
                __delay_ms(500);
 
                }
             else  if(chDimmerSpeedMSB == '7')
                {
                 //            TX1REG='7';  
              //    OUTPUT_REGULATOR = ON;
               // __delay_ms(500);
                OUTPUT_FAN_3=OFF;
                __delay_ms(500); 
                OUTPUT_FAN_1=ON;
                __delay_ms(500);
                OUTPUT_FAN_2=ON;
                __delay_ms(500);
                OUTPUT_REGULATOR = ON;
                __delay_ms(500);

                }
                 else  if(chDimmerSpeedMSB == '9')
                {
                   //               TX1REG='9';
                OUTPUT_FAN_1=OFF;
                __delay_ms(500);
                OUTPUT_FAN_2=OFF;
                __delay_ms(500);
                OUTPUT_FAN_3=ON;
                __delay_ms(500); 
                OUTPUT_REGULATOR = ON;
               __delay_ms(500);
                }
            }
        }
            break;
        case 8:
        {
#ifdef  SWITCH_8_RELAY            
                OUTPUT_RELAY5 = integerSwitchState;
#endif
        }
            break;
        default:
            break;
        }
    
}



void actiontouchPanel(char Switch_Num, char sw_status, char speeds) //, char speeds
{
  //  TX1REG = 'P';
  //   TX1REG = Switch_Num;
        M1=ON;   
        M2=ON;   
        M3=ON;  
        M4=ON;   
        M5=ON;
        M6=ON;
        M7=ON;
        M8=ON;
      //  TX1REG = speeds;
     //   TX1REG = Switch_Num;
    int switch_status = sw_status - '0';    
     int integer_switch_speed = speeds - '0';     
     int SwNum = Switch_Num - '@';//ASCII OF SWITCH NUMBER - ASCII OF @ i.e A>>65, B>>66, C>>67, D>>68 65-64=1 and so on
  // speeds = '0';
    char ch_sw_num = SwNum +'0';//send '1' for switch A, '2' for sww2 and so on 
 //   sendFeedback_TO_Gateway(sw_status,ch_sw_num);
   if(tempReceiveTouchpanelDataBuffer[0] != 'P'){
    __delay_ms(5);      TX1REG = 'G';
    __delay_ms(1);      TX1REG = sw_status;
    __delay_ms(1);      TX1REG = '0';
    __delay_ms(1);      TX1REG = Switch_Num - 16;
    }
    else
    {
    __delay_ms(5);      TX1REG = 'G';
    __delay_ms(1);      TX1REG = sw_status;
    __delay_ms(1);      TX1REG = '0';
    __delay_ms(1);      TX1REG = '7';    
    }
   
  
    switch(Switch_Num) {
       
       case 'A':
       {
        //   TX1REG = 'A';
    //     if(copy_parentalLockBuffer[1] == CHAR_OFF )
       if(M1 == ON && copy_parentalLockBuffer[1] == CHAR_OFF )
         {
           //    TX1REG='N';
              
                OUTPUT_RELAY1 = switch_status;
         //      parentalLockBuffer[1] = '0';
               M1 = OFF;
          }
    //     if(copy_parentalLockBuffer[1] == CHAR_ON )
        if(M1 == ON && copy_parentalLockBuffer[1] == CHAR_ON )
          {
             // TX1REG='C';
              
             //   OUTPUT_RELAY1 = switch_status;
          //     parentalLockBuffer[1] = '0';
               M1 = OFF;
           }
       }
       
       break;
       case 'B':
       {
        // TX1REG = 'B';
     //     if(copy_parentalLockBuffer[2] == CHAR_OFF )
         if(M2 == ON && copy_parentalLockBuffer[2] == CHAR_OFF  )
          {
               M2 = OFF;
               /// TX1REG='N';
               // TX1REG='B';
                 OUTPUT_RELAY2 = switch_status;
            //      parentalLockBuffer[2] = '0';
              
          }
         // if(copy_parentalLockBuffer[2] == CHAR_ON )
         if(M2 == ON && copy_parentalLockBuffer[2] == CHAR_ON  )
           {
             
                M2 = OFF;
        
                //TX1REG='C';
               // TX1REG='B';
               //  OUTPUT_RELAY2 = switch_status;
           //  parentalLockBuffer[2] = '0';
              
           }
       }
       
       break;
       case 'C':
       {
       //    TX1REG = 'c';
      if(M3 == ON && copy_parentalLockBuffer[3] == CHAR_OFF )
         {
               //TX1REG='N';
              
                OUTPUT_RELAY3 = switch_status;
         //      parentalLockBuffer[1] = '0';
               M1 = OFF;
          }
    //     if(copy_parentalLockBuffer[1] == CHAR_ON )
        if(M3 == ON && copy_parentalLockBuffer[3] == CHAR_ON )
          {
              //TX1REG='C';
              
             //   OUTPUT_RELAY1 = switch_status;
          //     parentalLockBuffer[1] = '0';
               M3 = OFF;
           }
       }
       break;
       case 'D':
       {
         //  TX1REG = 'D';
        //if(copy_parentalLockBuffer[4] == CHAR_OFF )
           if(M4 == ON && copy_parentalLockBuffer[4] == CHAR_OFF)
          {
               M4 = OFF;
               //TX1REG='N';
               // TX1REG='D';
                OUTPUT_RELAY4 = switch_status;
               
           //    parentalLockBuffer[4] = '0';
         }
        //if(copy_parentalLockBuffer[4] == CHAR_ON )
          if(M4 == ON && copy_parentalLockBuffer[4] == CHAR_ON)
           {
                M4 = OFF;
               //TX1REG='C';
               // OUTPUT_RELAY4 = switch_status;
           //    parentalLockBuffer[4] = '0';
               
          }
       }
       break;
       case 'E':
       {
        //   TX1REG = 'E';
        // if(copy_parentalLockBuffer[5] == CHAR_OFF )
        if(M5 == ON && copy_parentalLockBuffer[5] == CHAR_OFF)
           {
                M5 = OFF;
               // TX1REG='D';
               //TX1REG='N';
                OUTPUT_RELAY5 = switch_status;
           //    parentalLockBuffer[5] = '0';
               
          }
        //   if(copy_parentalLockBuffer[5] == CHAR_ON )
         if(M5 == ON && copy_parentalLockBuffer[5] == CHAR_ON)
           {
                M5 = OFF;
                //TX1REG='C';
               // TX1REG='D';
             //   OUTPUT_RELAY5 = switch_status;
            //    parentalLockBuffer[5] = '0';
               
          }
       }
       break;
       case 'F':
       {
        //   TX1REG = 'F';
          //     if(copy_parentalLockBuffer[6] == CHAR_OFF )
        //   if( parentalLockBuffer[4] == OFF){
          if(M6 == ON && copy_parentalLockBuffer[6] == CHAR_OFF)
           {
                M6 = OFF;
               //TX1REG='N';
               // TX1REG='D';
                OUTPUT_RELAY6 = switch_status;
            //    parentalLockBuffer[6] = '0';
               
               
          }
          //       if(copy_parentalLockBuffer[6] == CHAR_ON )  
           if(M6 == ON && copy_parentalLockBuffer[6] == CHAR_ON)
           {
                M6 = OFF;
               // TX1REG='C';
             //  // TX1REG='D';
             //  // OUTPUT_RELAY6 = switch_status;
               
             //    parentalLockBuffer[6] = '0';
          }
       }
       break;
              case 'P':
       {
           
          //     if(copy_parentalLockBuffer[6] == CHAR_OFF )
        //   if( parentalLockBuffer[4] == OFF){
          if(M7 == ON && copy_parentalLockBuffer[7] == CHAR_OFF) {
                M7 = OFF;
            //    TX1REG = 'P';
                __delay_ms(1);
            if(sw_status == '0')
            {
            //   TX1REG='A';
            //    __delay_ms(1);
            //     OUTPUT_REGULATOR = OFF;
                __delay_ms(1);
                OUTPUT_FAN_1=OFF;
                __delay_ms(500);
                OUTPUT_FAN_2=OFF;
                __delay_ms(500);
                OUTPUT_FAN_3=OFF;
                __delay_ms(500);
                //TX1REG='0';
            }
           else if(sw_status == '1'){
        //    __delay_ms(5);   
//              TX1REG='B'; 
//                __delay_ms(1);
//                 OUTPUT_FAN_1=ON;
//                __delay_ms(500);
//                OUTPUT_FAN_2=OFF;
//                __delay_ms(500);
//                OUTPUT_FAN_3=OFF;
//                __delay_ms(500);
//                switch(integer_switch_speed){
//                    case 1:
//                    {
//                   TX1REG='1';
//                __delay_ms(1);
//                 OUTPUT_FAN_1=ON;
//                __delay_ms(500);
//                OUTPUT_FAN_2=OFF;
//                __delay_ms(500);
//                OUTPUT_FAN_3=OFF;                     
//                    }
//                    break;
//                    case 2:
//                    {
//                 TX1REG='2';
//                __delay_ms(1);
//                 OUTPUT_FAN_1=ON;
//                __delay_ms(500);
//                OUTPUT_FAN_2=ON;
//                __delay_ms(500);
//                OUTPUT_FAN_3=OFF;
//                    }
//                    break;
//                    case 3:
//                    {
//                    TX1REG='3';
//                __delay_ms(1);
//                 OUTPUT_FAN_1=ON;
//                __delay_ms(500);
//                OUTPUT_FAN_2=ON;
//                __delay_ms(500);
//                OUTPUT_FAN_3=ON;
//                __delay_ms(500);
//                    }
//                    break;
//                    case 4:
//                    {
//                   TX1REG='4';
//                 __delay_ms(1);
//                 OUTPUT_REGULATOR = ON;
//                 __delay_ms(500);
//                OUTPUT_FAN_1=ON;
//                __delay_ms(500);
//                OUTPUT_FAN_2=ON;
//                __delay_ms(500);
//                OUTPUT_FAN_3=ON;
//                    }
//                    break;
//                    default:
//                        break;
//                }
            if(integer_switch_speed == 1)
            {
         //  __delay_ms(5);  
          //      TX1REG='1';
                  __delay_ms(1);
                 OUTPUT_REGULATOR = ON;
                __delay_ms(1);
                OUTPUT_FAN_2=OFF;
                __delay_ms(500);
                OUTPUT_FAN_3=OFF;
              __delay_ms(500);
                 OUTPUT_FAN_1=ON;
                __delay_ms(500);
                //__delay_ms(1000);
            }
             if(speeds == '2')
            {
   //   __delay_ms(5);     
             //    TX1REG='2';
                  __delay_ms(1);
                 OUTPUT_REGULATOR = ON;
                __delay_ms(1);
                OUTPUT_FAN_1=OFF;
                __delay_ms(500);
                OUTPUT_FAN_3=OFF;
               __delay_ms(500);
                 OUTPUT_FAN_2=ON;
                __delay_ms(500);
                //__delay_ms(500);
            }
             if(integer_switch_speed == 3)
            {
   //  __delay_ms(5);      
            //     TX1REG='3';
            //      __delay_ms(1);
                 OUTPUT_REGULATOR = ON;
                  __delay_ms(1);
                   OUTPUT_FAN_3=OFF;
                __delay_ms(500);
                __delay_ms(1);
                 OUTPUT_FAN_1=ON;
                __delay_ms(500);
                OUTPUT_FAN_2=ON;
                __delay_ms(500);
              
            }
             if(integer_switch_speed == 4)
            {
  //    __delay_ms(5);     
            //     TX1REG='4';
                 __delay_ms(1);
                 OUTPUT_REGULATOR = ON;
                 __delay_ms(500);
                OUTPUT_FAN_1=OFF;
                __delay_ms(500);
                OUTPUT_FAN_2=OFF;
                __delay_ms(500);
                OUTPUT_FAN_3=ON;
                //__delay_ms(500);
            }   
               
               
          }
          }
          //       if(copy_parentalLockBuffer[6] == CHAR_ON )  
           if(M7 == ON && copy_parentalLockBuffer[7] == CHAR_ON)
           {
                M7 = OFF;
               // TX1REG='C';
             //  // TX1REG='D';
             //  // OUTPUT_RELAY6 = switch_status;
               
             //    parentalLockBuffer[6] = '0';
          }
       }
       break;
       default:
       break;
   }
}

        
    


/*
 * All input output pin initialization
 */
void GPIO_pin_Initialize(){
    clearAllPorts();
    pinINIT_extra();

//    INPUT_SWITCH_DIR_7 = 1;
 //   INPUT_SWITCH_DIR_8 = 1;
    
    OUTPUT_RELAY_DIR_1 = 0;
    OUTPUT_RELAY_DIR_2 = 0;
    OUTPUT_RELAY_DIR_3 = 0;
    OUTPUT_RELAY_DIR_4 = 0;
    OUTPUT_RELAY_DIR_5 = 0;
    OUTPUT_RELAY_DIR_6 = 0;
    OUTPUT_REGULATOR_DIR =0;
    OUTPUT_FAN_1_DIR =0;
    OUTPUT_FAN_2_DIR =0;
    OUTPUR_FAN_3_DIR =0;
    
 //   OUTPUT_RELAY_DIR_7 = 0;
 //   OUTPUT_RELAY_DIR_8 = 0;
    
    // peripherals directions
    
    // USART DIRECTIONS
    USART_1_TRANSMIT_OUTPUT_DIR = 0;
    USART_1_RECIEVE_INPUT_DIR = 1;
    
    USART_2_TRANSMIT_OUTPUT_DIR = 0;
    USART_2_TRANSMIT_OUTPUT_DIR = 1;
    
    clearAllPorts();
}

/*
 * ALL Peripheral Initialization
 */
void allPeripheralInit(){
    EUSART_Initialize();
    EUSART2_Initialize();
  //  TMR1_Initialize();
  //  TMR3_Initialize();
 //   CCP9_Initialize();
}

/*
 * USART Control Registers initialization
 */
void EUSART_Initialize(){
    PIE1bits.RC1IE = 0;
    PIE1bits.TX1IE = 0;

    // Set the EUSART module to the options selected in the user interface.

    // ABDOVF no_overflow; SCKP Non-Inverted; BRG16 16bit_generator; WUE enabled; ABDEN disabled;
    BAUD1CON = 0x0A;

    // SPEN enabled; RX9 8-bit; CREN enabled; ADDEN disabled; SREN disabled;
    RC1STA = 0x90;

    // TX9 8-bit; TX9D 0; SENDB sync_break_complete; TXEN enabled; SYNC asynchronous; BRGH hi_speed; CSRC slave;
    TX1STA = 0x24;

    // Baud Rate = 9600; SP1BRGL 12;
    //SPBRGL = 0x0C;
    //SPBRGL = 0x19;                  // SP1BRGL is 25 (hex value=0x19) for 9600 baud on 16 MHz crystal frequency
    SP1BRGL = 0xA0;                  // SYNC =0 ; BRGH = 1 ; BRG16=1;
    // Baud Rate = 9600; SP1BRGH 1;
    SP1BRGH = 0x01;

    // Enable all active interrupts ---> INTCON reg .... bit 7            page 105
    GIE = 1;

    // Enables all active peripheral interrupts -----> INTCON reg .... bit 6         page 105
    PEIE = 1;

    // enable receive interrupt
    PIE1bits.RC1IE = 1;                    // handled into INTERRUPT_Initialize()

    // Transmit Enabled
    TX1STAbits.TXEN = 1;

    // Serial Port Enabled
    RC1STAbits.SPEN = 1;
}
void EUSART2_Initialize()
{
    PIE4bits.RC2IE = 0;
    PIE4bits.TX2IE = 0;

    // Set the EUSART module to the options selected in the user interface.

    // ABDOVF no_overflow; SCKP Non-Inverted; BRG16 16bit_generator; WUE enabled; ABDEN disabled;
    BAUD2CON = 0x0A;

    // SPEN enabled; RX9 8-bit; CREN enabled; ADDEN disabled; SREN disabled;
    RC2STA = 0x90;

    // TX9 8-bit; TX9D 0; SENDB sync_break_complete; TXEN enabled; SYNC asynchronous; BRGH hi_speed; CSRC slave;
    TX2STA = 0x24;

    // Baud Rate = 9600; SP1BRGL 12;
    SP2BRGL = 0xA0;                  // SYNC =0 ; BRGH = 1 ; BRG16=1;
    // Baud Rate = 9600; SP1BRGH 1;
    SP2BRGH = 0x01;

    // Enable all active interrupts ---> INTCON reg .... bit 7            page 105
    GIE = 1;

    // Enables all active peripheral interrupts -----> INTCON reg .... bit 6         page 105
    PEIE = 1;

    // enable receive interrupt    
    PIE4bits.RC2IE = 1; // handled into INTERRUPT_Initialize()
    // Transmit Enabled
    TX2STAbits.TXEN = 1;

    // Serial Port Enabled
    RC2STAbits.SPEN = 1;
}

void TMR1_Initialize(void)
{
   
    T1CON = 0x00;

    //T1GSS T1G; TMR1GE disabled; T1GTM disabled; T1GPOL low; T1GGO_nDONE done; T1GSPM disabled;
    T1GCON = 0x00;

        //TMR1H 29;
    TMR1H = 0x00;

    //TMR1L 112;
    TMR1L = 0x00;

    // Clearing IF flag before enabling the interrupt.
    PIR1bits.TMR1IF = 0;

    // Enabling TMR1 interrupt.
    PIE1bits.TMR1IE = 1;

    // Start TMR1
   // T1CONbits.TMR1ON = 1;

    // Enable all active interrupts ---> INTCON reg .... bit 7            page 105
    GIE = 1;

    // Enables all active peripheral interrupts -----> INTCON reg .... bit 6         page 105
    PEIE = 1;

}

void TMR3_Initialize(void)
{

    T3CON = 0x00;

    //T1GSS T1G; TMR1GE disabled; T1GTM disabled; T1GPOL low; T1GGO_nDONE done; T1GSPM disabled;
    T3GCON = 0x00;

        //TMR1H 29;
    TMR3H = 0x00;
 
    //TMR1L 112;
    TMR3L = 0x00;

    // Clearing IF flag before enabling the interrupt.
    PIR3bits.TMR3IF = 0;

    // Enabling TMR1 interrupt.
    PIE3bits.TMR3IE = 1;

    // Start TMR1
   // T1CONbits.TMR1ON = 1;

    // Enable all active interrupts ---> INTCON reg .... bit 7            page 105
    GIE = 1;

    // Enables all active peripheral interrupts -----> INTCON reg .... bit 6         page 105
    PEIE = 1;

}
void CCP9_Initialize(){
    // Set the CCP1 to the options selected in the User Interface

    // MODE Every edge; EN enabled; FMT right_aligned;
    CCP9CON = 0x84;

    // RH 0;
    CCPR9H = 0x00;

    // RL 0;
    CCPR9L = 0x00;
    
//    CCPTMRS2bits.C9TSEL0=0;
//    CCPTMRS2bits.C9TSEL1=0;

    // Clear the CCP1 interrupt flag
    PIR4bits.CCP9IF = 0;

    // Enable the CCP1 interrupt
    PIE4bits.CCP9IE = 1;
}


void AllInterruptEnable(){
    // Enable all active interrupts ---> INTCON reg .... bit 7            page 105
    GIE = 1;

    // Enables all active peripheral interrupts -----> INTCON reg .... bit 6         page 105
    PEIE = 1;
    
    // enable receive interrupt
    PIE1bits.RC1IE = 1;                    // handled into INTERRUPT_Initialize()

}

void errorsISR(char* errNum){
    int Tx_count=0;
  	while(Tx_count!=4)
 	{ 
        while (!TX1STAbits.TRMT);
 		TX1REG = *errNum;
 		*errNum++;
        Tx_count++;
 	}
}
void errorsMain(char* errNum){
   int Tx_count=0;
  	while(Tx_count!=4)
 	{ 
        while (!TX1STAbits.TRMT);
 		TX1REG = *errNum;
 		*errNum++;
        Tx_count++;
 	}
}
void sendAcknowledgment(char* currentStateBuffer){
  int Tx_count=0;
  	while(Tx_count!=4)
 	{ 
        while (!TX1STAbits.TRMT);
//        TX1REG='S';
 		TX1REG = *currentStateBuffer;
 		*currentStateBuffer++;
        Tx_count++;
 	}
}
void sendFeedback_TO_Gateway(char sw_status, char Switch_Num){
    __delay_ms(5);      TX1REG = 'G';
    __delay_ms(1);      TX1REG = sw_status;
    __delay_ms(1);      TX1REG = '0';
    __delay_ms(1);      TX1REG = Switch_Num;
}
void sendFeedback_TO_Touch(char Switch_Num_1s, char sw_status){
  //  TX1REG='C';
    __delay_ms(5);      TX2REG = '(';
//    __delay_ms(1);      TX2REG = TouchMatikBoardAddress;
    __delay_ms(1);          TX2REG = 'f';
    __delay_ms(1);      TX2REG = Switch_Num_1s;
    __delay_ms(1);      TX2REG = sw_status;
//	__delay_ms(1);      TX2REG = '0';
//	__delay_ms(1);      TX2REG = '0';
//	__delay_ms(1);      TX2REG = '0';
    __delay_ms(1);      TX2REG = ')';
}
void copyReceivedDataBuffer(){
    int dataBufferCounter=2;
    for(dataBufferCounter=2;dataBufferCounter<9;dataBufferCounter++){
        tempReceivedDataBuffer[dataBufferCounter-2]=mainReceivedDataBuffer[dataBufferCounter]; // copy data buffer from main
        mainReceivedDataBuffer[dataBufferCounter]='#';  // clean data buffer
    }
}
void copyTouchpanelReceiveDataBuffer() ///(fp1100))
{
     int dataBufferCounter=2;
     for(dataBufferCounter=2; dataBufferCounter<5;dataBufferCounter++)
     {
         tempReceiveTouchpanelDataBuffer[dataBufferCounter-2] = touchpanleReceivedDatabuffer[dataBufferCounter];
         touchpanleReceivedDatabuffer[dataBufferCounter] = "#";
     }
}
/*
 * AANALOG and PULL up REGISTERS related initialization
 */
void pinINIT_extra(){
    ANSELG=0x00;    WPUG = 0;
    
    ANSELF=0x00; 
    
    ANSELE=0x00;    WPUE=0x00;
    
    ANSELD=0x00;    WPUD=0x00;
    
    ANSELB=0x00;    WPUB=0x00;
    
    ANSELA=0x00;     
} 

/*
 * always clear all the ports before initialization
 */
void clearAllPorts()
{
  //  TX1REG='C';
  OUTPUT_RELAY1 = 0;
  OUTPUT_RELAY2 = 0;
  OUTPUT_RELAY3 = 0;
  OUTPUT_RELAY4 = 0;
  OUTPUT_RELAY5 = 0;
  OUTPUT_RELAY6 = 0;
    OUTPUT_REGULATOR = 0;
    OUTPUT_FAN_1=0;
    OUTPUT_FAN_2=0;
    OUTPUT_FAN_3=0;
//OUTPUT_RELAY7=0;
// OUTPUT_RELAY8=0;
}
