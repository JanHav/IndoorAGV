/*****
Auteur: Jan Haverbeke
Datum: 30/04/2018
Project: Ontwikkeling van een robot die autonoom kan navigeren in een overdekte ruimte.
Doel:
Dit programma wordt weggeschreven naar een Arduino UNO dit op een CAN bus (500 kbps) is aangesloten.
De CAN bus bestaat uit 4 Arduino UNO's
- 1 UNO verzamelt al de info en bevat het navigatie-algoritme
- 1 UNO zet UWB (locatie) data op de CAN bus
- 1 UNO zet rotatiedata op de CAN bus (BNO055)
- 1 UNO zet encoderdata op de CAN bus
*****/

///BenodigdeLibrariesCanBus//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#include <Canbus.h>
#include <defaults.h>
#include <global.h>
#include <mcp2515.h>
#include <mcp2515_defs.h>

//BenodigdeFilesVoorRCcontrol////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#include <PWM.h>

///BenodigdeFilesNavSof//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#include "waypoints.h"
#include <math.h>

///Flags/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
byte UWB_lock = 0;
byte xyStart_lock = 0;
byte OpStart_lock = 0;
byte Encoder_lock = 0;
byte BNO_lock = 0;
byte EncoderOpstart = 0;

///RCcontrol//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int motor = 3;                                //Pin 9 UNO voor PWM aandrijfmotor (aandrijving)
int servoMotor = 9;                          //Pin 10 UNO voor PWM servomotor (sturen)
int32_t frequency = 100;                      //Frequentie (in Hz, we willen een PWM met een frequentie van 100 Hz
float GewensteDutyCycle = 43;              //Dutycycle (38/255) = 15,06%, kan verhoogd worden als er meer apparatuur op het RC car platform geplaatst wordt, 42.12
float GewensteDutyCycleServo = 35;            //Dutycycle voor de stuurservo in de rechtuitstand
volatile long RcBediening_startPulse;         //Bevat de waarde van micros() op het moment dat een stijgende flank op de interrupt wordt gezien
volatile unsigned int pulse_val;              //Bevat de tijdON van de PWM uitgestuurd door de RC zender
unsigned long previousMillis = 0;             //Variabelen die we gebruiken om het afbouwen van de dutycycle van de aansturing van de aandrijfmotor
unsigned long currentMillis = 0;              //constant te laten verlopen (het afbouwen mag langer maar niet korter duren, te kort = elektromotor heeft te weinig koppel
const int pingPin = 7;                        //Pin waarop signaal ultrasone sensor Parallax toekomt

///VariabelenCanBus////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int RotatieData = 0;
int EncoderData = 0;
int X_Huidig = 0;
int Y_Huidig = 0;

///VariabelenNavSof//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int waypoints;                      // Waypoint teller 
byte current_wp=0;                  // Bevat de index waarmee we in de array van de waypoints in de header file geraken, bepaald eindpunt trackline
byte previous_wp=0;                 // Bevat de index waarmee we in de array van de waypoints in de header file geraken, bepaald beginpunt trackline
float xhuidig_wp = 0.0;             // Bevat de huidige x-coördinaat van de AGV, in demonstratiesoftware verkrijgen we dit via userinput 
float yhuidig_wp = 0.0;             // Bevat de huidige y-coördinaat van de AGV, in demonstratiesoftware verkrijgen we dit via userinput
float xstart_wp = 0.0;              // Bevat de x-coördinaat begin trackline
float ystart_wp = 0.0;              // Bevat de y-coördinaat begin trackline

float HoekTrack = 0.0;              //Bevat de trackhoek
float HoekAGV = 0.0;                //Bevat de hoek van de AGV
float beginBNO = 0.0;               //Beginwaarde BNO (rotatiewaarde)
float dot = 0.0;                    //Dot product van twee vectoren
float det = 0.0;                    //Determinant van twee vectoren
float angle = 0.0;                  //De counter clockwise hoek tussen twee vectoren
float AfstandTotWaypoint = 0.0;     //De afstand tot het volgende waypoint
int beginEncoder = 0;               //Beginwaarde encoder 
///Constanten////////////////////////////////////////////////////////////////////////////////
const float pi = 3.1415;            //Het getal PI (wiskundige constante)
///Setup loop////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void setup() {
  Serial.begin(115200);                                                       //Instellen baudrate serial monitor
  Serial.println("CAN Read - Testing receival of CAN Bus message");  
  delay(1000);
  
  if(Canbus.init(CANSPEED_500))                                               //Initialise MCP2515 CAN controller at the specified speed
    Serial.println("CAN Init ok");
  else
    Serial.println("Can't init CAN");
    
///NavSof////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  waypoints = sizeof(wps) / sizeof(XasYas);                                   //Berekenen van het aantal waypoints in onze huidige array

///RC control///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  InitTimersSafe();                                                   //Nodig om timers hun frequentie aan te passen (buiten timer 0, deze gebruikt Arduino onder meer voor de millis() functie
  pinMode(2, INPUT);                                                  //Op pin 2 zit onze interrupt voor besturing over te nemen via de RC zender
  attachInterrupt(digitalPinToInterrupt(2), rc_begin, RISING);                               //De interrupt moet initieel kijken naar een stijgende flank, als deze gezien wordt gaan we naar de ISR (interrupt service routine)
  SetPinFrequencySafe(motor, frequency);                              //De juiste pinnen voorzien van de juiste frequentie voor het PWM signaal
  SetPinFrequencySafe(servoMotor, frequency);
}

///Main loop/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void loop(){
tCAN Ontvangen;
if (mcp2515_check_message()) 
  {
    if (mcp2515_get_message(&Ontvangen)) 
        {
        if(Ontvangen.id == 0xA1)                                                  //ID bericht dat UWBdata bevat
                {
               X_Huidig = (Ontvangen.data[0] + (Ontvangen.data[1]*256)+(Ontvangen.data[2]*65536)+(Ontvangen.data[3]*16777216));             //De data is opgeslaan volgens Intel Byte Order d.w.z. LSB eerst!
               Y_Huidig = (Ontvangen.data[4] + (Ontvangen.data[5]*256)+(Ontvangen.data[6]*65536)+(Ontvangen.data[7]*16777216));
               Serial.print("X huidig: ");                 
               Serial.println(X_Huidig);
               Serial.print("Y huidig: ");                 
               Serial.println(Y_Huidig);
                }
         else if(Ontvangen.id == 0xB1)                                                  //ID bericht dat rotatiedata bevat
                {
               RotatieData = (Ontvangen.data[0] + (Ontvangen.data[1]*256));             //De data is opgeslaan volgens Intel Byte Order d.w.z. LSB eerst!
               Serial.print("Rotatiedata: ");                 
               Serial.println(RotatieData);
                }
         else if(Ontvangen.id == 0xC1)                                                  //ID bericht dat encoderdata bevat
                {
               EncoderData = (Ontvangen.data[0] + (Ontvangen.data[1]*256)+(Ontvangen.data[2]*65536)+(Ontvangen.data[3]*16777216));             //De data is opgeslaan volgens Intel Byte Order d.w.z. LSB eerst!
               Serial.print("Encoderdata: ");                 
               Serial.println(EncoderData);
                }
        }
        }
  

 
///RCcontrol//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  if (pulse_val > 600 && pulse_val <2400)                           //Als pulse_val een waarde hiertussen bevat wil dat zeggen dat de RC zender aanstaat m.a.w.
  {                                                                 //dat de zender op channel 3 van de RC ontvanger een PWM plaatst.
    pinMode(motor,INPUT);                                           //De motorpin en servomotorpin als INPUT benoemen zodat ze niet het signaal van de RC zender beïnvloeden
    pinMode(servoMotor, INPUT);                                     //Opdat de auto na afzetten RC zender terug bestuurd zou kunnen worden door de UNO dienen we de variabele die
    --pulse_val;                                                    //de laatste tijdON bevat te verkleinen we in het else gedeelte (=UNO aansturing) geraken
    GewensteDutyCycle = 42.12;                                      //Variabelen krijgen deze waarden bij overgang controle RC zender naar UNO                                         
    GewensteDutyCycleServo = 35;               
  }                                                                 
  else
  {
  //CanBusData();                                                     //Volgende data wordt van de CAN bus gehaald: UWB, rotatiedata (BNO055) en encoderdata
  NavSof();                                                         //Bevat het waypoint navigatie algoritme
  Debugging();
  }
}
