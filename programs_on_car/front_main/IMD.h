#ifndef HYTECHIMD
#define HYTECHIMD

#define CUTOFF_RESISTANCE 175

// this is for IMD code
// whenisgood code: bmcxrj7
// andrew's response edit code: http://whenisgood.net/ehyc2sa/update/is9xziy5
namespace IMD {

	int checkError() {
        // LOW setting for more responsive behavior
		long pulseTime = pulseIn(IMDpin, LOW, 200000);
		float resistanceKOhms = (0.9 * 1200.0)
            / (float(100000 - pulseTime) / 1000.0 - 5.0) - 1200.0;

		if (pulseTime == 0) {
			return 1;
		} else if (resistanceKOhms < CUTOFF_RESISTANCE) {
			return 2;
		}
	}
}

#endif
