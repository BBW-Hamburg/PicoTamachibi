import machine
import sdcard
import uos
import json

mytestdata = {
                "health":11,
                "happiness":1,
                "energy":1
            }

# Create a file and write something to it
#with open("/sd/test01.txt", "w") as file:
#    file.write(json.dumps(mytestdata))

# Open the file we just created and read from it
#with open("/sd/test01.txt", "r") as file:
#    data = file.read()
#    print(data)
#    varx =json.loads(data)
#    print(varx["health"])
    
class SaveGameManager:
    __SaveGameData = {}
    __FileSocket = None
    __cs = None
    __spi = None
    __sd = None
    
    def __init__(self, savename="01"):
        print("initializing")
        try:
            # Assign chip select (CS) pin (and start it high)
            self.__cs = machine.Pin(9, machine.Pin.OUT)

            # Intialize SPI peripheral (start with 1 MHz)
            self.__spi = machine.SPI(1,
                              baudrate=1000000,
                              polarity=0,
                              phase=0,
                              bits=8,
                              firstbit=machine.SPI.MSB,
                              sck=machine.Pin(10),
                              mosi=machine.Pin(11),
                              miso=machine.Pin(8))

            # Initialize SD card
            self.__sd = sdcard.SDCard(self.__spi, self.__cs)

            # Mount filesystem
            self.__FileSocket = uos.VfsFat(self.__sd)
            uos.mount(self.__FileSocket, "/sd")
            
            #load values from FS
            self.LoadSaveData()
        except:
            pass

        
            
    @property
    def CheckFilesystemReady(self):
        if self.__FileSocket is None:
            print("No FileSocket")
            return False
        return True    

    def LoadSaveData(self):
        print("loading")
        # Open the file we just created and read from it
        if not self.CheckFilesystemReady:
            return # If we have no filesystem we cannot write or read anyways...
        
        with open("/sd/test00.txt", "rb") as file:
            file.read(1)	#ignore first byte, it containes the version number as binary... later :)
            data = file.read()
            self.__SaveGameData = json.loads(data)
        
    def WriteSaveData(self):
        if not self.CheckFilesystemReady:
            pass # If we have no filesystem we cannot write or read anyways...
        # Create a file and write something to it
        with open("/sd/test01.txt", "wb") as file:
            file.write("0")
            file.write(json.dumps(self.__SaveGameData))
            
    def UpdateData(self, key, value):
        print("updating internal data")
        self.__SaveGameData[key]=value
        
    def GetData(self, key, defaultvalue=""):
        print("getting internaldata")
        if key in self.__SaveGameData:
            return self.__SaveGameData 
        return defaultvalue
    
    
mysavegame = SaveGameManager()
mysavegame.UpdateData("bla","blub")
mysavegame.WriteSaveData()
mysavegame.LoadSaveData()
print(mysavegame.GetData("bla"))
print("x:"+mysavegame.GetData("something"))
print("x:"+mysavegame.GetData("something","defaultthing"))