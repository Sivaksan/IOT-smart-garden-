void callback(String topic, byte* message, unsigned int length) {
  
  Serial.print("Message arrived on topic: ");
  Serial.print(topic);
  Serial.print(". Message: ");
  String messageTemp;
  
  for (int i = 0; i < length; i++) {
    Serial.print((char)message[i]);
    messageTemp += (char)message[i];
  }
  Serial.println();

  // Feel free to add more if statements to control more GPIOs with MQTT

  // If a message is received on the topic room/lamp, you check if the message is either on or off. Turns the lamp GPIO according to the message
  if(topic=="Garden/Light"){
      Serial.print(" Switching Leds ");
      if(messageTemp == "1"){
        digitalWrite(relay_leds, LOW);
        Serial.print("On");
      }
      else if(messageTemp == "0"){
        digitalWrite(relay_leds, HIGH);
        Serial.print("Off");
      }
  }
  Serial.println();
}