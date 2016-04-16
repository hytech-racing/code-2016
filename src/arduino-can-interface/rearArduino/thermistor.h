#define THERM_110 628
#define THERM_100 821
#define THERM_90 1088
#define THERM_80 1462
#define THERM_70 1994
#define THERM_60 2763
#define THERM_50 3894
#define THERM_40 5593

int checkThermistor(long pullDownResist, int analogVal) { // pull down resistor value and the value read in from "analog read"
  float ratio = 1024.0/float(analogVal);
  long resistance = float(pullDownResist)*ratio - pullDownResist;
  
  if(analogVal < 50) { // this occurs if the thermistor gets disconnected
    return 120;
  }
  else if(resistance < THERM_110) {
    return 120;
  }
  else if(resistance < THERM_100) {
    return 110;
  }
  else if(resistance < THERM_90) {
    return 100;
  }
  else if(resistance < THERM_80) {
    return 90;
  }
  else if(resistance < THERM_70) {
    return 80;
  }
  else if(resistance < THERM_60) {
    return 70;
  }
  else if(resistance < THERM_50) {
    return 60;
  }
  else if(resistance < THERM_40) {
    return 50;
  }
  else {
    return 40;
  }
}
