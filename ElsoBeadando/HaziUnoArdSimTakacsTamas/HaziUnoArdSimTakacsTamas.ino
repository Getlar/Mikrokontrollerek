/*  Készítette: Takács Tamás 2020.03.26 13:02
 *  A feladat témája: A MOTOR működtetése soros port és pwm jel segítségével
 *  
 *  Program indításakor: (ezeket írjuk be)
 *          - MOTOR: - Pwm = 03
 *                   - Dir = A2
 *          - Csúszka: - Analóg jel bemenet - A0 (10 bit)
 *          
 *  Program setup:
 *          - Serial.begin(9600) - Inicializálja a soros portunkat 9600 bit/s-on
 *          - inputString.reserve(200) - inputString mint globális változónak 200 bájtnyi helyet foglalunk le
 *          - A0 - bemenet - analóg jel olvasás
 *          - A2 - kimenet - analóg jel küldés MOTOR irányváltozójára
 *          - 3 - kimenet - "~" pwm jel output
 *          
 *  Program futása:
 *  
 *      void loop();
 *          - Kezdetben beolvasunk analóg bemenetről egy 10 bites értéket amit átalakítunk 8 bitesre.
 *          - Ez azért fontos hiszen a pwm jelünk maximum 8 bites lehet.
 *          - Ha a sztringünket lezártuk kitöröljük a lezáró karaktert. (Azért fontos mert '*' karakterrel zárunk ezért az is a sztringben maradna)
 *          - Meghívjuk az "ellenorzo(short ertek);" függvényt. (lsd. lejjebb)
 *          - Kiiratjuk a soros kimenetre a beírt sztringünket.
 *          - Kiürítjük az "inputString" tartalmát.
 *          - Visszaállítjuk a sztringellenörzőt hamisra.
 *          
 *      void SerialEven();
 *          - Ez lesz felelős a soros porton való kommunikációért.
 *          - Karaktereket kérünk majd a soros portunkon és ezeket fűzögetjük egymásba, hogy megkapjuk a sztringünket.
 *          - Ha '*' karaktert észlelünk mondjuk meg a loop();-nak, hogy értelmezze a beírt sztringet.
 *          
 *      void ellenorzo(short ertek);
 *          - Ha a bemeneti sztringünk: "START" (itt már levettük csillag jelet) - pwm jelet küldünk MOTOR-nak.
 *                                      "STOP" - 0 értéket adunk át MOTOR-nak. (Ami annyit fog tenni hogy a kitöltési tényező 0% lesz -> 0V)
 *                                      "BALRA" - digitális 1 értéket küldünk a MOTOR irányváltozójának. (1-re balra, 0-ra jobbra forog)
 *                                      "JOBBRA" - digitális 0 értéket küldünk a MOTOR irányváltozójának.
 *                                      
 *  Program indítása utáni parancs lehetőségek:                         
 *          - <JOBBRA*> - MOTOR jobbra forog.
 *          - <BALRA*> - MOTOR balra forog.
 *          - <START*> - MOTOR megkapja pwm jelet.
 *          - <STOP*> - MOTOR leáll.
 */
String inputString = ""; 
bool stringComplete = false;
bool megy = false;

void setup() {
	Serial.begin(9600);
	inputString.reserve(200);
	pinMode(A0,INPUT);
	pinMode(A2,OUTPUT);
	pinMode(3,OUTPUT);
}

void loop() {
	int x = analogRead(A0);
	short y = x / 4;
	if(stringComplete){
		inputString[inputString.length()-1] = '\0';
		ellenorzo(y);
		Serial.println(inputString);
		inputString = "";
		stringComplete = false;
	}
}

void serialEvent(){
	while(Serial.available()){
		char inChar = (char)Serial.read();
		inputString += inChar;
		if(inChar == '*'){
			stringComplete = true;
		}
	}
}

void ellenorzo(short ertek){
	if(inputString.equals("START")){
		analogWrite(3,ertek);
		digitalWrite(A2,random(0,2));
	}
	if(inputString.equals("STOP")){
		analogWrite(3,0);
	}
	if(inputString.equals("BALRA")){
		digitalWrite(A2,1);
	}
	if(inputString.equals("JOBBRA")){
		digitalWrite(A2,0);
	}
}
