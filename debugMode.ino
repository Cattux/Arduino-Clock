void debugMode(){
   
 LEDbrightness = map(analogRead(photoSensor),minb,maxb,25,255);
  if (LEDbrightness > 255)
  {
    LEDbrightness = 255;
  }
  else if (LEDbrightness < 1)
  {
    LEDbrightness = 25;
  }
    
  analogWrite(backLight, LEDbrightness); // turn backlight on. Replace 'HIGH' with 'LOW' to turn it off.
  
  lcd.clear();
  lcd.setCursor(0,0);
  int val4 = analogRead(buttonsSet);
  lcd.print(val4);
  lcd.setCursor(5,0);
  int val8 = digitalRead(redgreenLED);
  lcd.print(val8);
  lcd.setCursor(10,0);
  lcd.print(LEDbrightness);
  
  lcd.setCursor(0,1);
  int val5 = analogRead(buttonsTime);
  lcd.print(val5);
  lcd.write("  ");
  lcd.setCursor(5,1);
  int val6 = analogRead(defusePin);
  lcd.print(val6);
  lcd.setCursor(10,1);
  int val7 = analogRead(photoSensor);
  lcd.print(val7);
  
  delay(1000);
  }
