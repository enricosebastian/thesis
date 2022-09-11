f = open("test.txt","w+")
f.write("yeet\n")
f.close()

f = open("test.txt", "r")
if f.mode == "r":
    contents = f.readline().strip("\n")
    print(contents)
    if contents == "yeet":
        print("Nice")
