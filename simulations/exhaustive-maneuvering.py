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
print(colors)

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

# Important x points
sizeOfArea = abs(x_max - x_min) // num_drones
x = list(range(x_min, x_max+1, sizeOfArea))
x_max = x[0:len(x)-1]
x_min = x[1:len(x)]
x_increment = np.ones(num_drones).astype(int) * -1
x_data = x_min.copy()

y_increment = np.ones(num_drones).astype(int) * 1
y_data = random.sample(range(y_min,y_min+4), num_drones)

print("x_max: ", end=" ")
print(x_max)
print("y_min: ", end=" ")
print(y_min)

print("x_data: ", end=" ")
print(x_data)
print("y_data: ", end=" ")
print(y_data)

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
    plt.xlim(x_lim)
    plt.ylim(y_lim)
    
    plt.draw()
    
    plt.pause(draw_delay)
    plt.clf()
    
    y_data += y_increment
    
    
#     for i in enumerate(y):
#         if y == y_max:
            
#     plt.scatter(x, y, c=colors)
#     plt.xlim(0,20)
#     plt.ylim(0,20)
    
#     plt.draw()
    
#     plt.pause(draw_delay)
#     plt.clf()
    
#     y += y_increment
    
# for i in arr:
#     print(i)

# def plot(x_data, y_data, color):
#     plt.scatter(x_data, y_data, c=color)
#     plt.xlim(0,20)
#     plt.ylim(0,20)
#     plt.draw()
#     plt.pause(draw_delay)
#     plt.clf()
    

# while True:
#     y += y_increment
    
#     if y == y_max:
#         y_increment *= -1
#         x += x_increment
        
#         temp = y_min
#         y_min = y_max
#         y_max = temp
        
#     if x == x_max+x_increment:
#         while x is not x_min:
#             x += -1*x_increment
#             plot(x, y, "blue")
        
#     plot(x, y, "blue")

            
            
        
    


            
        
# plt.scatter(x, y, c ="blue")
# plt.xlim(0,x_max)
# plt.ylim(0,y_max)
# plt.draw()
# plt.pause(draw_delay)
# plt.clf()
        
        
            