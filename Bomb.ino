void Bomb(int countDown){
  if (countDown != 11)
  {
  for (countDown; countDown > 0; countDown--)
  {
    lcd.clear();
    x=12;
    customBOMB();
    int y = map(countDown,10,0,1,11);
    lcd.setCursor(y,0);
    lcd.print("*");
    //for (int i = countDown; i>1; i--){lcd.print("_");}
    lcd.print("_");
    delay(1000);
  }
  lcd.setCursor(countDown,0);
  lcd.print(" ");
  lcd.print("*");
  delay(1000);
}
else{}
}
void Explosion(){
  for (int i=5;i>0;i--)
  {  
    lcd.clear();
    delay(250);
    customEXP();
    delay(250);
  }
  delay(500);
}


