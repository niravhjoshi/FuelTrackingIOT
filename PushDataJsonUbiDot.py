#import serial
import urllib2
import json
'''
# Serial port connection with baud rate of 9600
try:
    ser = serial.Serial(/dev/ttyACM0,9600,timeout=1)
except:
    ser = serial.Serial(/dev/ttyACM1,9600,timeout=1)
'''
pulses = 0
liters = 581984

APIKEY="YrrR0K4MtS4gdEjXXGfRaNSnsWjCh" # Replace with your APIKEY
DEVICE = "NiravIOTPi@niravjoshi.niravjoshi" # Replace with the id_developer of your devices

class ClientAPIDataPush(object):
    api_url="http://things.ubidots.com/api/v1.6/devices/niravpiflowsensordata"

    def __init__(self,api_key=None,client_type=json):
        self.client_type = client_type
        self.api_key = api_key
        self.content_type = "application/json"
        self.headers = {
                        "X-Auth-Token":self.api_key,
                        "Content-Type": self.content_type
                        }

    def send_data(self,data):
        self.data = json.dumps(data)
        request = urllib2.Request(ClientAPIDataPush.api_url,self.data,self.headers)
        self.response  = urllib2.urlopen(request)
        return self.response



#reated instance  of class ClinetAPIDataPush over here
client_ubidots  = ClientAPIDataPush(APIKEY)


while True:
        # read line received
        #line = ser.readline().strip()
        #if line:
            #print (line)
        #remove and cleaning
            #if line.startswith("Litres"):
            #    piece = line.split(":")
                #if 2 == len(piece):
                    #liters = int(piece[1].strip())
                    print (liters)
                    data = {"Timeslot": "100", "littersread": round(liters, 3)}
                    client_ubidots_response = client_ubidots.send_data(data)
                    print (client_ubidots_response)

