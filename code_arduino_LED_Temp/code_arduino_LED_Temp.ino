#include <LiquidCrystal.h> //On inclue les différentes librairies
#include <Wire.h>
#include <Adafruit_BMP085.h>
#include "DHT.h"
#include "RTClib.h"

#define DHTPIN 2 //on définit le pin du DHT
#define DHTTYPE DHT22 //on définit le type du DHT peut aussi etre DHT11 ou DHT21   

#define REDPIN 5 //On définit le Pin pour les LED rouge
#define GREENPIN 3 //On définit le Pin pour les LED bleu
#define BLUEPIN 6 //On définit le Pin pour les LED verte

LiquidCrystal lcd(7, 8, 9, 10, 11, 12); //On définie les Pins pour l'écran LCD
 
const int tempmax = 40;//On définit une valeur maximum pour les LED
const int tempmin = -10;//On définit une valeur minimal pour les LED

Adafruit_BMP085 bmp;//on démarre les variables de libraries
DHT dht(DHTPIN, DHTTYPE); 
RTC_DS1307 RTC;

void setup() { //On attaque le void setup
  Serial.begin(19200); //on demarre une communication serial
  dht.begin();//On démarre le DHT
  Wire.begin();//On démarre la librairie Wire
  if (!bmp.begin()) {
	Serial.println("BMP085 ERREUR !!!");//on vérifie si le bmp085 fonctionne
	while (1) {}
  }
  
  Serial.println("DHT21 TEST");//on vérifie si le DHT fonctionne

  RTC.begin();
  if (! RTC.isrunning()) {
    Serial.println("RTC ERREUR !!!");//On vérifie si le RTC fonctionne
    RTC.adjust(DateTime(__DATE__, __TIME__));// la ligne suivante définit l'heure du RTC à celle ou le code a été compilé
  }
  
  //On définie les Pins en mode sortie pour le ruban LED 
   pinMode(REDPIN, OUTPUT);
   pinMode(GREENPIN, OUTPUT);
   pinMode(BLUEPIN, OUTPUT);
   
  lcd.begin(20, 4); //On définit le nombre de caractères par ligne et le nombre de lignes du LCD 
  
}
  
void loop() { //On attaque le void loop
  
  int lumiValue = analogRead(A0); //On lit la valeur de la photorésistance 

/* Pour utiliser la valeur de la photorésistance, il faut 
ramener la valeur du port Analogique (0 à 1024) à une échelle de 0 à 100.
C'est pourquoi on utilise la fonction map*/
  int lumilcdValue = map(lumiValue,0,1024,0,100);
  
  int alti = bmp.readAltitude(101500);//on ne garde que la partie entière de la valeur pour l'altittude sa gagne toujours deux caracteres sur le lcd...

  int humi = dht.readHumidity(); //on definit humi pour stocker la valeur d'humidité
  float tempDHT = dht.readTemperature();//on definit tempDHT pour stocker la valeur de temperature du DHT
  float tempBMP = bmp.readTemperature();//on definit tempBMP pour stocker la valeur de temperature du BMP
  float tmoy = ((tempBMP + tempDHT)/2);//On fait la moyenne des deux valeurs de température : deux mesures valent mieux qu'une...
  int range = map(tmoy,tempmin, tempmax,0, 5);//On définit range qui va mapper la moyenne des températures pour changer les couleurs du ruban LED en fonction de la température
  switch (range) {
  case 0:    //pour -10°C à 0°C On fait du blanc
  
    analogWrite(REDPIN,255);
    analogWrite(BLUEPIN,255);
    analogWrite(GREENPIN,255);
    
    Serial.println("Cas 0");
    Serial.println("-10*C - 0*C");
    break;
  case 1:    //pour 0°C à 10°C On fait du bleu foncé
    analogWrite(BLUEPIN,255);
    analogWrite(REDPIN,0);
    analogWrite(GREENPIN,0);
    Serial.println("Cas 1");
    Serial.println("0*C - 10*C");
    break;
    
  case 2:    //pour 10°C à 20°C On fait du bleu clair
    analogWrite(BLUEPIN,255);
    analogWrite(REDPIN,0);
    analogWrite(GREENPIN,255);
    Serial.println("Cas 2");
    Serial.println("10*C - 20*C");
    break;
    
 case 3:    //pour 20°C à 30°C On fait du vert
    analogWrite(BLUEPIN,0);
    analogWrite(REDPIN,0);
    analogWrite(GREENPIN,255);
    Serial.println("Cas 3");
    Serial.println("20*C - 30*C");
    break;
    
 case 4:    //pour 30°C à 40°C On fait du orange
    analogWrite(BLUEPIN,0);
    analogWrite(REDPIN,220);
    analogWrite(GREENPIN,50);
    Serial.println("Cas 4");
    Serial.println("30*C - 40*C");
    break;
    
 case 5:    //pour 40°C et plus on fait du rouge
    analogWrite(BLUEPIN,0);
    analogWrite(REDPIN,255);
    analogWrite(GREENPIN,0);
    Serial.println("Cas 5");
    Serial.println("40*C & +");
    break;
  }
  
  delay(300);//On laisse le temps à l'arduino pour qu'il agisse en conséquence

  if (isnan(tempDHT) || isnan(humi)) 
    {
    Serial.println("DHT ERREUR !!!");//on revérifie le dht
    } else {
    Serial.print("Humidite: ");//on imprime les valeurs dans le serial si le lcd ne marche pas cela permet de débeguer...
    Serial.print(humi);
    Serial.print(" %\t");
    Serial.print("Temperature DHT: "); 
    Serial.print(tempDHT);
    Serial.println(" *C");
  }
    Serial.print("Temperature BMP = ");// a partir d'ici ce sont les données du bmp085
    Serial.print(bmp.readTemperature());
    Serial.println(" *C");   
    Serial.print("Pression = ");
    Serial.print(bmp.readPressure());
    Serial.println(" Pa");
    Serial.print("Altittude = ");
    Serial.print(bmp.readAltitude());
    Serial.println(" metres");
    Serial.print("Real altitude = ");
    Serial.print(bmp.readAltitude(101500));
    Serial.println(" metres");
    Serial.print("Temperature Moyenne = ");
    Serial.print(tmoy);
    Serial.println(" *C");
    Serial.println();
    
    Serial.print("Nous Sommes Le = ");
    DateTime now = RTC.now();// a partir d'ici ce sont les données du DS1307
    Serial.print(now.year(), DEC);
    Serial.print('/');
    Serial.print(now.month(), DEC);
    Serial.print('/');
    Serial.print(now.day(), DEC);
    Serial.print(' ');
    Serial.print(now.hour(), DEC);
    Serial.print(':');
    Serial.print(now.minute(), DEC);
    Serial.print(':');
    Serial.print(now.second(), DEC);
    Serial.println();
    Serial.print("Depuis Minuit 1/1/1970 = ");
    Serial.print(now.unixtime());
    Serial.print("s = ");
    Serial.print(now.unixtime() / 86400L);
    Serial.println("d");
    // calcul d'une date dans 7jours
    DateTime future (now.unixtime() + 7 * 86400L + 30);
    Serial.print("Maintenant + 7 Jours + 30 Secondes: ");
    Serial.print(future.year(), DEC);
    Serial.print('/');
    Serial.print(future.month(), DEC);
    Serial.print('/');
    Serial.print(future.day(), DEC);
    Serial.print(' ');
    Serial.print(future.hour(), DEC);
    Serial.print(':');
    Serial.print(future.minute(), DEC);
    Serial.print(':');
    Serial.print(future.second(), DEC);
    Serial.println();
    Serial.println();
    
  // on passe à l'affichage sur le lcd
  lcd.clear();
  lcd.print(now.day(), DEC); //On affiche le jour
  lcd.print('/');
  lcd.print(now.month(), DEC); //On affiche le mois
  lcd.print('/');
  lcd.print(now.year(), DEC); //On affiche l'annee
  lcd.print("     "); //espace
  lcd.print(now.hour(), DEC); //On affiche l'heure
  lcd.print(':');
  lcd.print(now.minute(), DEC); //On affiche les minutes
  lcd.setCursor(0, 1); //On passe à la seconde ligne
  lcd.print(tmoy); //On affiche notre moyenne de température
  lcd.print("*C"); //On affiche l'unité de température
  lcd.print("    "); //espace
  lcd.print("%Humi:"); //On affiche le % d'humidité
  lcd.print(humi); //On affiche notre moyenne de température
  lcd.setCursor(0, 2); //On passe à la troisieme ligne
  lcd.print("Pression:  "); //On affiche la pression atmosphérique
  lcd.print(bmp.readPressure()); //on lit la valeur de pression du BMP085
  lcd.print("Pa"); //On affiche l'unité de pression : Pascal
  lcd.setCursor(0, 3); //On passe à la quatrième ligne
  lcd.print("Alti:"); //On affiche l'altitude
  lcd.print(alti); //On lit la valeur depuis le BMP085
  lcd.print("m"); //On affiche l'unité d'altitude : mètres
  lcd.print("  "); //espace
  lcd.print("%Lumi:"); //On affiche le % de lumiere
  lcd.print(lumilcdValue); //On affiche la valeur calculé plus haut
  
  delay(15000);// on patiente 15 secondes ^^
  lcd.clear(); // on nettoie le lcd pour les prochaines données 
  
}

