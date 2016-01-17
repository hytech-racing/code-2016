
#ifndef HYTECHIMD
#define HYTECHIMD
// this is for IMD code
//whenisgood code: bmcxrj7
// andrew's response edit code: http://whenisgood.net/ehyc2sa/update/is9xziy5
namespace IMD {
	int checkError() {
		long pulseTime = pulseIn(IMDpin, LOW, 200000); // i'm using the LOW setting because I think it will be more responsive
		float resistanceKOhms = (0.9*1200.0)/(float(100000-pulseTime)/1000.0 -5.0)-1200.0;
		if(pulseTime = 0) {
			return 1;
		}
		else if(resistanceKOhms < 175) { // or whatever we want the resistance cutoff to be.
			return 2;
		}
	
			
		}
	}
}
#endif