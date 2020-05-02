
/*	Készí­tette: Takács Tamás 2020.04.27 19:51
   A feladat témája: Autóvezetoi szimuláció. A program segítségével irányíthatunk olyan berendezé-
   seket, melyek általános rendeltetésu gépjármuvekbe is megtalalható. Pl: Ablaktörlo, Klíma, stb.
   Nyilánvalóan ez csak egy pszeudo-szimuláció, olyan programot szerettem volna írni, amiben min-
   den tanult elemet fel lehetne használni.
   
   Program indá­tásakor: (ezeket í­rjuk be)

           - LED - ek: - 06 és RYGB = Y
                    	   - 04 és RYGB = Y

           - Csúszka: - Analóg jel bemenet - A1
			          - Ventillátor sebességének állítására

		   - SERVO: - Analóg jel bemenet - A0
		
		   - PIEZO: - 05
		
		   - STEPR:P1 - 08
				   P2 - 09
				   P3 - 10
				   P4 - 11
				   steps - 32
					
		   - SERIAL: Baud ráta - 19200
		
		   - MOTOR: Pwm - 03
		
		   - PUSH Gomb: 02 
                                       
   Program indá­tása utáni parancs lehet?ségek: (SERIAL TX)                         
           - 	ABLAKTORLO BE! - Elindítja az ablaktörlot (servo)
		   - 	ABLAKTORLO KI! - Leállítja az ablaktörlot (servo)
		   -    ABLAKTORLO MAX! - Ablaktörlo maximális sebesség (servo)
		   - 	ABLAKTORLO MIN! - Ablaktörlo minimális sebesség (servo)
		   - 	ABLAK FEL! - Felhúzza az ablakot (stepper)
		   - 	ABLAK LE! - Lehúzza az ablakot (stepper)
		   - 	VENTILLATOR BE! - Klíma beindul (motor)
		   - 	VENTILLATOR KI! - Klíma leáll (motor)
		   - 	LAMPAK BE! - Lámpák bekapcsolása (led)
		   - 	LAMPAK KI! - Lámpák kikapcsolása (led)
		   - 	DUDA! - Dudálás (piezo)
   Gomb megnyomása - Leállítja az összes futó berendezést
 
   FONTOS!!
	    - VENTILLATOR BE! parancs után csuszkával lehet sebességet szabályozni.
		- Baud ráta beállítása nekem azért kellett, mert így minden kikerült soros kimenetre egyszerre.
		- Dudáláskor a SERVO le kell álljon, mert a két pwm jel interferálna

*/
//könyvtárak importálása
#include <Servo.h>
#include <Stepper.h>

//nevesített konstans, lépteto lépésszámát tárolja
#define STEPS 32

//Servo és Stepper példányosítása
Servo servo1;
Stepper Ablak(STEPS,8,9,10,11);

//pin inicializálások
const int ledPin1 = 6;
const int ledPin2 = 4;
const int horn = 5;
const int ventillatorPin = 3;
const int SW = 2;

//globális változók, amelyek állapotokat fognak tárolni
unsigned int sebesseg; //servo sebessége
String inputString; //soros portról olvasott sztring ebbe fog kerülni
bool stringComplete; //ha olvas '!' karaktert igazra állítja
int ablaktorlo; //ablaktörlo állása
int ventillator; //ventillátor állása
int lampak; //lámpák állása
int servohely1; //ablaktörlo állása
int ablak; //ablak állása


//Interrupt kezelésére használt függvény, mindent kinulláz
void Kikapcsolo(){
	ablak = 0;
	servohely1 = 0;
	stringComplete = false;
	inputString = "";
	ablaktorlo = 0;
	lampak = 0;
	ventillator = 0;
	sebesseg = 15;
	Serial.println("Autó leállítva!");
}

void setup()
{	
	//változók inicializálása
	ablak = 0;
	servohely1 = 0;
	stringComplete = false;
	inputString = "";
	ablaktorlo = 0;
	lampak = 0;
	ventillator = 0;
	sebesseg = 15;
	
	Serial.begin(19200); //Soros busz inicializálás
	inputString.reserve(200); //200 byte helyfoglalás inputString-nek
	pinMode(A0,INPUT); //Szervó irányítása
	pinMode(A1,INPUT); //Analóg jel olvasás
	pinMode(ventillatorPin,OUTPUT); //Motor ezen a pinen kap pwm jelet
	servo1.attach(A0); //Szervó változó A0 pinhez kötése
	pinMode(ledPin1, OUTPUT); //LED inicializálás
	pinMode(ledPin2, OUTPUT); //LED inicializálás
	pinMode(horn,OUTPUT); //Duda inicializálás
	Ablak.setSpeed(60); //Nem forgatja motor, csak beállítja hányat forogjon 1 perc alatt
	pinMode(SW, INPUT_PULLUP); //2-es pinen érzékel megszakítást
	attachInterrupt(0, Kikapcsolo, CHANGE); //Megmondjuk az Arduinonak ha interrupt jön 2-es pinen hívja meg a megszakításkezelo függvényünk
	//Soros porton segítség céljából
	Serial.println("----------------------------------------------------------------------");
	Serial.println("               Üdvözlöm az autóvezetoi szimulációmban!                ");
	Serial.println("                           Jó utat kivánok!                           ");
	Serial.println("----------------------------------------------------------------------");
	Serial.println("Az autó funkciói a következok:\n");
	Serial.println("ABLAKTORLO BE! - Elindítja az ablaktörlot (servo)");
	Serial.println("ABLAKTORLO KI! - Leállítja az ablaktörlot (servo)");
	Serial.println("ABLAKTORLO MAX! - Ablaktörlo maximális sebesség (servo)");
	Serial.println("ABLAKTORLO MIN! - Ablaktörlo minimális sebesség (servo)");
	Serial.println("ABLAK FEL! - Felhúzza az ablakot (stepper)");
	Serial.println("ABLAK LE! - Lehúzza az ablakot (stepper)");
	Serial.println("VENTILLATOR BE! - Klíma beindul (motor)");
	Serial.println("VENTILLATOR KI! - Klíma leáll (motor)");
	Serial.println("LAMPAK BE! - Lámpák bekapcsolása (led)");
	Serial.println("LAMPAK KI! - Lámpák kikapcsolása (led)");
	Serial.println("DUDA! - Dudálás (piezo)\n");
	Serial.println("----------------------------------------------------------------------");
	
}
//Ez a függvény kapja meg a véglegesített sztringet, majd meghívja a megfelelo kezelofüggvényt
//Pl. ha azt írjuk be, hogy DUDA! akkor switch-case szerkezetbe lemegy a DUDA! case-ig
//Meghívja a dudakezelo függvényt, majd kibreakel. (C-be kötelezo, másképp switch-be maradna)
//Default ágon hibaüzenet, hogy nem ismert a bekapott sztring.
void ellenorzo(String messageGot){
	switch(messageGot){
		case "ABLAKTORLO BE" :
			ablaktorlo = 1;
			Serial.print("Ablaktörlo bekapcsolva!\n");
			break;
		case "ABLAKTORLO KI":
			ablaktorlo = 0;
			Serial.print("Ablaktörlo kikapcsolva!\n");
			break;
		case "ABLAKTORLO MAX":
			sebesseg = 10;
			Serial.print("Ablaktörlo maximum sebesség!\n");
			break;
		case "ABLAKTORLO MIN":
			sebesseg = 30;
			Serial.print("Ablaktörlo minimum sebesség!\n");
			break;
		case "VENTILLATOR BE":
			ventillator = 1;
			Serial.print("Ventillátor bekapcsolva!\n");
			break;
		case "VENTILLATOR KI":
			ventillator = 0;
			Serial.print("Ventillátor kikapcsolva!\n");
			break;
		case "LAMPAK BE":
			lampak = 1;
			Serial.print("Lámpák bekapcsolva!\n");
			break;
		case "LAMPAK KI":
			lampak = 0;
			Serial.print("Lámpák kikapcsolva!\n");
			break;
		case "DUDA":
			dudaKezelo();
			break;
		case "ABLAK FEL":
			ablak = 1;
			ablakKezelo();
			Serial.println("Ablak felhúzva!");
			break;
		case "ABLAK LE":
			ablak = 0;
			ablakKezelo();
			Serial.println("Ablak lehúzva!");
			break;
		default:
			Serial.println("Nem ismerem ezt a parancsot!");
			break;
	}
}
//STEPR-t állítja, ha felfele húzzuk ablak, forog balra 4 kört, lefele jobbra 4 kört.
void ablakKezelo(){
	if(ablak == 1){
		Ablak.step(4*32);
	}else{
		Ablak.step(4*-32);
	}
}
//PIEZO-t állítja, ha SERVO már fut, azt egy másodpercre leállítja ne legyen interferencia
//Ha SERVO leállt 1kHz-es hangot fog PIEZO-nak átadni
//tone() négyszögjelet generál 50% telítettséggel
//Mivel ez folytonos jel, noTone() fogja megállítani
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
	Serial.println("Dudáltam!");
}
//Motort kezeli, ha bekapcsoljuk, csúszkáról olvas, ha kikapcsoljuk 0-át kötünk rá
//10 bitrol 8 bitre alakít
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
//Ha kikapcsoljuk logikai alacsonyat kötünk rájuk
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

//Szervót fogja kezelni, ha bekapcsoljuk alap sebességgel elindul, majd azt állíthatjuk további parancsokkal
//Jobbról indul: 0 fok, majd elfordul 180 fokot, majd vissza ezt ismételve
//Ha sebességet állítjuk, változni fog a késleltetés mozgások között ezzel manipulálva sebességet
//Leállításnál 0 fokra állítjuk és sebesség vissza normálra (15)
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


//Ha sztring utasítás megvalósul, leveszi a ! jelet a végérol, majd átadja ellenorzo függvénynek az új sztringet
//Reseteli inputString tartalom, majd vár újra olvasásra
//Minden ciklus végén ellenorizzuk a folytonos tevékenységek helyes muködését (resource igényes, de biztos)
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
//Ha változás történik soros bemeneten kezeli
//Karaktereket olvasunk, ezeket konkatenáljuk sztringgé
//Ha ! karaktert olvasunk kezeljük a parancsot.

void serialEvent(){
	while(Serial.available()){
		char inChar = (char)Serial.read();
		inputString += inChar;
		if(inChar == '!'){
			stringComplete = true;
		}
	}
}


