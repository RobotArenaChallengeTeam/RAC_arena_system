#pip install playsound
#pip install pyserial

from playsound import playsound
import serial
import serial.tools.list_ports
import tkinter as tk
ports = serial.tools.list_ports.comports()

for port, desc, hwid in sorted(ports):
        print("{}: {} [{}]".format(port, desc, hwid))


ser_fail = True
while ser_fail:
    try:
        port_n = input("PORT NUMBER: ").strip()
        ser = serial.Serial(port='COM'+port_n,baudrate=9600, dsrdtr = True, timeout=.1)  # open serial port
        print(ser.name)
        ser_fail = False
    except serial.SerialException as e:
        print("SERIAL ERR", e)
        ser_fail = True



# Variabile globale che tiene traccia del tempo in secondi
time_sec = 180

def format_time(seconds):
    """Converte i secondi in formato MM:SS."""
    minutes = seconds // 60
    seconds = seconds % 60
    return f"{minutes:02}:{seconds:02}"

def update_timer():
    """Aggiorna il timer ogni 200 ms."""
    global time_sec
    if time_sec >= 0:
        # Aggiorna il testo del label con il timer formattato
        label.config(text=format_time(time_sec))

def write_label(txt):
    label.config(text=format_time(time_sec))
    
    
        
# Creazione della finestra principale
root = tk.Tk()
root.title("Timer Countdown")
root.geometry("400x300")  # Dimensione della finestra

# Impostazione dello sfondo verde
root.config(bg="#00ff00")

# Creazione del label con testo bianco e sfondo trasparente
label = tk.Label(root, text="-", font=("Arial Bold", 100), bg="#00ff00", fg="white")
label.place(relx=0.5, rely=0.5, anchor="center")  # Posizionamento centrale

cmd = ""
audio_path ='E:/repos/RAC_arena_system/pc_system/audio/'
while True:
    in_ch = ser.read().decode("utf-8")
    if in_ch != '\n':
        cmd += in_ch
    elif in_ch == '\n':
        print(cmd)
        command_int = int(cmd)
        if command_int <= 180:
            time_sec = command_int
            update_timer()
        if command_int ==200:
            label.config(text="-")
        if command_int ==202:
            playsound(audio_path+'pause.wav',block =False)
            label.config(text="II" )
        if command_int ==203:
            playsound(audio_path+'cease.wav',block =False)
            label.config(text="END")
        if command_int >=300:
            count_i = command_int-300
            print("playing:" + audio_path+str(count_i)+".wav")
            playsound(audio_path+str(count_i)+".wav",block =False)
            if count_i > 0:
                label.config(text=str(count_i)+'..')
            else:
                label.config(text='GO!')
        cmd = ""
    root.update()
    
            
    
