/*
 * File:   ruby.c
 * Author: VARUN SAHNI
 * CLIENT NAME: ZOHAB KHAN
 * DESCRIPTION: THIS IS TEST CODE FOR RUBY TOUCHPANEL(10 switch and 1 FAN)
 * Created on 17 September, 2018, 4:48 PM
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
#define OUTPUT_RELAY7 RA2
#define OUTPUT_RELAY8 RB3
#define OUTPUT_RELAY9 RA5
#define OUTPUT_RELAY10 RC0
#define OUTPUT_RELAY11 RF0
//#define OUTPUT_DIMMER RG1

#define OUTPUT_RELAY_DIR_1 TRISBbits.TRISB1
#define OUTPUT_RELAY_DIR_2 TRISCbits.TRISC1
#define OUTPUT_RELAY_DIR_3 TRISAbits.TRISA0
#define OUTPUT_RELAY_DIR_4 TRISFbits.TRISF1
#define OUTPUT_RELAY_DIR_5 TRISAbits.TRISA3        
#define OUTPUT_RELAY_DIR_6 TRISAbits.TRISA1 
#define OUTPUT_RELAY_DIR_7 TRISAbits.TRISA2
#define OUTPUT_RELAY_DIR_8 TRISBbits.TRISB3
#define OUTPUT_RELAY_DIR_9 TRISAbits.TRISA5
#define OUTPUT_RELAY_DIR_10 TRISCbits.TRISC0
#define OUTPUT_RELAY_DIR_11 TRISFbits.TRISF0
//#define OUTPUT_DIMMER_DIR TRISGbits.TRISG1

#define INPUTSWITCH1 RF2
#define INPUTSWITCH2 RF3
#define INPUTSWITCH3 RF4
#define INPUTSWITCH4 RF5
#define INPUTSWITCH5 RF6   
#define INPUTSWITCH6 RD7
#define INPUTSWITCH7 RD6
#define INPUTSWITCH8 RD5
#define INPUTSWITCH9 RD4
#define INPUTSWITCH10 RD3
#define INPUTSWITCH11 RD2
//#define INPUTSWITCH12 RD1



#define INPUT_SWITCH_DIR_1 TRISFbits.TRISF2
#define INPUT_SWITCH_DIR_2 TRISFbits.TRISF3
#define INPUT_SWITCH_DIR_3 TRISFbits.TRISF4
#define INPUT_SWITCH_DIR_4 TRISFbits.TRISF5
#define INPUT_SWITCH_DIR_5 TRISFbits.TRISF6
#define INPUT_SWITCH_DIR_6 TRISDbits.TRISD7
#define INPUT_SWITCH_DIR_7 TRISDbits.TRISD6
#define INPUT_SWITCH_DIR_8 TRISDbits.TRISD5
#define INPUT_SWITCH_DIR_9 TRISDbits.TRISD4
#define INPUT_SWITCH_DIR_10 TRISDbits.TRISD3
#define INPUT_SWITCH_DIR_11 TRISDbits.TRISD2
//#define INPUT_SWITCH_DIR_12 TRISDbits.TRISD1

//FAN PINS
#define REGULATOR RA4 // regulator
#define FAN1 RB4//fan1
#define FAN2 RB2//fan2
#define FAN3 RB0//fan3

#define REGULATOR_DIR TRISAbits.TRISA4
#define FAN1_DIR TRISBbits.TRISB4
#define FAN2_DIR TRISBbits.TRISB2
#define FAN3_DIR TRISBbits.TRISB0




/*
 * Extra Periferals Direction and PORT
 */
//#define ZCD_CCP9_DIR TRISEbits.TRISE3
// USART Directions
#define USART_1_TRANSMIT_OUTPUT_DIR TRISCbits.TRISC6
#define USART_1_RECIEVE_INPUT_DIR TRISCbits.TRISC7
#define UART2_TX_DIR TRISGbits.TRISG1                // Tx2 pin = output
#define UART2_RX_DIR TRISGbits.TRISG2 

#define RECIEVED_DATA_LENGTH (16*2)
#define TOTAL_NUMBER_OF_SWITCH (12*2)


#define TOUCHPANEL_DATA_LENGTH (12*2)

#define TRUE 1
#define FALSE 0

#define CHAR_TRUE '1'
#define CHAR_FALSE '0'
#define TouchMatikBoardAddress 'j'
/* 
 * #define WRONG_DATA_RECIEVED_ERROR_CODE ERRX
 * #define RECIVING_OVERRUN_ERROR EROV
 * #define RECEIVING_DATA_LOST_IN_MAIN ERLS
 */
/* DATA USED IN MANUAL  STARTS HERE*/
unsigned int M1;unsigned int M2;unsigned int M3;unsigned int M4;unsigned int M5;unsigned int M6;
unsigned int M7;unsigned int M8;unsigned int M9;unsigned int M10;unsigned int M11;unsigned int M12;

unsigned int R1;unsigned int R2;unsigned int R3;unsigned int R4;unsigned int R5;unsigned int R6;
unsigned int R7;unsigned int R8;unsigned int R9;unsigned int R10;unsigned int R11;unsigned int R12;
//*condition compilation


 
 
#define ON 1
#define OFF 0
#define CHAR_OFF '0'
#define CHAR_ON '1'
#define DEBUG9
#define DEBUG10
#define DEBUG11
//#define DEBUG12


int checkFlag = FALSE;



unsigned char ErrorNames[5]="####";

volatile int mainReceivedDataPosition=0, mainDataReceived=FALSE;
unsigned char mainReceivedDataBuffer[RECIEVED_DATA_LENGTH]="#"; 
unsigned char tempReceivedDataBuffer[RECIEVED_DATA_LENGTH-8]="#";
unsigned char parentalLockBuffer[TOTAL_NUMBER_OF_SWITCH]="000000000000000000000000";
unsigned char copy_parentalLockBuffer[TOTAL_NUMBER_OF_SWITCH]="000000000000000000000000";
unsigned char currentStateBuffer[(TOTAL_NUMBER_OF_SWITCH*4)+2]="#";


int touchpanelReceivedataPosition = 0; 
volatile int touchPanelDataReceived = FALSE;
unsigned char touchpanleReceivedDatabuffer[TOUCHPANEL_DATA_LENGTH]="#";
unsigned char tempReceiveTouchpanelDataBuffer[TOUCHPANEL_DATA_LENGTH-8]="#";




int start_PWM_Generation_in_ISR_FLAG=FALSE;
char levelofDimmer_MSB='0',levelofDimmer_LSB='0';
void errorsISR(char* errNum);
void errorsMain(char* errNum);
void sendAcknowledgment(char* currentStateBuffer);
void sendFeedback_TO_Gateway(char sw_status,char Switch_Num_MSB, char Switch_Num_LSB);


void clearAllPorts();
void pinINIT_extra();
void GPIO_pin_Initialize();

//void AllInterruptEnable();
void EUSART_Initialize();
void EUSART2_Initialize();


void allPeripheralInit();

void copyReceivedDataBuffer();
void copyTouchpanelReceiveDataBuffer();
void applianceControl(char switchMSB, char switchLSB, char switchSTATE, char dimmerSpeedMSB, char dimmerSpeedLSB, char parentalControl, char finalFrameState);


void actiontouchPanel(char Switch_Num, char sw_status,char sw_speed );
void send_Response_To_Touch(char switch_no, char switch_status, char fan_speed);

interrupt void isr(){

    // ************************************* XbEE UART INTERRUPT *********************************************** //
    if(RC1IF){        
        if(RC1STAbits.OERR){    // If over run error, then reset the receiver
            RC1STAbits.CREN = 0; // countinuous Recieve Disable
            RC1STAbits.CREN = 1; // countinuous Recieve Enable
            
            ErrorNames[0]='E';      ErrorNames[1]='R';      ErrorNames[2]='O';      ErrorNames[3]='V';
            errorsISR(ErrorNames); 
        } 
         if(RC1STAbits.FERR){    // If over run error, then reset the receiver
            RC1STAbits.CREN = 0; // countinuous Recieve Disable
            RC1STAbits.CREN = 1; // countinuous Recieve Enable
            
            ErrorNames[0]='F';      ErrorNames[1]='E';      ErrorNames[2]='R';      ErrorNames[3]='R';
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
       if(RC2STAbits.FERR){    // If over run error, then reset the receiver
            RC2STAbits.CREN = 0; // countinuous Recieve Disable
            RC2STAbits.CREN = 1; // countinuous Recieve Enable
            
            ErrorNames[0]='R';      ErrorNames[1]='E';      ErrorNames[2]='R';      ErrorNames[3]='R';
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
int main()
{
        R1=ON;R2=ON;R3=ON;R4=ON;R5=ON;R6=ON;R7=ON;R8=ON;R9=ON;R10=ON;R11=ON;R12=ON;
    __delay_ms(2000); 
    REGULATOR = ON;
    FAN1 = 0;
    FAN2 = 0;
    FAN3 = 0;
    GPIO_pin_Initialize();
    allPeripheralInit();
   // AllInterruptEnable();

    while(1)
    {
         ///STARTING OF MOBILE APP DATA RECEIVE
        if(mainDataReceived==TRUE){
            mainDataReceived=FALSE;
			checkFlag=TRUE;
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
                        if(tempReceiveTouchpanelDataBuffer[0] != '@')
                        {
                           actiontouchPanel(tempReceiveTouchpanelDataBuffer[0]
                                   ,tempReceiveTouchpanelDataBuffer[1]
                                   ,tempReceiveTouchpanelDataBuffer[2]); //,tempReceiveTouchpanelDataBuffer[2]
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
            
        }//end of touchpanel received data
    
    
//       /******************** MANUAL RESPONE STARTS HERE************ */
//        
//        //check switch one status
//        //off condition
       int man = 1;
        if(copy_parentalLockBuffer[1] == CHAR_OFF && INPUTSWITCH1 == OFF && R1 == OFF)
        {
            if(man == 1)
            {
        //    TX1REG='T';
            __delay_ms(5);
            TX1REG = 'R';__delay_ms(1);
            TX1REG = '0';__delay_ms(1);
            TX1REG = '0';__delay_ms(1);
            TX1REG = '1';__delay_ms(1);
            OUTPUT_RELAY1=OFF;
			send_Response_To_Touch('A','0','0');
            }
            man=0;
            R1=1;
            
        }
        //on condition
        if(copy_parentalLockBuffer[1] == CHAR_OFF && INPUTSWITCH1 == ON &&  R1 == ON)
        {
           if(man==1)
           {
            __delay_ms(5);
            TX1REG = 'R';__delay_ms(1);
            TX1REG = '1';__delay_ms(1);
            TX1REG = '0';__delay_ms(1);
            TX1REG = '1';__delay_ms(1);
            OUTPUT_RELAY1=ON;
			send_Response_To_Touch('A','1','0');
           }
            man=0;
            R1=0;
        }
        
       // //check switch second status 
        //off condition
        if(copy_parentalLockBuffer[2] == CHAR_OFF && INPUTSWITCH2 == OFF && R2 == OFF)
        {
            if(man==1)
            {
            __delay_ms(5);
            TX1REG = 'R';__delay_ms(1);
            TX1REG = '0';__delay_ms(1);
            TX1REG = '0';__delay_ms(1);
            TX1REG = '2';__delay_ms(1);
            OUTPUT_RELAY2=OFF;
			send_Response_To_Touch('B','0','0');
            }
            man=0;
            R2=1;
        }
        //on condtion
        if(copy_parentalLockBuffer[2] == CHAR_OFF && INPUTSWITCH2 == ON && R2 == ON)
        {
            if(man==1)
            {
            __delay_ms(5);
            TX1REG = 'R';__delay_ms(1);
            TX1REG = '1';__delay_ms(1);
            TX1REG = '0';__delay_ms(1);
            TX1REG = '2';__delay_ms(1);
            OUTPUT_RELAY2=ON;
			send_Response_To_Touch('B','1','0');
            }
            man=0;
            R2=0;
        }
        
        
       // //check switch third status 
        //off condition
        if(copy_parentalLockBuffer[3] == CHAR_OFF && INPUTSWITCH3 == OFF && R3 == OFF)
        {
            if(man == 1)
            {
            __delay_ms(5);
            TX1REG = 'R';__delay_ms(1);
            TX1REG = '0';__delay_ms(1);
            TX1REG = '0';__delay_ms(1);
            TX1REG = '3';__delay_ms(1);
            OUTPUT_RELAY3=OFF;
			send_Response_To_Touch('C','0','0');
            }
            man=0;
            R3=1;
          
        }
        //on condtion
        if(copy_parentalLockBuffer[3] == CHAR_OFF && INPUTSWITCH3 == ON && R3 == ON)
        {
            if(man==1)
            {
            __delay_ms(5);
            TX1REG = 'R';__delay_ms(1);
            TX1REG = '1';__delay_ms(1);
            TX1REG = '0';__delay_ms(1);
            TX1REG = '3';__delay_ms(1);
            OUTPUT_RELAY3=ON;
			send_Response_To_Touch('C','1','0');
            }
            man=0;
            R3=0;
            
        }
        
        
       // //check switch fourth status 
        //off condition
        if(copy_parentalLockBuffer[4] == CHAR_OFF && INPUTSWITCH4 == OFF && R4 == OFF)
        {
            if(man==1)
            {
            __delay_ms(5);
            TX1REG = 'R';__delay_ms(1);
            TX1REG = '0';__delay_ms(1);
            TX1REG = '0';__delay_ms(1);
            TX1REG = '4';__delay_ms(1);
            OUTPUT_RELAY4=OFF;
			send_Response_To_Touch('D','0','0');
            }
            man=0;
            R4=1;
            
        }
        //on condtion
        if(copy_parentalLockBuffer[4] == CHAR_OFF && INPUTSWITCH4 == ON && R4 == ON)
        {
            if(man==1)
            {
            __delay_ms(5);
            TX1REG = 'R';__delay_ms(1);
            TX1REG = '1';__delay_ms(1);
            TX1REG = '0';__delay_ms(1);
            TX1REG = '4';__delay_ms(1);
            OUTPUT_RELAY4=ON;
			send_Response_To_Touch('D','1','0');
            }
            man=0;
            R4=0;
           
        }
        
             // //check switch fifth status 
        //off condition
        if(copy_parentalLockBuffer[5] == CHAR_OFF && INPUTSWITCH5 == OFF && R5 == OFF)
        {
            if(man==1)
            {

            __delay_ms(5);
            TX1REG = 'R';__delay_ms(1);
            TX1REG = '0';__delay_ms(1);
            TX1REG = '0';__delay_ms(1);
            TX1REG = '5';__delay_ms(1);
            OUTPUT_RELAY5=OFF;
			send_Response_To_Touch('E','0','0');
            }
            man=0;
            R5=1;
           
        }
        //on condtion
        if(copy_parentalLockBuffer[5] == CHAR_OFF && INPUTSWITCH5 == ON && R5 == ON)
        {
          if(man==1)
            {
            __delay_ms(5);
            TX1REG = 'R';__delay_ms(1);
            TX1REG = '1';__delay_ms(1);
            TX1REG = '0';__delay_ms(1);
            TX1REG = '5';__delay_ms(1);  
            OUTPUT_RELAY5=ON;
			send_Response_To_Touch('E','1','0');
          }
           man=0;
           R5=0;
        }
       //off
        if(copy_parentalLockBuffer[6] == CHAR_OFF && INPUTSWITCH6 == OFF && R6 == OFF)
        {
            if(man==1)
            {
         
            __delay_ms(5);
            TX1REG = 'R';__delay_ms(1);
            TX1REG = '0';__delay_ms(1);
            TX1REG = '0';__delay_ms(1); 
            TX1REG = '6';__delay_ms(1);
            OUTPUT_RELAY6=OFF;
			send_Response_To_Touch('G','0','0');
            }
            man=0;
            R6=1;
           
        }
        //on condtion
        if(copy_parentalLockBuffer[6] == CHAR_OFF && INPUTSWITCH6 == ON && R6 == ON)
        {
            if(man==1)
            {
            __delay_ms(5);
            TX1REG = 'R';__delay_ms(1);
            TX1REG = '1';__delay_ms(1);
            TX1REG = '0';__delay_ms(1);
            TX1REG = '6';__delay_ms(1);  
            OUTPUT_RELAY6=ON;
			send_Response_To_Touch('G','1','0');
            }
            man=0;
            R6=0;
        }
       
              //off
        if(copy_parentalLockBuffer[7] == CHAR_OFF && INPUTSWITCH7 == OFF && R7 == OFF)
        {
            if(man==1)
            {
         
            __delay_ms(5);
            TX1REG = 'R';__delay_ms(1);
            TX1REG = '0';__delay_ms(1);
            TX1REG = '0';__delay_ms(1); 
            TX1REG = '7';__delay_ms(1);
            OUTPUT_RELAY7=OFF;
			send_Response_To_Touch('H','0','0');
            }
            man=0;
            R7=1;
           
        }

        //on condtion
        if(copy_parentalLockBuffer[7] == CHAR_OFF && INPUTSWITCH7 == ON && R7 == ON)
        {
            if(man==1)
            {
            __delay_ms(5);
            TX1REG = 'R';__delay_ms(1);
            TX1REG = '1';__delay_ms(1);
            TX1REG = '0';__delay_ms(1);
            TX1REG = '7';__delay_ms(1);  
            OUTPUT_RELAY7=ON;
			send_Response_To_Touch('H','1','0');
            }
            man=0;
            R7=0;
        }
    
              //off
        if(copy_parentalLockBuffer[8] == CHAR_OFF && INPUTSWITCH8 == OFF && R8 == OFF)
        {
            if(man==1)
            {
         
            __delay_ms(5);
            TX1REG = 'R';__delay_ms(1);
            TX1REG = '0';__delay_ms(1);
            TX1REG = '0';__delay_ms(1); 
            TX1REG = '8';__delay_ms(1);
            OUTPUT_RELAY8=OFF;
			send_Response_To_Touch('I','0','0');
            }
            man=0;
            R8=1;
           
        }
        //on condtion
        if(copy_parentalLockBuffer[8] == CHAR_OFF && INPUTSWITCH8 == ON && R8 == ON)
        {
            if(man==1)
            {
            __delay_ms(5);
            TX1REG = 'R';__delay_ms(1);
            TX1REG = '1';__delay_ms(1);
            TX1REG = '0';__delay_ms(1);
            TX1REG = '8';__delay_ms(1);
            OUTPUT_RELAY8=ON;
			send_Response_To_Touch('I','1','0');
            }
            man=0;
            R8=0;
        }
    
    
#ifdef DEBUG9
      //off
        if(copy_parentalLockBuffer[9] == CHAR_OFF && INPUTSWITCH9 == OFF && R9 == OFF)
        {
            if(man==1)
            {
         
            __delay_ms(5);
            TX1REG = 'R';__delay_ms(1);
            TX1REG = '0';__delay_ms(1);
            TX1REG = '0';__delay_ms(1); 
            TX1REG = '9';__delay_ms(1);
            OUTPUT_RELAY9=OFF;
            send_Response_To_Touch('J','0','0');
            }
            man=0;
            R9=1;
           
        }
        //on condtion
        if(copy_parentalLockBuffer[9] == CHAR_OFF && INPUTSWITCH9 == ON && R9 == ON)
        {
            if(man==1)
            {
            __delay_ms(5);
            TX1REG = 'R';__delay_ms(1);
            TX1REG = '1';__delay_ms(1);
            TX1REG = '0';__delay_ms(1);
            TX1REG = '9';__delay_ms(1);
            OUTPUT_RELAY9=ON;
            send_Response_To_Touch('J','1','0');
            }
            man=0;
            R9=0;
        }
#endif 
  
#ifdef DEBUG10       
       //off
        if(copy_parentalLockBuffer[10] == CHAR_OFF && INPUTSWITCH10 == OFF && R10 == OFF)
        {
            if(man==1)
            {
         
            __delay_ms(5);
            TX1REG = 'R';__delay_ms(1);
            TX1REG = '0';__delay_ms(1);
            TX1REG = '1';__delay_ms(1); 
            TX1REG = '0';__delay_ms(1);
            OUTPUT_RELAY10=OFF;
            send_Response_To_Touch('K','0','0');
            }
            man=0;
            R10=1;
           
        }
        //on condtion
        if(copy_parentalLockBuffer[10] == CHAR_OFF && INPUTSWITCH10 == ON && R10 == ON)
        {
            if(man==1)
            {
            __delay_ms(5);
            TX1REG = 'R';__delay_ms(1);
            TX1REG = '1';__delay_ms(1);
            TX1REG = '1';__delay_ms(1);
            TX1REG = '0';__delay_ms(1);
            OUTPUT_RELAY10=ON;
            send_Response_To_Touch('K','1','0');
            }
            man=0;
            R10=0;
        }
#endif
       
       
#ifdef DEBUG11
         //off
        if(copy_parentalLockBuffer[11] == CHAR_OFF && INPUTSWITCH11 == OFF && R11 == OFF)
        {
            if(man==1)
            {
         
            __delay_ms(5);
            TX1REG = 'R';__delay_ms(1);
            TX1REG = '0';__delay_ms(1);
            TX1REG = '1';__delay_ms(1); 
            TX1REG = '1';__delay_ms(1);
            send_Response_To_Touch('P','1','0');
           REGULATOR=ON; 
          __delay_ms(1000);
          FAN1=OFF;
          __delay_ms(1000);
          FAN2=OFF;
         __delay_ms(1000);
          FAN3=OFF;
            }
            man=0;
            R11=1;
           
        }
        //on condtion
        if(copy_parentalLockBuffer[11] == CHAR_OFF && INPUTSWITCH11 == ON && R11 == ON)
        {
            if(man==1)
            {
            __delay_ms(5);
            TX1REG = 'R';__delay_ms(1);
            TX1REG = '1';__delay_ms(1);
            TX1REG = '1';__delay_ms(1);
            TX1REG = '1';__delay_ms(1);
            send_Response_To_Touch('P','0','0');
            REGULATOR=OFF;
            }
            man=0;
            R11=0;
        }
       
#endif
       
#ifdef DEBUG12
       //off
        if(copy_parentalLockBuffer[12] == CHAR_OFF && INPUTSWITCH12 == OFF && R12 == OFF)
        {
            if(man==1)
            {
         
            __delay_ms(5);
            TX1REG = 'R';__delay_ms(1);
            TX1REG = '0';__delay_ms(1);
            TX1REG = '1';__delay_ms(1); 
            TX1REG = '2';__delay_ms(1);
            OUTPUT_DIMMER=ON;
            }
            man=0;
            R12=1;
           
        }
        //on condtion
        if(copy_parentalLockBuffer[12] == CHAR_OFF && INPUTSWITCH12 == ON && R12 == ON)
        {
            if(man==1)
            {
            __delay_ms(5);
            TX1REG = 'R';__delay_ms(1);
            TX1REG = '1';__delay_ms(1);
            TX1REG = '1';__delay_ms(1);
            TX1REG = '2';__delay_ms(1);
            OUTPUT_DIMMER=OFF;
            }
            man=0;
            R12=0;
        }
#endif       
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
    char charSwitchNumber=NULL;
    switchNumberStringBuffer[0]=charSwitchMSB;
    switchNumberStringBuffer[1]=charSwitchLSB;    
    integerSwitchNumber = atoi(switchNumberStringBuffer);//convert string into integer
    charSwitchNumber=integerSwitchNumber+'0';//convert interger into character>>>conver 1 into '1'
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
                    if(integerSwitchNumber<=5){//in this loop send response for upto 10 switch
                    __delay_ms(5);
                    TX2REG = '(' ;
                    __delay_ms(1);
                    TX2REG = TouchMatikBoardAddress ;//touchmatoc address
                    __delay_ms(1);
                    TX2REG =charSwitchNumber+16;//convert application switch>>touchpanel i.e 1>>A,2>>B,3>>c etc
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
                    else if(integerSwitchNumber>5 && integerSwitchNumber<=10)
                    {
                    __delay_ms(5);
                    TX2REG = '(' ;
                    __delay_ms(1);
                    TX2REG = TouchMatikBoardAddress ;//touchmatoc address
                    __delay_ms(1);
                    TX2REG =charSwitchNumber+17;//we are skipping F in zohab panel '6'+17=71>>G
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
                    else if(integerSwitchNumber==11) {
                       switch(integerSwitchState){
                        case 0: {
                                send_Response_To_Touch('P','0','0');  
                               } break;

                        case 1: {
                            switch(chDimmerSpeedMSB){
                                case '0':
                                {
                                send_Response_To_Touch('P','1','1'); //SPEED1
                                }
                                break;
                                case '2':
                                {
                                  send_Response_To_Touch('P','1','1');  //SPEED1
                                }
                                break;
                                case '5':
                                {
                                  send_Response_To_Touch('P','1','2'); //SPEED2
                                }
                                break;
                                case '7':
                                {
                                  send_Response_To_Touch('P','1','3');  //SPEED3
                                }
                                break;
                                case '9':
                                {
                                  send_Response_To_Touch('P','1','4'); //SPEED4

                                }
                                break;
                                default:
                                break;
                        }//close of switch
                    }
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
                OUTPUT_RELAY7 = integerSwitchState;
        }
            break;
        case 8:
        {          
                OUTPUT_RELAY8 = integerSwitchState;
        }
            break;
        case 9:
        {          
                OUTPUT_RELAY9 = integerSwitchState;
        }
            break;
         case 10:
        {          
                OUTPUT_RELAY10 = integerSwitchState;
        }
            break;
         case 11:
        {          
                if(integerSwitchState==0){  
                
                FAN1=OFF;
                __delay_ms(1000);
                FAN2=OFF;
                __delay_ms(1000);
                FAN3=OFF;
                 }
        else if(integerSwitchState==1)
                    {  
               
                       if(chDimmerSpeedMSB == '0')             // speed 1
                        {
                                 
                                  FAN3=OFF;
                                  __delay_ms(1000);
                                  FAN2=OFF;
                                  __delay_ms(1000);
                                  FAN1=ON;

                          }
                        else if(chDimmerSpeedMSB == '2')             // speed 1
                        {
                                  
                                  FAN3=OFF;
                                  __delay_ms(1000);
                                  FAN2=OFF;
                                  __delay_ms(1000);
                                  FAN1=ON;

                          }
                       else if(chDimmerSpeedMSB == '5')         // speed 2
                              {
                                  
                                  FAN1=OFF;
                                  __delay_ms(1000);
                                  FAN3=OFF;               
                                  __delay_ms(1000);
                                  FAN2=ON;
                              }

                          else if( chDimmerSpeedMSB=='7')      // speed 3
                              {    
                                  
                                  FAN3=OFF;
                                  __delay_ms(1000);
                                  FAN2=ON;
                                  __delay_ms(1000);
                                  FAN1=ON;
                              }
                      else if( chDimmerSpeedMSB == '9')                         // speed 4
                              {   

                                  
                                  REGULATOR=ON; 
                                  __delay_ms(1000);
                                  FAN1=OFF;//OFF;
                                  __delay_ms(1000);
                                  FAN2=OFF;//OFF;
                                 __delay_ms(1000);
                                  FAN3=ON;
                              }
                     }       

        }
            break;

        default:
            break;
        }
    
}

void actiontouchPanel(char Switch_Num, char sw_status, char Sw_speed) //
{

    M1=ON;M2=ON;M3=ON;M4=ON;M5=ON;M6=ON;M7=ON;M8=ON; M9=ON;M10=ON;M11=ON;M12=ON;

    int switch_status = sw_status - '0';        
    int SwNum = Switch_Num - '@';//ASCII OF SWITCH NUMBER - ASCII OF @ i.e A>>65, B>>66, C>>67, D>>68 65-64=1 and so on
    char ch_sw_num = SwNum +'0';//send '1' for switch A, '2' for sww2 and so on 

 if(checkFlag == TRUE)  
  
 {  
     checkFlag=FALSE;   
 }   
    else
    {
        switch(Switch_Num) {
			
               case 'A':
               {
               if(M1 == ON && copy_parentalLockBuffer[1] == CHAR_OFF )
                  {
                       sendFeedback_TO_Gateway(sw_status,'0','1');
                       OUTPUT_RELAY1 = switch_status; M1 = OFF;

                  }
               }

               break;
               case 'B':
               {

                 if(M2 == ON && copy_parentalLockBuffer[2] == CHAR_OFF  )
                  {       
                       sendFeedback_TO_Gateway(sw_status,'0','2');
                         OUTPUT_RELAY2 = switch_status;  M2 = OFF;

                  }

               }

               break;
               case 'C':
               {
              if(M3 == ON && copy_parentalLockBuffer[3] == CHAR_OFF )
                 {    
                     sendFeedback_TO_Gateway(sw_status,'0','3');
                     OUTPUT_RELAY3 = switch_status;
                       M3 = OFF;
                  }

               }
               break;
               case 'D':
               {
                   if(M4 == ON && copy_parentalLockBuffer[4] == CHAR_OFF)
                  {
                       sendFeedback_TO_Gateway(sw_status,'0','4');
                        OUTPUT_RELAY4 = switch_status;M4 = OFF;
                 }

               }
               break;
               case 'E':
               {
                if(M5 == ON && copy_parentalLockBuffer[5] == CHAR_OFF)
                   {
                        sendFeedback_TO_Gateway(sw_status,'0','5');        
                        OUTPUT_RELAY5 = switch_status;  M5 = OFF;

                  }
               }
               break;
               case 'G':
               {

                  if(M6 == ON && copy_parentalLockBuffer[6] == CHAR_OFF)
                   {               
                       sendFeedback_TO_Gateway(sw_status,'0','6');  
                      OUTPUT_RELAY6 = switch_status;M6 = OFF;

                  } 
               }
               break;
                case 'H':
               {
                  if(M7 == ON && copy_parentalLockBuffer[7] == CHAR_OFF)
                   {
                       sendFeedback_TO_Gateway(sw_status,'0','7');
                        OUTPUT_RELAY7 = switch_status;   M7 = OFF;             

                  } 
               }
               break;
              case 'I':
               {
                  if(M8 == ON && copy_parentalLockBuffer[8] == CHAR_OFF)
                   {
                       sendFeedback_TO_Gateway(sw_status,'0','8');
                        OUTPUT_RELAY8 = switch_status;  M8 = OFF;

                  }

               }
               break;
			    case 'J':
               {
                  if(M9 == ON && copy_parentalLockBuffer[9] == CHAR_OFF)
                   {
                       sendFeedback_TO_Gateway(sw_status,'0','9');
                        OUTPUT_RELAY9 = switch_status;  M9 = OFF;

                  }

               }
               break;
			   case 'K':
               {
                  if(M10 == ON && copy_parentalLockBuffer[10] == CHAR_OFF)
                   {
                       sendFeedback_TO_Gateway(sw_status,'1','0');
                        OUTPUT_RELAY10 = switch_status;  M10 = OFF;

                  }

               }
               break;
			   case 'P':
             {
                        if(copy_parentalLockBuffer[11]==CHAR_OFF && M11==ON )
                        {
                            sendFeedback_TO_Gateway(sw_status,'1','1');
                                M11=OFF;
                                if(switch_status == 0)
                                {
                                     REGULATOR = ON;
                                     __delay_ms(1000);
                                    FAN1=OFF;
                                    __delay_ms(1000);
                                    FAN2=OFF;
                                    __delay_ms(1000);
                                    FAN3=OFF;
                                    __delay_ms(1000);

                                }
                               else if(switch_status == 1)
                                    {

                                        if(Sw_speed == '1')
                                        {
                                            
                                             REGULATOR = ON;
                                             __delay_ms(1000);             
                                            FAN1=ON;
                                            __delay_ms(1000);
                                            FAN2=OFF;
                                            __delay_ms(1000);
                                            FAN3=OFF;
                                        }
                                         else if(Sw_speed == '2')
                                        {
                                              
                                             REGULATOR = ON;
                                             __delay_ms(1000);
                                            FAN1=OFF;
                                            __delay_ms(1000);
                                            FAN3=OFF;
                                            __delay_ms(1000);
                                            FAN2=ON;

                                        }
                                         else if(Sw_speed == '3')
                                        {
                                             
                                             REGULATOR = ON;
                                             __delay_ms(1000);
                                            FAN3=OFF;
                                            __delay_ms(1000);
                                            FAN2=ON;
                                            __delay_ms(1000);
                                            FAN1=ON;
                                            __delay_ms(1000);
                                        }
                                         else if(Sw_speed == '4')
                                        {
                                              
                                             REGULATOR = ON;
                                             __delay_ms(1000);
                                            FAN1=OFF;
                                            __delay_ms(1000);
                                            FAN2=OFF;
                                            __delay_ms(1000);
                                            FAN3=ON;
                                        }
                                    }
                        }

            }
            break;
               default:
               break;
        }
    }       
}

        

/*
 * All input output pin initialization
 */
void GPIO_pin_Initialize(){
    clearAllPorts();
    pinINIT_extra();

     INPUT_SWITCH_DIR_1 = 1;
    INPUT_SWITCH_DIR_2 = 1;
    INPUT_SWITCH_DIR_3 = 1;
    INPUT_SWITCH_DIR_4 = 1;
    INPUT_SWITCH_DIR_5 = 1;
    INPUT_SWITCH_DIR_6 = 1;
   INPUT_SWITCH_DIR_7 = 1;
   INPUT_SWITCH_DIR_8 = 1;
   INPUT_SWITCH_DIR_9 = 1;
   INPUT_SWITCH_DIR_10 = 1;
   INPUT_SWITCH_DIR_11 = 1;
//   INPUT_SWITCH_DIR_12 = 1;
    
    OUTPUT_RELAY_DIR_1 = 0;
    OUTPUT_RELAY_DIR_2 = 0;
    OUTPUT_RELAY_DIR_3 = 0;
    OUTPUT_RELAY_DIR_4 = 0;
    OUTPUT_RELAY_DIR_5 = 0;
    OUTPUT_RELAY_DIR_6 = 0;
   OUTPUT_RELAY_DIR_7 = 0;
   OUTPUT_RELAY_DIR_8 = 0;
   OUTPUT_RELAY_DIR_9 = 0;
   OUTPUT_RELAY_DIR_10 = 0;
   OUTPUT_RELAY_DIR_11 = 0;
//  OUTPUT_DIMMER_DIR = 1;
    REGULATOR_DIR=0;
    FAN1_DIR=0;
    FAN2_DIR=0;
    FAN3_DIR=0;
    
    // peripherals directions
  //  ZCD_CCP9_DIR = 1;
    // USART DIRECTIONS
    USART_1_TRANSMIT_OUTPUT_DIR = 0;
    USART_1_RECIEVE_INPUT_DIR = 1;
    
	UART2_TX_DIR=0;//tx2 ouuput
    UART2_RX_DIR=1;//rx2 input
    
    clearAllPorts();
}

/*
 * ALL Peripheral Initialization
 */
void allPeripheralInit(){
    EUSART_Initialize();
	EUSART2_Initialize();
//    TMR1_Initialize();
//    TMR3_Initialize();
   // CCP9_Initialize();
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
void EUSART2_Initialize(){
    // disable interrupts before changing states
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
void sendFeedback_TO_Gateway(char sw_status,char Switch_Num_MSB, char Switch_Num_LSB)
{
     __delay_ms(5);     TX1REG = 'R';
    __delay_ms(1);      TX1REG = sw_status;
    __delay_ms(1);      TX1REG = Switch_Num_MSB;
    __delay_ms(1);      TX1REG = Switch_Num_LSB;
}


void send_Response_To_Touch(char switch_no, char switch_status, char fan_speed)
{
       __delay_ms(5);
        TX2REG = '(' ;
        __delay_ms(1);
        TX2REG = TouchMatikBoardAddress ;//touchmatoc address
        __delay_ms(1);
        TX2REG =switch_no ;
        __delay_ms(1);
        TX2REG=switch_status;
        __delay_ms(1);
        TX2REG=fan_speed;
        __delay_ms(1);
        TX2REG='0';
        __delay_ms(1);
        TX2REG='0';
        __delay_ms(1);
        TX2REG=')';
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
  OUTPUT_RELAY7 = 0;
  OUTPUT_RELAY8 = 0;
  OUTPUT_RELAY9 = 0;
  OUTPUT_RELAY10 = 0;
  OUTPUT_RELAY11 = 0;
  
  
    FAN1=OFF;
    FAN2=OFF;
    FAN3=OFF;
    REGULATOR=OFF;
    
  //OUTPUT_DIMMER = 1;
}

