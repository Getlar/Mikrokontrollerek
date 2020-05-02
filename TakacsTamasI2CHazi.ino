

/*  Készí­tette: Takács Tamás 2020.04.02 19:51
   A feladat témája: A MOTOR m?ködtetése soros port(SPI) és pwm jel segítségével
   
   Program indá­tásakor: (ezeket í­rjuk be)

           - MOTOR: - Pwm = 03
                    - Dir = 08
           - Csúszka: - Analóg jel bemenet - A0 (10 bit)

		   - I2CSLV: - Addr(Cím) = 08
                                       
   Program indá­tása utáni parancs lehet?ségek: (I2C TX)                         
           - cc - MOTOR jobbra forog.
           - dd - MOTOR balra forog.
           - aa - MOTOR megkapja pwm jelet.
           - bb - MOTOR leáll.
 
   FONTOS!!
	    - START* beírása el?tt a csúszkát állítani kell.
		- Ez fogja befolyásolni a motor sebességét.
		- Az utasításokat a Slave Transmitjába kell írni, a Master(Arduino) csak olvas.
 	
 
 */


#include <Wire.h>

void setup()
{
	Wire.begin();//I2C busz bekapcsolása
	Serial.begin(9600);//Soros busz inicializálás
	pinMode(A0,INPUT);//Analóg bemenet.
	pinMode(3,OUTPUT);//Motor itt kap pwm jelet.
	pinMode(7,OUTPUT);//Motor irányváltozója.
	Serial.println("----------------------------------------------------------------");
	Serial.println("Kérlek adj be hexadecimális értékéket az I2C Slave Transmitjára!");
	Serial.println("<aa> Elindítja a motor.");
	Serial.println("<bb> Megállítja a motor.");
	Serial.println("<cc> Balra forog a motor.");
	Serial.println("<dd> Jobbra forog a motor.");
	Serial.println("----------------------------------------------------------------");
}

void loop()
{
	int x = analogRead(A0); //Csúszkáról olvasunk
	short y = x / 4; //8 bitre konvertálás
	Wire.requestFrom(8,1);//1 byteot kérünk 8-as számú szolgától
	byte c = Wire.read();//Beolvasunk 1 byteot
	ertekvizsgalo(&c, y);//Megnézzük mit kaptunk
	delay(2000);//2 sec delay
}

void ertekvizsgalo(byte* y, short x){
	if(*y == 170){	//ha aa akkor elindul MOTOR random irányba
		analogWrite(3,x);
		digitalWrite(8,random(0,2));
		Serial.print("A motor elindul.\n");
		*y = 0;
	}
	else if(*y == 187){	//ha bb akkor MOTOR leáll
		analogWrite(3,0);
		Serial.print("A motor leáll.\n");
		*y = 0;
	}
	else if(*y == 204){	//ha cc akkor MOTOR balra forog
		digitalWrite(7,HIGH);
		Serial.print("Balra forog.\n");
		*y = 0;
	}
	else if(*y == 221){	//ha dd akkor MOTOR jobbra forog
		digitalWrite(7,LOW);
		Serial.print("Jobbra forog.\n");
		*y = 0;
	}else{
		if (*y != 255){	//ha más az érték kiírja hogy nem értelmezheto
			Serial.print("Nem értelmezheto hexa szám!\n");
			*y = 0;
		}
	}
}

