import os

path = "./images/plastic_bottle/sample/"

for count, filename in enumerate(os.listdir(path)):
    print(filename,"is being renamed")
    os.rename(path+filename, path+"test_"+str(count)+".jpg")