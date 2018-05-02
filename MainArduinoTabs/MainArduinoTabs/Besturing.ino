///Functies voor Arduino control

/*****
Doel: De RC car vooruit laten rijden
Parameters:
void
Return value:
geen
*****/

void VooruitRijden()
{
  pinMode(motor,OUTPUT);
  currentMillis = millis();
   if (currentMillis-previousMillis>=1 && GewensteDutyCycle > 42.00)     //Met deze waarden kan je spelen als de belasting op de RC car stijgt of je hem trager/ sneller wilt laten rijden 42.01
    {
      GewensteDutyCycle = GewensteDutyCycle-0.01;
      previousMillis = millis();
    }
  pwmWrite(motor, GewensteDutyCycle);                                   //De motor aansturen met de gewenste dutycycle 
}

/*****
Doel: De RC car laten stoppen.
Parameters:
void
Return value:
geen
*****/

void Stoppen()
{
  pinMode(motor,OUTPUT);
  pwmWrite(motor, 26);                                   //De motor aansturen met de gewenste dutycycle 
  delay(10);                                             //Ervoor zorgen dat er lang genoeg kan geremd worden op de motor
  pinMode(motor,INPUT);                                  //De rc car kan pas weer naar voor rijden als de
}                                                        //VooruitRijden functie opgeroepen wordt
/*****
Doel: De RC car naar links laten rijden
Parameters:
void
Return value:
geen
*****/
void LinksStuur()
{
  pinMode(servoMotor,OUTPUT);
  //float error = abs(afstandError);
  //GewensteDutyCycleServo = 44;    //47
  //GewensteDutyCycleServo = 35 + (abhoek*2/10);
  //if (100<= error <= 150)
  //{
   // GewensteDutyCycleServo = 35 + ((error-50)/100*5);
   // pwmWrite(servoMotor, GewensteDutyCycleServo);
  //}
  pwmWrite(servoMotor, 41);                         //De motor aansturen met de gewenste dutycycle
}

/*****
Doel: Stuur RC car in neutraalpositie brengen
Parameters:
void
Return value:
geen
*****/
void NeutraalStuur()
{
  pinMode(servoMotor,OUTPUT);
  GewensteDutyCycleServo = 35;    //35
  pwmWrite(servoMotor, GewensteDutyCycleServo);                     //De motor aansturen met de gewenste dutycycle
}

/*****
Doel: De RC car naar rechts laten rijden
Parameters:
void
Return value:
geen
*****/
void RechtsStuur()
{
  pinMode(servoMotor,OUTPUT);
 //float error = abs(afstandError);
   
  //GewensteDutyCycleServo = 25;//22
  //GewensteDutyCycleServo = 35 - (abhoek*2/10);
  //if (100<= error <= 150)
  //{
    //GewensteDutyCycleServo = 35 - ((error-50)/100*5);
    //pwmWrite(servoMotor, GewensteDutyCycleServo);
  //}
  pwmWrite(servoMotor, 29);                         //De motor aansturen met de gewenste dutycycle
}

