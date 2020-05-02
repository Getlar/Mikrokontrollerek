
/*	K�sz�tette: Tak�cs Tam�s 2020.04.27 19:51
   A feladat t�m�ja: Aut�vezetoi szimul�ci�. A program seg�ts�g�vel ir�ny�thatunk olyan berendez�-
   seket, melyek �ltal�nos rendeltet�su g�pj�rmuvekbe is megtalalhat�. Pl: Ablakt�rlo, Kl�ma, stb.
   Nyil�nval�an ez csak egy pszeudo-szimul�ci�, olyan programot szerettem volna �rni, amiben min-
   den tanult elemet fel lehetne haszn�lni.
   
   Program ind�t�sakor: (ezeket �rjuk be)

           - LED - ek: - 06 �s RYGB = Y
                    	   - 04 �s RYGB = Y

           - Cs�szka: - Anal�g jel bemenet - A1
			          - Ventill�tor sebess�g�nek �ll�t�s�ra

		   - SERVO: - Anal�g jel bemenet - A0
		
		   - PIEZO: - 05
		
		   - STEPR:P1 - 08
				   P2 - 09
				   P3 - 10
				   P4 - 11
				   steps - 32
					
		   - SERIAL: Baud r�ta - 19200
		
		   - MOTOR: Pwm - 03
		
		   - PUSH Gomb: 02 
                                       
   Program ind�t�sa ut�ni parancs lehet?s�gek: (SERIAL TX)                         
           - 	ABLAKTORLO BE! - Elind�tja az ablakt�rlot (servo)
		   - 	ABLAKTORLO KI! - Le�ll�tja az ablakt�rlot (servo)
		   -    ABLAKTORLO MAX! - Ablakt�rlo maxim�lis sebess�g (servo)
		   - 	ABLAKTORLO MIN! - Ablakt�rlo minim�lis sebess�g (servo)
		   - 	ABLAK FEL! - Felh�zza az ablakot (stepper)
		   - 	ABLAK LE! - Leh�zza az ablakot (stepper)
		   - 	VENTILLATOR BE! - Kl�ma beindul (motor)
		   - 	VENTILLATOR KI! - Kl�ma le�ll (motor)
		   - 	LAMPAK BE! - L�mp�k bekapcsol�sa (led)
		   - 	LAMPAK KI! - L�mp�k kikapcsol�sa (led)
		   - 	DUDA! - Dud�l�s (piezo)
   Gomb megnyom�sa - Le�ll�tja az �sszes fut� berendez�st
 
   FONTOS!!
	    - VENTILLATOR BE! parancs ut�n csuszk�val lehet sebess�get szab�lyozni.
		- Baud r�ta be�ll�t�sa nekem az�rt kellett, mert �gy minden kiker�lt soros kimenetre egyszerre.
		- Dud�l�skor a SERVO le kell �lljon, mert a k�t pwm jel interfer�lna

*/
//k�nyvt�rak import�l�sa
#include <Servo.h>
#include <Stepper.h>

//neves�tett konstans, l�pteto l�p�ssz�m�t t�rolja
#define STEPS 32

//Servo �s Stepper p�ld�nyos�t�sa
Servo servo1;
Stepper Ablak(STEPS,8,9,10,11);

//pin inicializ�l�sok
const int ledPin1 = 6;
const int ledPin2 = 4;
const int horn = 5;
const int ventillatorPin = 3;
const int SW = 2;

//glob�lis v�ltoz�k, amelyek �llapotokat fognak t�rolni
unsigned int sebesseg; //servo sebess�ge
String inputString; //soros portr�l olvasott sztring ebbe fog ker�lni
bool stringComplete; //ha olvas '!' karaktert igazra �ll�tja
int ablaktorlo; //ablakt�rlo �ll�sa
int ventillator; //ventill�tor �ll�sa
int lampak; //l�mp�k �ll�sa
int servohely1; //ablakt�rlo �ll�sa
int ablak; //ablak �ll�sa


//Interrupt kezel�s�re haszn�lt f�ggv�ny, mindent kinull�z
void Kikapcsolo(){
	ablak = 0;
	servohely1 = 0;
	stringComplete = false;
	inputString = "";
	ablaktorlo = 0;
	lampak = 0;
	ventillator = 0;
	sebesseg = 15;
	Serial.println("Aut� le�ll�tva!");
}

void setup()
{	
	//v�ltoz�k inicializ�l�sa
	ablak = 0;
	servohely1 = 0;
	stringComplete = false;
	inputString = "";
	ablaktorlo = 0;
	lampak = 0;
	ventillator = 0;
	sebesseg = 15;
	
	Serial.begin(19200); //Soros busz inicializ�l�s
	inputString.reserve(200); //200 byte helyfoglal�s inputString-nek
	pinMode(A0,INPUT); //Szerv� ir�ny�t�sa
	pinMode(A1,INPUT); //Anal�g jel olvas�s
	pinMode(ventillatorPin,OUTPUT); //Motor ezen a pinen kap pwm jelet
	servo1.attach(A0); //Szerv� v�ltoz� A0 pinhez k�t�se
	pinMode(ledPin1, OUTPUT); //LED inicializ�l�s
	pinMode(ledPin2, OUTPUT); //LED inicializ�l�s
	pinMode(horn,OUTPUT); //Duda inicializ�l�s
	Ablak.setSpeed(60); //Nem forgatja motor, csak be�ll�tja h�nyat forogjon 1 perc alatt
	pinMode(SW, INPUT_PULLUP); //2-es pinen �rz�kel megszak�t�st
	attachInterrupt(0, Kikapcsolo, CHANGE); //Megmondjuk az Arduinonak ha interrupt j�n 2-es pinen h�vja meg a megszak�t�skezelo f�ggv�ny�nk
	//Soros porton seg�ts�g c�lj�b�l
	Serial.println("----------------------------------------------------------------------");
	Serial.println("               �dv�zl�m az aut�vezetoi szimul�ci�mban!                ");
	Serial.println("                           J� utat kiv�nok!                           ");
	Serial.println("----------------------------------------------------------------------");
	Serial.println("Az aut� funkci�i a k�vetkezok:\n");
	Serial.println("ABLAKTORLO BE! - Elind�tja az ablakt�rlot (servo)");
	Serial.println("ABLAKTORLO KI! - Le�ll�tja az ablakt�rlot (servo)");
	Serial.println("ABLAKTORLO MAX! - Ablakt�rlo maxim�lis sebess�g (servo)");
	Serial.println("ABLAKTORLO MIN! - Ablakt�rlo minim�lis sebess�g (servo)");
	Serial.println("ABLAK FEL! - Felh�zza az ablakot (stepper)");
	Serial.println("ABLAK LE! - Leh�zza az ablakot (stepper)");
	Serial.println("VENTILLATOR BE! - Kl�ma beindul (motor)");
	Serial.println("VENTILLATOR KI! - Kl�ma le�ll (motor)");
	Serial.println("LAMPAK BE! - L�mp�k bekapcsol�sa (led)");
	Serial.println("LAMPAK KI! - L�mp�k kikapcsol�sa (led)");
	Serial.println("DUDA! - Dud�l�s (piezo)\n");
	Serial.println("----------------------------------------------------------------------");
	
}
//Ez a f�ggv�ny kapja meg a v�gleges�tett sztringet, majd megh�vja a megfelelo kezelof�ggv�nyt
//Pl. ha azt �rjuk be, hogy DUDA! akkor switch-case szerkezetbe lemegy a DUDA! case-ig
//Megh�vja a dudakezelo f�ggv�nyt, majd kibreakel. (C-be k�telezo, m�sk�pp switch-be maradna)
//Default �gon hiba�zenet, hogy nem ismert a bekapott sztring.
void ellenorzo(String messageGot){
	switch(messageGot){
		case "ABLAKTORLO BE" :
			ablaktorlo = 1;
			Serial.print("Ablakt�rlo bekapcsolva!\n");
			break;
		case "ABLAKTORLO KI":
			ablaktorlo = 0;
			Serial.print("Ablakt�rlo kikapcsolva!\n");
			break;
		case "ABLAKTORLO MAX":
			sebesseg = 10;
			Serial.print("Ablakt�rlo maximum sebess�g!\n");
			break;
		case "ABLAKTORLO MIN":
			sebesseg = 30;
			Serial.print("Ablakt�rlo minimum sebess�g!\n");
			break;
		case "VENTILLATOR BE":
			ventillator = 1;
			Serial.print("Ventill�tor bekapcsolva!\n");
			break;
		case "VENTILLATOR KI":
			ventillator = 0;
			Serial.print("Ventill�tor kikapcsolva!\n");
			break;
		case "LAMPAK BE":
			lampak = 1;
			Serial.print("L�mp�k bekapcsolva!\n");
			break;
		case "LAMPAK KI":
			lampak = 0;
			Serial.print("L�mp�k kikapcsolva!\n");
			break;
		case "DUDA":
			dudaKezelo();
			break;
		case "ABLAK FEL":
			ablak = 1;
			ablakKezelo();
			Serial.println("Ablak felh�zva!");
			break;
		case "ABLAK LE":
			ablak = 0;
			ablakKezelo();
			Serial.println("Ablak leh�zva!");
			break;
		default:
			Serial.println("Nem ismerem ezt a parancsot!");
			break;
	}
}
//STEPR-t �ll�tja, ha felfele h�zzuk ablak, forog balra 4 k�rt, lefele jobbra 4 k�rt.
void ablakKezelo(){
	if(ablak == 1){
		Ablak.step(4*32);
	}else{
		Ablak.step(4*-32);
	}
}
//PIEZO-t �ll�tja, ha SERVO m�r fut, azt egy m�sodpercre le�ll�tja ne legyen interferencia
//Ha SERVO le�llt 1kHz-es hangot fog PIEZO-nak �tadni
//tone() n�gysz�gjelet gener�l 50% tel�tetts�ggel
//Mivel ez folytonos jel, noTone() fogja meg�ll�tani
void dudaKezelo(){
	if(ablaktorlo == 1){
		ablaktorlo == 0;
		ablaktorloKezelo();
		tone(horn,1000);
		delay(2000);
		noTone(horn);
		ablaktorlo = 1;
		ablaktorloKezelo();
	}else{
		tone(horn,2000);
		delay(1000);
		noTone(horn);
	}
	Serial.println("Dud�ltam!");
}
//Motort kezeli, ha bekapcsoljuk, cs�szk�r�l olvas, ha kikapcsoljuk 0-�t k�t�nk r�
//10 bitrol 8 bitre alak�t
void ventillatorKezelo(){
	if(ventillator == 1){
		int x = analogRead(A1);
		short y = x/4;
		analogWrite(ventillatorPin,y);
		
	}else{
		int x = 0;
		analogWrite(ventillatorPin,x);
	}
}
//LED-eket kezel, ha bekapcsoljuk oket felgyulladnak, mert logikai magasat kapnak
//Ha kikapcsoljuk logikai alacsonyat k�t�nk r�juk
void lampaKezelo(){
	if(lampak == 1 && digitalRead(ledPin1)!=1){
		digitalWrite(ledPin1,HIGH);
		digitalWrite(ledPin2,HIGH);
	}
	if(lampak == 0 && digitalRead(ledPin1)!=0)
	{
		digitalWrite(ledPin1,LOW);
		digitalWrite(ledPin2,LOW);
	}
}

//Szerv�t fogja kezelni, ha bekapcsoljuk alap sebess�ggel elindul, majd azt �ll�thatjuk tov�bbi parancsokkal
//Jobbr�l indul: 0 fok, majd elfordul 180 fokot, majd vissza ezt ism�telve
//Ha sebess�get �ll�tjuk, v�ltozni fog a k�sleltet�s mozg�sok k�z�tt ezzel manipul�lva sebess�get
//Le�ll�t�sn�l 0 fokra �ll�tjuk �s sebess�g vissza norm�lra (15)
void ablaktorloKezelo(){
	if(ablaktorlo == 1 && (servo1.read()==0 || servo1.read()==3)){
		for(servohely1 = 0; servohely1 < 180; servohely1+=3){
			servo1.write(servohely1);
			delay(sebesseg);
		}	 
		delay(250);
		for(servohely1 = 180; servohely1 >0; servohely1-=3){
			servo1.write(servohely1);
			delay(sebesseg);
		}
	}
	if(ablaktorlo == 0 && servo1.read()!=0)
	{
		servo1.write(0);
		sebesseg = 15;
	}
}	


//Ha sztring utas�t�s megval�sul, leveszi a ! jelet a v�g�rol, majd �tadja ellenorzo f�ggv�nynek az �j sztringet
//Reseteli inputString tartalom, majd v�r �jra olvas�sra
//Minden ciklus v�g�n ellenorizzuk a folytonos tev�kenys�gek helyes muk�d�s�t (resource ig�nyes, de biztos)
void loop()
{
	if(stringComplete){
		inputString[inputString.length()-1] = '\0';
		ellenorzo(inputString);
		inputString ="";
		stringComplete = false;
		delay(250);
	}
	ablaktorloKezelo();
	ventillatorKezelo();
	lampaKezelo();
}
//Ha v�ltoz�s t�rt�nik soros bemeneten kezeli
//Karaktereket olvasunk, ezeket konkaten�ljuk sztringg�
//Ha ! karaktert olvasunk kezelj�k a parancsot.

void serialEvent(){
	while(Serial.available()){
		char inChar = (char)Serial.read();
		inputString += inChar;
		if(inChar == '!'){
			stringComplete = true;
		}
	}
}


