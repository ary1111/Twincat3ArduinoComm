/* RS485 AMT21 Communication
 * v 2.0, stable, 1/8/2019
 * Description: This script is for communicating with an encoder over RS485 (AMT 213A-V by CUI Inc. is used in this example)
 *              Once uploaded, the output from Serial1 can be read by C++ through Serial using the C++ library. 
 *              For debugging,
 *                1) Upload to Arduino Mega
 *                2) Open Serial Monitor (CTRL+Shift+M)
 *                3) Set baud rate to 115200
 *                4) Enter 'T' (or the respective command char)
 *                5) The encoder reading should be output
 */

#define RX        19  //For Arduino Mega
#define TX        18

//#define RxTx 3
#define Re    3
#define De    5

#define Transmit    HIGH
#define Receive     LOW

#define Pin13LED         13

uint8_t byteIn;
uint8_t byteLow;
uint8_t byteHigh;
int byteOut;

uint16_t currentPOS = 0xFFFF;
bool binaryArray[16];

void setup()   /****** SETUP: RUNS ONCE ******/
{
  Serial.begin(115200);           // Set the data rate to be recognized by the C++ script
  
  pinMode(Pin13LED, OUTPUT);   
  pinMode(Re, OUTPUT);
  pinMode(De, OUTPUT);    
  
  //digitalWrite(RxTx, Receive);  //Initialize transciever to receive data
  RS485Receive();  

  Serial1.begin(2000000);         // Set the data rate for the encoder

}//--(end setup )---


void loop()   /****** LOOP: RUNS CONSTANTLY ******/
{
  digitalWrite(Pin13LED, HIGH);       //On when not transmitting
  if (Serial.available())
  {
    byteOut = Serial.read();          //Locally store the transmitted string
    
    RS485Transmit(); 
    Serial1.write(byteOut);           // Send byte to encoder
    
    digitalWrite(Pin13LED, LOW);      // Off momentarily 
    delay(10);
    RS485Receive();
    delay(10);  
  }

  if (Serial1.available())                //Look for data from encoder
   {
    /*Debugging functions*/   
    //Serial.println(Serial1.available());
    //Serial.println("Received");
    
    digitalWrite(Pin13LED, LOW);          // Off momentarily
    byteIn = Serial1.read();              // Read received byte. Not sure what this signifies
    byteLow = Serial1.read();             // Read received byte. Low Byte
    byteHigh = Serial1.read();            // Read received byte. High Byte

    /*Debugging functions*/
    //Serial.println(byteIn);
    //Serial.println(byteLow);
    //Serial.println(byteHigh);

    currentPOS = byteLow + (byteHigh << 8);
    for(int i=0; i<16; i++)
    {
      binaryArray[i] = (0x01) & (currentPOS >> (i));
    }

    if((binaryArray[15] == !(binaryArray[13] ^ binaryArray[11] ^ binaryArray[9] ^ binaryArray[7] ^ binaryArray[5] ^ binaryArray[3] ^ binaryArray[1]))
          && (binaryArray[14] == !(binaryArray[12] ^ binaryArray[10] ^ binaryArray[8] ^ binaryArray[6] ^ binaryArray[4] ^ binaryArray[2] ^ binaryArray[0])))
    {
            currentPOS &= 0x3FFF;
    }
    else
    {
      currentPOS = 0xFFFF; //Bad Pos
    }
          
    Serial.print(currentPOS);
    delay(10);   
   }     
}

void RS485Transmit()
{
  digitalWrite(Re, LOW);   
  digitalWrite(De, HIGH);   
}

void RS485Receive()
{
  digitalWrite(Re, HIGH);   
  digitalWrite(De, LOW);  
}

