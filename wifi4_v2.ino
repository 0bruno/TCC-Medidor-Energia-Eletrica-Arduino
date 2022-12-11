#define BLYNK_PRINT Serial// BLINK
#include <SPI.h>
#include <ESP8266_Lib.h> //INCLUSÃO DE BIBLIOTECA
#include <BlynkSimpleShieldEsp8266.h> //INCLUSÃO DE BIBLIOTECA
#include <LiquidCrystal_I2C.h> //INCLUSÃO DE BIBLIOTECA
#include "EmonLib.h" //INCLUSÃO DE BIBLIOTECA


char auth[] = "ReJxVlDvlwJylSubx-jiEVOFsCdSeDq-"; //TOKEN GERADO PELO APLICATIVO MOBILE
char ssid[] = "ODIN"; //VARIÁVEL QUE ARMAZENA O NOME DA REDE SEM FIO EM QUE VAI CONECTAR
char pass[] = "chi81247405"; //VARIÁVEL QUE ARMAZENA A SENHA DA REDE SEM FIO EM QUE VAI CONECTAR


#define ESP8266_BAUD 9600        //ESP8266 baud rate:

ESP8266 wifi(&Serial);           //PASSO OS PARÂMETROS PARA A FUNÇÃO

EnergyMonitor emon1;             //CRIA UMA INSTÂNCIA emon1

EnergyMonitor emon2;             //criar outra instancia para a quantidade de dispositivos a ser medidos EnergyMonitor emon2; EnergyMonitor emon3; ......

EnergyMonitor EnerMonitor;       //CRIA uma INSTANCIA EnerMonitor

EnergyMonitor EnerMonitor2;      //CRIA uma INSTANCIA EnerMonitor

#define VOLT_CAL 211.6           //VALOR DE CALIBRAÇÃO (DEVE SER AJUSTADO EM PARALELO COM UM MULTÍMETRO)
#define InputSCT1 0              // define o canal analógico para o sinal do SCT = pino A8
#define InputSCT2 2              // define o canal analógico para o sinal do SCT = pino A10
#define I_calibration 60         // fator de calibração da corrente


LiquidCrystal_I2C lcd(0x27,2,1,0,4,5,6,7,3, POSITIVE); //ENDEREÇO DO I2C E DEMAIS INFORMAÇÕES
float Irms = 0;
float Irms1 = 0;
float Irms2 = 0;
//float Tempo = 0;
float kwh1 = 0;

//==================================================================================================================================

  //INICIALIZANDO VARIAVEIS PRA MEDIR A MINIMA E MAXIMA TENSAO DAS 2 FASES
  
  //FASE1
  
  float tensaoMaxima1 = 0;
  float tensaoMinima1 = 140;

  //FASE 2
  
  float tensaoMaxima2 = 0;
  float tensaoMinima2 = 140;

//=====================================SETUP=============================================================================================

void setup(){
 Serial.begin(9600); //INICIALIZA A SERIAL
 

 emon1.voltage(1, VOLT_CAL, 1.7); //PASSA PARA A FUNÇÃO OS PARÂMETROS (PINO ANALÓGIO 9/ VALOR DE CALIBRAÇÃO / MUDANÇA DE FASE)
 emon2.voltage(3, VOLT_CAL, 1.7); //PASSA PARA A FUNÇÃO OS PARÂMETROS (PINO ANALÓGIO 11/ VALOR DE CALIBRAÇÃO / MUDANÇA DE FASE)
 
//replicar o codigo para cada instancia de Tensão.
  
 lcd.begin (20,4);                                  //SETA A QUANTIDADE DE COLUNAS(20) E O NÚMERO DE LINHAS(2) DO DISPLAY
 lcd.setBacklight(HIGH);                            //LIGA O BACKLIGHT (LUZ DE FUNDO)EnergyMonitor EnerMonitor;
 lcd.clear();                                       // limpa tela do LCD

 Blynk.begin(auth, wifi, ssid, pass);              //INICIALIZA A COMUNICAÇÃO BLYNK INFORMANDO OS PARÂMETROS

 EnerMonitor.current(InputSCT1, I_calibration);   // configura pino SCT1 e fator de calibração
 EnerMonitor2.current(InputSCT2, I_calibration);   // configura pino SCT2 e fator de calibração

  lcd.setCursor(0,0);
  lcd.print("***TESLA MONITOR***");
  lcd.setCursor(0,1);                       // seleciona coluna 0, linha 0
  lcd.print("Irms(A)=");                    // mostra texto
  lcd.setCursor(0,2);                       // seleciona coluna 0, linha 0
  lcd.print("Tensao(V)=");                  // mostra texto
  lcd.setCursor(0,3);                       // seleciona coluna 0, linha 1
  lcd.print("Energia(Wh)=");                // mostra texto
}
 
//=====================================LOOP=============================================================================================

void loop(){

  // INICIA A BIBLIOTECA BLINK
  Blynk.run();

  emon1.calcVI(17,2000);                 //FUNÇÃO DE CÁLCULO (17 SEMICICLOS, TEMPO LIMITE PARA FAZER A MEDIÇÃO)
  emon2.calcVI(17,2000);                 //FUNÇÃO DE CÁLCULO (17 SEMICICLOS, TEMPO LIMITE PARA FAZER A MEDIÇÃO)     
  float supplyVoltage1   = emon1.Vrms;   //VARIÁVEL RECEBE O VALOR DE TENSÃO RMS OBTIDO
  float supplyVoltage2   = emon2.Vrms;   //VARIÁVEL RECEBE O VALOR DE TENSÃO RMS OBTIDO
  
  float supplyVoltage = (supplyVoltage1 + supplyVoltage2)/2; // Calcula a média das supply voltage das duas fases


    //FASE 1

  if (supplyVoltage1 < tensaoMinima1 ){
  tensaoMinima1 = supplyVoltage1;
  }

  if ( supplyVoltage1 > tensaoMaxima1 ){
    tensaoMaxima1 = supplyVoltage1;
  }

    //FASE 2

  if (supplyVoltage2 < tensaoMinima2 ){
  tensaoMinima2 = supplyVoltage2;
  }

  if ( supplyVoltage2 > tensaoMaxima2 ){
    tensaoMaxima2 = supplyVoltage2;
  }
 

 //CALCULANDO A CORRENTE DAS DUAS FASES 

  Irms1 = EnerMonitor.calcIrms(1240);
  Irms2 = EnerMonitor2.calcIrms(1240);
  Irms = (Irms1 + Irms2);  // calculo da corrente RMS
   
 //DEFININDO O CALCULO DE KHW PRAS 2 FASES
 
  float w1 = (Irms1*supplyVoltage1)/3600;
  float w2 = (Irms2*supplyVoltage2)/3600;
  float kwh = (w1+w2)/1000;
  kwh1 = (kwh1 + kwh); 
  
  //LIGANDO OS PINOS DO APLICATIVO AO ARDUINO
  
  Blynk.virtualWrite (V3, kwh1);

 //MOSTRANDO NO BLYNK AS 2 FASES DA TENSAO

Blynk.virtualWrite (V1, supplyVoltage1);
Blynk.virtualWrite (V2, supplyVoltage2);

//MOSTRANDO NO BLYNK AS 2 FASES DA CORRENTE

Blynk.virtualWrite (V8, Irms1);
Blynk.virtualWrite (V9, Irms2);

//MOSTRANDO NO BLYNK TENSAO MINIMA E MAXIMA DA FASE 1 E 2 

Blynk.virtualWrite (V4, tensaoMinima1);
Blynk.virtualWrite (V5, tensaoMaxima1);
Blynk.virtualWrite (V6, tensaoMinima2);
Blynk.virtualWrite (V7, tensaoMaxima2);

  
  lcd.setCursor(15,1);                        // seleciona coluna 9, linha 0
  lcd.print(Irms,1);                           // mostra valor da corrente
  lcd.setCursor(15,2);                        // seleciona coluna 9, linha 1
  lcd.print(supplyVoltage,1);                  // mostra a tensão
  lcd.setCursor(15,3);                        // seleciona coluna 9, linha 2
  lcd.print(kwh1,1);                            // calculo de Watt/hora
  //Tempo++;
  delay(1000);                               // atraso de 1 segundo
 
}
  
