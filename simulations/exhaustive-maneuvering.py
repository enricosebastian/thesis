import matplotlib.pyplot as plt

num_drones = 3 # number of drones

x_max = 20 # maximum distance of deployment (x)
x_min = range(0, x_max, 20//num_drones)
x_increment = -1

y_max = 20 # maximum distance of deployment (y)
y_min = 0
y_increment = 1

draw_delay = 0.01


y = y_min

y_range = range(y_min, y_max+y_increment, y_increment)
for x in range(x_min[1], x_min[0], x_increment):
    for y in y_range:   
        if y == 6:
            y_range = y_range = range(y_max+y_increment, y_min, -y_increment)
        print(y_range[2])
            
        
        # plt.scatter(x, y, c ="blue")
        # plt.xlim(0,x_max)
        # plt.ylim(0,y_max)
        # plt.draw()
        # plt.pause(draw_delay)
        # plt.clf()
        
        
            