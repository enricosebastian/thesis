from tkinter import *

root = Tk()

message = Label(root, text="hello world")
name = Label(root, text="sebastian")

message.grid(row=0, column=0)
name.grid(row=0,column=1)

root.mainloop()