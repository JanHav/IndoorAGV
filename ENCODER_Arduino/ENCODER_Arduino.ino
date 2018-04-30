///BenodigdeLibrariesCanBus//////////////////////////////////////////////////////////////////
#include <Canbus.h>
#include <defaults.h>
#include <global.h>
#include <mcp2515.h>
#include <mcp2515_defs.h>

///VariabelenCANbus//////////////////////////////////////////////////////////////////////////
unsigned long prevTX1 = 0;                                        // Variabele om laatste tijd te onthouden
const unsigned int invlTX1 = 100;                                 // Interval tussen de CAN-berichten
unsigned long prevTX2 = 0;                                        // Variabele om laatste tijd te onthouden
const unsigned int invlTX2 = 100;                                 // Interval tussen de CAN-berichten

///VariabelenEncoder/////////////////////////////////////////////////////////////////////////
const byte ledPinIRSensor = 7;                                   //Pin 13 wordt gebruikt voor SPI communicatie tussen UNO en CAN shield daarom wordt er voor 7 gekozen
const byte interruptPin = 3;
volatile byte state = LOW;
int toer = 0;

///SetupLoop/////////////////////////////////////////////////////////////////////////////////
void setup() {
///Encoder///////////////////////////////////////////////////////////////////////////////////
  Serial.begin(115200);
  pinMode(ledPinIRSensor, OUTPUT);
  digitalWrite(ledPinIRSensor, HIGH);
  pinMode(interruptPin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(interruptPin), triggerEncoder, RISING);
///CANbus////////////////////////////////////////////////////////////////////////////////////
 Serial.println("CAN Write - Testing transmission of CAN Bus testBerichts");
  delay(1000);
  
  if(Canbus.init(CANSPEED_500))       //Initialise MCP2515 CAN controller at the specified speed
  {
  Serial.println("CAN Init ok");
  }
  else
  {
  Serial.println("Can't init CAN");
  } 
}

///Loop//////////////////////////////////////////////////////////////////////////////////////
void loop() {
  
  Serial.print("Aantal toeren: ");
  Serial.println(toer);
///Encoderdata in CANbericht plaatsen////////////////////////////////////////////////////////
   tCAN ENCODERdata;

    ENCODERdata.id = 0xC1;                                      //B1 is de hexadecimale identifier van het bericht die de rotatiedata bevat
    ENCODERdata.header.rtr = 0;
    ENCODERdata.header.length = 4;                              //de data length code is 2 decimaal
    ENCODERdata.data[0] = toer;             //Intel byte order event.orientation.x (LSB eerst)
    ENCODERdata.data[1] = toer>>8;        //Om een waarde die varieert tussen 0 en 360 door te sturen hebben we genoeg aan twee bytes
    ENCODERdata.data[2] = toer>>16;   
    ENCODERdata.data[3] = toer>>24;
    
    if (millis() - prevTX1 >= invlTX1)                     //Interval waarop canbericht 1 verstuurd wordt
    {
    prevTX1 = millis();
    mcp2515_send_message(&ENCODERdata);                         //Message is een pointer naar een geheugenlocatie waar we de gewenste data kunnen terugvinden
    } 
}

///ISR (interrupt service routine)///////////////////////////////////////////////////////////
void triggerEncoder() {
  toer++;
}
