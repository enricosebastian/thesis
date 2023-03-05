import matplotlib.pyplot as plt

plt.ion()
for y in range(10):
    x = 1
    plt.scatter(x, y, c ="blue")
    plt.xlim(0,10)
    plt.ylim(0,10)
    plt.draw()
    plt.pause(0.1)
    plt.clf()