import matplotlib.pyplot as plt
import time
import numpy as np


# Number of drones to deploy
num_drones = 3 

# You have to match the amount of colors to the number of drones
colors = ["red", 
          "blue", 
          "green",
          ]

# How fast/slow the coordinates will update
draw_delay = 0.01

# How big of small your plot image is
x_lim = (-5,20)
y_lim = (0, 20)


# DEPLOYMENT PARAMETERS
# How big or small your deployment zone is
x_max = 20
x_min = 0

# How far or near your drones reach
y_max = 20
y_min = 0

x = list(range(x_min, x_max, abs(x_max - x_min)//num_drones))
x_max = x[0:len(x)-1]
x_min = x[1:len(x)]
x_increment = np.ones(num_drones).astype(int) * -1
x_data = x_min.copy()

y_max = np.ones(num_drones).astype(int) * y_max #<--- edit number constant
y_min = np.zeros(num_drones).astype(int)
y_increment = np.ones(num_drones).astype(int) * 1
y_data = y_min.copy()
y_data = [1, 2, 0]

while True:
    for idx, y in enumerate(y_data):
        if y == y_max[idx]:
            y_increment[idx] *= -1
            temp = y_min[idx]
            y_min[idx] = y_max[idx]
            y_max[idx] = temp
            x_data[idx] += x_increment[idx]
            
    for idx, x in enumerate(x_data):
        if x == x_max[idx]:
            x_increment[idx] *= -1
            temp = x_min[idx]
            x_min[idx] = x_max[idx]
            x_max[idx] = temp
    
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
        
        
            