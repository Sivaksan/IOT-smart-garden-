void getDhtData(void)
{
  digitalWrite (DHTVCC,HIGH);
  delay(500);
  float temIni = tem;
  float humIni = hum;
  tem = dht.readTemperature();
  hum = dht.readHumidity();
  if (isnan(hum) || isnan(tem))   
    Serial.println("Failed to read from DHT sensor...");
    tem = temIni;
    hum = humIni;
    return;
  }
  Serial.println("Temperature :");
  Serial.println(tem);
  Serial.println("Humidity :");
  Serial.println(hum);
  Blynk.virtualWrite(V2, hum);
  Blynk.virtualWrite(V0, tem);
  
}


void getSoilMoisterData(void)
{
  //Stop DHT 
  digitalWrite (DHTVCC,LOW);
  digitalWrite (soilMoisterVcc, HIGH);
  delay (500);  
  soilMoister = analogRead(soilMoisterPin);
  
  Serial.println("measured soil moisture :");
  Serial.println(soilMoister);
  soilMoister = (soilMoister*100)/1024;
  Blynk.virtualWrite(V3, soilMoister);
  
  ////////*************************////////
  digitalWrite (soilMoisterVcc, LOW);  
  delay(500);  

  digitalWrite (ptrVcc,HIGH);
  delay(500);  
  lum = analogRead(soilMoisterPin);
  lum = (lum*100)/1024;

  Serial.println("Light measured:");
  Serial.println(lum);
  //Publish the value of the light on Blynk
  Blynk.virtualWrite(V1,lum);
  digitalWrite(ptrVcc,LOW);  
}
