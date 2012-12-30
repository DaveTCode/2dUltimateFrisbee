import os

for root, dirs, files in os.walk("c:\\frisbee_game\\"):
    for file in files:
        if file.endswith(".c") or file.endswith(".h"):
            os.system("dos2unix " + os.path.join(root, file))