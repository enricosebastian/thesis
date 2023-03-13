import matplotlib.pyplot as plt
import numpy as np
import random
import time


# Number of drones to deploy
num_drones = 3

# You have to match the amount of colors to the number of drones
colors = ["red", 
          "green",
          "blue",
          "yellow",
          "purple",
          "orange",
          "pink",]

colors = colors[0:num_drones]

# How fast/slow the coordinates will update
draw_delay = 0.01

# DEPLOYMENT PARAMETERS
# How big or small your deployment zone is
x_max = 17
x_min = 0

# How far or near your drones reach
y_max = 18
y_min = 3

# How big of small your plot image is
x_lim = (x_min-2,x_max+2)
y_lim = (y_min-2, y_max+2)

# Random trash
random_trash_x = list(np.random.randint(low=x_min, high=x_max, size=20))
random_trash_y = list(np.random.randint(low=y_min, high=y_max, size=20))

# Important x points
sizeOfArea = abs(x_max - x_min) // num_drones
x = list(range(x_min, x_max+1, sizeOfArea))
x_max = x[0:len(x)-1]
x_min = x[1:len(x)]
x_increment = np.ones(num_drones).astype(int) * -1
x_data = x_min.copy()

y_increment = np.ones(num_drones).astype(int) * 1
y_data = list(np.random.randint(low=y_min, high=y_min+3, size=num_drones))

print("\ndrones: ", colors, end="\n\n")

print("x_max: ", end=" ")
print(x_max)
print("y_min: ", end=" ")
print(y_min, end='\n\n')

print("x_data: ", end=" ")
print(x_data)
print("y_data: ", end=" ")
print(y_data, end='\n\n')

print("random_trash_x: ", end=" ")
print(random_trash_x)
print("random_trash_y: ", end=" ")
print(random_trash_y)

while True:
    for idx, y in enumerate(y_data):
        if y > y_max or y < y_min:
            y_increment[idx] *= -1
            x_data[idx] += x_increment[idx]
    
    for idx, x in enumerate(x_data):
        if x == x_max[idx]:
            y_increment[idx] = 0
            x_increment[idx] *= -1
        if x == x_min[idx]:
            if y_data[idx] > y_max:
                y_increment[idx] = -1
                x_increment[idx] = 0
            elif y_data[idx] < y_min:
                y_increment[idx] = 1
                x_increment[idx] = -1
            
    
    plt.scatter(x_data, y_data, c=colors)
    plt.scatter(random_trash_x, random_trash_y, c='green', marker='x')
    plt.xlim(x_lim)
    plt.ylim(y_lim)
    
    plt.draw()
    
    plt.pause(draw_delay)
    plt.clf()
    
    y_data += y_increment