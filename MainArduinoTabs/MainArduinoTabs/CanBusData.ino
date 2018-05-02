void CanBusData()
{
  tCAN XYcoor;
if (mcp2515_check_message()) 
  {
    if (mcp2515_get_message(&XYcoor)) 
        {
        if(XYcoor.id == 0xA1)                                                  //ID bericht dat UWBdata bevat
                {
               X_Huidig = (XYcoor.data[0] + (XYcoor.data[1]*256)+(XYcoor.data[2]*65536)+(XYcoor.data[3]*16777216));             //De data is opgeslaan volgens Intel Byte Order d.w.z. LSB eerst!
               Y_Huidig = (XYcoor.data[4] + (XYcoor.data[5]*256)+(XYcoor.data[6]*65536)+(XYcoor.data[7]*16777216));
               Serial.print("X huidig: ");                 
               Serial.println(X_Huidig);
               Serial.print("Y huidig: ");                 
               Serial.println(Y_Huidig);
                }
        }
   }

  tCAN BNO;
if (mcp2515_check_message()) 
  {
    if (mcp2515_get_message(&BNO)) 
        {
        if(BNO.id == 0xB1)                                                  //ID bericht dat rotatiedata bevat
                {
               RotatieData = (BNO.data[0] + (BNO.data[1]*256));             //De data is opgeslaan volgens Intel Byte Order d.w.z. LSB eerst!
               Serial.print("Rotatiedata: ");                 
               Serial.println(RotatieData);
                }
        }
   }

  tCAN Encoder;
if (mcp2515_check_message()) 
  {
    if (mcp2515_get_message(&Encoder)) 
        {
        if(Encoder.id == 0xC1)                                                  //ID bericht dat encoderdata bevat
                {
               EncoderData = (Encoder.data[0] + (Encoder.data[1]*256)+(Encoder.data[2]*65536)+(Encoder.data[3]*16777216));             //De data is opgeslaan volgens Intel Byte Order d.w.z. LSB eerst!
               Serial.print("Encoderdata: ");                 
               Serial.println(EncoderData);
                }
        }
   }
}

