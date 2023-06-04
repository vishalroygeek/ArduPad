import serial
import keyboard
import re
import ctypes

#Declaring constants and variables
arduPadPort = None
serialPortName = "COM4"
serialPortBaudRate = 9600
serialCommandKeyStrokeMap = {'1':'previous track',
                             '2':'play/pause media',
                             '3':'next track',
                             '4':'left windows + period',
                             '5':'left windows + left shift + s',
                             '6':'left windows + left alt + k',
                             '7':'left windows + tab',
                             '8':'left ctrl + c',
                             '9':'left ctrl + v',
                             '10':'volume mute'}

def sendKeyStroke(command):    
    if "RE" not in command:
        if(command=='0'):
            ctypes.windll.user32.LockWorkStation()
        else:   
            keyboard.press_and_release(serialCommandKeyStrokeMap[command])
    else:
        reValue = int(command.replace("RE:",""))
        if reValue > 0:
            ['' for _ in [keyboard.press_and_release("volume up")]*abs(reValue)]
        else:
            ['' for _ in [keyboard.press_and_release("volume down")]*abs(reValue)]

if __name__ == "__main__" :
    #Actively listening for incoming data from the port & handling disconnections
    while True:
        try:
            if arduPadPort is None:
                arduPadPort = serial.Serial(port=serialPortName, baudrate=serialPortBaudRate)
            elif arduPadPort.in_waiting > 0:
                sendKeyStroke(re.sub(r'[\t \n \r]+', '', arduPadPort.readline().decode('ascii')))
        except:
            arduPadPort = None
            pass