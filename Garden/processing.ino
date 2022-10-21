void ProcessingLights()
{

  if(lum < s_lum && (leds_ON == false ))
  {
    Serial.println("LEDs ON");
    digitalWrite(relay_leds,LOW);
    Blynk.setProperty(V5, "color", "#00FF2E");
  }
  else if(lum < s_lum && (leds_ON == true))
  {
    Serial.println("LEDs already ON");
  }
  else
  {
    Serial.println("LEDs OFF");
    digitalWrite(relay_leds,HIGH);
    Blynk.setProperty(V5, "color", "#FF0000");
  }
  delay(200);
}

void Processing_Moisture()
{
  if(soilMoister < s_moisture)
  {
    Serial.println("PUMP OFF");
     digitalWrite(relay_pump,HIGH);
     //if pump is off the led is turned RED on blynk
     Blynk.setProperty(V4, "color", "#FF0000");
  }
  else
  {
    Serial.println("PUMP ON");
    digitalWrite(relay_pump,LOW);
    delay(time_pump_ON);
    digitalWrite(relay_pump,HIGH);
    //Water pump was set ON, the LED is set to Green until the next reading session
    Blynk.setProperty(V4, "color", "#00FF2E");
  }  
}
