int DispLgTime(){//, int second, int sec_old){
  
  x=0;
  String StrHour = addZero(hour12);  //convert number to string
  String StrMin = addZero(minute); 
  
  time[0] = int(StrHour[0]-48);  //convert back to number
  time[1] = int(StrHour[1]-48);  
  time[2] = int(StrMin[0]-48);
  time[3] = int(StrMin[1]-48);
  
  // Test if time has changed
  if (time[0] != timeOld[0] || time[1] != timeOld[1] || time[2] != timeOld[2] || time[3] != timeOld[3] || y==1)
  { 
    lcd.clear();
    //Serial.println(StrHour[1]);
    bigFont(time[0]);   //display first digit of hour
    bigFont(time[1]);
    x=x+1;
 
 
    customCOLON();
    x=x+2;
    bigFont(time[2]);
    bigFont(time[3]);
   
    lcd.setCursor(15,0);
    if (AM == 0){lcd.print("P");}
    else {lcd.print("A");}
    lcd.setCursor(15,1);
    lcd.print("M");
    timeOld[0] = time[0];
    timeOld[1] = time[1];
    timeOld[2] = time[2];
    timeOld[3] = time[3];
   
    return 0;
  }
  else{}
}
  

