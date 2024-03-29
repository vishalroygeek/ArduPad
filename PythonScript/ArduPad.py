import keyboard
import re
import ctypes
import asyncio
import aioserial
import serial.tools.list_ports

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

def deviceAvailable():
    return any([c.device==serialPortName for c in list(serial.tools.list_ports.comports())])

async def connectAndRead():
    global arduPadPort

    while True:
        if deviceAvailable():
            try:
                if arduPadPort.is_open:
                    data = await arduPadPort.readline_async()
                    sendKeyStroke(re.sub(r'[\t \n \r]+','', data.decode('ascii')))
                elif deviceAvailable():
                    arduPadPort = aioserial.AioSerial(port=serialPortName, baudrate=serialPortBaudRate)
                    print("Connected")
            except:
                arduPadPort.close()
                deviceAvailable()
                if not deviceAvailable():
                    print("Disconnected")
        
if __name__ == "__main__" :
    asyncio.run(connectAndRead())