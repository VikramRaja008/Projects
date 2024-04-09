
//---------------------------------------------------------ESP HEADER FILES------------------------------------------//

#include <WiFi.h>          

//---------------------------------------------------------ESP HEADER FILES------------------------------------------//

                                         
//--------------------------------------------------------FIRE BASE HEADER FILES-----------------------------------------------//

#include <FirebaseESP32.h>
#define FIREBASE_HOST "new-88204-default-rtdb.asia-southeast1.firebasedatabase.app"  //Do not include https:// in FIREBASE_HOST
#define FIREBASE_AUTH "xBB12pnx2kov4Ykp6wX996vEjxc7XTOPxgbORQOe"               // project secret API key
#define API_KEY "AIzaSyB7-9jEOubCbZeysc5K_znybUMkLNA-ctA"
#define DATABASE_URL "REPLACE_WITH_YOUR_FIREBASE_DATABASE_URL" 
FirebaseData stream,stream1,stream2,stream3;
FirebaseData firebaseData,firebaseData1;
FirebaseAuth auth;
FirebaseData fbdo1;
FirebaseData fbdo2;
FirebaseConfig config;
bool signupOK = false;
bool streamTO = true;
String parentPath = "/Test/Stream";
String childPath[4] = {"/relay","/relay1","/relay2","/relay3"};
size_t childPathSize = 4;
  
#define DEBUG_SW 0
//--------------------------------------------------------FIRE BASE HEADER FILES-----------------------------------------------//


//-----------------------------------------------------BLE HEADER FILE---------------------------------------------------------//


#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>

// See the following for generating UUIDs:
// https://www.uuidgenerator.net/

#define SERVICE_UUID        "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"
String l;

//-----------------------------------------------------BLE HEADER FILE---------------------------------------------------------//


//-------------------------------------------------------EXTRA HEADER FILE----------------------------------------------------//
#include <ezButton.h>
#include <EEPROM.h>
#define EEPROM_SIZE 64
//--------------------------------------------------------EXTRA HEADER FILE---------------------------------------------------//




class MyCallbacks: public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic *pCharacteristic) {
      std::string value = pCharacteristic->getValue();
l=value.c_str();
Serial.println(l);
    }
};



/// constants won't change
const int BUTTON_PIN = 0; // the number of the pushbutton pin
const int LED_PIN    = 3;
const int RELAY1    = 16;
const int RELAY2    = 17;
const int RELAY3    = 18;
const int RELAY4    = 19;

int relay[10]={16,17,18,19};
String R1s="OFF";
String R2s="OFF";
String R3s="OFF";
String R4s="OFF";

// the number of the LED pin
String strs[20];
int StringCount = 0;
String deviceucode="14";
ezButton button(BUTTON_PIN);  // 



String phoneid;
String ssid="users/"+phoneid+"/devices/"+deviceucode+"/wifi/SSID";
String password="users/"+phoneid+"/devices/"+deviceucode+"/wifi/PASSWORD";
String Relay1path="users/"+phoneid+"/devices/"+deviceucode+"/relay";
String Relay2path="users/"+phoneid+"/devices/"+deviceucode+"/relay1";
String Relay3path="users/"+phoneid+"/devices/"+deviceucode+"/relay2";
String Relay4path="users/"+phoneid+"/devices/"+deviceucode+"/relay3";
String Relay11path="users/"+phoneid+"/devices/"+deviceucode;


int ledState = LOW;   // the current state of LED




//========================================================================EEPROM READ AND WRITE===========================================//

int writeStringToEEPROM(int addrOffset, const String &strToWrite)
{
  byte len = strToWrite.length();
  EEPROM.write(addrOffset, len);
  for (int i = 0; i < len; i++)
  {
    EEPROM.write(addrOffset + 1 + i, strToWrite[i]);
  }
  return addrOffset + 1 + len;
}
int readStringFromEEPROM(int addrOffset, String *strToRead)
{
  int newStrLen = EEPROM.read(addrOffset);
  char data[newStrLen + 1];
  for (int i = 0; i < newStrLen; i++)
  {
    data[i] = EEPROM.read(addrOffset + 1 + i);
  }
  data[newStrLen] = '\0'; // !!! NOTE !!! Remove the space between the slash "/" and "0" (I've added a space because otherwise there is a display bug)
  *strToRead = String(data);
  return addrOffset + 1 + newStrLen;
}

//========================================================================EEPROM READ AND WRITE===========================================//



//========================================================================BLUETOOTH MODE=================================================//

void bluetoothmode()
{
  
 Serial.println("1- Download and install an BLE scanner app in your phone");
  Serial.println("2- Scan for BLE devices in the app");
  Serial.println("3- Connect to MyESP32");
  Serial.println("4- Go to CUSTOM CHARACTERISTIC in CUSTOM SERVICE and write something");
  Serial.println("5- See the magic =)");

  BLEDevice::init("MyESP32");
  BLEServer *pServer = BLEDevice::createServer();

  BLEService *pService = pServer->createService(SERVICE_UUID);

  BLECharacteristic *pCharacteristic = pService->createCharacteristic(
                                         CHARACTERISTIC_UUID,
                                         BLECharacteristic::PROPERTY_READ |
                                         BLECharacteristic::PROPERTY_WRITE
                                       );

  pCharacteristic->setCallbacks(new MyCallbacks());

  pCharacteristic->setValue("Hello World");
  pService->start();

  BLEAdvertising *pAdvertising = pServer->getAdvertising();
  pAdvertising->start();

     while(1)
      { 
        
        String i = l;
        Serial.println(i);
         

        if(i!= NULL)
           {

while (i.length() > 0)
  {
    int index = i.indexOf('§');
    if (index == -1) // No space found
    {
      strs[StringCount++] = i;
      break;
    }
    else
    {
      strs[StringCount++] = i.substring(0, index);
      i = i.substring(index+1);
    }
  }
  int k=strs[1].length()-1;
  int l=strs[3].length()-1;
  strs[1][k]='\0';
  strs[3][l]='\0';
  Serial.println(" "+strs[1]+" "+strs[3]);

    int eepromOffset = 0;
  // Writing
  String str1 = strs[1];
  String str2 = strs[3];
  String str3 = strs[5];
  phoneid=str3;
  int str1AddrOffset = writeStringToEEPROM(eepromOffset, str1);
  int str2AddrOffset = writeStringToEEPROM(str1AddrOffset, str2);
  writeStringToEEPROM(str2AddrOffset, str3);
  EEPROM.commit();

  WiFi.begin(strs[1].c_str(), strs[3].c_str());                                      //try to connect with wifi
Serial.print("Connecting to ");
Serial.print("wifi");
int c=0;
while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(800);
    c++;
    if(c==7)
    {
      break;
    }
  }
  
Serial.println();
Serial.print("Connected to ");
Serial.println("wifi");
Serial.print("IP Address is : ");
Serial.println(WiFi.localIP());   
Relay1path="users/"+str3+"/devices/"+deviceucode+"/relay";
Relay2path="users/"+str3+"/devices/"+deviceucode+"/relay1";
Relay3path="users/"+str3+"/devices/"+deviceucode+"/relay2";
Relay4path="users/"+str3+"/devices/"+deviceucode+"/relay3";
Serial.println(str3);

              Serial.println("bt ended");

              
              ESP.restart();
              break;
           }
        else
            Serial.println("recieving input");

 
       }
}
//========================================================================BLUETOOTH MODE=================================================//

void streamCallback(MultiPathStreamData stream)
{
  //if stream timeout status is true, set it to false and return (ignore only the first stream since the stream is first established or reconnected)
  if (streamTO)
  {
    streamTO = false;
    return;
  }
  
  Serial.println();
  Serial.println("Stream Data1 available...");

  size_t numChild = sizeof(childPath)/sizeof(childPath[0]);

  for(size_t i = 0;i< numChild;i++)
  {
    if (stream.get(childPath[i]))
    {
      Serial.println("path: " + stream.dataPath + ", type: " + stream.type + ", value: " + stream.value);
      if(stream.value=="ON")
{
digitalWrite(relay[i],1);
}
else
{
digitalWrite(relay[i],0);
}
  
    }
  }

  Serial.println();
  
}

void streamTimeoutCallback(bool timeout)
{
  //store stream timeout status
  streamTO = timeout;
  if (timeout)
  {
    Serial.println();
    Serial.println("Stream timeout, resume streaming...");
    Serial.println();
  }
}


void setup() {

String phoneid;


//========================================================================EEPROM INITIAL READ===========================================//

  if (!EEPROM.begin(EEPROM_SIZE)) {
        Serial.println("failed to init EEPROM");
        delay(1000000);
    }
      int eepromOffset = 0;
  // Writing
  // Reading
  String newStr1;
  String newStr2;
  String newStr3;

  int newStr1AddrOffset = readStringFromEEPROM(eepromOffset, &newStr1);
  int newStr2AddrOffset = readStringFromEEPROM(newStr1AddrOffset, &newStr2);
readStringFromEEPROM(newStr2AddrOffset, &newStr3);
char Buf[50];
    newStr3.toCharArray(Buf, 29);
  
  Serial.begin(9600); 
       
  Serial.println(newStr1);
  Serial.println(Buf);
  Serial.println(newStr2);

if(newStr1==NULL)
{
  bluetoothmode();
}
//========================================================================EEPROM INITIAL READ===========================================//
    

phoneid=Buf;
ssid="users/"+phoneid+"/devices/"+deviceucode+"/wifi/SSID";
password="users/"+phoneid+"/devices/"+deviceucode+"/wifi/PASSWORD";
Relay1path="users/"+phoneid+"/devices/"+deviceucode+"/relay";
Relay2path="users/"+phoneid+"/devices/"+deviceucode+"/relay1";
Relay3path="users/"+phoneid+"/devices/"+deviceucode+"/relay2";
Relay4path="users/"+phoneid+"/devices/"+deviceucode+"/relay3";
Relay11path="users/"+phoneid+"/devices/"+deviceucode;

  
       // initialize serial
  pinMode(LED_PIN, OUTPUT);   // set arduino pin to output mode
  button.setDebounceTime(50); // set debounce time to 50 milliseconds

pinMode(RELAY1, OUTPUT); 
pinMode(RELAY2, OUTPUT); 
pinMode(RELAY3, OUTPUT); 
pinMode(RELAY4, OUTPUT); 


  
  // //Bluetooth device name
  
WiFi.begin(newStr1.c_str(), newStr2.c_str());                                      //try to connect with wifi
Serial.print("Connecting to ");
Serial.print("wifi");
int c=0;
while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(800);
    c++;
    if(c==7)
    {
      break;
    }
  }
Serial.println();
Serial.print("Connected to ");
Serial.println("wifi");
Serial.print("IP Address is : ");
Serial.println(WiFi.localIP());                                                      //print local IP address
config.host = FIREBASE_HOST;
config.api_key = API_KEY;

if (Firebase.signUp(&config, &auth, "", "")){
    Serial.println("ok");
    signupOK = true;
  }
  else{
    Serial.printf("%s\n", config.signer.signupError.message.c_str());
  }
Firebase.begin(&config, &auth);    
Firebase.reconnectWiFi(true);

  if (!Firebase.beginMultiPathStream(fbdo1,Relay11path.c_str(), childPath, childPathSize))
  {
    Serial.println("------------------------------------");
    Serial.println("Can't begin stream connection...");
    Serial.println("REASON: " + fbdo1.errorReason());
    Serial.println("------------------------------------");
    Serial.println();
  }

  //Set the reserved size of stack memory in bytes for internal stream callback processing RTOS task.
  //8192 is the minimum size.
  Firebase.setMultiPathStreamCallback(fbdo1, streamCallback, streamTimeoutCallback, 8192);
    
Firebase.getString(firebaseData,Relay1path.c_str());
if(firebaseData.stringData()==NULL)
{
Firebase.setString(firebaseData,Relay1path.c_str(),"OFF");
Firebase.setString(firebaseData,Relay2path.c_str(),"OFF");
Firebase.setString(firebaseData,Relay3path.c_str(),"OFF");
Firebase.setString(firebaseData,Relay4path.c_str(),"OFF");
}

}


//===================================================================================RELAY CONTROL OPERATION==================================================//
void rcontrolop()
{
 if (Firebase.ready())
  {
    if (stream.streamAvailable())
    {

      Serial.printf("sream path, %s\nevent path, %s\ndata type, %s\nevent type, %s\nvalue, %s\n\n",
                    stream.streamPath().c_str(),
                    stream.dataPath().c_str(),
                    stream.dataType().c_str(),
                    stream.eventType().c_str(),
                    stream.stringData());
/*
/      
      if (Firebase.RTDB.getString(&firebaseData, Relay1path.c_str())) 
 {
  Serial.print("Relay1 - ");
 Serial.println(firebaseData.stringData());
if(firebaseData.stringData()=="ON")
{
digitalWrite(RELAY1,1);

}
else
{
digitalWrite(RELAY1,0);
}
  }

 if (Firebase.RTDB.getString(&firebaseData, Relay2path.c_str())) {
 Serial.print("Relay2 - ");
 Serial.println(firebaseData.stringData());
if(firebaseData.stringData()=="ON")
{
digitalWrite(RELAY2,1);
}
else
{
digitalWrite(RELAY2,0);
}
  }


 if (Firebase.RTDB.getString(&firebaseData, Relay3path.c_str())) {
Serial.print("Relay3 - ");
  Serial.println(firebaseData.stringData());
if(firebaseData.stringData()=="ON")
{
digitalWrite(RELAY3,1);

}
else
{
digitalWrite(RELAY3,0);
}
  }


 if (Firebase.RTDB.getString(&firebaseData, Relay4path.c_str())) {
  Serial.print("Relay4 - ");
  Serial.println(firebaseData.stringData());
if(firebaseData.stringData()=="ON")
{
digitalWrite(RELAY4,1);

}
else
{
digitalWrite(RELAY4,0);
}
  }
  */
    }


  }


 
}
 

//===================================================================================RELAY CONTROL OPERATION==================================================//


void loop() {

 // Serial.print("LOOP");
 // delay(800);
button.loop(); // MUST call the loop() function first

  if(button.isPressed()) {


    // toggle state of LED
    ledState = !ledState;

    // control LED arccoding to the toggleed sate
    Serial.println(ledState);
    if(ledState==1)
   {
    for (int i = 0; i < 512; i++) {
 EEPROM.write(i, 0);
 }
EEPROM.commit();
delay(500);
ESP.restart();
    }
  }
  //  Firebase.setString(switch_1, "hello world222");
rcontrolop();
}
