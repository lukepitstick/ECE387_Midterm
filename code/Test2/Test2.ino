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
const int r1 = 11;
const int r2 = 10;
const int r3 = 9;
const int r4 = 8;

//Constants
const int onTime = 2000;

void setup() {
  pinMode(green, OUTPUT); 
  pinMode(r1, OUTPUT); 
  pinMode(r2, OUTPUT); 
  pinMode(r3, OUTPUT); 
  pinMode(r4, OUTPUT); 
  Serial.begin(9600);                                 // connect to the serial port
}

void loop () {
  byte i = 0;
  byte val = 0;
  byte code[6];
  byte checksum = 0;
  byte bytesread = 0;
  byte tempbyte = 0;

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

      // Output to Serial:
      
      if (bytesread == 12) {         // if 12 digit read is complete
        boolean flag1 = true;
        boolean flag2 = true;
        boolean flag3 = true;
        boolean flag4 = true;
        
        Serial.print("5-byte code: ");
        for (i=0; i<5; i++) {
          if (code[i] < 16) Serial.print("0");
          if (code[i] != ref1[i]) flag1 = false;
          if (code[i] != ref2[i]) flag2 = false;
          if (code[i] != ref3[i]) flag3 = false;
          if (code[i] != ref4[i]) flag4 = false;
          Serial.print(code[i], HEX);
          Serial.print(" ");
        }
        if(flag1){
          Serial.println("Card1: access granted");
          digitalWrite(green,HIGH);
          delay(onTime);
          digitalWrite(green,LOW);
        }
        if(flag2){
          Serial.println("Card2: access Denied");
          digitalWrite(r1,HIGH);
//          digitalWrite(r2,HIGH);
//          digitalWrite(r3,HIGH);
//          digitalWrite(r4,HIGH);
          delay(onTime);
          digitalWrite(r1,LOW);
//          digitalWrite(r2,LOW);
//          digitalWrite(r3,LOW);
//          digitalWrite(r4,LOW);
        }
        if(flag3){
          Serial.println("Card3: access Denied");
          digitalWrite(r1,HIGH);
//          digitalWrite(r2,HIGH);
//          digitalWrite(r3,HIGH);
//          digitalWrite(r4,HIGH);
          delay(onTime);
          digitalWrite(r1,LOW);
//          digitalWrite(r2,LOW);
//          digitalWrite(r3,LOW);
//          digitalWrite(r4,LOW);
        }
        if(flag4){
          Serial.println("Card4: access Denied");
          digitalWrite(r1,HIGH);
//          digitalWrite(r2,HIGH);
//          digitalWrite(r3,HIGH);
//          digitalWrite(r4,HIGH);
          delay(onTime);
          digitalWrite(r1,LOW);
//          digitalWrite(r2,LOW);
//          digitalWrite(r3,LOW);
//          digitalWrite(r4,LOW);
        }
        
        Serial.println();

        Serial.print("Checksum: ");
        Serial.print(code[5], HEX);
        Serial.println(code[5] == checksum ? " -- passed." : " -- error.");
        Serial.println();
      }

      bytesread = 0;
    }
  }
}
