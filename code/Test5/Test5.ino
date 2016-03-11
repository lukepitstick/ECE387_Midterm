//card 1: 7E 00 1F F6 B4
//card 2: 7E 00 1F D9 32
//card 3: 7E 00 20 3B C6
//card 4: 7E 00 20 3C B0

//card values
byte ref1[] = {0x7E,0x00,0x1F,0xF6,0xB4};
byte ref2[] = {0x7E,0x00,0x1F,0xD9,0x32};
byte ref3[] = {0x7E,0x00,0x20,0x3B,0xC6};
byte ref4[] = {0x7E,0x00,0x20,0x3C,0xB0};

//define pins
const int green = 13;
const int r[] = {11,10,9,8};

//Constants
const int onTime = 1500;
const int postTime = 200;
const int simonLength = 4;
int sequence[simonLength];
int stored[simonLength];

void setup() {
  pinMode(green, OUTPUT); //Set pins
  pinMode(r[0], OUTPUT); 
  pinMode(r[1], OUTPUT); 
  pinMode(r[2], OUTPUT); 
  pinMode(r[3], OUTPUT); 
  Serial.begin(9600);    // connect to the serial port
  randomSeed(analogRead(0));    //Takes analog seed from unconnected pin A0
}

boolean doSimon = true;
boolean setSimon = true;
int pos = 0;
int storedPos = 0;

void loop () {
  byte i = 0;
  byte val = 0;
  byte code[6];
  byte checksum = 0;
  byte bytesread = 0;
  byte tempbyte = 0;
  
  
  //Simon display
  if(doSimon){
    if(setSimon){
      for(int i=0; i<simonLength; i++) {
        sequence[i] =  random(4);
      }
      setSimon = false;
    }
    delay(onTime);
    for(int j=0; j<=pos; j++) {
      digitalWrite(r[sequence[j]],HIGH);
      delay(onTime);
      digitalWrite(r[sequence[j]],LOW);
      delay(postTime);
    } 
    doSimon = false;
  }
  
  if(Serial.available() > 0) {
    if((val = Serial.read()) == 2) {                  // check for header 
      bytesread = 0; 
      while (bytesread < 12) {                        // read 10 digit code + 2 digit checksum
        if( Serial.available() > 0) { 
          val = Serial.read();
          if((val == 0x0D)||(val == 0x0A)||(val == 0x03)||(val == 0x02)) { // if header or stop bytes before the 10 digit reading 
            break;                                    // stop reading
          }

          // Do Ascii/Hex conversion:
          if ((val >= '0') && (val <= '9')) {
            val = val - '0';
          } else if ((val >= 'A') && (val <= 'F')) {
            val = 10 + val - 'A';
          }

          // Every two hex-digits, add byte to code:
          if (bytesread & 1 == 1) {
            // make some space for this hex-digit by
            // shifting the previous hex-digit with 4 bits to the left:
            code[bytesread >> 1] = (val | (tempbyte << 4));

            if (bytesread >> 1 != 5) {                // If we're at the checksum byte,
              checksum ^= code[bytesread >> 1];       // Calculate the checksum... (XOR)
            };
          } else {
            tempbyte = val;                           // Store the first hex digit first...
          };

          bytesread++;                                // ready to read next digit
        } 
      } 

      
      if (bytesread == 12) {         // if 12 digit read is complete
        boolean flag[] = {true,true,true,true};
        
        doSimon = true;
        
//        Serial.print("5-byte code: ");
        //Compare read code to stored cards
        for (i=0; i<5; i++) {
//          if (code[i] < 16) Serial.print("0");
          if (code[i] != ref1[i]) flag[0] = false;
          if (code[i] != ref2[i]) flag[1] = false;
          if (code[i] != ref3[i]) flag[2] = false;
          if (code[i] != ref4[i]) flag[3] = false;
//          Serial.print(code[i], HEX);
        }
        
        //Output for each card
        if(flag[0]){
          processInput(0);
        }
        if(flag[1]){
          processInput(1);
        }
        if(flag[2]){
          processInput(2);
        }
        if(flag[3]){
          processInput(3);
        }
      }
      bytesread = 0;
    }
  }
}

void processInput(int n){
//  Serial.println(pos,DEC);
//  Serial.println(storedPos,DEC);
//  Serial.print(stored[0],DEC);
//  Serial.print(stored[1],DEC);
//  Serial.print(stored[2],DEC);
//  Serial.println(stored[3],DEC);
//  Serial.print(sequence[0],DEC);
//  Serial.print(sequence[1],DEC);
//  Serial.print(sequence[2],DEC);
//  Serial.println(sequence[3],DEC);
  //add input to entered cards
  stored[storedPos] = n;
  
  //if enough cards to match current sequence
  if(storedPos == pos) {
    boolean tempFlag = true;
    for(int k=0; k<=pos; k++) {
      if(stored[k] != sequence[k]){ //bad match
        tempFlag = false;
        digitalWrite(r[0],HIGH);
        digitalWrite(r[1],HIGH);
        digitalWrite(r[2],HIGH);
        digitalWrite(r[3],HIGH);
        delay(onTime);
        digitalWrite(r[0],LOW);
        digitalWrite(r[1],LOW);
        digitalWrite(r[2],LOW);
        digitalWrite(r[3],LOW);
        delay(onTime);
        
        doSimon = true;
        setSimon = true;
        pos = 0;
        storedPos = 0;
      } 
    }
    
    if(tempFlag){ //matches current pattern
      doSimon = true;
      if(pos == simonLength-1) {//WIN
        digitalWrite(green,HIGH);
        delay(onTime);
        digitalWrite(green,LOW);
        delay(onTime);
        setSimon = true;
        doSimon = true;
        pos = 0;
        storedPos = 0;
      }
      else{
        storedPos = 0;
        pos = pos+1; 
      }
     
    }
  }
  else{
    doSimon = false;
    storedPos = storedPos+1;
  }
}
