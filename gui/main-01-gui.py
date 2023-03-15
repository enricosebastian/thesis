from tkinter import *
from tkinter import ttk

import serial.tools.list_ports

root = Tk()
root.title("LinDDA Control Panel")

# For port label
ports = serial.tools.list_ports.comports()

select_port_label = Label(root, text="Select an Arduino COM port: ", width=50)
select_port_label.grid(column=0, row=1, sticky="ew")
#######################

# For port drop down list
port_list = ["N/A"]

for port in ports:
    port_list.append(str(port))

selected_port = StringVar(root)
selected_port.set(port_list[0])

port_dropdown_list = ttk.OptionMenu(root, selected_port, *port_list)
port_dropdown_list.config(width=50)
port_dropdown_list.grid(column=1, row=1, sticky="ew")
#################################

# For drone counter
drone_count = 3
drone_counter = Label(root, text="Drones connected: "+str(drone_count))
drone_counter.grid(column=0, row=0, columnspan=3, sticky="ew")

################

# For deploy button
deploy_button = Button(root, text="Deploy drone")
deploy_button.grid(column=0, row=2, sticky="ew")
##################

# For home button
home_button = Button(root, text="Send drone home")
home_button.grid(column=1, row=2, sticky="ew")
##################

# For stop button
stop_button = Button(root, text="Stop drone")
stop_button.grid(column=0, row=3, sticky="ew")
##################

# For where button
location_button = Button(root, text="Ask location of drone")
location_button.grid(column=1, row=3, sticky="ew")
##################

root.mainloop()