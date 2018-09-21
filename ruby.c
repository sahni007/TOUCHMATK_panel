/* 
 * File:   4+1fan_main.c
 * Author: varun sahni
 * client: sunil kamble(shambhu)
 * module: 4 switches (EMRALD)
 * avalability: 4 switches  touch panel with manual 2 switch way
 *
 * Created on 28 August, 2018, 10:59 AM
 */



#include <stdio.h>
#include <stdlib.h>

// PIC16F1526 Configuration Bit Settings

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
#include <pic16f1526.h>
#define _XTAL_FREQ 16000000







#define RELAY1 RF1
#define RELAY2 RF0
#define RELAY3 RA3
#define RELAY4 RA2

#define RELAY1_DIR TRISFbits.TRISF1
#define RELAY2_DIR TRISFbits.TRISF0
#define RELAY3_DIR TRISAbits.TRISA3
#define RELAY4_DIR TRISAbits.TRISA2

//switch for fan
//input switch
#define INPUT1_SWITCH PORTFbits.RF7
#define INPUT2_SWITCH PORTFbits.RF5
#define INPUT3_SWITCH PORTFbits.RF3
#define INPUT4_SWITCH PORTFbits.RF2
#define INPUT_FAN PORTAbits.RA5

#define INPUT1_SWITCH_DIR TRISFbits.TRISF7
#define INPUT2_SWITCH_DIR TRISFbits.TRISF5
#define INPUT3_SWITCH_DIR TRISFbits.TRISF3
#define INPUT4_SWITCH_DIR TRISFbits.TRISF2
#define INPUT_FAN_DIR TRISAbits.TRISA5

#define UART1_TX_DIR TRISCbits.TRISC6
#define UART1_RX_DIR TRISCbits.TRISC7


#define UART2_TX_DIR TRISGbits.TRISG1                // Tx2 pin = output
#define UART2_RX_DIR TRISGbits.TRISG2               // RX2 pin = input  

#define RECIEVED_DATA_LENGTH (16*2)
#define TOTAL_NUMBER_OF_SWITCH (8*2)


#define TOUCHPANEL_DATA_LENGTH (8*2)
#define TRUE 1
#define FALSE 0

#define CHAR_TRUE '1'
#define CHAR_FALSE '0'

#define TouchMatikBoardAddress 'c'
// fan response switch
unsigned int M1;unsigned int M2;unsigned int M3;unsigned int M4;unsigned int M5;unsigned int M6;unsigned int M7;unsigned int M8;
unsigned int R1;unsigned int R2;unsigned int R3;unsigned int R4;unsigned int R5;unsigned int R6;unsigned int R7;unsigned int R8;

#define ON 1
#define OFF 0
#define CHAR_OFF '0'
#define CHAR_ON '1'
        
/* DATA USED IN MANUAL END HERE*/




unsigned char ErrorNames[5]="####";

volatile int mainReceivedDataPosition=0, mainDataReceived=FALSE;
unsigned char mainReceivedDataBuffer[RECIEVED_DATA_LENGTH]="#"; 
unsigned char tempReceivedDataBuffer[RECIEVED_DATA_LENGTH-8]="#";
unsigned char parentalLockBuffer[TOTAL_NUMBER_OF_SWITCH]="0000000000";
unsigned char copy_parentalLockBuffer[TOTAL_NUMBER_OF_SWITCH]="0000000000";
unsigned char currentStateBuffer[(TOTAL_NUMBER_OF_SWITCH*4)+2]="#";



int touchpanelReceivedataPosition = 0; 
volatile int touchPanelDataReceived = FALSE;
unsigned char touchpanleReceivedDatabuffer[TOUCHPANEL_DATA_LENGTH]="#";
unsigned char tempReceiveTouchpanelDataBuffer[TOUCHPANEL_DATA_LENGTH-8]="#";
unsigned int M1;unsigned int M2;unsigned int M3;unsigned int M4;unsigned int M5;

int start_PWM_Generation_in_ISR_FLAG=FALSE;
char levelofDimmer_MSB='0',levelofDimmer_LSB='0';
int checkFlag = FALSE;


void errorsISR(char* errNum);
void errorsMain(char* errNum);
void sendAcknowledgment(char* currentStateBuffer);
void sendFeedback_TO_Gateway(char sw_status, char Switch_Num);
//void sendFeedback_TO_Touch(char Switch_Num_1s, char sw_status);

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

void actiontouchPanel(char Switch_Num, char sw_status,char sw_speed );//, char speeds
void send_Response_To_Touch(char switch_no, char switch_status);

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






                
        // ************************************* MAIN *********************************************** //
void main() {
    
    
    GPIO_pin_Initialize();
    allPeripheralInit();
    R1=ON;R2=ON;R3=ON;R4=ON;R5=ON;
  

    RELAY1 = OFF;
    RELAY2 = OFF;
    RELAY3 = OFF;
    RELAY4 = OFF;

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
        
        
        
        
         /******************** MANUAL RESPONE STARTS HERE************ */
        
        
        
        int man=1;
         if(copy_parentalLockBuffer[1] == CHAR_OFF && INPUT1_SWITCH == OFF && R1 == OFF)
        {
         //   if(man == 1)
         //   {
            TX1REG = 'R';__delay_ms(1);
            TX1REG = '0';__delay_ms(1);
            TX1REG = '0';__delay_ms(1);
            TX1REG = '1';__delay_ms(1);
            send_Response_To_Touch('A','0');
            RELAY1=OFF;
         //   }
         //   man=0;
            R1=1;
            
        }
        //on condition
        if(copy_parentalLockBuffer[1] == CHAR_OFF && INPUT1_SWITCH == ON &&  R1 == ON)
        {
         //  if(man==1)
         //  {
               
             
            TX1REG = 'R';__delay_ms(1);
            TX1REG = '1';__delay_ms(1);
            TX1REG = '0';__delay_ms(1);
            TX1REG = '1';__delay_ms(1);
            send_Response_To_Touch('A','1');
            RELAY1=ON;
         //  }
          //  man=0;
            R1=0;
        }
        
       // //check switch second status 
        //off condition
        if(copy_parentalLockBuffer[2] == CHAR_OFF && INPUT2_SWITCH == OFF && R2 == OFF)
        {
         //   if(man==1)
         //   {
            
            TX1REG = 'R';__delay_ms(1);
            TX1REG = '0';__delay_ms(1);
            TX1REG = '0';__delay_ms(1);
            TX1REG = '2';__delay_ms(1);
            send_Response_To_Touch('B','0');
            RELAY2=OFF;
          //  }
         //   man=0;
            R2=1;
        }
        //on condtion
        if(copy_parentalLockBuffer[2] == CHAR_OFF && INPUT2_SWITCH == ON && R2 == ON)
        {
         //   if(man==1)
         //   {
           
            TX1REG = 'R';__delay_ms(1);
            TX1REG = '1';__delay_ms(1);
            TX1REG = '0';__delay_ms(1);
            TX1REG = '2';__delay_ms(1);
            send_Response_To_Touch('B','1');
           RELAY2=ON;
         //   }
         //   man=0;
            R2=0;
        }
        
        
       // //check switch third status 
        //off condition
        if(copy_parentalLockBuffer[3] == CHAR_OFF && INPUT3_SWITCH == OFF && R3 == OFF)
        {
            if(man == 1)
            {
            
            TX1REG = 'R';__delay_ms(1);
            TX1REG = '0';__delay_ms(1);
            TX1REG = '0';__delay_ms(1);
            TX1REG = '3';__delay_ms(1);
            send_Response_To_Touch('C','0');
            RELAY3=OFF;
            }
            man=0;
            R3=1;
          
        }
        //on condtion
        if(copy_parentalLockBuffer[3] == CHAR_OFF && INPUT3_SWITCH == ON && R3 == ON)
        {
            if(man==1)
            {
            
            TX1REG = 'R';__delay_ms(1);
            TX1REG = '1';__delay_ms(1);
            TX1REG = '0';__delay_ms(1);
            TX1REG = '3';__delay_ms(1);
            send_Response_To_Touch('C','1');
            RELAY3=ON;
            }
            man=0;
            R3=0;
            
        }
        
        
       // //check switch fourth status 
        //off condition
        if(copy_parentalLockBuffer[4] == CHAR_OFF && INPUT4_SWITCH == OFF && R4 == OFF)
        {
            if(man==1)
            {
            
            TX1REG = 'R';__delay_ms(1);
            TX1REG = '0';__delay_ms(1);
            TX1REG = '0';__delay_ms(1);
            TX1REG = '4';__delay_ms(1);
            send_Response_To_Touch('D','0');
            RELAY4=OFF;
            }
            man=0;
            R4=1;
            
        }
        //on condtion
        if(copy_parentalLockBuffer[4] == CHAR_OFF && INPUT4_SWITCH == ON && R4 == ON)
        {
            if(man==1)
            {
            
            TX1REG = 'R';__delay_ms(1);
            TX1REG = '1';__delay_ms(1);
            TX1REG = '0';__delay_ms(1);
            TX1REG = '4';__delay_ms(1);
            send_Response_To_Touch('D','1');
             RELAY4=ON;
            }
            man=0;
            R4=0;
           
        } 
       
    }//end of while
   
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
//    TX1REG=chDimmerSpeedMSB;
//    TX1REG=chDimmerSpeedLSB;
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
        __delay_ms(5);
        TX2REG = '(' ;
        __delay_ms(1);
        TX2REG = TouchMatikBoardAddress ;//touchmatoc address
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
    
    switch(integerSwitchNumber){
        case 1:
        {
             RELAY1 = integerSwitchState;
        }
            break;
        case 2:
            {
              RELAY2 = integerSwitchState;

            break;
            }
        case 3:
        {
            RELAY3 = integerSwitchState;
        }
            break;
        case 4:
        {
            RELAY4 = integerSwitchState;
        }
            break;
     
        default:
            break;
        }
    
}
  
void actiontouchPanel(char Switch_Num, char sw_status, char Sw_speed) //, char speeds
{

     M1=ON;M2=ON;M3=ON;M4=ON;  M5=ON; M6=ON; M7=ON; M8=ON;

    int switch_status = sw_status - '0';        
    int SwNum = Switch_Num - '@';//ASCII OF SWITCH NUMBER - ASCII OF @ i.e A>>65, B>>66, C>>67, D>>68 65-64=1 and so on
    int int_swSpeed = Sw_speed-'0';
    char ch_sw_num = SwNum +'0';//send '1' for switch A, '2' for sww2 and so on 
    
 if(checkFlag == TRUE)  
  
 {  
     checkFlag=FALSE;   
 }   
    else
    {
 //    TX1REG='T';
        switch(Switch_Num)
        {

               case 'A':
               {
               if(copy_parentalLockBuffer[1] == CHAR_OFF && M1 == ON)
                  {
                       sendFeedback_TO_Gateway('1',sw_status);
                       RELAY1 = switch_status; M1 = OFF;

                  }
               }

               break;
               case 'B':
               {

                 if( copy_parentalLockBuffer[2] == CHAR_OFF && M2 == ON  )
                  {       
                        sendFeedback_TO_Gateway('2',sw_status);
                         RELAY2 = switch_status;  M2 = OFF;

                  }

               }

               break;
               case 'C':
               {
              if( copy_parentalLockBuffer[3] == CHAR_OFF && M3 == ON )
                 {    
                    sendFeedback_TO_Gateway('3',sw_status);
                     RELAY3 = switch_status;
                       M3 = OFF;
                  }

               }
               break;
               case 'D':
               {
                   if(copy_parentalLockBuffer[4] == CHAR_OFF && M4 == ON  )
                  {
                       sendFeedback_TO_Gateway('4',sw_status);
                        RELAY4 = switch_status;M4 = OFF;
                 }

               }
               break;
             default:
             break;
        }
    }       
}
void allPeripheralInit(){
    EUSART_Initialize();
    EUSART2_Initialize();

}
void copyReceivedDataBuffer(){
    int dataBufferCounter=2;
    for(dataBufferCounter=2;dataBufferCounter<9;dataBufferCounter++){
        tempReceivedDataBuffer[dataBufferCounter-2]=mainReceivedDataBuffer[dataBufferCounter]; // copy data buffer from main
        mainReceivedDataBuffer[dataBufferCounter]='#';  // clean data buffer
    }
}
void sendAcknowledgment(char* currentStateBuffer){
  int Tx_count=0;
  	while(Tx_count!=4)
 	{ 
        while (!TX1STAbits.TRMT);

 		TX1REG = *currentStateBuffer;
 		*currentStateBuffer++;
        Tx_count++;
 	}
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

void sendFeedback_TO_Gateway(char Switch_Num, char sw_status)
{
     __delay_ms(5);     TX1REG = 'R';
    __delay_ms(1);      TX1REG = sw_status;
    __delay_ms(1);      TX1REG = '0';
    __delay_ms(1);      TX1REG = Switch_Num;
}
 
void GPIO_pin_Initialize() {         
    clearAllPorts();
    pinINIT_extra();
    
    INPUT_FAN_DIR=1;
    INPUT1_SWITCH_DIR=1;
    INPUT2_SWITCH_DIR=1;
    INPUT3_SWITCH_DIR=1;
    INPUT4_SWITCH_DIR=1;
    
    RELAY1_DIR=0;
    RELAY2_DIR=0;
    RELAY3_DIR=0;
    RELAY4_DIR=0;
    

    
    UART2_TX_DIR=0;//tx2 ouuput
    UART2_RX_DIR=1;//rx2 input
    
    UART1_TX_DIR=0;//tx1 output
    UART1_RX_DIR=1;//rx1 input
    
  clearAllPorts();
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
void send_Response_To_Touch(char switch_no, char switch_status)
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
        TX2REG=0;
        __delay_ms(1);
        TX2REG='0';
        __delay_ms(1);
        TX2REG='0';
        __delay_ms(1);
        TX2REG=')';
    }
void EUSART_Initialize(){
    // disable interrupts before changing states
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
void pinINIT_extra(){
    ANSELG=0x00;    WPUG = 0;
    
    ANSELF=0x00; 
    
    ANSELE=0x00;    WPUE=0x00;
    
    ANSELD=0x00;    WPUD=0x00;
    
    ANSELB=0x00;    WPUB=0x00;
    
    ANSELA=0x00;     
} 
void clearAllPorts()
{
 
    RELAY1=OFF;
    RELAY2=OFF;
    RELAY3=OFF;
    RELAY4=OFF;
}

