import matplotlib.pyplot as plt
import time

num_drones = 3 # number of drones

x_max = 0
x_min = 5
x_increment = -1

y_max = 20
y_min = 0
y_increment = 1

draw_delay = 0.01

x = x_min
y = y_min

def plot(x_data, y_data, color):
    plt.scatter(x_data, y_data, c=color)
    plt.xlim(0,20)
    plt.ylim(0,20)
    plt.draw()
    plt.pause(draw_delay)
    plt.clf()
    

while True:
    y += y_increment
    
    if y == y_max:
        y_increment *= -1
        x += x_increment
        
        temp = y_min
        y_min = y_max
        y_max = temp
        
    if x == x_max+x_increment:
        while x is not x_min:
            x += -1*x_increment
            plot(x, y, "blue")
        
    plot(x, y, "blue")

            
            
        
    


            
        
# plt.scatter(x, y, c ="blue")
# plt.xlim(0,x_max)
# plt.ylim(0,y_max)
# plt.draw()
# plt.pause(draw_delay)
# plt.clf()
        
        
            