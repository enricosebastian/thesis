from tkinter import *
from threading import *
from tkinter import ttk
from datetime import datetime

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
drone_counter = Label(root, text="Drones connected: "+str(drone_count), pady=10)
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

port_dropdown_list = ttk.OptionMenu(root, port_list, ports[0], *ports, command=set_port)
port_dropdown_list.config(width=50)
port_dropdown_list.grid(column=3, row=2, sticky="ew", columnspan=3)
#################################

# For drone selection list label
drones = serial.tools.list_ports.comports()

select_drone_label = Label(root, text="Selected drone: ", width=50)
select_drone_label.grid(column=0, row=3, sticky="ew", columnspan=2)
#######################

# For drone selection list
drones = ["N/A"]
found_drones = ["ALL"]

for found_drone in found_drones:
    drones.append(str(found_drone))

drone_list = StringVar(root)

select_drone_dropdown_list = ttk.OptionMenu(root, drone_list, drones[0], *drones)
select_drone_dropdown_list.config(width=50)
select_drone_dropdown_list.grid(column=3, row=3, sticky="ew", columnspan=3)
#################################

# For deploy button
deploy_button = Button(root, text="Start deployment")
deploy_button.grid(column=0, row=4, sticky="ew", padx=5, pady=5)
##################

# For home button
move_button = Button(root, text="Start moving")
move_button.grid(column=1, row=4, sticky="ew", padx=5, pady=5)
##################

# For stop button
stop_button = Button(root, text="Stop moving")
stop_button.grid(column=2, row=4, sticky="ew", padx=5, pady=5)
##################

# For where button
location_button = Button(root, text="Ask for location")
location_button.grid(column=3, row=4, sticky="ew", padx=5, pady=5)
##################

# For cancel command button
home_button = Button(root, text="Go home")
home_button.grid(column=4, row=4, sticky="ew", padx=5, pady=5)
##################

# For cancel command button
cancel_command_button = Button(root, text="Cancel command")
cancel_command_button.grid(column=2, row=5, sticky="ew", padx=5, pady=5)
##################

# For region of responsibility label
drones = serial.tools.list_ports.comports()

select_drone_label = Label(root, text="Deployment area", width=50, pady=10)
select_drone_label.grid(column=0, row=6, sticky="ew", columnspan=5)
#######################

# For y_max
y_max_label = Label(root, text="Y max", width=50)
y_max_label.grid(column=0, row=7, sticky="ew", columnspan=2)

y_max_input_label = ttk.Entry()
y_max_input_label.grid(column=3, row=7, sticky="ew", columnspan=2)
#################################

# For y_max
y_min_label = Label(root, text="Y min", width=50)
y_min_label.grid(column=0, row=8, sticky="ew", columnspan=2)

y_min_input_label = ttk.Entry()
y_min_input_label.grid(column=3, row=8, sticky="ew", columnspan=2)
#################################

# For anchor distance
anchor_distance_label = Label(root, text="Anchor distance", width=50)
anchor_distance_label.grid(column=0, row=9, sticky="ew", columnspan=2)

anchor_distance_input_label = ttk.Entry()
anchor_distance_input_label.grid(column=3, row=9, sticky="ew", columnspan=2)
#################################

# For serial log terminal
serial_terminal = Text(root, width=50)

def check_serial_port_thread():
    t1 = Thread(target=check_serial_port)
    t1.start()

def check_serial_port():
    received_a_message = False
    
    while not received_a_message:
        if selected_port != 0:
            if serial_instance.in_waiting:
                current_time = datetime.now().strftime("%H:%M:%S") + ": "
                message = serial_instance.readline()
                received_a_message = True
                
                serial_terminal.insert("1.0", message)
                serial_terminal.insert("1.0", current_time)
        else:
            received_a_message = True
        
    root.after(500, check_serial_port_thread)


serial_terminal.grid(column=0, row=10, sticky="ew", columnspan=5)
#################################

def interpret_message(message):
    if message.startswith("CONNNREP"):
        print("Connected")
    

root.after(500, check_serial_port_thread)
print("Done")
root.mainloop()