/*************************************************************

  This is a simple demo of sending and receiving some data.
  Be sure to check out other examples!
 *************************************************************/

// Template ID, Device Name and Auth Token are provided by the Blynk.Cloud
// See the Device Info tab, or Template settings
#define BLYNK_TEMPLATE_ID           "TMPLIL1djL1G"
#define BLYNK_DEVICE_NAME           "Quickstart Device"
#define BLYNK_AUTH_TOKEN            "9NkJMtezA0FQYwXTW_CDin1HRbRmaO_I"


// Comment this out to disable prints and save space
#define BLYNK_PRINT Serial
//Aqui se configutan los pines dende debemos conectar el sensor de temperatura y el SSR
#define pinData 5
#define SSR 4
#define tiempoCiclo 100
#define Button1 13
#define Button2 15
#define Bomba1  12
#define Bomba2  14

// Aqui se configuran los pines donde debemos conectar el sensor de distancia
#define TRIGGER_PIN 0
#define ECHO_PIN 2
#define MAX_DISTANCE 400 // Distancia máxima


#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <PID_v1.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <NewPing.h>


// Your WiFi credentials.
// Set password to "" for open networks.
char ssid[] = "OmitarMovil";
char pass[] = "Omar6666";
char auth[] = BLYNK_AUTH_TOKEN;


double Setpoint, Input, Output;                                     // Define Variables
double Kp = 4, Ki = 6, Kd = 400;                                   // Especifica parametros iniciales
float temperatura = 20;

unsigned long respuestaUltimaTemperatura = 0;
unsigned long lastPIDCalculation = 0;
float prevTemperature = -9999.0;


NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE);                 //Crear el objeto de la clase NewPing
PID myPID(&Input, &Output, &Setpoint, Kp, Ki, Kd, DIRECT);
OneWire ourWire(pinData);                                           // Pin como bus para la comunicación OneWire
DallasTemperature sensors(&ourWire);                                //Se instancia la librería DallasTemperature

int buttonState[] = {0, 0};                                         // Estado global de los botones


//BlynkTimer timer;

// This function is called every time the Virtual Pin 4 state changes
BLYNK_WRITE(V4)         // MOD Temperatura
{
  // Set incoming value from pin V0 to a variable
  double value = param.asDouble();

  // Update state
  //Blynk.virtualWrite(V3, value);
  Setpoint = value;
  Serial.println("***** TEMPERATURA: " + String(Setpoint) + " *****");
}


BLYNK_WRITE(V5)         // MOD Tanque
{
  // Set incoming value from pin V0 to a variable
  double value = param.asDouble();

  // Update state
  Serial.println("***** TANQUE: " + String(value) + " *****");
  Blynk.virtualWrite(V5, value);
}


BLYNK_WRITE(V3)         // BOMBA 1
{
  // Set incoming value from pin V0 to a variable
  int value = param.asInt();

  // Update state
  buttonState[0] = value;
  digitalWrite(Bomba1, value);
  Serial.println("***** BOMBA 1: " + String(value) + " *****");
}


BLYNK_WRITE(V0)         // BOMBA 2
{
  // Set incoming value from pin V0 to a variable
  int value = param.asInt();

  // Update state
  buttonState[1] = value;
  digitalWrite(Bomba2, value);
  Serial.println("***** BOMBA 2: " + String(value) + " *****");
}



void setup()
{
  // Debug console
  Serial.begin(115200);
  Serial.println("Inicia el loop!!!!!!");
  Blynk.begin(auth, ssid, pass);
  
  Setpoint = 30.0;                                                 // initialize the variables we're linked to
  myPID.SetOutputLimits(0, tiempoCiclo);
  myPID.SetSampleTime(tiempoCiclo);
  myPID.SetMode(AUTOMATIC);
  //Serial.begin(115200);                                             // Aranca comunicacion serie
  pinMode(SSR, OUTPUT);
  digitalWrite(SSR, LOW);

  sensors.begin();                                                // arranca librerias
  pinMode(Bomba1, OUTPUT);
  pinMode(Bomba2, OUTPUT);
  pinMode(Button1, INPUT);
  pinMode(Button2, INPUT);
}

void loop()
{
  //Serial.println("Inicia el loop!!!!!!");
  if (millis() - respuestaUltimaTemperatura >= tiempoCiclo) {
    temperatura = sensors.getTempCByIndex(0);
    Input = (double)temperatura;

    myPID.Compute();
    lastPIDCalculation = millis();
    Serial.print(temperatura);
    Serial.print("  ");
    Serial.println();
    Blynk.virtualWrite(V1, temperatura);


    sensors.requestTemperatures();
    respuestaUltimaTemperatura = millis();
  }
  control();

  if (!(buttonState[0]))
    digitalWrite(Bomba1, digitalRead(Button1));
  if (!(buttonState[1]))
    digitalWrite(Bomba2, digitalRead(Button2));


  float distancia = sonar.ping_cm();
  Blynk.virtualWrite(V6, 22.22);
  Serial.println(String(22.22) + "\t\t");

    
  Blynk.run();
  // You can inject your own code or combine it with other sketches.
  // Check other examples on how to communicate with Blynk. Remember
  // to avoid delay() function!
}


void control() {

  if ((millis() <= (lastPIDCalculation + Output)) || (Output == tiempoCiclo)) {
    // Power on:
    digitalWrite(SSR, HIGH);
    //Serial.println("SSR On");
  } else {
    // Power off:
    digitalWrite(SSR, LOW);
    //Serial.println("SSR Off");
  }
}
