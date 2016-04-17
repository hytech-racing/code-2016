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
    return int(map(resistance, THERM_100, THERM_110, 100, 110));
  }
  else if(resistance < THERM_90) {
    return int(map(resistance, THERM_90, THERM_100, 90, 100));
  }
  else if(resistance < THERM_80) {
    return int(map(resistance, THERM_80, THERM_90, 80, 90));
  }
  else if(resistance < THERM_70) {
    return int(map(resistance, THERM_70, THERM_80, 70, 80));
  }
  else if(resistance < THERM_60) {
    return int(map(resistance, THERM_60, THERM_70, 60, 70));
  }
  else if(resistance < THERM_50) {
    return int(map(resistance, THERM_50, THERM_60, 50, 60));
  }
  else if(resistance < THERM_40) {
    return int(map(resistance, THERM_40, THERM_50, 40, 50));
  }
  else {
    return 40;
  }
}
