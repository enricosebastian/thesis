from tkinter import *
from tkinter import ttk

import serial.tools.list_ports

root = Tk()
root.title("LinDDA Control Panel")

# Drone LED connection indicator
drone_1_button = Button(root, text="Drone 1", bg="red")
drone_1_button.grid(column=0, row=0)

drone_2_button = Button(root, text="Drone 2", bg="red")
drone_2_button.grid(column=1, row=0)

drone_3_button = Button(root, text="Drone 3", bg="red")
drone_3_button.grid(column=2, row=0)

drone_4_button = Button(root, text="Drone 4", bg="red")
drone_4_button.grid(column=3, row=0)

drone_5_button = Button(root, text="Drone 5", bg="red")
drone_5_button.grid(column=4, row=0)
################

# Drone counter
drone_count = 3
drone_counter = Label(root, text="Drones connected: "+str(drone_count), bg="blue")
drone_counter.grid(column=0, row=1, columnspan=6, sticky="ew")
################

# For port label
found_ports = serial.tools.list_ports.comports()

ports_dropdown_label = Label(root, text="Select an Arduino COM port: ", width=50)
ports_dropdown_label.grid(column=0, row=2, sticky="ew", columnspan=2)
#######################

selected_port = 0
serial_instance = serial.Serial()

# For port drop down list
def set_port(port_name):
    global selected_port
    selected_port = port_name[0:4]
    print("Selected port is:",selected_port)
    serial_instance.port = selected_port
    serial_instance.baudrate = 9600
    serial_instance.open()
    
    
    
ports = ["N/A"]

for found_port in found_ports:
    ports.append(str(found_port))

port_list = StringVar(root)
port_list.set(ports[0])

port_dropdown_list = ttk.OptionMenu(root, port_list, *ports, command=set_port)
port_dropdown_list.config(width=50)
port_dropdown_list.grid(column=3, row=2, sticky="ew", columnspan=3)
#################################

# For drone selection list label
drones = serial.tools.list_ports.comports()

select_drone_label = Label(root, text="Select drone: ", width=50)
select_drone_label.grid(column=0, row=3, sticky="ew", columnspan=2)
#######################

# For drone selection list
drone_list = ["N/A"]
drones = ["DRO1", "DRO2", "DRO3"]

for drone in drones:
    drone_list.append(str(drone))

selected_drone = StringVar(root)
selected_drone.set(drone_list[0])

select_drone_dropdown_list = ttk.OptionMenu(root, selected_drone, *drone_list)
select_drone_dropdown_list.config(width=50)
select_drone_dropdown_list.grid(column=3, row=3, sticky="ew", columnspan=3)
#################################

# For deploy button
deploy_button = Button(root, text="Deploy drone")
deploy_button.grid(column=0, row=4, sticky="ew", padx=5, pady=5)
##################

# For home button
home_button = Button(root, text="Send drone home")
home_button.grid(column=1, row=4, sticky="ew", padx=5, pady=5)
##################

# For stop button
stop_button = Button(root, text="Stop drone")
stop_button.grid(column=2, row=4, sticky="ew", padx=5, pady=5)
##################

# For where button
location_button = Button(root, text="Ask location of drone")
location_button.grid(column=3, row=4, sticky="ew", padx=5, pady=5)
##################

# For cancel command button
cancel_command_button = Button(root, text="Stop sending command")
cancel_command_button.grid(column=4, row=4, sticky="ew", padx=5, pady=5)
##################

# For region of responsibility label
drones = serial.tools.list_ports.comports()

select_drone_label = Label(root, text="Deployment area", width=50)
select_drone_label.grid(column=0, row=5, sticky="ew", columnspan=5)
#######################

# For y_max
y_max_label = Label(root, text="Y max", width=50)
y_max_label.grid(column=0, row=6, sticky="ew", columnspan=2)

y_max_input_label = ttk.Entry()
y_max_input_label.grid(column=3, row=6, sticky="ew", columnspan=2)
#################################

# For y_max
y_min_label = Label(root, text="Y min", width=50)
y_min_label.grid(column=0, row=7, sticky="ew", columnspan=2)

y_min_input_label = ttk.Entry()
y_min_input_label.grid(column=3, row=7, sticky="ew", columnspan=2)
#################################

# For anchor distance
anchor_distance_label = Label(root, text="Anchor distance", width=50)
anchor_distance_label.grid(column=0, row=8, sticky="ew", columnspan=2)

anchor_distance_input_label = ttk.Entry()
anchor_distance_input_label.grid(column=3, row=8, sticky="ew", columnspan=2)
#################################

# For serial log terminal
serial_terminal = Text(root, width=50)

def check_serial_port():
    received_a_message = False
    
    while not received_a_message:
        if selected_port != 0:
            if serial_instance.in_waiting:
                message = serial_instance.readline()
                received_a_message = True
                serial_terminal.insert("1.0", message)
        else:
            received_a_message = True
        
    root.after(500, check_serial_port)


serial_terminal.grid(column=0, row=9, sticky="ew", columnspan=5)
#################################

root.after(500, check_serial_port)
print("Done")
root.mainloop()