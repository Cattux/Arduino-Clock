void DispDate(){

    // print day of week, month and day of month
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(myWeekDays[weekDay]); 
    lcd.print(" ");
    lcd.print(myMonths2[month]);
    lcd.print(" ");
    lcd.print(monthDay);

  
    // print time and year
    lcd.setCursor(0, 1);
    // print the number of seconds since reset:
    lcd.print(addZero(hour12));
    lcd.print(":");
    lcd.print(addZero(minute));
    //lcd.print(":");
    //lcd.print(addZero(second)); 
    lcd.print(" ");//20");
    year = year2*100+year;
    lcd.print(year);
    
}
