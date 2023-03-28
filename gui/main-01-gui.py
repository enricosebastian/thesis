from tkinter import *
from threading import *
from tkinter import ttk
from datetime import datetime

import serial.tools.list_ports

root = Tk()
root.title("LinDDA Control Panel")

# Drone counter
drone_count = 0
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

selected_drone = "0"

def set_drone(drone_name):
    global selected_drone
    selected_drone = drone_name
    print("Selected drone is:",selected_drone)
    if selected_drone == "ALL":
        x_max_input_label.config(state="disabled")
        x_min_input_label.config(state="disabled")
        
    else:
        x_max_input_label.config(state="enabled")
        x_min_input_label.config(state="enabled") 
    
drones = ["N/A"]
found_drones = ["ALL"]

drone_list = StringVar(root)

for found_drone in found_drones:
    drones.append(str(found_drone))

select_drone_dropdown_list = ttk.OptionMenu(root, drone_list, drones[0], *drones, command=set_drone)
select_drone_dropdown_list.config(width=50)
select_drone_dropdown_list.grid(column=3, row=3, sticky="ew", columnspan=3)

def refresh_drone_list():
    select_drone_dropdown_list = ttk.OptionMenu(root, drone_list, drones[0], *drones, command=set_drone)
    select_drone_dropdown_list.config(width=50)
    select_drone_dropdown_list.grid(column=3, row=3, sticky="ew", columnspan=3)
    
    drone_counter = Label(root, text="Drones connected: "+str(len(drones)-2), pady=10)
    drone_counter.grid(column=0, row=1, columnspan=6, sticky="ew")

#################################

# For deploy button
def start_deployment():
    print("Deploying", selected_drone)
    deployment_command = "DEPL ALL DETA\n"
    serial_instance.write(deployment_command.encode("utf-8"))
    
    
deploy_button = Button(root, text="Start deployment", command=start_deployment)
deploy_button.grid(column=0, row=4, sticky="ew", padx=5, pady=5)
##################

# For go button
def start_moving():
    print("Moving", selected_drone)
    move_command = "GO " + selected_drone + " DETA\n"
    serial_instance.write(move_command.encode("utf-8"))
    
move_button = Button(root, text="Start moving", command=start_moving)
move_button.grid(column=1, row=4, sticky="ew", padx=5, pady=5)
##################

# For stop button
def stop_moving():
    print("Stopping", selected_drone)
    stop_command = "STOP " + selected_drone + " DETA\n"
    serial_instance.write(stop_command.encode("utf-8"))
    
stop_button = Button(root, text="Stop moving", command=stop_moving)
stop_button.grid(column=2, row=4, sticky="ew", padx=5, pady=5)
##################

# For where button
def ask_drone_location():
    print("Asking for current location of", selected_drone)
    where_command = "WHER " + selected_drone + " CURR\n"
    serial_instance.write(where_command.encode("utf-8"))
    
location_button = Button(root, text="Ask for location", command=ask_drone_location)
location_button.grid(column=3, row=4, sticky="ew", padx=5, pady=5)
##################

# For home button
def go_home():
    print("Sending", selected_drone, "home")
    home_command = "HOME " + selected_drone + " DETA\n"
    print(home_command)
    serial_instance.write(home_command.encode("utf-8"))
    
home_button = Button(root, text="Go home", command=go_home)
home_button.grid(column=4, row=4, sticky="ew", padx=5, pady=5)
##################

# For cancel command button
def cancel_command():
    serial_instance.write("c".encode("utf-8"))
    
cancel_command_button = Button(root, text="Cancel command", command=cancel_command)
cancel_command_button.grid(column=2, row=5, sticky="ew", padx=5, pady=5)
##################

# For region of responsibility label
select_drone_label = Label(root, text="Deployment area", width=50, pady=10)
select_drone_label.grid(column=0, row=6, sticky="ew", columnspan=5)
#######################

# Maximum water body distance
x_sub_label = Label(root, text="Maximum water body width", width=50)
x_sub_label.grid(column=0, row=7, sticky="ew", columnspan=2)

x_sub_input_label = ttk.Entry()
x_sub_input_label.insert(0, "0.0")
x_sub_input_label.grid(column=3, row=7, sticky="ew", columnspan=2)

########################

# For y_max
y_max_label = Label(root, text="Y max", width=50)
y_max_label.grid(column=0, row=8, sticky="ew", columnspan=2)

y_max_input_label = ttk.Entry()
y_max_input_label.insert(0, "0.0")
y_max_input_label.grid(column=3, row=8, sticky="ew", columnspan=2)
#################################

# For y_min
y_min_label = Label(root, text="Y min", width=50)
y_min_label.grid(column=0, row=9, sticky="ew", columnspan=2)

y_min_input_label = ttk.Entry()
y_min_input_label.insert(0, "0.0")
y_min_input_label.grid(column=3, row=9, sticky="ew", columnspan=2)
#################################

# For x_max
x_max_label = Label(root, text="X max", width=50)
x_max_label.grid(column=0, row=10, sticky="ew", columnspan=2)

x_max_input_label = ttk.Entry()
x_max_input_label.insert(0, "0.0")
x_max_input_label.config(state="disabled")
x_max_input_label.grid(column=3, row=10, sticky="ew", columnspan=2)
#################################

# For x_min
x_min_label = Label(root, text="X min", width=50)
x_min_label.grid(column=0, row=11, sticky="ew", columnspan=2)

x_min_input_label = ttk.Entry()
x_min_input_label.insert(0, "0.0")
x_min_input_label.config(state="disabled")
x_min_input_label.grid(column=3, row=11, sticky="ew", columnspan=2)
#################################

# For anchor distance
anchor_distance_label = Label(root, text="Anchor distance", width=50)
anchor_distance_label.grid(column=0, row=12, sticky="ew", columnspan=2)

anchor_distance_input_label = ttk.Entry()
anchor_distance_input_label.insert(0, "0.0")
anchor_distance_input_label.grid(column=3, row=12, sticky="ew", columnspan=2)
#################################

# For serial terminal label
serial_terminal_label = Label(root, text="Serial terminal", width=50, pady=5)
serial_terminal_label.grid(column=0, row=13, sticky="ew", columnspan=5)

# For serial log terminal
def interpret_message(message):
    message_arr = str(message).split(' ')
    print(message_arr)
    if message_arr[0] == "b'Received:":
        if len(message_arr) < 3:
            command = message_arr[1]
        if len(message_arr) < 4:
            command = message_arr[1]
            to_name = message_arr[2]
        if len(message_arr) < 5:
            command = message_arr[1]
            to_name = message_arr[2]
            from_name = message_arr[3]
        if len(message_arr) < 6:
            command = message_arr[1]
            to_name = message_arr[2]
            from_name = message_arr[3]
            details = message_arr[4]
        
        if command == "CONN":    
            if from_name == "DRO1" or from_name == "DRO2" or from_name == "DRO3":
                if from_name not in drones:
                    drones.append(from_name)
                    print(drones)
                    refresh_drone_list()
                else:
                    print(from_name,"is already connected!")

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
                interpret_message(message)
                serial_terminal.insert("1.0", message)
                serial_terminal.insert("1.0", current_time)
        else:
            received_a_message = True
        
    root.after(500, check_serial_port_thread)


serial_terminal.grid(column=0, row=14, sticky="ew", columnspan=5)
#################################
    

root.after(500, check_serial_port_thread)
print("Done")
root.mainloop()