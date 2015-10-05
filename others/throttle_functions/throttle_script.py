import matplotlib.pyplot as plt
import numpy as np

x = np.arange(0, 1, 0.001);

y1 = x * (2 - x);
y2 = np.power(x, 1.5) * (2.5 - 1.5 * x)
y3 = np.power(x,   2) * (3   -   2 * x)
y4 = np.power(x, 2.5) * (3   -   2 * x)
y5 = np.power(x,   3) * (2.5 - 1.5 * x)
y6 = np.power(x,   4) * (2   -       x)

plt.plot(x, y1, x, y2, x, y3, x, y4, x, y5, x, y6)
plt.axis([0, 1, 0, 1])
plt.show()
