#pip install playsound
#pip install pyserial

from playsound import playsound
import serial
try:
    ser = serial.Serial(port='COM12',baudrate=9600, dsrdtr = True, timeout=.1)  # open serial port
    print(ser.name)         # check which port was really used
except:
    print("SERIAL ERR")

cmd = ""
while True:
    in_ch = ser.read().decode("utf-8")
    if in_ch != '\n':
        cmd += in_ch
    elif in_ch == '\n':
        print(cmd)
        command_int = int(cmd)
        if command_int ==202:
            playsound('C:/Users/Paolo/Documents/repos/RAC_arena_system/pc_system/audio/pause.wav')
        if command_int ==203:
            playsound('C:/Users/Paolo/Documents/repos/RAC_arena_system/pc_system/audio/cease.wav')
        if command_int >=300:
            playsound('C:/Users/Paolo/Documents/repos/RAC_arena_system/pc_system/audio/'+str(command_int-300)+'.wav')
        cmd = ""
    
