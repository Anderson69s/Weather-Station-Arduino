#include <LiquidCrystal.h> //On inclue les différentes librairies
#include <Wire.h>
#include <Adafruit_BMP085.h>
#include "DHT.h"
#include "RTClib.h"

#define DHTPIN 2 //on définit le pin du DHT
#define DHTTYPE DHT22 //on définit le type du DHT peut aussi etre DHT11 ou DHT21   

#define REDPIN 5 //On définit le Pin pour les LED rouge
#define GREENPIN 6 //On définit le Pin pour les LED bleu
#define BLUEPIN 3 //On définit le Pin pour les LED verte
//#define FADESPEED 20 //On définit un indicateur de temps pour la rotation des couleurs et le rafraissichement des données

LiquidCrystal lcd(7, 8, 9, 10, 11, 12); //On définie les Pins pour l'écran LCD

const int lumiPin = A0; //On définit le Pin de la photorésistance 

Adafruit_BMP085 bmp;//on démarre les variables de lesibraries
DHT dht(DHTPIN, DHTTYPE); 
RTC_DS1307 RTC;

void setup() {
  Serial.begin(19200); //on demarre une communication serial
  dht.begin();
  Wire.begin();
  if (!bmp.begin()) {
	Serial.println("BMP085 ERREUR !!!");//on vérifie si le bmp085 fonctionne
	while (1) {}
  }
  Serial.println("DHT21 TEST");//on vérifie si le DHT fonctionne

  RTC.begin();
  if (! RTC.isrunning()) {
    Serial.println("RTC ERREUR !!!");// la ligne suivante définit l'heure du RTC à celle ou le code a été compilé
    RTC.adjust(DateTime(__DATE__, __TIME__));
  }
  
  //On définie les Pins en mode sortie pour le ruban LED 
   pinMode(REDPIN, OUTPUT);
   pinMode(GREENPIN, OUTPUT);
   pinMode(BLUEPIN, OUTPUT);
   
  lcd.begin(20, 4); //On définit le nombre de caractères par ligne et le nombre de lignes du LCD 
  
}
  
void loop() {
  
  int lumiValue = analogRead(lumiPin); //On lit la valeur de la photorésistance 

/* Pour utiliser la valeur de la photorésistance, il faut faire un peu de math. 
La photorésistance fournit un nombre à 10-bits. 
Mais analogWrite() utilise des chiffres à 8-bits.
Il faut diviser la valeur par 10 pour ramener celle-ci à un % */
  int lumilcdValue = lumiValue/10;
  
  int alti = bmp.readAltitude();//on ne garde que la partie entière de la valeur pour l'altittude sa gagne toujours deux caracteres sur le lcd...

  int h = dht.readHumidity(); //on definit h pour stocker la valeur d'humidité
  int t = dht.readTemperature();//on definit t pour stocker la valeur de temperature
  float temp = bmp.readTemperature();
  float tfi = (temp + t)/2;
  delay(300);

  if (isnan(t) || isnan(h)) 
    {
    Serial.println("DHT ERREUR !!!");//on revérifie le dht
    } else {
    Serial.print("Humidite: ");//on imprime les valeurs dans le serial si le lcd ne marche pas cela permet de débeguer...
    Serial.print(h);
    Serial.print(" %\t");
    Serial.print("Temperature: "); 
    Serial.print(t);
    Serial.println(" *C");
  }
    Serial.print("Temperature = ");// a partir d'ici ce sont les données du bmp085
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
    Serial.println("temp moyenne");
    Serial.print(tfi);
    Serial.println(" *C");
    Serial.println();
    
    DateTime now = RTC.now();// a partir d'icic ce sont les données du DS1307
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
  lcd.print(tfi); //On affiche notre moyenne de température
  lcd.print("*C"); //On affiche l'unité de température
  lcd.print("    "); //espace
  lcd.print("%Humi:"); //On affiche le % d'humidité
  lcd.print(h); //On affiche notre moyenne de température
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
   
  delay(15000);// on patiente ^^
  lcd.clear(); // on nettoie le lcd pour les prochaines données 
  
}
