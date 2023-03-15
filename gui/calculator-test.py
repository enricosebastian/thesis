from tkinter import *

root = Tk()
root.title("calculator test")

input_screen = Entry(root, width=50)

# global variable
answer = 0
current_number_as_text = ""

def insert_number_to_input_screen(input_text):
    global answer
    global current_number_as_text
    
    input_screen.insert(END, input_text)
    if(input_text == '='):
        answer += int(current_number_as_text)
        input_screen.insert(END, answer)
    elif(input_text == '+'):
        answer += int(current_number_as_text)
        current_number_as_text = ""
    else:
        current_number_as_text += str(input_text)

button_1 = Button(root, text="1", padx=40, pady=20, command=lambda: insert_number_to_input_screen(button_1.cget("text")))
button_2 = Button(root, text="2", padx=40, pady=20, command=lambda: insert_number_to_input_screen(button_2.cget("text")))
button_3 = Button(root, text="3", padx=40, pady=20, command=lambda: insert_number_to_input_screen(button_3.cget("text")))
button_4 = Button(root, text="4", padx=40, pady=20, command=lambda: insert_number_to_input_screen(button_4.cget("text")))
button_5 = Button(root, text="5", padx=40, pady=20, command=lambda: insert_number_to_input_screen(button_5.cget("text")))
button_6 = Button(root, text="6", padx=40, pady=20, command=lambda: insert_number_to_input_screen(button_6.cget("text")))

button_plus = Button(root, text="+", padx=80, pady=20, command=lambda: insert_number_to_input_screen(button_plus.cget("text")))
button_equals = Button(root, text="=", padx=40, pady=20, command=lambda: insert_number_to_input_screen(button_equals.cget("text")))

input_screen.grid(row=0, column=0, columnspan=3)

button_1.grid(row=1, column=0)
button_2.grid(row=1, column=1)
button_3.grid(row=1, column=2)
button_4.grid(row=2, column=0)
button_5.grid(row=2, column=1)
button_6.grid(row=2, column=2)

button_plus.grid(row=3, column=0, columnspan=2)
button_equals.grid(row=3, column=2)






root.mainloop()