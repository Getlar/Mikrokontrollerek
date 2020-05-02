
/*  K�sz�tette: Tak�cs Tam�s 2020.04.02 19:21
   A feladat t�m�ja: A MOTOR m?k�dtet�se soros port(SPI) �s pwm jel seg�ts�g�vel
   
   Program ind�t�sakor: (ezeket �rjuk be)

           - MOTOR: - Pwm = 03
                    - Dir = 08
           - Cs�szka: - Anal�g jel bemenet - A0 (10 bit)

		   - SPISLV: - SS*(Slave Select) = 10
                                       
   Program ind�t�sa ut�ni parancs lehet?s�gek: (SPI TX)                         
           - cc - MOTOR jobbra forog.
           - dd - MOTOR balra forog.
           - aa - MOTOR megkapja pwm jelet.
           - bb - MOTOR le�ll.
 
   FONTOS!!
	    - START* be�r�sa el?tt a cs�szk�t �ll�tani kell.
		- Ez fogja befoly�solni a motor sebess�g�t.
		- Az utas�t�sokat a Slave Transmitj�ba kell �rni, a Master(Arduino) csak olvas.
 	
 
 */

#include <SPI.h>

const int SlaveSelectPin = 10;

void setup()
{
	pinMode(SlaveSelectPin, OUTPUT); //Master errol a pinrol olvas.
	pinMode(8,OUTPUT); //Motor ir�nyv�ltoz�ja.
	pinMode(3,OUTPUT); //Motor itt kap pwm jelet.
	pinMode(A0,INPUT); //Anal�g bemenet.
	SPI.begin(): //SPI busz inicializ�l�s
	Serial.begin(9600); //Soros busz inicializ�l�s
	Serial.println("----------------------------------------------------------------");
	Serial.println("K�rlek adj be hexadecim�lis �rt�k�ket az SPI Slave Transmitj�ra!");
	Serial.println("<aa> Elind�tja a motor.");
	Serial.println("<bb> Meg�ll�tja a motor.");
	Serial.println("<cc> Balra forog a motor.");
	Serial.println("<dd> Jobbra forog a motor.");
	Serial.println("----------------------------------------------------------------");
}

void loop()
{
	digitalWrite(SlaveSelectPin,LOW); //beaktiv�ljuk a szolg�t
	int x = analogRead(A0); //Cs�szk�r�l olvasunk
	short y = x / 4; //8 bitre konvert�l�s
	byte value = SPI.transfer(0); // transfer(0) = olvasunk szolg�r�l
	ertekvizsgalo(&value,y); //Megn�zi mi a parancs
	digitalWrite(SlaveSelectPin, HIGH); //le�ll�tjuk a szolg�t 
	delay(2000);
}

void ertekvizsgalo(byte* y, short x){
	if(*y == 170){ //ha aa akkor elindul MOTOR random ir�nyba
		analogWrite(3,x);
		digitalWrite(8,random(0,2));
		Serial.print("A motor elindul.\n");
		*y = 0;
	}
	else if(*y == 187){ //ha bb akkor MOTOR le�ll
		analogWrite(3,0);
		Serial.print("A motor le�ll.\n");
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
		if (*y != 255){ //ha m�s az �rt�k ki�rja hogy nem �rtelmezheto
			Serial.print("Nem �rtelmezheto hexa sz�m!\n");
			*y = 0;
		}
	}
}
