
/*  Készí­tette: Takács Tamás 2020.04.02 19:21
   A feladat témája: A MOTOR m?ködtetése soros port(SPI) és pwm jel segítségével
   
   Program indá­tásakor: (ezeket í­rjuk be)

           - MOTOR: - Pwm = 03
                    - Dir = 08
           - Csúszka: - Analóg jel bemenet - A0 (10 bit)

		   - SPISLV: - SS*(Slave Select) = 10
                                       
   Program indá­tása utáni parancs lehet?ségek: (SPI TX)                         
           - cc - MOTOR jobbra forog.
           - dd - MOTOR balra forog.
           - aa - MOTOR megkapja pwm jelet.
           - bb - MOTOR leáll.
 
   FONTOS!!
	    - START* beírása el?tt a csúszkát állítani kell.
		- Ez fogja befolyásolni a motor sebességét.
		- Az utasításokat a Slave Transmitjába kell írni, a Master(Arduino) csak olvas.
 	
 
 */

#include <SPI.h>

const int SlaveSelectPin = 10;

void setup()
{
	pinMode(SlaveSelectPin, OUTPUT); //Master errol a pinrol olvas.
	pinMode(8,OUTPUT); //Motor irányváltozója.
	pinMode(3,OUTPUT); //Motor itt kap pwm jelet.
	pinMode(A0,INPUT); //Analóg bemenet.
	SPI.begin(): //SPI busz inicializálás
	Serial.begin(9600); //Soros busz inicializálás
	Serial.println("----------------------------------------------------------------");
	Serial.println("Kérlek adj be hexadecimális értékéket az SPI Slave Transmitjára!");
	Serial.println("<aa> Elindítja a motor.");
	Serial.println("<bb> Megállítja a motor.");
	Serial.println("<cc> Balra forog a motor.");
	Serial.println("<dd> Jobbra forog a motor.");
	Serial.println("----------------------------------------------------------------");
}

void loop()
{
	digitalWrite(SlaveSelectPin,LOW); //beaktiváljuk a szolgát
	int x = analogRead(A0); //Csúszkáról olvasunk
	short y = x / 4; //8 bitre konvertálás
	byte value = SPI.transfer(0); // transfer(0) = olvasunk szolgáról
	ertekvizsgalo(&value,y); //Megnézi mi a parancs
	digitalWrite(SlaveSelectPin, HIGH); //leállítjuk a szolgát 
	delay(2000);
}

void ertekvizsgalo(byte* y, short x){
	if(*y == 170){ //ha aa akkor elindul MOTOR random irányba
		analogWrite(3,x);
		digitalWrite(8,random(0,2));
		Serial.print("A motor elindul.\n");
		*y = 0;
	}
	else if(*y == 187){ //ha bb akkor MOTOR leáll
		analogWrite(3,0);
		Serial.print("A motor leáll.\n");
		*y = 0;
	}
	else if(*y == 204){ //ha cc akkor MOTOR balra forog
		digitalWrite(8,HIGH);
		Serial.print("Balra forog.\n");
		*y = 0;
	}
	else if(*y == 221){ //ha dd akkor MOTOR jobbra forog
		digitalWrite(8,LOW);
		Serial.print("Jobbra forog.\n");
		*y = 0;
	}else{
		if (*y != 255){ //ha más az érték kiírja hogy nem értelmezheto
			Serial.print("Nem értelmezheto hexa szám!\n");
			*y = 0;
		}
	}
}
