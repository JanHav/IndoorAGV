void Debugging()
{
  Serial.print("UWB lock: ");
  Serial.println(UWB_lock);
  Serial.print("xyStart lock: ");
  Serial.println(xyStart_lock);
  Serial.print("Opstart lock: ");
  Serial.println(OpStart_lock);
  Serial.print("Encoder lock: ");
  Serial.println(Encoder_lock);
  Serial.print("Gewenste duty: ");
  Serial.println(GewensteDutyCycle);
  Serial.print("GewensteDutyCycleServo: ");
  Serial.println(GewensteDutyCycleServo);
  Serial.print("Encoderdata ");
  Serial.println(EncoderData);
  Serial.print("BeginEncoder ");
  Serial.println(beginEncoder);
  Serial.print("EncoderOpstart");
  Serial.println(EncoderOpstart);
  Serial.print("EncoderafstandIni: ");
  Serial.println(((125 * abs(EncoderData - beginEncoder))+125));
  Serial.print("HoekTrack: ");
  Serial.println(HoekTrack);
  Serial.print("HoekAGV: ");
  Serial.println(HoekAGV);
  Serial.print("Hoekverschil: ");
  Serial.println(abs(HoekTrack - HoekAGV));
  Serial.print("AfstandTotWaypoint: ");
  Serial.println(AfstandTotWaypoint);
  Serial.print("Huidige index waypoint: ");
  Serial.println(current_wp);
}

