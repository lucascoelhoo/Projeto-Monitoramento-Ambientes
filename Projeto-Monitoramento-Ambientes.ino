/* Programa: Monitoramento de ambientes técnicos
   Autor: Lucas Coelho de Almeida
   Plataforma: Arduíno (UNO ou Mega) + shield Ethernet WIZNET W5100
   Linguagem: C/C++
   Descrição do sistema: O sistema é capaz de fornecer dados de uma quantidade de sensores limitada apenas às portas da plataforma através de
   varios metodos como web, serial, telnet, snmp e agente-zabbix.

   OBS.1: É indicado que o intervalo entre as requisições via Telnet seja de 30 segundos (30000 millis)
   OBS.2: A página HTTP que ele fornece se atualiza sozinha a cada 30 segundos (ou 30000 millis)
   OBS.3: A sintaxe para os comandos telnet estão nos comentários da função " generateAction " (use "Crtl+F" e busque o nome da função).
*/
#include <Wire.h>
/*********INÍCIO*************MONITORAMENTO DE MEMÓRIA***************************************************************************************************************************************************************************/
#include <MemoryFree.h> //Biblioteca usada como auxílio no monitoramento em tempo real da quantidade de memória disponível.
/*********FIM*************MONITORAMENTO DE MEMÓRIA******************************************************************************************************************************************************************************/
/*********INÍCIO*************SNMP***************************************************************************************************************************************************************************/
#include <SPI.h>
#include <Agentuino.h>

//
// tkmib - linux mib browser
//
// RFC1213-MIB OIDs
// .iso (.1)
// .iso.org (.1.3)
// .iso.org.dod (.1.3.6)
// .iso.org.dod.internet (.1.3.6.1)
// .iso.org.dod.internet.mgmt (.1.3.6.1.2)
// .iso.org.dod.internet.mgmt.mib-2 (.1.3.6.1.2.1)
// .iso.org.dod.internet.mgmt.mib-2.system (.1.3.6.1.2.1.1)
// .iso.org.dod.internet.mgmt.mib-2.system.sysDescr (.1.3.6.1.2.1.1.1)
static const char sysDescr[20] PROGMEM     = "1.3.6.1.2.1.1.1.0";  // read-only  (DisplayString)

// .iso.org.dod.internet.mgmt.mib-2.system.sysObjectID (.1.3.6.1.2.1.1.2)
static const char sysObjectID[20] PROGMEM   = "1.3.6.1.2.1.1.2.0";  // read-only  (ObjectIdentifier)

// .iso.org.dod.internet.mgmt.mib-2.system.sysUpTime (.1.3.6.1.2.1.1.3)
static const char sysUpTime[20] PROGMEM     = "1.3.6.1.2.1.1.3.0";  // read-only  (TimeTicks)

// .iso.org.dod.internet.mgmt.mib-2.system.sysContact (.1.3.6.1.2.1.1.4)
static const char sysContact[20] PROGMEM    = "1.3.6.1.2.1.1.4.0";  // read-write (DisplayString)

// .iso.org.dod.internet.mgmt.mib-2.system.sysName (.1.3.6.1.2.1.1.5)
static const char sysName[20] PROGMEM       = "1.3.6.1.2.1.1.5.0";  // read-write (DisplayString)

// .iso.org.dod.internet.mgmt.mib-2.system.sysLocation (.1.3.6.1.2.1.1.6)
static const char sysLocation[20] PROGMEM   = "1.3.6.1.2.1.1.6.0";  // read-write (DisplayString)

// .iso.org.dod.internet.mgmt.mib-2.system.sysServices (.1.3.6.1.2.1.1.7)
static const char sysServices[20] PROGMEM   = "1.3.6.1.2.1.1.7.0";  // read-only  (Integer)

// Arduino defined OIDs
// .iso.org.dod.internet.private (.1.3.6.1.4)
// .iso.org.dod.internet.private.enterprises (.1.3.6.1.4.1)
// .iso.org.dod.internet.private.enterprises.arduino (.1.3.6.1.4.1.36582)
// .iso.org.dod.internet.private.enterprises.arduino.value.valA0-A5 (.1.3.6.1.4.1.36582.3.1-6)
/*
  static char valA0[] PROGMEM   = "1.3.6.1.4.1.36582.3.1.0";  // read-only  (Integer)

  static char valA1[] PROGMEM   = "1.3.6.1.4.1.36582.3.2.0";  // read-only  (Integer)

  // .iso.org.dod.internet.private.enterprises.arduino.value.valD0-D13 (.1.3.6.1.4.1.36582.3.7-20)
  static char valD0[] PROGMEM   = "1.3.6.1.4.1.36582.3.7.0";  // read-only  (Integer)

  static char valD1[] PROGMEM   = "1.3.6.1.4.1.36582.3.8.0";  // read-only  (Integer)
*/
//
//static const byte temperature[24] PROGMEM = "1.3.6.1.2.1.1.1.10";  // read-only  (Integer)
//
//static const char humidity[24] PROGMEM = "1.3.6.1.2.1.2.4.20";  // read-only  (Integer)
//
//static const char temp_humidity[24] PROGMEM = "1.3.6.1.2.1.1.1.11";  // read-only  (Integer)
//
//static const char media_corredor[24] PROGMEM = "1.3.6.1.4.1.36582.0.13.";  // read-only  (Integer)
//
//static const char media_g[24] PROGMEM = "1.3.6.1.4.1.36582.0.137.";  // read-only  (Integer)
//
//
//static const char media_h[24] PROGMEM = "1.3.6.1.4.1.36582.0.138.";  // read-only  (Integer)
//
//static const char presence[24] PROGMEM = "1.3.6.1.4.1.36582.0.16.";  // read-only  (Integer)
//
//
//static const char access[24] PROGMEM = "1.3.6.1.4.1.36582.0.1.";  // read-only  (Integer)
//
//static const char inund[24] PROGMEM = "1.3.6.1.4.1.36582.0.9.";  // read-only  (Integer)
//
//static const char light[24] PROGMEM = "1.3.6.1.4.1.36582.0.12.";  // read-only  (Integer)
//
//static const char lux[24] PROGMEM = "1.3.6.1.4.1.36582.0.122124.";  // read-only  (Integer)

static const char ENTERPRISE_NUMBER[20] PROGMEM = "1.3.6.1.4.1.49191.";

#define SIZE_ENTERPRISE_NUMBER 18

#define temperature_dallas 20  // read-only  (Integer)

#define humidity  8  // read-only  (Integer)

#define temp_humidity  820  // read-only  (Integer)

#define media_corredor  13  // read-only  (Integer)

#define media_g  137 // read-only  (Integer)


#define media_h  138  // read-only  (Integer)

#define media_ht 13820 // m->13, h->8 , t->20

#define presence  16  // read-only  (Integer)

#define access  1  // read-only  (Integer)

#define inund  9 // read-only  (Integer)

#define smoke_value 6

#define light  12  // read-only  (Integer)

#define lux_value 122124  // read-only  (Integer)

#define sound_db 19 //s -> som: s -> 19 no alfabeto

#define vibration_snmp 22 //vibration: v -> 22 n alfabeto

#define flame_snmp 3 // chama: c -> 3 no alfabeto

#define door_snmp 1916 //sensor de porta: s -> 19 / p -> 16

#define pressao_snmp 1618519 //pres: p -> 16 , r -> 18 , e -> 5 , s -> 19

#define altitude_snmp 11220 //alt: a -> 1 , l -> 12 , t -> 20

#define temp_interna_snmp 209 //ti: t -> 20 , i ->9

#define current_snmp 32118 //cur: c->3 , u->21 , r->18

#define power_ap_snmp 16116 //pap: p->16, a->1 , p->16

#define power_re_snmp 16185 //pre: p->16 , r->18 , e->5

#define power_im_snmp 16913 //pim: p->16, i->9 , m->13

#define consumo_ap_snmp 3116 //cap: c->3 , a->1 , p->16

#define consumo_re_snmp 3185 //cre: c->3 , r->18 , e->5

#define consumo_im_snmp 3913 //cim: c->3 , i->9, m->13

#define current_snmp_total 3211820 //curt: c->3 , u->21 , r->18 , t->20

#define power_ap_snmp_total 1611620 //papt: p->16, a->1 , p->16 , t->20

#define consumo_ap_snmp_total 311620 //capt: c->3 , a->1 , p->16 , t->20

#define current_snmp_neutro 3211814 //curn: c->3 , u->21 , r->18 , n->14



//#define power_re_snmp_total 1618520 //pre: p->16 , r->18 , e->5 , t->20
//
//#define power_im_snmp_total 1691320 //pim: p->16, i->9 , m->13 , t->20
//
//#define consumo_re_snmp_total 318520 //cre: c->3 , r->18 , e->5 , t->20
//
//#define consumo_im_snmp_total 391320 //cim: c->3 , i->9, m->13 , t->20

#define just_int 0 //enviar apenas valor inteiro

#define just_string_value 1 //enviar valor em forma de string (alguns softwares conseguem usar, para entender float)

char buffer[30];




// Arduino defined OIDs
// .iso.org.dod.internet.private (.1.3.6.1.4)
// .iso.org.dod.internet.private.enterprises (.1.3.6.1.4.1)
// .iso.org.dod.internet.private.enterprises.arduino (.1.3.6.1.4.1.36582)
//
//
// RFC1213 local values
static char locDescr[]              = "LCA Systems Infrastructure Monitoring Module - v15.2";  // read-only (static)
//static char locObjectID[]         = "1.3.6.1.3.2009.0";                       // read-only (static)
static uint32_t locUpTime           = 0;                                        // read-only (static)
static char locContact[]          = "LCA Systems - atendimento.lca.systems@gmail.com";       // should be stored/read from EEPROM - read/write (not done for simplicity)
static char locName[]             = "LCA Central Module";                              // should be stored/read from EEPROM - read/write (not done for simplicity)
static char locLocation[]         = "Brasília, DF - BRAZIL";                         // should be stored/read from EEPROM - read/write (not done for simplicity)
static int32_t locServices          = 12;                                        // read-only (static)



uint32_t prevMillis = millis();
char oid[SNMP_MAX_OID_LEN];
SNMP_API_STAT_CODES api_status;
SNMP_ERR_CODES status;


char aux_command[5] = {'\0', '\0', '\0', '\0', '\0'};
char aux_corredor[5] = {'\0', '\0', '\0', '\0', '\0'};
char aux_sensor[5] = {'\0', '\0', '\0', '\0', '\0'};

byte corredor_snmp;
byte sensor_snmp;
byte type_response;
int type_command;
/*********FIM*************SNMP******************************************************************************************************************************************************************************/

/*********INÍCIO*************Sensores de Temperatura Dallas*********************************************************************************************************************************************************************/
#include <OneWire.h> //Biblioteca para usar sensores endereçáveis
#include <DallasTemperature.h> //Biblioteca referente aos sensores Dallas DS18B20

//Precisão na medição de temperatura
#define TEMPERATURE_PRECISION 13

//USAR APENAS PORTAS DIGITAIS 2, 3, 5, 6, 7, 8 E 9!!!!!!!!! (A porta 4 é usada pelo sistema de leitura de cartões SD e as portas 10,11,12 e 13 são usadas pela interface Ethernet)
#define NUM_ONEWIRE_PORTS 1

//Array de valores referentes às portas digitais disponíveis para uso após o acoplamento do shield Ethernet
uint8_t digitalPorts[] = {44};

//Ponteiro duplo que guardará o endereço dos arrays de temperaturas referentes à cada corredor
float** tempC;
//Ponteiro que guardará o array de valores das médias de cada corredor
float* media_leitura;
//Variável que guarda a media geral da sala
float media_geral;

//Ponteiro duplo para guardar o endereço dos arrays de objetos DallasTemperature
DallasTemperature **nodos_sensores_temperatura;
//Ponteiro triplo que será usado para guardar os endereços dos múltiplos sensores usados em cada corredor
uint8_t*** devices;

//Ponteiro duplo que servirá de auxílio para a verificação das portas digitais sendo usadas pelos sensores
OneWire** devs;
//Ponteiro duplo que servirá de auxílio para a verificação das portas digitais sendo usadas pelos sensores
DallasTemperature** dallastemp;

byte real_num_onewire_ports;  //Número real de portas sendo usadas por sensores
byte* portas_ocupadas;        //Ponteiro usado para guardar quais portas estão sendo usadas pelos sensores
/*********FIM****************Sensores de Temperatura Dallas*********************************************************************************************************************************************************************/

/********INÍCIO**********************Sensores de Umidade e Temperatura DHT******************************************************************************************************************************************************/
#include "DHT.h"

#define DHTTYPE DHT11 // Tipo de sensor DHT sendo usado, no caso, DHT11
#define NUM_ANALOG_PORTS 1

DHT **humidity_sensors; //Ponteiro duplo que guardará os arrays de objetos DHT
//Ponteiro duplo que será usado na verificação das portas analógicas sendo usadas pelos sensores DHT
DHT **auxiliar;
uint8_t analogPorts[] = {A8};
DHT *dht;
int real_num_hum_sensors;

//Variáveis que guardam temperatura e número de sensores, respectivamente

float* umidade; //variável que guardará o valor da última umidade lida
float* temp_sensor_umidade; //variável que guardará a última temperatura do sensor de umidade lida
float media_umidade;

float media_temp_dht;
/********FIM**********************Sensores de Umidade e Temperatura DHT*********************************************************************************************************************************************************/

/********INÍCIO********************Ethernet*************************************************************************************************************************************************************************************/
#include <Ethernet.h>
#include<String.h>
#include <utility/w5100.h>
#include <utility/socket.h>
#include <EEPROM.h>


#define TYPE_CONNECTION EEPROM.read(6) //0 -> CABO IP FIXO / 1 -> CABO DHCP / 2 ->WIFI DHCP
#define TIMEOUT_CONNECTION 300000 //Tenta se conectar via DHCP ou wifi por 5 min, antes de usar o ultimo IP valido (DHCP) ou desistir (wifi)

//IP sendo lido (cada byte foi guardado na posição 0,1,2,3 da EEPROM)
static byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xEE};
//IP sendo lido (cada byte foi guardado na posiÃ§Ã£o 0,1,2,3 da EEPROM)
IPAddress ip(EEPROM.read(2), EEPROM.read(3), EEPROM.read(4), EEPROM.read(5)); //ip fixo fornecido pelo pessoal da infraestrutura de rede

////Gateway sendo lido (cada byte foi guardado na posiÃ§Ã£o 4,5,6,7 da EEPROM)
//IPAddress gateway(EEPROM.read(4), EEPROM.read(5), EEPROM.read(6), EEPROM.read(7));
////Subnet sendo lido (cada byte foi guardado na posiÃ§Ã£o 8,9,10,11 da EEPROM)
//IPAddress subnet(EEPROM.read(8), EEPROM.read(9), EEPROM.read(10), EEPROM.read(11));
//byte Socket; //guarda o numero d socket de um dado cliente
////Caso deseje mudar os endereços, use um sketch diferente e grave na posição de memória adequada cada valor do IP ou Gateway ou ainda Subnet e servidor DNS, é recomendável não alterar estes campos



/********FIM***********************Ethernet*************************************************************************************************************************************************************************************/

/********INÍCIO********************Web Server***********************************************************************************************************************************************************************************/
//Inicia um servidor web na porta 80 e cria um cliente vazio
EthernetServer server(80);
EthernetClient client = 0;

//Threesholds para temperatura
#define lowlowThreeshold_temp 17.0
#define lowThreeshold_temp 26.0
#define highThreeshold_temp 28.0

//Threesholds para umidade
#define lowThreeshold_humidity 40
#define highThreeshold_humidity 60
#define lowlowThreeshold_humidity 30
#define highhighThreeshold_humidity 70
/********FIM********************Web Server**************************************************************************************************************************************************************************************/

/********INÍCIO********************telnet Server********************************************************************************************************************************************************************************/
//Inicia o servidor Telnet para o telnet na porta 10050 e cria um cliente vazio
EthernetServer server_telnet(10050);
EthernetClient client_telnet = 0;
EthernetClient client_telnet_aux = 0;
//Define os parâmentros necessários à leitura dos comandos recebidos (CR = Carriage Return e LF=Linefeed)
#define max_command_size 7
#define CR 13
#define LF 10

//Array que guardará os comandos recebidos
char command[7];
byte param = 0;
//boolean alreadyConnected = false; // Indica se o cliente já está conectado ou não ao código
/********FIM********************telnet Server***********************************************************************************************************************************************************************************/

/********INÍCIO********************Sensor de Presença********************************************************************************************************************************************************************************/
byte pir = 29; //Pino ligado ao sensor PIR
byte presenca = 0;
byte media = 0;
byte num_leituras = 0;
byte media_verdadeira = 0;
byte num_vezes = 0;
byte presenca_web = 0;
long tempo_web = 0;
//int acionamento; //Variavel para guardar valor do sensor
/********FIM********************Sensor de Presença***********************************************************************************************************************************************************************************/

/********INÍCIO********************Sensor de Fumaça MQ2********************************************************************************************************************************************************************************/
/************************Hardware Related Macros************************************/
#define         MQ_PIN                       (A15)     //define which analog input channel you are going to use
#define         RL_VALUE                     (5.1)     //define the load resistance on the board, in kilo ohms
#define         RO_CLEAN_AIR_FACTOR          (9.83)  //RO_CLEAR_AIR_FACTOR=(Sensor resistance in clean air)/RO,
//which is derived from the chart in datasheet

/***********************Software Related Macros************************************/
#define         CALIBARAION_SAMPLE_TIMES     (50)    //define how many samples you are going to take in the calibration phase
//#define         CALIBRATION_SAMPLE_INTERVAL  (500)   //define the time interal(in milisecond) between each samples in the
//cablibration phase
//#define         READ_SAMPLE_INTERVAL         (50)    //define how many samples you are going to take in normal operation
#define         READ_SAMPLE_TIMES            (15)     //define the time interal(in milisecond) between each samples in 
//normal operation

/**********************Application Related Macros**********************************/
#define         GAS_LPG                      (0)
#define         GAS_CO                       (1)
#define         GAS_SMOKE                    (2)

/*****************************Globals***********************************************/
float           LPGCurve[3]  =  {2.3, 0.21, -0.47}; //two points are taken from the curve.
//with these two points, a line is formed which is "approximately equivalent"
//to the original curve.
//data format:{ x, y, slope}; point1: (lg200, 0.21), point2: (lg10000, -0.59)
float           COCurve[3]  =  {2.3, 0.72, -0.34};  //two points are taken from the curve.
//with these two points, a line is formed which is "approximately equivalent"
//to the original curve.
//data format:{ x, y, slope}; point1: (lg200, 0.72), point2: (lg10000,  0.15)
float           SmokeCurve[3] = {2.3, 0.53, -0.44}; //two points are taken from the curve.
//with these two points, a line is formed which is "approximately equivalent"
//to the original curve.
//data format:{ x, y, slope}; point1: (lg200, 0.53), point2: (lg10000,  -0.22)
float           Ro           =  10;                 //Ro is initialized to 10 kilo ohms
float val_calibration = 0;
float val_read = 0;
long time_mq2 = 0;
boolean alarms = false;
boolean calibrate = false;
float rs = 0;
byte cont_calibration = 0;
byte cont_read = 0;
#define TIME_NO_ALARMS 1800000L //30 min

#define VOLTAGE_COMPENSATION 130 //Se a tensão no mq2 fosse 5v, a saída analógica ficaria no ponto médio do intervalo 0-1023 na maioria do tempo
//em ar limpo. Logo, como a tensão verificada foi de 3.8 aproximadamente, fez-se uma compensação nas leituras
//analógicas de 130 para que a sensibilidade do sensor fosse aumentada. Porém, é importante notar que o sensor
//seria bem mais acurado caso se observasse sua derivada da mudança dos valores da saída analógica. A sensibilidade
//seria até mesmo comparável com sensores ópticos e sistemas como o stratus.

unsigned int smoke = 0;
unsigned int lpg = 0;
unsigned int co = 0;
int air_quality = 0; //air_quality: variável que será repassada no telnet (contém uma soma de lpg, co e smoke, porém, pode-se
//alterá-la para guardar apenas smoke) - UNIDADE É DADA PELA SOME DE 3 VALORES EM PPM
/********FIM********************Sensor de Fumaça MQ2***********************************************************************************************************************************************************************************/

/********INICIO********************Controle de Acesso***********************************************************************************************************************************************************************************/
String* currentLine;
String* user[2];
byte aux_user = 0;
#define pass "sermad"
String* parameter_name[2];
String* definite_user;
String text_form_action = "<form action='http://" + String(ip[0]) + "." + String(ip[1]) + "." + String(ip[2]) + "." + String(ip[3]) + ":" + String(80) + String("' method=post >");
String* msg_output;
#define MAX_NAME 30 //30 characters max
#define EMPTY " "
/********FIM********************Controle de Acesso***********************************************************************************************************************************************************************************/

/********INICIO********************Sensor de inundaçao***********************************************************************************************************************************************************************************/
#define NUMBER_INUNDACAO 0

#define Threeshold_inundacao 30

#define NO_INUNDACAO 0

byte pino_inundacao[] = {A4};

byte valor_inundacao[sizeof(pino_inundacao) / sizeof(byte)];
/********FIM********************Sensor de inundaçao***********************************************************************************************************************************************************************************/





/********INICIO********************Sensor de Luz***********************************************************************************************************************************************************************************/

/****SENSOR LDR********************************************************************************************************/
#define NUMBER_LDRs 0

byte photocellPin[] = {A0};    // the cell and 10K pulldown are connected to a0

float percent_ldr[sizeof(photocellPin) / sizeof(byte)];
//int lux[NUMBER_LDRs];

float Res = 1.0;            // Resistance used will always be 10 KOhms!!!!!!!!!!!!!!!!!!!1 ALTERAR!!!!!!!!!!!!!!!!!!!!

#define highThreeshold_light 50.0
#define lowThreeshold_light 20.0
// depending of the Resistance used, you could measure better at dark or at bright conditions.
// you could use a double circuit (using other LDR connected to analog pin 1) to have fun testing the sensors.
// Change the value of Res0 depending of what you use in the circuit
/***SENSOR LDR********************************************************************************************************/


/****SENSOR BH1750FVI********************************************************************************************************/
#include <BH1750.h>

#define LUX_SENSOR true //se for falso, não é ele o parametro para saber se a lux está ligada na sala, será o ldr com maior valor


//byte luxPin = {A4 - SDA, A5 - SCL};    // IF UNO
//byte luxPin = {20 - SDA, 21 - SCL};    // IF MEGA
//they wont be used, just for records

uint16_t lux;

//Define o sensor BH1750
BH1750 lightMeter;

#define highThreeshold_lux 500.0
#define lowThreeshold_lux 1.0
// depending of the Resistance used, you could measure better at dark or at bright conditions.
// you could use a double circuit (using other LDR connected to analog pin 1) to have fun testing the sensors.
// Change the value of Res0 depending of what you use in the circuit
/****SENSOR BH1750FVI********************************************************************************************************/


/********FIM********************Sensor de Luz***********************************************************************************************************************************************************************************/




/********INICIO********************Sensor de Som***********************************************************************************************************************************************************************************/
#include "math.h"

#define NUMBER_SOUND 0

byte soundPin[] = {A3};    // the cell and 10K pulldown are connected to a0

float db[sizeof(soundPin) / sizeof(byte)];

#define ADC_SOUND_REF float(40.0)
#define DB_SOUND_REF float(50.0)

#define lowThreeshold_sound float(65.0) // segundo norma para salas de computadores na NBR10152 sobre conforto acustico

#define highThreeshold_sound float(95.0) //pegou o lowthreeshold e aumentou-se em 30db 
/********FIM********************Sensor de Som***********************************************************************************************************************************************************************************/

/********INICIO********************Sensor de Vibracao***********************************************************************************************************************************************************************************/
#define NUMBER_VIBRATION 0

byte vibrationPin[] = {A1};    // the cell and 10K pulldown are connected to a0

float vibration[sizeof(vibrationPin) / sizeof(byte)];
float vibration_samp[sizeof(vibrationPin) / sizeof(byte)];

#define VIBRATION_INTERVAL 1000 //a cada um segundo, ele atualizará os valores dos sensores de vibração

long time_last_vibration = 0;

#define lowThreeshold_vibration float(5.0) // 5 mm de vibração
#define highThreeshold_vibration float(10.0) // 10 mm de vibração

//O sensor de vibração SWVB18010 tem saida analogica invertida, igual ao sensor de chama ky-026
//FOI PROGRAMADO PARA SALVAR O MAIOR VALOR LIDO NO INTERVALO VIBRATION_INTERVAL
/********FIM********************Sensor de Vibracao***********************************************************************************************************************************************************************************/


/********INICIO********************Sensor de Chama***********************************************************************************************************************************************************************************/
#define NUMBER_FLAME 0

byte flamePin[] = {A2};    // the cell and 10K pulldown are connected to a0

float percent_flame[sizeof(flamePin) / sizeof(byte)];

#define NO_FLAME float(0.0) // 0% de chance
#define Threeshold_flame float(20.0) // 20% de chance

//MEDE CALOR IRRADIADO
//O sensor ky-026 funciona ao contrário, ou seja, quando um valor muito baixo é verificado na saída analógica, é porque o calor detectado é muito alto.
/********FIM********************Sensor de Chama***********************************************************************************************************************************************************************************/

/********INICIO********************Sensor de Porta***********************************************************************************************************************************************************************************/
#define NUMBER_DOOR 0

byte doorPin[] = {2};

byte door[sizeof(doorPin) / sizeof(byte)];

#define DOOR_OPEN 1
#define DOOR_CLOSED 0

//TROCAR ACIMA DEPOIS DE TESTAR O SENSOR
/********FIM********************Sensor de Porta***********************************************************************************************************************************************************************************/


/********INICIO********************Sensor de Pressão,Altitude e Temp Interna***********************************************************************************************************************************************************************************/
#include <Adafruit_BMP085.h>

#define BMP_SENSOR false

Adafruit_BMP085 bmp180;
float pressure;
float temp_interna;
float altitude;
/********FIM********************Sensor de Pressão,Altitude e Temp Interna***********************************************************************************************************************************************************************************/

/********INICIO********************Sensor de Corrente eletrica***********************************************************************************************************************************************************************************/
#include "EmonLib.h"

#define NUMBER_CURRENT_SENSORS 0

EnergyMonitor current[NUMBER_CURRENT_SENSORS];

uint8_t current_ports[] = {A15, A14, A13,A12};

float power_factor[] = {0.8, 0.8, 0.8, 1};
boolean tem_power_factor[] = {true , true , true, false };

#define AUX_POWER_READS 5
byte aux_power_cont = 0;
double aux_Irms[sizeof(current_ports) / sizeof(byte)];

double Irms[NUMBER_CURRENT_SENSORS];
float potencia_aparente[NUMBER_CURRENT_SENSORS];
float potencia_ativa[NUMBER_CURRENT_SENSORS];
float potencia_reativa[NUMBER_CURRENT_SENSORS];

float media_potencia_aparente[NUMBER_CURRENT_SENSORS]; //unidade é KVA
float media_potencia_ativa[NUMBER_CURRENT_SENSORS]; //unidade é KW
float media_potencia_reativa[NUMBER_CURRENT_SENSORS]; //unidade é Kvar
int contador_power;

double consumo_kwh[NUMBER_CURRENT_SENSORS];
double consumo_kvarh[NUMBER_CURRENT_SENSORS];
double consumo_kvah[NUMBER_CURRENT_SENSORS];

long tempo_power;
byte MONTH_power[13] = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
byte months_power = EEPROM.read(16);
byte days_power = EEPROM.read(17);
byte hours_power = EEPROM.read(18);

#define YEAR_power 12
#define DAY_power 24
#define HOUR_power 3600000
//#define HOUR_power 100 //use para testes rapidos

#define TENSAO_REDE 220.0
//#define TENSAO_REDE 380.0
#define CALIBRATION_CURRENT 67.6
#define NUMBER_READS_power 50

byte sensor_to_read_power = 0;



#define MULTIPHASE true
double Irms_total = 0;
float potencia_aparente_total = 0;
double consumo_kvah_total = 0; //aparente
double Irms_neutro = 0;


//float potencia_ativa_total = 0;
//float potencia_reativa_total = 0;
//double consumo_kwh_total = 0;  //real
//double consumo_kvarh_total = 0; //imaginario
/********FIM********************Sensor de Corrente eletrica***********************************************************************************************************************************************************************************/





byte aux;                     //variável de auxílio para todo o código
//Duas variáveis usadas para controle do tempo de atualização dos valores dos sensores
long time_enter;
long time_exit;
long time_inicio;
long time_fim;


void setup() {

  Serial.begin(9600); //Início da comunicação Serial do arduíno
  //Verificação da memória disponível no começo do Setup()
  //  Serial.print("freeMemory()=");
  //  Serial.println(freeMemory());





  boolean program_mode = false;
 int what_to_program=0;
  long time_program = 15000;
  String aux_comando_serial = "";
  String comando_serial = "";
  byte aux_serial = 0;
  long time_wait_serial = millis();
  bool end_while=false;
  Serial.println(F("DESEJA ENTRAR NO MODO DE CONFIGURACAO? 1 -> SIM / 0 -> NAO"));
  while (millis() - time_wait_serial < time_program && !end_while) {
    delay(10);
    if (Serial.available()) { //verifica se chegou algum dado na serial
      aux_comando_serial = String(Serial.read() - '0'); //Lê o byte mais recente disponível na serial
      //      Serial.println(aux_comando_serial);
      if (aux_comando_serial.toInt() != -35) {
        comando_serial += aux_comando_serial;
        Serial.println(comando_serial);
      }
      else {
            int teste=comando_serial.toInt();
              if(teste==0 || teste==1){
                if(teste==1){
                  program_mode = true;
                }
                end_while=true;
              }
              else{
                Serial.println(F("\r\nVALOR INVALIDO, DIGITE 0 OU 1."));
                aux_comando_serial = "";
                comando_serial = "";
              }
      }
    }
  }

  if (program_mode) {
    
    Serial.println(F("\r\n\nINICIO DO MODO DE PROGRAMACAO, TECLE ENTER AO FIM DE CADA COMANDO. CASO ERRE NA DIGITAÇÃO, REINICIE O SISTEMA E TENTE NOVAMENTE."));
    aux_comando_serial = "";
    comando_serial = "";
    Serial.println(F("\rO QUE DESEJA PROGRAMAR? 0->DATA E HORA / 1->CONEXAO?"));
      end_while=false;
      while (!end_while) {
      delay(10);
      if (Serial.available()) { //verifica se chegou algum dado na serial
        aux_comando_serial = String(Serial.read() - '0'); //Lê o byte mais recente disponível na serial
        if (aux_comando_serial.toInt() != -35) {
          comando_serial += aux_comando_serial;
          Serial.print(aux_comando_serial);
        }
        else {
            int teste=comando_serial.toInt();
              if(teste==0 || teste==1){
                what_to_program = comando_serial.toInt();
                end_while=true;
              }
              else{
                Serial.println(F("\r\nVALOR INVALIDO, DIGITE 0 OU 1."));
                aux_comando_serial = "";
                comando_serial = "";
              }
        }
      }
    }
    
    
    
    
    
   switch (int(what_to_program)){
    case 0:{
      Serial.println(F("\r\nMES ATUAL (SEM ZERO NO COMECO -> 0-12 MESES):"));
      end_while=false;
      while (!end_while) {
        delay(10);
        if (Serial.available()) { //verifica se chegou algum dado na serial
          aux_comando_serial = String(Serial.read() - '0'); //Lê o byte mais recente disponível na serial
          if (aux_comando_serial.toInt() != -35) {
            comando_serial += aux_comando_serial;
            Serial.print(aux_comando_serial);
          }
          else {
            int teste=comando_serial.toInt();
              if(teste>=1 && teste<=12){
                EEPROM.write(16, comando_serial.toInt());
                end_while=true;
              }
              else{
                Serial.println(F("\r\nVALOR INVALIDO, DIGITE UM VALOR ENTRE 1 E 12."));
                aux_comando_serial = "";
                comando_serial = "";
              }
          }
        }
      }

      aux_comando_serial = "";
      comando_serial = "";
      Serial.print(F("\r\nDIA DO MES "));
      Serial.print(String(String(int(months_power))+" (SEM ZERO NO COMECO -> 0-"));
      Serial.println(String(String(MONTH_power[int(months_power)])+"):"));
      end_while=false;
      while (!end_while) {
        delay(10);
        if (Serial.available()) { //verifica se chegou algum dado na serial
          aux_comando_serial = String(Serial.read() - '0'); //Lê o byte mais recente disponível na serial
          if (aux_comando_serial.toInt() != -35) {
            comando_serial += aux_comando_serial;
            Serial.print(aux_comando_serial);
          }
          else {
            int teste=comando_serial.toInt();
              if(teste>=1 && teste<=MONTH_power[int(months_power)]){
                EEPROM.write(17, comando_serial.toInt());
                end_while=true;
              }
              else{
                Serial.print(F("\r\nVALOR INVALIDO, DIGITE UM VALOR ENTRE 1 E "));
                Serial.println(String(String(MONTH_power[int(months_power)])+"."));
                aux_comando_serial = "";
                comando_serial = "";
              }
          }
        }
      }


      aux_comando_serial = "";
      comando_serial = "";
      Serial.println(F("\r\nHORA ATUAL (DOIS DIGITOS PARA HORA, IGNORE MINUTOS E SEGUNDOS -> 0-24 HORAS):"));
      end_while=false;
      while (!end_while) {
        delay(10);
        if (Serial.available()) { //verifica se chegou algum dado na serial
          aux_comando_serial = String(Serial.read() - '0'); //Lê o byte mais recente disponível na serial
          if (aux_comando_serial.toInt() != -35) {
            comando_serial += aux_comando_serial;
            Serial.print(aux_comando_serial);
          }
          else {
            int teste=comando_serial.toInt();
              if(teste>=0 && teste<=24){
                EEPROM.write(18, comando_serial.toInt());
                end_while=true;
              }
              else{
                Serial.println(F("\r\nVALOR INVALIDO, DIGITE UM VALOR ENTRE 0 E 24."));
                aux_comando_serial = "";
                comando_serial = "";
              }
          }
        }
      }

//      aux_comando_serial = "";
//      comando_serial = "";
//      Serial.println(F("\r\nQUARTA PARTE DO IP:"));
//      while (1) {
//        delay(10);
//        if (Serial.available()) { //verifica se chegou algum dado na serial
//          aux_comando_serial = String(Serial.read() - '0'); //Lê o byte mais recente disponível na serial
//          if (aux_comando_serial.toInt() != -35) {
//            comando_serial += aux_comando_serial;
//            Serial.print(aux_comando_serial);
//          }
//          else {
//            EEPROM.write(5, comando_serial.toInt());
//            break;
//          }
//        }
//      }
      break;
    }
    case 1:{
    
    //Serial.println(F("\rQUAL TIPO DE CONEXAO? 1 -> CABO IP FIXO / 2 -> CABO DHCP / 3-> WIFI DHCP"));
    Serial.println(F("\rQUAL TIPO DE CONEXAO? 0 -> CABO IP FIXO / 1 -> CABO DHCP"));
    end_while=false;
      aux_comando_serial = "";
      comando_serial = "";
    while (!end_while) {
      delay(10);
      if (Serial.available()) { //verifica se chegou algum dado na serial
        aux_comando_serial = String(Serial.read() - '0'); //Lê o byte mais recente disponível na serial
        if (aux_comando_serial.toInt() != -35) {
          comando_serial += aux_comando_serial;
          Serial.print(aux_comando_serial);
        }
        else {
            int teste=comando_serial.toInt();
              if(teste==0 || teste==1){
                EEPROM.write(6, comando_serial.toInt());
                end_while=true;
              }
              else{
                Serial.println(F("\r\nVALOR INVALIDO, DIGITE 0 OU 1."));
                aux_comando_serial = "";
                comando_serial = "";
              }

        }
      }
    }

    switch (int(TYPE_CONNECTION)) {
    case 0:{
      aux_comando_serial = "";
      comando_serial = "";
      Serial.println(F("\r\n\nMODO CABO IP FIXO ESCOLHIDO"));
      Serial.println(F("\r\nPRIMEIRA PARTE DO IP:"));
      end_while=false;
      while (!end_while) {
        delay(10);
        if (Serial.available()) { //verifica se chegou algum dado na serial
          aux_comando_serial = String(Serial.read() - '0'); //Lê o byte mais recente disponível na serial
          if (aux_comando_serial.toInt() != -35) {
            comando_serial += aux_comando_serial;
            Serial.print(aux_comando_serial);
          }
          else {
            int teste=comando_serial.toInt();
              if(teste>=0 && teste<=255){
                EEPROM.write(2, comando_serial.toInt());
                end_while=true;
              }
              else{
                Serial.println(F("\r\nVALOR INVALIDO, DIGITE UM VALOR ENTRE 0 E 255."));
                aux_comando_serial = "";
                comando_serial = "";
              }
          }
        }
      }

      aux_comando_serial = "";
      comando_serial = "";
      Serial.println(F("\r\nSEGUNDA PARTE DO IP:"));
      end_while=false;
      while (!end_while) {
        delay(10);
        if (Serial.available()) { //verifica se chegou algum dado na serial
          aux_comando_serial = String(Serial.read() - '0'); //Lê o byte mais recente disponível na serial
          if (aux_comando_serial.toInt() != -35) {
            comando_serial += aux_comando_serial;
            Serial.print(aux_comando_serial);
          }
          else {
            int teste=comando_serial.toInt();
              if(teste>=0 && teste<=255){
                EEPROM.write(3, comando_serial.toInt());
                end_while=true;
              }
              else{
                Serial.println(F("\r\nVALOR INVALIDO, DIGITE UM VALOR ENTRE 0 E 255."));
                aux_comando_serial = "";
                comando_serial = "";
              }
          }
        }
      }


      aux_comando_serial = "";
      comando_serial = "";
      Serial.println(F("\r\nTERCEIRA PARTE DO IP:"));
      end_while=false;
      while (!end_while) {
        delay(10);
        if (Serial.available()) { //verifica se chegou algum dado na serial
          aux_comando_serial = String(Serial.read() - '0'); //Lê o byte mais recente disponível na serial
          if (aux_comando_serial.toInt() != -35) {
            comando_serial += aux_comando_serial;
            Serial.print(aux_comando_serial);
          }
          else {
            int teste=comando_serial.toInt();
              if(teste>=0 && teste<=255){
                EEPROM.write(4, comando_serial.toInt());
                end_while=true;
              }
              else{
                Serial.println(F("\r\nVALOR INVALIDO, DIGITE UM VALOR ENTRE 0 E 255."));
                aux_comando_serial = "";
                comando_serial = "";
              }
          }
        }
      }

      aux_comando_serial = "";
      comando_serial = "";
      Serial.println(F("\r\nQUARTA PARTE DO IP:"));
      end_while=false;
      while (!end_while) {
        delay(10);
        if (Serial.available()) { //verifica se chegou algum dado na serial
          aux_comando_serial = String(Serial.read() - '0'); //Lê o byte mais recente disponível na serial
          if (aux_comando_serial.toInt() != -35) {
            comando_serial += aux_comando_serial;
            Serial.print(aux_comando_serial);
          }
          else {
            int teste=comando_serial.toInt();
              if(teste>=1 && teste<=254){
                EEPROM.write(5, comando_serial.toInt());
                end_while=true;
              }
              else{
                Serial.println(F("\r\nVALOR INVALIDO, DIGITE UM VALOR ENTRE 1 E 254."));
                aux_comando_serial = "";
                comando_serial = "";
              }
          }
        }
      }
    break;
    }


    case 1:{
      Serial.println(F("\r\n\nMODO CABO DHCP ESCOLHIDO."));
      break;
    }
    }
/*
    if (byte(TYPE_CONNECTION) == 3) {
      aux_comando_serial = "";
      comando_serial = "";
      char c;
      Serial.println(F("\r\n\nMODO WIFI ESCOLHIDO"));


      Serial.println(F("\r\nSSID:"));
      while (1) {
        delay(10);
        if (Serial.available()) { //verifica se chegou algum dado na serial
          c = Serial.read();
          if (isalpha(c)) {
            aux_comando_serial = String(c); //Lê o byte mais recente disponível na serial
          } else if (isdigit(c)) {
            aux_comando_serial = String(c - '0'); //Lê o byte mais recente disponível na serial
          } else {
            aux_comando_serial = String(c); //Lê o byte mais recente disponível na serial
          }
          if (aux_comando_serial != "\r" && aux_comando_serial != "\n") {
            comando_serial += aux_comando_serial;
            Serial.print(aux_comando_serial);
          }
          else {
            EEPROM.write(6, comando_serial.length());

            for (byte i = 0; i < comando_serial.length(); i++) {
              EEPROM.write(300 + i, comando_serial.charAt(i));
            }
            break;
          }
        }
      }




      comando_serial = "";
      aux_serial = 0;
      Serial.println(F("\r\n\nPASSWORD:"));
      while (1) {
        delay(10);
        if (Serial.available()) { //verifica se chegou algum dado na serial
          c = Serial.read();
          if (isalpha(c)) {
            aux_comando_serial = String(c); //Lê o byte mais recente disponível na serial
          } else if (isdigit(c)) {
            aux_comando_serial = String(c - '0'); //Lê o byte mais recente disponível na serial
          } else {
            aux_comando_serial = String(c); //Lê o byte mais recente disponível na serial
          }
          if (aux_comando_serial != "\r" && aux_comando_serial != "\n") {
            comando_serial += aux_comando_serial;
            Serial.print(aux_comando_serial);
          }
          else {
            EEPROM.write(7, comando_serial.length());

            for (byte i = 0; i < comando_serial.length(); i++) {
              EEPROM.write(450 + i, comando_serial.charAt(i));
            }
            break;
          }
        }
      }
    }
*/
    break;
    }
   
  }
    Serial.println(F("\r\n\nREINICIE O SISTEMA POR FAVOR!"));
    while (1) {
      delay(100);
    }
  }
  else {
    Serial.println(F("MODO DE PROGRAMACAO NAO ATIVADO, CONTINUANDO BOOT NORMALMENTE."));
  }









  /*********INÍCIO DO SETUP*************Sensores de Temperatura Dallas********************************************************************************************************************************************/

  //Inicializar as portas para poder verificá-las
  inicializa_devs_e_dallastemp();

  //Descobrir qual o real número de sensores conectados
  for (byte i = 0; i < (byte) (NUM_ONEWIRE_PORTS); i++) {
    if ( ((byte)dallastemp[i]->getDeviceCount()) > 0 ) {
      //      Serial.println(i);
      real_num_onewire_ports++;
    }
    //    else {
    //            free(dallastemp[i]);  //É necessário gerenciar muito bem a memória
    //            free(devs[i]);
    //    }
  }

  //Serial.println(real_num_onewire_ports); //Use para checagem

  portas_ocupadas = (byte*) malloc(real_num_onewire_ports * sizeof(byte));  //Alocando espaços necessários para as portas livres

  tempC = (float**) malloc(real_num_onewire_ports * sizeof(float*));        //Alocando espaços necessários para os guardar os arrays de temperatura
  media_leitura = (float*) malloc(real_num_onewire_ports * sizeof(float));  //Alocando espaços necessários para as médias dos corredores
  devices = (uint8_t***) malloc(real_num_onewire_ports * sizeof(uint8_t**));//Alocando espaços necessários para os endreços dos sensores

  nodos_sensores_temperatura = (DallasTemperature**) malloc(real_num_onewire_ports * sizeof(DallasTemperature*));  //Alocando espaços necessários para os múltiplos objetos Dallas presentes em cada corredor

  //Trecho usado para guardar o acesso apenas às portas que de fato contém pelo menos um sensor Dallas conectado
  aux = 0;
  for (byte i = 0; i < NUM_ONEWIRE_PORTS; i++) {
    if ( ((byte)dallastemp[i]->getDeviceCount()) > 0 ) {
      portas_ocupadas[aux] = i;
      nodos_sensores_temperatura[aux] = dallastemp[i];
      aux++;
    }
    else {
      free(dallastemp[i]);
      free(devs[i]);
    }
  }


  //Trecho para terminar a inicialização de tudo que é necessário aos sensores Dallas para funcionarem corretamente
  for (byte i = 0; i < real_num_onewire_ports; i++) {
    aux = (byte) nodos_sensores_temperatura[i]->getDeviceCount(); //guardando o número de sensores Dallas conectados na mesma porta

    //Caso existam sensores conectados à uma porta, guarda-se o endereço de cada um deles e aloca-se o espaço necessário ao array que guardará suas temperaturas
    if (aux > 0) {
      devices[i] = alocaMemoriaDevices(i, aux);
      tempC[i] = (float*) malloc(aux * sizeof(float));

      for (byte j = 0; j < aux; j++) {
        if (!(nodos_sensores_temperatura[i]->getAddress(devices[i][j], j))) {
          devs[portas_ocupadas[i]]->reset_search();
          devs[portas_ocupadas[i]]->search(devices[i][j]);
        }
        devs[portas_ocupadas[i]]->reset_search();
      }


    }

  }

  //Use o código abaixo (ou trechos dele) caso necessite descobrir o endereço dos sensores conectados.
  /***********************Código que fornece uma lista com os endereços dos sensores (array de duas dimensoes em que a primeira é o numero do sensor)***********
    int addr[NUMBER_SENSORS][8];

    byte addresses[8];

    for(int j=0;j< NUMBER_SENSORS;j++){
    ourBus.search(addresses);

    for(int i=0; i<8; i++){
    addr[j][i]=(int)addresses[i];
    }
    }
    ourBus.reset_search();


    Serial.println();
    for(int j=0;j< NUMBER_SENSORS;j++){
    Serial.print("\n\r\n\rFound \'1-Wire\' device with address:\n\r");

    for(int i = 0; i < 8; i++) {
       Serial.print("0x");
       if (addr[j][i] < 16) {
          Serial.print('0');
       }
       Serial.print(addr[j][i], HEX);
       if (i < 7) {
         Serial.print(", ");
       }
    }
    'Serial.println();
    }
    Serial.print("Done");
  *************************************************************************************************************************************************************/
  //  Serial.print("freeMemory()=");
  //  Serial.println(freeMemory());
  /*********FIM DO SETUP****************Sensores de Temperatura Dallas********************************************************************************************************************************************/

  /********INÍCIO DO SETUP**********************Sensores de Umidade e Temperatura DHT*****************************************************************************************************************************/
  //   for (byte i = 0; i < NUM_ANALOG_PORTS; i++) {
  //    pinMode(analogPorts[i], INPUT);
  //  }

  dht = (DHT*) malloc(NUM_ANALOG_PORTS * sizeof(DHT)); //Alocando espaço para todas as portas Analógicas do Arduino
  auxiliar = (DHT**) malloc(NUM_ANALOG_PORTS * sizeof(DHT*));  //Alocando espaço para todas as portas Analógicas do Arduino

  //Inicializando cada porta analógica do arduino para testá-las
  for (byte i = 0; i < NUM_ANALOG_PORTS; i++) {
    auxiliar[i] = cria_objeto_dht(analogPorts[i], i);
    auxiliar[i]->begin();
  }

  //Descobrindo quantos sensores DHT realmente estão conectados ao arduino
  real_num_hum_sensors = 0;
  for (byte i = 0; i < NUM_ANALOG_PORTS; i++) {
    if (!isnan(auxiliar[i]->readHumidity())) {
      real_num_hum_sensors++;
    }
  }

  //  Serial.println(real_num_hum_sensors); //Use para checagem

  humidity_sensors = (DHT**) malloc(real_num_hum_sensors * sizeof(DHT*)); //Alocando espaço para a real quantidade de sensores DHT conectados
  umidade = (float*) malloc(real_num_hum_sensors * sizeof(float)); //Alocando espaço para a real quantidade de umidades
  temp_sensor_umidade = (float*) malloc(real_num_hum_sensors * sizeof(float)); //Alocando espaço para a real quantidade de temperaturas associadas aos sensores dht11


  //Salvando o acesso aos sensores DHT que estão conectados
  byte aux = 0;
  for (byte i = 0; i < NUM_ANALOG_PORTS; i++) {
    if (!isnan(auxiliar[i]->readTemperature())) {
      humidity_sensors[aux] = auxiliar[i];
      aux++;
    }
  }
  //free(auxiliar);  //É necessário gerenciar muito bem a memória
  //  Serial.print("freeMemory()=");
  //  Serial.println(freeMemory());
  /********FIM DO SETUP**********************Sensores de Umidade e Temperatura DHT*******************************************************************************************************************************/

  /********INICIO DO SETUP********************Sensor de Corrente eletrica***********************************************************************************************************************************************************************************/

  for (byte i = 0; i < NUMBER_CURRENT_SENSORS; i++) {
    current[i].current(current_ports[i], CALIBRATION_CURRENT);
    Irms[i] = current[i].calcIrms(1480);

    consumo_kwh[i] = 0;
    consumo_kvarh[i] = 0;
    consumo_kvah[i] = 0;

    media_potencia_aparente[i] = 0;
    media_potencia_ativa[i] = 0;
    media_potencia_reativa[i] = 0;

    aux_Irms[i] = 0;
  }

  //rotina de leitura intensa para acelerar calibragem dos sensores
  for (sensor_to_read_power = 0; sensor_to_read_power< (NUMBER_CURRENT_SENSORS); sensor_to_read_power++) {
      for (byte i = 0; i < (15); i++) {
        Irms[sensor_to_read_power] =current[sensor_to_read_power].calcIrms(1480);
      }
}

  Serial.println("INICIALIZACAO FINALIZADA!");
  tempo_power = millis();
  contador_power = 0;
  sensor_to_read_power=0;
  /********FIM DO SETUP********************Sensor de Corrente eletrica***********************************************************************************************************************************************************************************/



  /********INÍCIO DO SETUP********************Ethernet***********************************************************************************************************************************************************/
  //Inicializa a interface ethernet
  //  Ethernet.begin(mac, ip, dns_server, gateway, subnet);
  //Ethernet.begin(mac, ip);


  switch (TYPE_CONNECTION) {
    case 0: {
        // start the Ethernet connection and the server:
        Ethernet.begin(mac, ip);
        Serial.print(F("MEU ENDERECO IP: "));
        for (byte i = 0; i < 4 ; i++) {
          Serial.print(ip[i]);
          Serial.print(".");
        }
        Serial.println();
        break;
      }
    case 1: {
        // try to start the Ethernet DHCP connection:
        long tempo_DHCP = millis();
        byte connec;
        while ((millis() - tempo_DHCP) < TIMEOUT_CONNECTION) {
          connec = Ethernet.begin(mac);
          if (connec == 0) {
            Serial.println(F("FALHA DHCP, REPETINDO TENTATIVA EM 5 SEGUNDOS..."));
            //            delay(5000);
          }
          else {
            break;
          }
        }
        if (connec == 0) {
          //Will start the Ethernet connection with the last valid DHCP IP acquired (which connected to cloud successfully):
          Ethernet.begin(mac, ip);
          Serial.print(F("DHCP FALHOU COMPLETAMENTE! INCIANDO COM ULTIMO IP GRAVADO EM MEMORIA."));
          Serial.println(Ethernet.localIP());
        }
        else {
          // print your local IP address:
          Serial.print(F("MEU ENDERECO IP: "));
          for (byte thisByte = 0; thisByte < 4; thisByte++) {
            // print the value of each byte of the IP address:
            Serial.print(Ethernet.localIP()[thisByte], DEC);
            Serial.print(F("."));
          }
          Serial.println();
        }
        break;
      }
    /*
    case 3: {
        //        // Configure na linha abaixo a velocidade inicial do
        //        // modulo ESP8266
        //        mySerial.begin(115200);
        //        sendData("AT+RST\r\n", 2000, DEBUG);
        //        delay(1000);
        //        Serial.println("Versao de firmware");
        //        delay(3000);
        //        sendData("AT+GMR\r\n", 2000, DEBUG); // rst
        //        // Configure na linha abaixo a velocidade desejada para a
        //        // comunicacao do modulo ESP8266 (9600, 19200, 38400, etc)
        //        sendData("AT+CIOBAUD=19200\r\n", 2000, DEBUG);
        //        Serial.println("** Final **");

        pinMode(A0, OUTPUT);
        analogWrite(A0, 716); //Escrevendo 3.5V para a porta do ESP
        delay(3000);

        wifi.restart();

        // try to start the Wifi connection:
        long tempo_Wifi = millis();
        boolean connec;
        while ((millis() - tempo_Wifi) < TIMEOUT_CONNECTION) {
          connec = connect_wifi_esp01();
          if (connec == false) {
            Serial.println(F("Failed to configure Wifi, will try again after 5 sec."));
            delay(5000);
          }
          else {
            break;
          }
        }
        if (connec == false) {
          Serial.print(F("\r\nWifi falhou completamente, inicializacao vai terminar e modulo resetara apos 30 minutos.\r\n"));
        }
        else {
          Serial.print(F("\r\nWifi conectado com sucesso! Continuando boot normalmente...\r\n"));
        }
        break;
      }*/
  }


  /********FIM DO SETUP***********************Ethernet***********************************************************************************************************************************************************/
  /********INICIO DO SETUP********************SNMP***********************************************************************************************************************************************************************************/
  api_status = Agentuino.begin();
  //
  if ( api_status == SNMP_API_STAT_SUCCESS ) {
    //
    Agentuino.onPduReceive(pduReceived);
    //
    delay(10);
    //
  }
  //
  /********FIM DO SETUP********************SNMP***********************************************************************************************************************************************************************************/

  /********INÍCIO DO SETUP********************Web Server*********************************************************************************************************************************************************/
  server.begin(); //inicia o servidor web
  /********FIM DO SETUP***********************Web Server*********************************************************************************************************************************************************/

  /********INÍCIO DO SETUP********************telnet Server******************************************************************************************************************************************************/
  server_telnet.begin(); //inicia o servidor para o telnet
  /********FIM DO SETUP***********************telnet Server******************************************************************************************************************************************************/

  /********INÍCIO DO SETUP********************Sensor de presença***********************************************************************************************************************************************************/
  pinMode(pir, INPUT); //Define pino sensor como entrada
  /********FIM DO SETUP***********************Sensor de presença***********************************************************************************************************************************************************/


  /********INÍCIO DO SETUP********************Sensor de Fumaça MQ2********************************************************************************************************************************************************************************/
  time_mq2 = 2000000; //se o millis()-time_mq2>30 min, entao a calibragem é feita. Como é interessante começar pela calibragem, mas sem
  //adicionar complexidade ao código, faz-se fingir que as mediçoes ja começaram atrasadas em relação à calibragem.
  /********FIM DO SETUP********************Sensor de Fumaça MQ2***********************************************************************************************************************************************************************************/

  /********INICIO DO SETUP********************Controle de Acesso***********************************************************************************************************************************************************************************/
  //  currentLine.reserve(80);
  definite_user = new String;
  *definite_user = EMPTY;
  /********FIM DO SETUP********************Controle de Acesso***********************************************************************************************************************************************************************************/

  /********INICIO DO SETUP********************Sensor de inundaçao***********************************************************************************************************************************************************************************/
  for (byte i = 0; i < NUMBER_INUNDACAO; i++) {
    pinMode(pino_inundacao[i], INPUT);
  }
  /********FIM DO SETUP********************Sensor de inundaçao***********************************************************************************************************************************************************************************/

  /********INICIO DO SETUP********************Sensor de Luz***********************************************************************************************************************************************************************************/
  for (byte i = 0; i < NUMBER_LDRs; i++) {
    pinMode(photocellPin[i], INPUT);
  }

  Serial.print("freeMemory()=");
  Serial.println(freeMemory()); //Verificar o uso de memória após cada iteração do Loop

  if (LUX_SENSOR) {
    //Inicializa o BH1750
    lightMeter.begin();
  }

  Serial.print("freeMemory()=");
  Serial.println(freeMemory()); //Verificar o uso de memória após cada iteração do Loop
  /********FIM DO SETUP********************Sensor de Luz***********************************************************************************************************************************************************************************/

  /********INICIO DO SETUP********************Sensor de Som***********************************************************************************************************************************************************************************/
  for (byte i = 0; i < NUMBER_SOUND; i++) {
    pinMode(soundPin[i], INPUT);
  }
  /********FIM DO SETUP********************Sensor de Som***********************************************************************************************************************************************************************************/

  /********INICIO DO SETUP********************Sensor de Vibração***********************************************************************************************************************************************************************************/
  for (byte i = 0; i < NUMBER_VIBRATION; i++) {
    pinMode(vibrationPin[i], INPUT);
  }

  time_last_vibration = millis();
  /********FIM DO SETUP********************Sensor de Vibração***********************************************************************************************************************************************************************************/


  /********INICIO DO SETUP********************Sensor de Chama***********************************************************************************************************************************************************************************/
  for (byte i = 0; i < NUMBER_FLAME; i++) {
    pinMode(flamePin[i], INPUT);
  }
  /********FIM DO SETUP********************Sensor de Chama***********************************************************************************************************************************************************************************/

  /********INICIO DO SETUP********************Sensor de Porta***********************************************************************************************************************************************************************************/
  for (byte i = 0; i < NUMBER_DOOR; i++) {
    pinMode(doorPin[i], INPUT);
  }
  /********FIM DO SETUP********************Sensor de Porta***********************************************************************************************************************************************************************************/

  /********INICIO DO SETUP********************Sensor de Pressão,Altitude e Temp Interna***********************************************************************************************************************************************************************************/
  if (BMP_SENSOR) {
    //Inicializa o BH1750
    bmp180.begin();
  }
  /********FIM DO SETUP********************Sensor de Pressão,Altitude e Temp Interna***********************************************************************************************************************************************************************************/


  time_enter = millis(); //Marcando o momento em que se sai do Setup() e se inicia o Loop()
  time_inicio = millis();

  //  while(1){

  for (byte i = 0; i < real_num_onewire_ports; i++) {
    Serial.println("!!!!!!!!!!!!!");
    nodos_sensores_temperatura[i]->requestTemperatures();
    aux = (byte) nodos_sensores_temperatura[i]->getDeviceCount();  //descobrindo quantos sensores existem num dado corredor
    for (byte k = 0; k < aux; k++) { //percorrendo todos os sensores do corredor "i"
      getTemperature(*nodos_sensores_temperatura[i], devices[i][k], i, k);
    }
    atualiza_medias(i, aux);
  }
  atualiza_media_geral();

  for (byte i = 0; i < real_num_hum_sensors; i++) {
    Serial.println("????????????");
    getTemperature(i);
    getHumidity(i);
  }
  atualiza_media_umidade();
  //  Serial.print("freeMemory()=");
  //  Serial.println(freeMemory());
  //Serial.println(umidade[0]);
  //delay(100);
  //}


}


/************************************************************************************************************************
*****INÍCIO************************************************FUNÇÕES*******************************************************
*************************************************************************************************************************/

//Quando chamada, fecha todos os sockets
//void disconnectSockets()
//{
//
//  for (int i = 0; i < MAX_SOCK_NUM; i++) {
//    disconnect(i);
//  }
//
//}

//Quando chamada, testa a entrada do usuário para saber se ele deseja se desconectar
void try_disconnecttelnet() {
  String exittelnet = "";
  for (byte i = 0; i < 5 ; i++) { //5 = tamanho de "close"
    exittelnet += command[i];
  }
  if (exittelnet.equals("close")) {
    client_telnet.println(F("Fim da conexao."));
    //    Socket = client_telnet.getSocketNumber();
    client_telnet.stop();
    //disconnect(Socket);
    //W5100.writeSnCR(Socket, Sock_DISCON);
    client_telnet = 0;
    //    alreadyConnected = false;

  }
}

//Apaga o conteudo do array de caracteres
void cleanWord (char command[max_command_size], int param) {
  for (byte i = 0; i < param; i++) {
    command[i] = 0;
  }
  return;
}

//Interpreta e executa o comando recebido do cliente telnet
/*
   Formato dos comandos Telnet:

   ->Usuários (devem ter 1 antes de cada comando):
    -Receber temperatura do sensor 1 no corredor 0: 1t01 (altere os números de acordo com a necessidade)
    -Receber temperatura do sensor de umidade 2: 1ht3 (altere os números de acordo com a necessidade)
    -Receber umidade do sensor e umidade 1: 1h1 (altere os números de acordo com a necessidade)
    -Receber média da temperatura no corredor 0: 1m0 (altere os números de acordo com a necessidade)
    -Receber média da temperatura geral (sala toda): 1mg (altere os números de acordo com a necessidade)
    -Receber média da umidade geral: 1mh
    -Desconectar-se: close

    ->Sistemas (devem ter 0 antes de cada comando):
     -Receber temperatura do sensor 1 no corredor 0: 0t01 (altere os números de acordo com a necessidade)
     -Receber temperatura do sensor de umidade 2: 0ht3 (altere os números de acordo com a necessidade)
     -Receber umidade do sensor De umidade 1: 0h1 (altere os números de acordo com a necessidade)
     -Receber média da temperatura no corredor 0: 0m0 (altere os números de acordo com a necessidade)
     -Receber média da temperatura geral (sala toda): 0mg
     -Receber média da umidade geral: 0mh
     -Receber chance em porcentagem de inundacao de do sensor de inundacao 2: 0i2
     -Receber nome de quem está com acesso autorizado à sala: 0a
     -Receber indicação em inteiro de acesso à sala: 0p
      Obs.: 0 - nenhum movimento na porta | 1 - movimento autorizado previamente pela página http | 2 - movimento não autorizado
     -Receber indicativo de nivel de fumaca na sala: 0f
     -Receber nível de luz na sala em porcentagem do ideal no sensor 3: 0l3
     -Receber nivel de luz no sensor de lux BH1750FVI: 0lux
     -Receber temepratura interna do sistema: 0tint
     -Receber altitude: 0alt
     -Receber pressão: 0pres


     -Receber corrente no sensor de corrente 0: 0cur0
     -Receber potencia aparente no sensor de corrente 0: 0pap0
     -Receber potencia ativa no sensor de corrente 0 (se houver fator de potencia associado): 0pre0 (Potencia real)
     -Receber potencia reativa no sensor de corrente 0 (se houver fator de potencia associado): 0pim0 (Potencia imaginaria/reativo)
     -Receber consumo de energia aparente no sensor de corrente 0 : 0cap0 (Consumo aparente)
     -Receber consumo ativo de energia no sensor de corrente 0 (se houver fator de potencia associado: 0cre0 (Consumo real)
     -Receber consumo reativo de energia no sensor de corrente 0 (se houver fator de potencia associado: 0cim0 (Consumo imaginario/reativo)
      Mais de uma fase (neutro sempre será a última porta do array de portas dos sensores de corrente) - requer habilitação na variável adequada (com true/false):
      -Receber corrente somando os sensores de corrente e excluindo o neutro: 0curt
      -Receber potencia aparente somando os sensores de corrente nas fases sem o neutro: 0papt
      -Receber consumo aparente somando os sensores de corrente nas fases sem o neutro: 0capt (soma o consumo dos três sensores)
      -Receber corrente no sensor do neutro: 0curn

      OBS.: Para sistemas (ou seja, com um "0" antes de cada comando), após cada requisição, a conexão é fechada.
      OBS: A numeração dos sensores de temperatura, de umidade e dos corredores começa em zero. Para se entender a disposição dos
           sensores de TEMPERATURA, imagine que estes formam uma MATRIZ, em que o cliente requisita um elemento por vez da matriz ou a média dela ou ainda a média de cada corredor
           (a numeração dos corredores corresponde à numeração das linhas de uma matriz, portanto requisitar o valor e um determinado sensor é requisitar o valor de um elemento de uma matriz).
           Os sensores de UMIDADE contém valores de umidade e, também, valores de temperatura associados, e estão dispostos na memória em formato de ARRAY UNIDIMENSIONAL.

*/
void generateAction(char command[7], byte param) {
  byte corredor = 0;
  byte sensor = 0;
  String integer;

  integer = "";
  integer += command[0];
  byte isPersson = integer.toInt();

  //testa se o usuário está tentando se desconectar
  if (command[0] == 'c') {
    try_disconnecttelnet();
  }

  switch (command[1]) {

    case 'l': {
        if (String(String(command[2]) + String(command[3])) == "ux") {
          //Testa se é uma pessoa ou um sistema que está conectado, se for uma pessoa, ele envia o texto relacionado À variável, se for sistema, apenas o valor da variável
          if (isPersson) {
            client_telnet.print(F("Luminosidade no sensor em lux: "));
            client_telnet.print(lux);
            client_telnet.print(F(" lx\n\r"));
            client_telnet.stop();
            client_telnet = 0;
            return;
          }
          else if (!isPersson) {
            client_telnet.print(lux);
            client_telnet.stop();
            client_telnet = 0;
            //            alreadyConnected = false;
            return;
          }

        }
        else {
          integer = "";
          integer += command[2];
          sensor = integer.toInt();
          //          getHumidity(*humidity_sensors[sensor], sensor);

          //Testa se é uma pessoa ou um sistema que está conectado, se for uma pessoa, ele envia o texto relacionado À variável, se for sistema, apenas o valor da variável
          if (isPersson) {
            client_telnet.print(F("Porcentagem da luminosidade ideal no sensor "));
            client_telnet.print(sensor);
            client_telnet.print(F(" : "));
            client_telnet.print(percent_ldr[sensor]);
            client_telnet.print(F(" percent\n\r"));
            client_telnet.stop();
            client_telnet = 0;
            return;
          }
          else if (!isPersson) {
            client_telnet.print(percent_ldr[sensor]);
            client_telnet.stop();
            client_telnet = 0;
            return;
          }

        }
        break;
      }

    case 's': {
        if (command[2] == 'p') {
          integer = "";
          integer += command[3];
          sensor = integer.toInt();
          //          getTemperature(*humidity_sensors[sensor]);

          //Testa se é uma pessoa ou um sistema que está conectado, se for uma pessoa, ele envia o texto relacionado À variável, se for sistema, apenas o valor da variável
          if (isPersson) {
            if (door[sensor] == DOOR_OPEN) {
              client_telnet.print(F("Porta Aberta no sensor "));
              client_telnet.print(sensor);
              client_telnet.print(F(" !"));
            }
            else if (door[sensor] == DOOR_CLOSED) {
              client_telnet.print(F("Porta Fechada no sensor "));
              client_telnet.print(sensor);
              client_telnet.print(F(" ."));
            }

            client_telnet.stop();
            client_telnet = 0;
            return;
          }
          else if (!isPersson) {
            client_telnet.print(door[sensor]);
            client_telnet.stop();
            client_telnet = 0;
            //            alreadyConnected = false;
            return;
          }
        }
        else {
          integer = "";
          integer += command[2];
          sensor = integer.toInt();
          //          getTemperature(*humidity_sensors[sensor]);

          //Testa se é uma pessoa ou um sistema que está conectado, se for uma pessoa, ele envia o texto relacionado À variável, se for sistema, apenas o valor da variável
          if (isPersson) {
            client_telnet.print(F("Nivel de som no sensor "));
            client_telnet.print(sensor);
            client_telnet.print(F(" : "));
            client_telnet.print(db[sensor]);
            client_telnet.print(F(" dB\n\r"));

            client_telnet.stop();
            client_telnet = 0;
            return;
          }
          else if (!isPersson) {
            client_telnet.print(db[sensor]);
            client_telnet.stop();
            client_telnet = 0;
            //            alreadyConnected = false;
            return;
          }
        }
        break;
      }

    case 'v': {
        integer = "";
        integer += command[2];
        sensor = integer.toInt();
        //          getTemperature(*humidity_sensors[sensor]);

        //Testa se é uma pessoa ou um sistema que está conectado, se for uma pessoa, ele envia o texto relacionado À variável, se for sistema, apenas o valor da variável
        if (isPersson) {
          client_telnet.print(F("Vibracao no sensor "));
          client_telnet.print(sensor);
          client_telnet.print(F(" : "));
          client_telnet.print(vibration[sensor]);
          client_telnet.print(F(" mm per (square second)\n\r"));
          client_telnet.stop();
          client_telnet = 0;
          return;
        }
        else if (!isPersson) {
          client_telnet.print(vibration[sensor]);
          client_telnet.stop();
          client_telnet = 0;
          //            alreadyConnected = false;
          return;
        }
        break;
      }
    case 'c': {
        if (String(String(command[2]) + String(command[3])) == "ur") {
          if (String(command[4]) == "n") {
            //Testa se é uma pessoa ou um sistema que está conectado, se for uma pessoa, ele envia o texto relacionado À variável, se for sistema, apenas o valor da variável
            if (isPersson) {
              client_telnet.print(F("Corrente no neutro : "));
              client_telnet.print(Irms_neutro);
              client_telnet.println(F(" A rms"));
              client_telnet.stop();
              client_telnet = 0;
              return;
            }
            else if (!isPersson) {
              client_telnet.print(Irms_neutro);
              client_telnet.stop();
              client_telnet = 0;
              //            alreadyConnected = false;
              return;
            }
          }
          else if (String(command[4]) == "t") {
            //Testa se é uma pessoa ou um sistema que está conectado, se for uma pessoa, ele envia o texto relacionado À variável, se for sistema, apenas o valor da variável
            if (isPersson) {
              client_telnet.print(F("Corrente total : "));
              client_telnet.print(Irms_total);
              client_telnet.println(F(" A rms"));
              client_telnet.stop();
              client_telnet = 0;
              return;
            }
            else if (!isPersson) {
              client_telnet.print(Irms_total);
              client_telnet.stop();
              client_telnet = 0;
              //            alreadyConnected = false;
              return;
            }
          }
          else {

            integer = "";
            byte i = 4;
            while (isDigit(command[i])) {
              integer += command[i];
              i++;
            }
            sensor = integer.toInt();

            //Testa se é uma pessoa ou um sistema que está conectado, se for uma pessoa, ele envia o texto relacionado À variável, se for sistema, apenas o valor da variável
            if (isPersson) {
              client_telnet.print(F("Corrente no sensor de corrente "));
              client_telnet.print(sensor);
              client_telnet.print(F(" : "));
              client_telnet.print(Irms[sensor]);
              client_telnet.println(F(" A rms"));
              client_telnet.stop();
              client_telnet = 0;
              return;
            }
            else if (!isPersson) {
              client_telnet.print(Irms[sensor]);
              client_telnet.stop();
              client_telnet = 0;
              //            alreadyConnected = false;
              return;
            }
          }
        }

        else if (String(String(command[2]) + String(command[3])) == "ap") {
          if (String(command[4]) == "t") {
            //Testa se é uma pessoa ou um sistema que está conectado, se for uma pessoa, ele envia o texto relacionado À variável, se for sistema, apenas o valor da variável
            if (isPersson) {
              client_telnet.print(F("Consumo aparente de energia total : "));
              client_telnet.print(consumo_kvah_total);
              client_telnet.println(F(" kVAh"));
              client_telnet.stop();
              client_telnet = 0;
              return;
            }
            else if (!isPersson) {
              client_telnet.print(consumo_kvah_total);
              client_telnet.stop();
              client_telnet = 0;
              //            alreadyConnected = false;
              return;
            }
          }
          else {
            integer = "";
            byte i = 4;
            while (isDigit(command[i])) {
              integer += command[i];
              i++;
            }
            sensor = integer.toInt();
            //Testa se é uma pessoa ou um sistema que está conectado, se for uma pessoa, ele envia o texto relacionado À variável, se for sistema, apenas o valor da variável
            if (isPersson) {
              client_telnet.print(F("Consumo aparente de energia no sensor de corrente "));
              client_telnet.print(sensor);
              client_telnet.print(F(" : "));
              client_telnet.print(consumo_kvah[sensor]);
              client_telnet.println(F(" kVAh"));
              client_telnet.stop();
              client_telnet = 0;
              return;
            }
            else if (!isPersson) {
              client_telnet.print(consumo_kvah[sensor]);
              client_telnet.stop();
              client_telnet = 0;
              //            alreadyConnected = false;
              return;
            }
          }
        }
        else if (String(String(command[2]) + String(command[3])) == "re") {
          integer = "";
          byte i = 4;
          while (isDigit(command[i])) {
            integer += command[i];
            i++;
          }
          sensor = integer.toInt();
          //Testa se é uma pessoa ou um sistema que está conectado, se for uma pessoa, ele envia o texto relacionado À variável, se for sistema, apenas o valor da variável
          if (isPersson) {
            if (tem_power_factor[sensor]) {
              client_telnet.print(F("Consumo ativo de energia no sensor de corrente "));
              client_telnet.print(sensor);
              client_telnet.print(F(" : "));
              client_telnet.print(consumo_kwh[sensor]);
              client_telnet.println(F(" kWh"));
            }
            else {
              client_telnet.print(F("Nenhum fator de potencia associado ao sensor "));
              client_telnet.println(sensor);
            }
            client_telnet.stop();
            client_telnet = 0;
            return;
          }
          else if (!isPersson) {
            if (tem_power_factor[sensor]) {
              client_telnet.print(consumo_kwh[sensor]);
            }
            else {
              client_telnet.print(F("NONE FP ASSIGNED"));
            }
            client_telnet.stop();
            client_telnet = 0;
            //            alreadyConnected = false;
            return;
          }

        }

        else if (String(String(command[2]) + String(command[3])) == "im") {
          integer = "";
          byte i = 4;
          while (isDigit(command[i])) {
            integer += command[i];
            i++;
          }
          sensor = integer.toInt();
          //Testa se é uma pessoa ou um sistema que está conectado, se for uma pessoa, ele envia o texto relacionado À variável, se for sistema, apenas o valor da variável
          if (isPersson) {
            if (tem_power_factor[sensor]) {
              client_telnet.print(F("Excedente de energia reativa no sensor de corrente "));
              client_telnet.print(sensor);
              client_telnet.print(F(" : "));
              client_telnet.print(consumo_kvarh[sensor]);
              client_telnet.println(F(" kvarh"));
            }
            else {
              client_telnet.print(F("Nenhum fator de potencia associado ao sensor "));
              client_telnet.println(sensor);
            }
            client_telnet.stop();
            client_telnet = 0;
            return;
          }
          else if (!isPersson) {
            if (tem_power_factor[sensor]) {
              client_telnet.print(consumo_kvarh[sensor]);
            }
            else {
              client_telnet.print(F("NONE FP ASSIGNED"));
            }
            client_telnet.stop();
            client_telnet = 0;
            //            alreadyConnected = false;
            return;
          }

        }

        else {
          integer = "";
          integer += command[2];
          sensor = integer.toInt();

          //Testa se é uma pessoa ou um sistema que está conectado, se for uma pessoa, ele envia o texto relacionado À variável, se for sistema, apenas o valor da variável
          if (isPersson) {
            client_telnet.print(F("Chance de incendio no sensor de chama "));
            client_telnet.print(sensor);
            client_telnet.print(F(" : "));
            client_telnet.print(percent_flame[sensor]);
            client_telnet.print(F(" percent\n\r"));
            client_telnet.stop();
            client_telnet = 0;
            return;
          }
          else if (!isPersson) {
            client_telnet.print(percent_flame[sensor]);
            client_telnet.stop();
            client_telnet = 0;
            //            alreadyConnected = false;
            return;
          }
          break;
        }
      }

    case 'i': {
        integer = "";
        integer += command[2];
        sensor = integer.toInt();

        //Testa se é uma pessoa ou um sistema que está conectado, se for uma pessoa, ele envia o texto relacionado À variável, se for sistema, apenas o valor da variável
        if (isPersson) {
          client_telnet.print(F("Chance de inundacao no sensor "));
          client_telnet.print(sensor);
          client_telnet.print(F(" : "));
          client_telnet.print(valor_inundacao[sensor]);
          client_telnet.print(F(" percent\n\r"));
          client_telnet.stop();
          client_telnet = 0;
          return;
        }
        else if (!isPersson) {
          client_telnet.print(valor_inundacao[sensor]);
          client_telnet.stop();
          client_telnet = 0;
          //            alreadyConnected = false;
          return;
        }
        break;
      }

    case 'a': {
        if (String(String(command[2]) + String(command[3])) == "lt") {
          //Testa se é uma pessoa ou um sistema que está conectado, se for uma pessoa, ele envia o texto relacionado À variável, se for sistema, apenas o valor da variável
          if (isPersson) {

            client_telnet.print(F("Altitude: "));
            client_telnet.print(altitude);
            client_telnet.print(F(" Metros\n\r"));
            client_telnet.stop();
            client_telnet = 0;
            return;
          }
          else if (!isPersson) {
            client_telnet.print(temp_interna);
            client_telnet.stop();
            client_telnet = 0;
            //            alreadyConnected = false;
            return;
          }
        }
        else {
          if (isPersson) {
            client_telnet.println(F("Acesso Autorizado: "));
            client_telnet.println(*definite_user);
            client_telnet.stop();
            client_telnet = 0;
            return;
          }
          else if (!isPersson) {
            client_telnet.print(*definite_user);
            client_telnet.stop();
            client_telnet = 0;
            return;
          }
        }
        break;
      }

    case 'f': {
        if (isPersson) {
          client_telnet.println(F("Nivel indicativo de incêndio: "));
          client_telnet.println(air_quality);
          client_telnet.println(F("%"));
          client_telnet.stop();
          client_telnet = 0;
          return;
        }
        else if (!isPersson) {
          client_telnet.print(air_quality);
          client_telnet.stop();
          client_telnet = 0;
          return;
        }
        break;
      }

    case 'p': {
        if (String(String(command[2]) + String(command[3]) + String(command[4])) == "res") {
          //Testa se é uma pessoa ou um sistema que está conectado, se for uma pessoa, ele envia o texto relacionado À variável, se for sistema, apenas o valor da variável
          if (isPersson) {
            client_telnet.print(F("Pressão: "));
            client_telnet.print(pressure);
            client_telnet.print(F(" Pascals\n\r"));
            client_telnet.stop();
            client_telnet = 0;
            return;
          }
          else if (!isPersson) {
            client_telnet.print(pressure);
            client_telnet.stop();
            client_telnet = 0;
            //            alreadyConnected = false;
            return;
          }
        }
        else if (String(String(command[2]) + String(command[3])) == "ap") {
          if (String(command[4]) == "t") {
            //Testa se é uma pessoa ou um sistema que está conectado, se for uma pessoa, ele envia o texto relacionado À variável, se for sistema, apenas o valor da variável
            if (isPersson) {
              client_telnet.print(F("Potencia aparente total : "));
              client_telnet.print(potencia_aparente_total);
              client_telnet.println(F(" VA"));
              client_telnet.stop();
              client_telnet = 0;
              return;
            }
            else if (!isPersson) {
              client_telnet.print(potencia_aparente_total);
              client_telnet.stop();
              client_telnet = 0;
              //            alreadyConnected = false;
              return;
            }
          }
          else {
            integer = "";
            byte i = 4;
            while (isDigit(command[i])) {
              integer += command[i];
              i++;
            }
            sensor = integer.toInt();
            //Testa se é uma pessoa ou um sistema que está conectado, se for uma pessoa, ele envia o texto relacionado À variável, se for sistema, apenas o valor da variável
            if (isPersson) {
              client_telnet.print(F("Potencia aparente no sensor de corrente "));
              client_telnet.print(sensor);
              client_telnet.print(F(" : "));
              client_telnet.print(potencia_aparente[sensor]);
              client_telnet.println(F(" VA"));
              client_telnet.stop();
              client_telnet = 0;
              return;
            }
            else if (!isPersson) {
              client_telnet.print(potencia_aparente[sensor]);
              client_telnet.stop();
              client_telnet = 0;
              //            alreadyConnected = false;
              return;
            }
          }
        }
        else if (String(String(command[2]) + String(command[3])) == "re") {
          integer = "";
          byte i = 4;
          while (isDigit(command[i])) {
            integer += command[i];
            i++;
          }
          sensor = integer.toInt();
          //Testa se é uma pessoa ou um sistema que está conectado, se for uma pessoa, ele envia o texto relacionado À variável, se for sistema, apenas o valor da variável
          if (isPersson) {
            if (tem_power_factor[sensor]) {
              client_telnet.print(F("Potencia ativa no sensor de corrente "));
              client_telnet.print(sensor);
              client_telnet.print(F(" : "));
              client_telnet.print(potencia_ativa[sensor]);
              client_telnet.println(F(" W"));
            }
            else {
              client_telnet.print(F("Nenhum fator de potencia associado ao sensor "));
              client_telnet.println(sensor);
            }
            client_telnet.stop();
            client_telnet = 0;
            return;
          }
          else if (!isPersson) {
            if (tem_power_factor[sensor]) {
              client_telnet.print(potencia_ativa[sensor]);
            }
            else {
              client_telnet.print(F("NONE FP ASSIGNED"));
            }
            client_telnet.stop();
            client_telnet = 0;
            //            alreadyConnected = false;
            return;
          }

        }
        else if (String(String(command[2]) + String(command[3])) == "im") {
          integer = "";
          byte i = 4;
          while (isDigit(command[i])) {
            integer += command[i];
            i++;
          }
          sensor = integer.toInt();
          //Testa se é uma pessoa ou um sistema que está conectado, se for uma pessoa, ele envia o texto relacionado À variável, se for sistema, apenas o valor da variável
          if (isPersson) {
            if (tem_power_factor[sensor]) {
              client_telnet.print(F("Potencia reativa no sensor de corrente "));
              client_telnet.print(sensor);
              client_telnet.print(F(" : "));
              client_telnet.print(potencia_reativa[sensor]);
              client_telnet.println(F(" var"));
            }
            else {
              client_telnet.print(F("Nenhum fator de potencia associado ao sensor "));
              client_telnet.println(sensor);
            }
            client_telnet.stop();
            client_telnet = 0;
            return;
          }
          else if (!isPersson) {
            if (tem_power_factor[sensor]) {
              client_telnet.print(potencia_reativa[sensor]);
            }
            else {
              client_telnet.print(F("NONE FP ASSIGNED"));
            }
            client_telnet.stop();
            client_telnet = 0;
            //            alreadyConnected = false;
            return;
          }

        }
        else {
          //        verifica_presenca();
          if (isPersson) {
            if (presenca_web == 0) //Sem movimento, mantem rele desligado
            {
              client_telnet.println(F("Sem movimentos detectados..."));
            }
            else if (presenca_web == 1) //Caso seja detectado um movimento
            {
              client_telnet.println(F("Movimento autorizado na sala."));
            }
            else if (presenca_web == 2) {
              client_telnet.println(F("Movimento nao autorizado na sala!"));
            }
            client_telnet.stop();
            client_telnet = 0;
            return;
          }
          else if (!isPersson) {
            client_telnet.print(presenca_web);
            client_telnet.stop();
            client_telnet = 0;
            return;
          }

        }
        break;
      }
    case 't': {
        if (String(command[2]) == "i") {
          //Testa se é uma pessoa ou um sistema que está conectado, se for uma pessoa, ele envia o texto relacionado À variável, se for sistema, apenas o valor da variável
          if (isPersson) {
            client_telnet.print(F("Temperatura Interna: "));
            client_telnet.print(temp_interna);
            client_telnet.print(F(" Celsius Degrees\n\r"));
            client_telnet.stop();
            client_telnet = 0;
            return;
          }
          else if (!isPersson) {
            client_telnet.print(temp_interna);
            client_telnet.stop();
            client_telnet = 0;
            //            alreadyConnected = false;
            return;
          }
        }

        else {
          integer = "";
          integer += command[2];
          corredor = integer.toInt();
          integer = "";
          integer += command[3];
          sensor = integer.toInt();
          //        nodos_sensores_temperatura[corredor]->requestTemperatures();
          //        getTemperature(*nodos_sensores_temperatura[corredor], devices[corredor][sensor], corredor, sensor);

          //Testa se é uma pessoa ou um sistema que está conectado, se for uma pessoa, ele envia o texto relacionado À variável, se for sistema, apenas o valor da variável
          if (isPersson) {
            client_telnet.print(F("Temperatura no Sensor "));
            client_telnet.print(corredor);
            client_telnet.print(sensor);
            client_telnet.print(F(" : "));
            client_telnet.print(tempC[corredor][sensor]);
            //          Serial.print("TempC: ");
            //          Serial.println(tempC[corredor][sensor]);
            client_telnet.print(F(" Celsius Degrees\n\r"));
            client_telnet.stop();
            client_telnet = 0;
            return;

          }
          else if (!isPersson) {
            client_telnet.print(tempC[corredor][sensor]);
            client_telnet.stop();
            client_telnet = 0;
            //          alreadyConnected = false;
            return;
          }

        }
        break;
      }
    case 'h': {

        if (command[2] == 't') {
          integer = "";
          integer += command[3];
          sensor = integer.toInt();
          //          getTemperature(*humidity_sensors[sensor]);

          //Testa se é uma pessoa ou um sistema que está conectado, se for uma pessoa, ele envia o texto relacionado À variável, se for sistema, apenas o valor da variável
          if (isPersson) {
            client_telnet.print(F("Temperatura no Sensor de umidade "));
            client_telnet.print(sensor);
            client_telnet.print(F(" : "));
            client_telnet.print(temp_sensor_umidade[sensor]);
            client_telnet.print(F(" Celsius Degrees\n\r"));
            client_telnet.stop();
            client_telnet = 0;
            return;
          }
          else if (!isPersson) {
            client_telnet.print(temp_sensor_umidade[sensor]);
            client_telnet.stop();
            client_telnet = 0;
            //            alreadyConnected = false;
            return;
          }
        }
        else {
          integer = "";
          integer += command[2];
          sensor = integer.toInt();
          //          getHumidity(*humidity_sensors[sensor], sensor);

          //Testa se é uma pessoa ou um sistema que está conectado, se for uma pessoa, ele envia o texto relacionado À variável, se for sistema, apenas o valor da variável
          if (isPersson) {
            client_telnet.print(F("Umidade no Sensor de umidade "));
            client_telnet.print(sensor);
            client_telnet.print(F(" : "));
            client_telnet.print(umidade[sensor]);
            client_telnet.print(F(" percent\n\r"));
            client_telnet.stop();
            client_telnet = 0;
            return;
          }
          else if (!isPersson) {
            client_telnet.print(umidade[sensor]);
            client_telnet.stop();
            client_telnet = 0;
            return;
          }

        }
        break;
      }

    case 'm': {

        if (command[2] == 'g') {
          //          atualiza_media_geral();

          //Testa se é uma pessoa ou um sistema que está conectado, se for uma pessoa, ele envia o texto relacionado À variável, se for sistema, apenas o valor da variável
          if (isPersson) {
            client_telnet.print(F("Media geral :"));
            client_telnet.print(media_geral);
            client_telnet.print(F(" Celsius Degrees\n\r"));
            //            Serial.println(media_geral);
            client_telnet.stop();
            client_telnet = 0;
            return;
          }
          else if (!isPersson) {
            client_telnet.print(media_geral);
            client_telnet.stop();
            client_telnet = 0;
            //            alreadyConnected = false;
            return;
          }

        }
        else if (command[2] == 'h') {
          //          atualiza_media_umidade();

          if (command[3] == 't') {
            //Testa se é uma pessoa ou um sistema que está conectado, se for uma pessoa, ele envia o texto relacionado À variável, se for sistema, apenas o valor da variável
            if (isPersson) {
              client_telnet.print("Media geral temperatura DHT:");
              client_telnet.print( media_temp_dht );
              client_telnet.print(" Celsius Degrees\n\r");
              return;
            }
            else if (!isPersson) {
              client_telnet.print( media_temp_dht );
              client_telnet.stop();
              client_telnet = 0;
              return;
            }

          }

          //Testa se é uma pessoa ou um sistema que está conectado, se for uma pessoa, ele envia o texto relacionado À variável, se for sistema, apenas o valor da variável
          if (isPersson) {
            client_telnet.print(F("Umidade relativa media geral :"));
            client_telnet.print(media_umidade);
            client_telnet.print(F(" percent\n\r")); client_telnet.stop();
            client_telnet = 0;
            return;
          }
          else if (!isPersson) {
            client_telnet.print(media_umidade);
            client_telnet.stop();
            client_telnet = 0;
            //            alreadyConnected = false;
            return;
          }

        }
        else {
          integer = "";
          integer += command[2];
          corredor = integer.toInt();
          aux = (int) nodos_sensores_temperatura[corredor]->getDeviceCount();
          //          atualiza_medias(corredor, aux);

          //Testa se é uma pessoa ou um sistema que está conectado, se for uma pessoa, ele envia o texto relacionado À variável, se for sistema, apenas o valor da variável
          if (isPersson) {
            client_telnet.print(F("Media no corredor "));
            client_telnet.print(corredor);
            client_telnet.print(F(" : "));
            client_telnet.print(media_leitura[corredor]);
            client_telnet.print(F(" Celsius Degrees\n\r"));
            client_telnet.stop();
            client_telnet = 0;
            return;
          }
          else if (!isPersson) {
            client_telnet.print(float(media_leitura[corredor]));
            client_telnet.stop();
            client_telnet = 0;
            //            alreadyConnected = false;
            return;
          }

        }
        break;
      }

    default: {
        client_telnet.stop();
        client_telnet = 0;
        break;
      }
  }
  return;

}

//Inicializa devs[] e dallastemp[], necessários durante o setup dos sensores de temperatura Dallas.
void inicializa_devs_e_dallastemp() {
  devs = (OneWire**)malloc(NUM_ONEWIRE_PORTS * sizeof(OneWire*));
  dallastemp = (DallasTemperature**)malloc(NUM_ONEWIRE_PORTS * sizeof(DallasTemperature*));
  for (byte i = 0; i < NUM_ONEWIRE_PORTS; i++) {
    devs[i] = new OneWire(digitalPorts[i]);
    devs[i]->reset();
    dallastemp[i] = new DallasTemperature;
    dallastemp[i]->setOneWire(&(*devs[i]));
    dallastemp[i]->begin();
  }
  //  Serial.print("freeMemory()=");
  //  Serial.println(freeMemory());
  return;
}
//Atualiza a média de cada corredor
void atualiza_medias(byte i, byte num_sensores_nodo) {
  float med = 0;
  for (byte j = 0; j < num_sensores_nodo; j++) {
    med += tempC[i][j];
  }
  med = med / float(num_sensores_nodo);
  //  if (med <= float(0 - 55) || med >= float(0 + 99) || med == float(85)) {
  //    Serial.println(F("Erro na media "));
  //    Serial.println(i);
  //    Serial.println(F(" !!!!!"));
  //  }
  if (med > float(-55) && med < float(99) && med != float(85)) {
    media_leitura[i] = med;
  }
}

//Atualiza a média geral de temepratura da sala
float atualiza_media_geral() {
  float med = 0;
  for (byte i = 0; i < real_num_onewire_ports; i++) {
    med += media_leitura[i];
  }
  med = med / float(real_num_onewire_ports);

  //  if ((double)med <= (double)(-55) || (double)med >= (double)99 || (double)med == (double)85) {
  //    Serial.println(F("Erro na media geral !!!!!!!!! "));
  //  }
  if (med > float(-55) && med < float(99) && med != float(85)) {
    media_geral = med;
  }
}

float atualiza_media_umidade () {
  float med = 0;
  for (byte i = 0; i < real_num_hum_sensors; i++) {
    med += umidade[i];
  }
  med = med / (float) real_num_hum_sensors;

  //  if ((double)med < (double) 20 || (double)med > (double) 80) {
  //    Serial.println(F("Erro na media da umidade!!!!!!"));
  //  }
  if (med >= 20 &&  med <=  80) {
    media_umidade = med;
  }
}

//Função para obter a temperatura de um dispositivo Dallas e salvá-la na matriz de temperaturas tempC
void getTemperature(DallasTemperature sensors, uint8_t* dev, byte m, byte n) {
  float t;
  t = float(sensors.getTempC(dev));
  //  if (t <= float(-55) || t >= float(+99) || t == float(85)) {
  //    Serial.println(F("Erro no dallas!!!!!!!!"));
  //  }
  if (t > float(-55) && t < float(99) && t != float(85)) {
    tempC[m][n] = t;
    //  Serial.println(tempC[m][n]);
  }
}

//Função para obter a temperatura de um dispositivo DHT11 e salvá-la na variável adequada
void getTemperature(byte i) {
  float t;
  t = (*humidity_sensors[i]).readTemperature();
  //  if ((double)t <= (double)0 || (double) t >= (double)50) {
  //    Serial.println(F("Erro no DHT!!!!!!"));
  //  }
  if (t >= 0 && t <= 50) {
    temp_sensor_umidade[i] = t;
  }
}


//Função para obter media das temperaturas DHT
void atualiza_media_temp_dht() {
  float med = 0;
  //  byte tamanho = sizeof(analogPorts) / sizeof(analogPorts[0]);
  byte tamanho = (float) real_num_hum_sensors;
  for (byte i = 0; i < tamanho; i++) {
    med += temp_sensor_umidade[i];
  }
  med /= tamanho;
  media_temp_dht = med;
}



//Função para obter a umidade de um dispositivo DHT11 e salvá-la na variável adequada
void getHumidity(byte i) {
  float umid = 0;
  umid = float((*humidity_sensors[i]).readHumidity());
  //  if (umid < float(20) || umid > float(80))   {
  //    Serial.println(F("Erro na umidade!!!!!!!!"));
  //  }
  if (umid >= float(20) && umid <= float(80) ) {
    umidade[i] = umid;
  }
}

//Função usada no setup dos sensores de temepratura Dallas (para inicializar auxiliar[])
DHT * cria_objeto_dht (uint8_t port, byte i) {
  DHT device(port, DHTTYPE);
  dht[i] = device;
  return &dht[i];
}

//Função para alocar os 'DeviceAddress' na memória e retornar seus endereços de alocação
uint8_t** alocaMemoriaDevices (byte x, byte y) {
  if (y == 0) {}
  else {
    uint8_t** dev;
    dev = (uint8_t**) malloc(y * sizeof(uint8_t*));
    for (int i = 0 ; i < y ; i++) {
      dev[i] = (uint8_t*) malloc(8 * sizeof(uint8_t));
    }
    return dev;
  }
}

void verifica_presenca() { //só muda o valor de presença quando faz 4 leituras que indicam presença
  if (presenca_web != 0 && ((millis() - tempo_web) >= 60000) ) {
    presenca_web = 0;
  }



  byte p;
  //  Serial.println(digitalRead(pir));
  if (digitalRead(pir)) {
    p = 1;
  }
  else {
    p = 0;
  }
  num_leituras++;
  media += p;
  if (num_leituras == 15) {
    if (media <= (num_leituras - 1)) {
      presenca = 0;
    }
    else {
      presenca = 1;

      if (definite_user->equals(" ")) {
        presenca_web = 2;
      }
      else {
        presenca_web = 1;
      }
      tempo_web = millis();
    }
    media = 0;
    num_leituras = 0;
  }
}

void read_inundacao() {
  byte moist = 0;
  for (byte i = 0; i < NUMBER_INUNDACAO; i++) {
    moist = byte(map(analogRead(pino_inundacao[i]), 0, 400, 0, 100));
    if (moist > 100) {
      moist = 100;
    }
    valor_inundacao[i] = moist;
  }
}

void read_sound() {
  float sound = 0;
  for (byte i = 0; i < NUMBER_SOUND; i++) {
    sound = analogRead(soundPin[i]);
    db[i] = float(20 * log(sound / float(ADC_SOUND_REF)) + float(DB_SOUND_REF));
  }
}

void read_vibration() {
  if ((millis() - time_last_vibration) <= VIBRATION_INTERVAL) {


    float vib = 0;
    for (byte i = 0; i < NUMBER_VIBRATION; i++) {
      //    Serial.println(analogRead(vibrationPin[i]));
      vib = map((1024 - analogRead(vibrationPin[i])), 0, 1024, float(0.0), float(15.5));
      if (vib > float(15.5)) {
        vib = float(15.5);
      }
      if (vib > vibration_samp[i]) {
        vibration_samp[i] = vib;
      }

    }
  }
  else {
    for (byte i = 0; i < NUMBER_VIBRATION; i++) {
      //    Serial.println(analogRead(vibrationPin[i]));
      vibration[i] = vibration_samp[i];
      vibration_samp[i] = 0;
      //      Serial.println(vibration[i]);
    }
    time_last_vibration = millis();
  }
}

void read_flame() {
  float flam = 0;
  for (byte i = 0; i < NUMBER_FLAME; i++) {
    //    Serial.println(analogRead(flamePin[i]));
    flam = map((1024 - analogRead(flamePin[i])), 0, 1024, float(0.0), float(100.0));
    if (flam > float(100.0)) {
      flam = float(100.0);
    }
    percent_flame[i] = flam;
  }
}

void read_door() {
  byte porta = 0;
  for (byte i = 0; i < NUMBER_DOOR; i++) {
    porta = digitalRead(doorPin[i]);
    if (porta == DOOR_OPEN && definite_user->equals(" ")) {
      porta = 2;
    }
    door[i] = porta;
  }
}

void read_light() {
  int photocellReading = 0;  // the analog reading from the analog resistor divider
  float vout = 0;
  for (byte i = 0; i < NUMBER_LDRs; i++) {

    percent_ldr[i] = byte(map(analogRead(photocellPin[i]), 0, 1023, 0, 100));
    //    Serial.print("Iluminacao na sala no local ");
    //    Serial.print(i);
    //    Serial.print(" :");
    //    Serial.print(percent_ldr[i]);
    //    Serial.println(" %");


    //    photocellReading = analogRead(photocellPin[i]);   // Read the analogue pin
    //    vout = photocellReading * 0.0048828125;  // calculate the voltage
    //    lux[i] = 500 / (Res * ((5 - vout) / vout)); // calculate the Lux
    //    //    Serial.print("Luminosidad 0: ");                 // Print the measurement (in Lux units) in the screen
    //    //    Serial.print(lux[i]);
  }

  if (LUX_SENSOR) {
    lux = lightMeter.readLightLevel();
  }
}


/****************** MQResistanceCalculation ****************************************
  Input:   raw_adc - raw value read from adc, which represents the voltage
  Output:  the calculated sensor resistance
  Remarks: The sensor and the load resistor forms a voltage divider. Given the voltage
         across the load resistor and its resistance, the resistance of the sensor
         could be derived.
************************************************************************************/
float MQResistanceCalculation(int raw_adc)
{
  return ( ((float)RL_VALUE * (1023 - raw_adc) / raw_adc));
}

/***************************** MQCalibration ****************************************
  Input:   mq_pin - analog channel
  Output:  Ro of the sensor
  Remarks: This function assumes that the sensor is in clean air. It use
         MQResistanceCalculation to calculates the sensor resistance in clean air
         and then divides it with RO_CLEAN_AIR_FACTOR. RO_CLEAN_AIR_FACTOR is about
         10, which differs slightly between different sensors.
************************************************************************************/
float MQCalibration(int mq_pin)
{
  if (cont_calibration < CALIBARAION_SAMPLE_TIMES) {         //take multiple samples
    val_calibration += MQResistanceCalculation(map(analogRead(mq_pin) + VOLTAGE_COMPENSATION, 0, 777, 0, 1023)); //o map é para adequar os 5v com os 3.8v verificados no sensor
    cont_calibration++;
  }
  else {
    val_calibration = val_calibration / CALIBARAION_SAMPLE_TIMES;                 //calculate the average value

    val_calibration = val_calibration / RO_CLEAN_AIR_FACTOR;                      //divided by RO_CLEAN_AIR_FACTOR yields the Ro
    //according to the chart in the datasheet
    if (val_calibration < 1) {
      val_calibration = 1;
    }
    Ro = val_calibration;
    val_calibration = 0;
    cont_calibration = 0;
    calibrate = false;
    //    Serial.println(F("Calibration complete!"));
    //    Serial.print(F("Ro = "));
    //    Serial.println(Ro);
  }
}
/*****************************  MQRead *********************************************
  Input:   mq_pin - analog channel
  Output:  Rs of the sensor
  Remarks: This function use MQResistanceCalculation to caculate the sensor resistenc (Rs).
         The Rs changes as the sensor is in the different consentration of the target
         gas. The sample times and the time interval between samples could be configured
         by changing the definition of the macros.
************************************************************************************/
float MQRead(int mq_pin)
{
  if (cont_read < READ_SAMPLE_TIMES) {
    val_read += MQResistanceCalculation(map(analogRead(mq_pin) + VOLTAGE_COMPENSATION, 0, 777, 0, 1023));
    cont_read++;
  }
  else {
    val_read = val_read / READ_SAMPLE_TIMES;
    rs = val_read;
    val_read = 0;
    cont_read = 0;
  }
}

/*****************************  MQGetGasPercentage **********************************
  Input:   rs_ro_ratio - Rs divided by Ro
         gas_id      - target gas type
  Output:  ppm of the target gas
  Remarks: This function passes different curves to the MQGetPercentage function which
         calculates the ppm (parts per million) of the target gas.
************************************************************************************/
int MQGetGasPercentage(float rs_ro_ratio, int gas_id)
{
  if ( gas_id == GAS_LPG ) {
    return MQGetPercentage(rs_ro_ratio, LPGCurve);
  } else if ( gas_id == GAS_CO ) {
    return MQGetPercentage(rs_ro_ratio, COCurve);
  } else if ( gas_id == GAS_SMOKE ) {
    return MQGetPercentage(rs_ro_ratio, SmokeCurve);
  }

  return 0;
}

/*****************************  MQGetPercentage **********************************
  Input:   rs_ro_ratio - Rs divided by Ro
         pcurve      - pointer to the curve of the target gas
  Output:  ppm of the target gas
  Remarks: By using the slope and a point of the line. The x(logarithmic value of ppm)
         of the line could be derived if y(rs_ro_ratio) is provided. As it is a
         logarithmic coordinate, power of 10 is used to convert the result to non-logarithmic
         value.
************************************************************************************/
int  MQGetPercentage(float rs_ro_ratio, float *pcurve)
{
  return (pow(10, ( ((log(rs_ro_ratio) - pcurve[1]) / pcurve[2]) + pcurve[0])));
}




void pduReceived()
{
  SNMP_PDU pdu;
  api_status = Agentuino.requestPdu(&pdu);
  //
  if ((pdu.type == SNMP_PDU_GET || pdu.type == SNMP_PDU_GET_NEXT || pdu.type == SNMP_PDU_SET)
      && pdu.error == SNMP_ERR_NO_ERROR && api_status == SNMP_API_STAT_SUCCESS ) {
    //
    pdu.OID.toString(oid);
    Serial.println(oid);
    // Implementation SNMP GET NEXT
    if ( pdu.type == SNMP_PDU_GET_NEXT ) {
      char tmpOIDfs[SNMP_MAX_OID_LEN];
      if ( strcmp_P( oid, sysDescr ) == 0 ) {
        strcpy_P ( oid, sysUpTime );
        strcpy_P ( tmpOIDfs, sysUpTime );
        pdu.OID.fromString(tmpOIDfs);
      } else if ( strcmp_P(oid, sysUpTime ) == 0 ) {
        strcpy_P ( oid, sysContact );
        strcpy_P ( tmpOIDfs, sysContact );
        pdu.OID.fromString(tmpOIDfs);
      } else if ( strcmp_P(oid, sysContact ) == 0 ) {
        strcpy_P ( oid, sysName );
        strcpy_P ( tmpOIDfs, sysName );
        pdu.OID.fromString(tmpOIDfs);
      } else if ( strcmp_P(oid, sysName ) == 0 ) {
        strcpy_P ( oid, sysLocation );
        strcpy_P ( tmpOIDfs, sysLocation );
        pdu.OID.fromString(tmpOIDfs);
      } else if ( strcmp_P(oid, sysLocation ) == 0 ) {
        strcpy_P ( oid, sysServices );
        strcpy_P ( tmpOIDfs, sysServices );
        pdu.OID.fromString(tmpOIDfs);
      } else if ( strcmp_P(oid, sysServices ) == 0 ) {
        strcpy_P ( oid, "1.0" );
      } else {
        int ilen = strlen(oid);
        if ( strncmp_P(oid, sysDescr, ilen ) == 0 ) {
          strcpy_P ( oid, sysDescr );
          strcpy_P ( tmpOIDfs, sysDescr );
          pdu.OID.fromString(tmpOIDfs);
        } else if ( strncmp_P(oid, sysUpTime, ilen ) == 0 ) {
          strcpy_P ( oid, sysUpTime );
          strcpy_P ( tmpOIDfs, sysUpTime );
          pdu.OID.fromString(tmpOIDfs);
        } else if ( strncmp_P(oid, sysContact, ilen ) == 0 ) {
          strcpy_P ( oid, sysContact );
          strcpy_P ( tmpOIDfs, sysContact );
          pdu.OID.fromString(tmpOIDfs);
        } else if ( strncmp_P(oid, sysName, ilen ) == 0 ) {
          strcpy_P ( oid, sysName );
          strcpy_P ( tmpOIDfs, sysName );
          pdu.OID.fromString(tmpOIDfs);
        } else if ( strncmp_P(oid, sysLocation, ilen ) == 0 ) {
          strcpy_P ( oid, sysLocation );
          strcpy_P ( tmpOIDfs, sysLocation );
          pdu.OID.fromString(tmpOIDfs);
        } else if ( strncmp_P(oid, sysServices, ilen ) == 0 ) {
          strcpy_P ( oid, sysServices );
          strcpy_P ( tmpOIDfs, sysServices );
          pdu.OID.fromString(tmpOIDfs);
        }
      }
    }
    // End of implementation SNMP GET NEXT / WALK

    if ( strcmp_P(oid, sysDescr ) == 0 ) {
      // handle sysDescr (set/get) requests
      if ( pdu.type == SNMP_PDU_SET ) {
        // response packet from set-request - object is read-only
        pdu.type = SNMP_PDU_RESPONSE;
        pdu.error = SNMP_ERR_READ_ONLY;
      } else {
        // response packet from get-request - locDescr
        status = pdu.VALUE.encode(SNMP_SYNTAX_OCTETS, locDescr);
        pdu.type = SNMP_PDU_RESPONSE;
        pdu.error = status;
      }
      //
    } else if ( strcmp_P(oid, sysUpTime ) == 0 ) {
      // handle sysName (set/get) requests
      if ( pdu.type == SNMP_PDU_SET ) {
        // response packet from set-request - object is read-only
        pdu.type = SNMP_PDU_RESPONSE;
        pdu.error = SNMP_ERR_READ_ONLY;
      } else {
        // response packet from get-request - locUpTime
        status = pdu.VALUE.encode(SNMP_SYNTAX_TIME_TICKS, locUpTime);
        pdu.type = SNMP_PDU_RESPONSE;
        pdu.error = status;
      }
      //
    } else if ( strcmp_P(oid, sysName ) == 0 ) {
      // handle sysName (set/get) requests
      if ( pdu.type == SNMP_PDU_SET ) {
        // response packet from set-request - object is read/write
        status = pdu.VALUE.decode(locName, strlen(locName));
        pdu.type = SNMP_PDU_RESPONSE;
        pdu.error = status;
      } else {
        // response packet from get-request - locName
        status = pdu.VALUE.encode(SNMP_SYNTAX_OCTETS, locName);
        pdu.type = SNMP_PDU_RESPONSE;
        pdu.error = status;
      }
      //
    } else if ( strcmp_P(oid, sysContact ) == 0 ) {
      // handle sysContact (set/get) requests
      if ( pdu.type == SNMP_PDU_SET ) {
        // response packet from set-request - object is read/write
        status = pdu.VALUE.decode(locContact, strlen(locContact));
        pdu.type = SNMP_PDU_RESPONSE;
        pdu.error = status;
      } else {
        // response packet from get-request - locContact
        status = pdu.VALUE.encode(SNMP_SYNTAX_OCTETS, locContact);
        pdu.type = SNMP_PDU_RESPONSE;
        pdu.error = status;
      }
      //
    } else if ( strcmp_P(oid, sysLocation ) == 0 ) {
      // handle sysLocation (set/get) requests
      if ( pdu.type == SNMP_PDU_SET ) {
        // response packet from set-request - object is read/write
        status = pdu.VALUE.decode(locLocation, strlen(locLocation));
        pdu.type = SNMP_PDU_RESPONSE;
        pdu.error = status;
      } else {
        // response packet from get-request - locLocation
        status = pdu.VALUE.encode(SNMP_SYNTAX_OCTETS, locLocation);
        pdu.type = SNMP_PDU_RESPONSE;
        pdu.error = status;
      }
      //
    } else if ( strcmp_P(oid, sysServices) == 0 ) {
      // handle sysServices (set/get) requests
      if ( pdu.type == SNMP_PDU_SET ) {
        // response packet from set-request - object is read-only
        pdu.type = SNMP_PDU_RESPONSE;
        pdu.error = SNMP_ERR_READ_ONLY;
      } else {
        // response packet from get-request - locServices
        status = pdu.VALUE.encode(SNMP_SYNTAX_INT, locServices);
        pdu.type = SNMP_PDU_RESPONSE;
        pdu.error = status;
      }
      //
    }

    else if ( verify_oid(oid, strcpy_P(buffer, ENTERPRISE_NUMBER)) == 0 ) {
      // handle sysServices (set/get) requests
      if ( pdu.type == SNMP_PDU_SET ) {
        // response packet from set-request - object is read-only
        pdu.type = SNMP_PDU_RESPONSE;
        pdu.error = SNMP_ERR_READ_ONLY;
      } else {
        // response packet from get-request - locServices

        //        Serial.print("freeMemory()=");
        //        Serial.println(freeMemory()); //Verificar o uso de memória após cada iteração do Loop

        type_response = oid[SIZE_ENTERPRISE_NUMBER] - '0';
        //        Serial.println(type_response);



        for (byte m = 0; m < (sizeof(aux_command) / sizeof(char)); m++) {
          aux_command[m] = '\0';
        }

        byte i = 0;
        while (oid[i + SIZE_ENTERPRISE_NUMBER + 2] != '.' && (i + SIZE_ENTERPRISE_NUMBER + 2) < strlen(oid)) {
          aux_command[i] = char(oid[i + SIZE_ENTERPRISE_NUMBER + 2]);
          i++;
        }
        type_command = atoi(aux_command);
        //        Serial.println(type_command);

        //        Serial.print("freeMemory()=");
        //        Serial.println(freeMemory()); //Verificar o uso de memória após cada iteração do Loop


        for (byte m = 0; m < (sizeof(aux_corredor) / sizeof(char)); m++) {
          aux_corredor[m] = '\0';
        }
        for (byte m = 0; m < (sizeof(aux_sensor) / sizeof(char)); m++) {
          aux_sensor[m] = '\0';
        }


        switch (type_command) {


          case temperature_dallas: {

              //              Serial.print("freeMemory()=");
              //              Serial.println(freeMemory()); //Verificar o uso de memória após cada iteração do Loop
              byte j = 0;
              i++;
              while (oid[j + i + SIZE_ENTERPRISE_NUMBER + 2] != '.' && (j + i + SIZE_ENTERPRISE_NUMBER + 2) < strlen(oid)) {
                aux_corredor[j] = char(oid[j + SIZE_ENTERPRISE_NUMBER + 2 + i]);
                j++;
              }
              corredor_snmp = atoi(aux_corredor);
              //              Serial.println(aux_corredor);


              //              Serial.print("freeMemory()=");
              //              Serial.println(freeMemory()); //Verificar o uso de memória após cada iteração do Loop

              byte k = 0;
              j++;
              while (oid[k + j + i + SIZE_ENTERPRISE_NUMBER + 2] != '.' && (k + j + i + SIZE_ENTERPRISE_NUMBER + 2) < strlen(oid)) {
                aux_sensor[k] = char(oid[k + j + i + SIZE_ENTERPRISE_NUMBER + 2]);
                k++;
              }
              sensor_snmp = atoi(aux_sensor);
              //              Serial.println(aux_sensor);


              //              Serial.print("freeMemory()=");
              //              Serial.println(freeMemory()); //Verificar o uso de memória após cada iteração do Loop

              if (type_response == just_int) {
                status = pdu.VALUE.encode(SNMP_SYNTAX_INT, int(tempC[corredor_snmp][sensor_snmp]));
                pdu.type = SNMP_PDU_RESPONSE;
                pdu.error = status;

                //                Serial.print("freeMemory()=");
                //                Serial.println(freeMemory()); //Verificar o uso de memória após cada iteração do Loop
              }
              else if (type_response == just_string_value) {
                dtostrf(float(tempC[corredor_snmp][sensor_snmp]), verify_size_number(float(tempC[corredor_snmp][sensor_snmp])), 2, buffer);
                status = pdu.VALUE.encode(SNMP_SYNTAX_OCTETS, buffer);
                pdu.type = SNMP_PDU_RESPONSE;
                pdu.error = status;

                //                Serial.print("freeMemory()=");
                //                Serial.println(freeMemory()); //Verificar o uso de memória após cada iteração do Loop
              }
              else {
                // oid does not exist
                // response packet - object not found
                pdu.type = SNMP_PDU_RESPONSE;
                pdu.error = SNMP_ERR_NO_SUCH_NAME;
              }
              break;
            }


          case humidity: {

              byte j = 0;
              i++;
              while (oid[j + i + SIZE_ENTERPRISE_NUMBER + 2] != '.' && (j + i + SIZE_ENTERPRISE_NUMBER + 2) < strlen(oid)) {
                aux_sensor[j] = char(oid[j + SIZE_ENTERPRISE_NUMBER + 2 + i]);
                j++;
              }
              sensor_snmp = atoi(aux_sensor);
              //              Serial.println(sensor_snmp);

              if (type_response == just_int) {
                status = pdu.VALUE.encode(SNMP_SYNTAX_INT, int(umidade[sensor_snmp]));
                pdu.type = SNMP_PDU_RESPONSE;
                pdu.error = status;
              }
              else if (type_response == just_string_value) {
                dtostrf(float(umidade[sensor_snmp]), verify_size_number(float(umidade[sensor_snmp])), 2, buffer);
                status = pdu.VALUE.encode(SNMP_SYNTAX_OCTETS, buffer);
                pdu.type = SNMP_PDU_RESPONSE;
                pdu.error = status;
              }
              else {
                // oid does not exist
                // response packet - object not found
                pdu.type = SNMP_PDU_RESPONSE;
                pdu.error = SNMP_ERR_NO_SUCH_NAME;
              }
              break;
            }

          case temp_humidity: {

              byte j = 0;
              i++;
              while (oid[j + i + SIZE_ENTERPRISE_NUMBER + 2] != '.' && (j + i + SIZE_ENTERPRISE_NUMBER + 2) < strlen(oid)) {
                aux_sensor[j] = char(oid[j + SIZE_ENTERPRISE_NUMBER + 2 + i]);
                j++;
              }
              sensor_snmp = atoi(aux_sensor);
              //              Serial.println(sensor_snmp);

              if (type_response == just_int) {
                status = pdu.VALUE.encode(SNMP_SYNTAX_INT, int(temp_sensor_umidade[sensor_snmp]));
                pdu.type = SNMP_PDU_RESPONSE;
                pdu.error = status;
              }
              else if (type_response == just_string_value) {
                dtostrf(float(temp_sensor_umidade[sensor_snmp]), verify_size_number(float(temp_sensor_umidade[sensor_snmp])), 2, buffer);
                status = pdu.VALUE.encode(SNMP_SYNTAX_OCTETS, buffer);
                pdu.type = SNMP_PDU_RESPONSE;
                pdu.error = status;
              }
              else {
                // oid does not exist
                // response packet - object not found
                pdu.type = SNMP_PDU_RESPONSE;
                pdu.error = SNMP_ERR_NO_SUCH_NAME;
              }
              break;
            }

          case media_corredor: {

              byte j = 0;
              i++;
              while (oid[j + i + SIZE_ENTERPRISE_NUMBER + 2] != '.' && (j + i + SIZE_ENTERPRISE_NUMBER + 2) < strlen(oid)) {
                aux_corredor[j] = char(oid[j + SIZE_ENTERPRISE_NUMBER + 2 + i]);
                j++;
              }
              corredor_snmp = atoi(aux_corredor);
              //              Serial.println(corredor_snmp);

              if (type_response == just_int) {
                status = pdu.VALUE.encode(SNMP_SYNTAX_INT, int(media_leitura[corredor_snmp]));
                pdu.type = SNMP_PDU_RESPONSE;
                pdu.error = status;
              }
              else if (type_response == just_string_value) {
                dtostrf(float(media_leitura[corredor_snmp]), verify_size_number(float(media_leitura[corredor_snmp])), 2, buffer);
                status = pdu.VALUE.encode(SNMP_SYNTAX_OCTETS, buffer);
                pdu.type = SNMP_PDU_RESPONSE;
                pdu.error = status;
              }
              else {
                // oid does not exist
                // response packet - object not found
                pdu.type = SNMP_PDU_RESPONSE;
                pdu.error = SNMP_ERR_NO_SUCH_NAME;
              }
              break;
            }


          case media_g: {
              if (type_response == just_int) {
                status = pdu.VALUE.encode(SNMP_SYNTAX_INT, int(media_geral));
                pdu.type = SNMP_PDU_RESPONSE;
                pdu.error = status;
              }
              else if (type_response == just_string_value) {
                dtostrf(float(media_geral), verify_size_number(float(media_geral)), 2, buffer);
                status = pdu.VALUE.encode(SNMP_SYNTAX_OCTETS, buffer);
                pdu.type = SNMP_PDU_RESPONSE;
                pdu.error = status;
              }
              else {
                // oid does not exist
                // response packet - object not found
                pdu.type = SNMP_PDU_RESPONSE;
                pdu.error = SNMP_ERR_NO_SUCH_NAME;
              }

              break;
            }

          case media_h: {
              if (type_response == just_int) {
                status = pdu.VALUE.encode(SNMP_SYNTAX_INT, int(media_umidade));
                pdu.type = SNMP_PDU_RESPONSE;
                pdu.error = status;
              }
              else if (type_response == just_string_value) {
                dtostrf(float(media_umidade), verify_size_number(float(media_umidade)), 2, buffer);
                status = pdu.VALUE.encode(SNMP_SYNTAX_OCTETS, buffer);
                pdu.type = SNMP_PDU_RESPONSE;
                pdu.error = status;
              }
              else {
                // oid does not exist
                // response packet - object not found
                pdu.type = SNMP_PDU_RESPONSE;
                pdu.error = SNMP_ERR_NO_SUCH_NAME;
              }
              break;
            }


          case media_ht: {
              if (type_response == just_int) {
                status = pdu.VALUE.encode(SNMP_SYNTAX_INT, int(media_temp_dht));
                pdu.type = SNMP_PDU_RESPONSE;
                pdu.error = status;
              }
              else if (type_response == just_string_value) {
                dtostrf(float(media_temp_dht), verify_size_number(float(media_temp_dht)), 2, buffer);
                status = pdu.VALUE.encode(SNMP_SYNTAX_OCTETS, buffer);
                pdu.type = SNMP_PDU_RESPONSE;
                pdu.error = status;
              }
              else {
                // oid does not exist
                // response packet - object not found
                pdu.type = SNMP_PDU_RESPONSE;
                pdu.error = SNMP_ERR_NO_SUCH_NAME;
              }
              break;
            }

          case presence: {
              if (type_response == just_int) {
                status = pdu.VALUE.encode(SNMP_SYNTAX_INT, int(presenca_web));
                pdu.type = SNMP_PDU_RESPONSE;
                pdu.error = status;
              }
              else {
                // oid does not exist
                // response packet - object not found
                pdu.type = SNMP_PDU_RESPONSE;
                pdu.error = SNMP_ERR_NO_SUCH_NAME;
              }
              break;
            }

          case access: {
              if (type_response == just_string_value) {
                status = pdu.VALUE.encode(SNMP_SYNTAX_OCTETS, String(*definite_user).c_str());
                pdu.type = SNMP_PDU_RESPONSE;
                pdu.error = status;
              }
              else {
                // oid does not exist
                // response packet - object not found
                pdu.type = SNMP_PDU_RESPONSE;
                pdu.error = SNMP_ERR_NO_SUCH_NAME;
              }
              break;
            }

          case inund: {

              byte j = 0;
              i++;
              while (oid[j + i + SIZE_ENTERPRISE_NUMBER + 2] != '.' && (j + i + SIZE_ENTERPRISE_NUMBER + 2) < strlen(oid)) {
                aux_sensor[j] = char(oid[j + SIZE_ENTERPRISE_NUMBER + 2 + i]);
                j++;
              }
              sensor_snmp = atoi(aux_sensor);
              //              Serial.println(sensor_snmp);
              if (type_response == just_int) {
                status = pdu.VALUE.encode(SNMP_SYNTAX_INT, int(valor_inundacao[sensor_snmp]));
                pdu.type = SNMP_PDU_RESPONSE;
                pdu.error = status;
              }
              else if (type_response == just_string_value) {
                dtostrf(float(valor_inundacao[sensor_snmp]), verify_size_number(float(valor_inundacao[sensor_snmp])), 2, buffer);
                status = pdu.VALUE.encode(SNMP_SYNTAX_OCTETS, buffer);
                pdu.type = SNMP_PDU_RESPONSE;
                pdu.error = status;
              }
              else {
                // oid does not exist
                // response packet - object not found
                pdu.type = SNMP_PDU_RESPONSE;
                pdu.error = SNMP_ERR_NO_SUCH_NAME;
              }
              break;
            }

          case sound_db: {

              byte j = 0;
              i++;
              while (oid[j + i + SIZE_ENTERPRISE_NUMBER + 2] != '.' && (j + i + SIZE_ENTERPRISE_NUMBER + 2) < strlen(oid)) {
                aux_sensor[j] = char(oid[j + SIZE_ENTERPRISE_NUMBER + 2 + i]);
                j++;
              }
              sensor_snmp = atoi(aux_sensor);
              //              Serial.println(sensor_snmp);
              if (type_response == just_int) {
                status = pdu.VALUE.encode(SNMP_SYNTAX_INT, int(db[sensor_snmp]));
                pdu.type = SNMP_PDU_RESPONSE;
                pdu.error = status;
              }
              else if (type_response == just_string_value) {
                dtostrf(float(db[sensor_snmp]), verify_size_number(float(db[sensor_snmp])), 2, buffer);
                status = pdu.VALUE.encode(SNMP_SYNTAX_OCTETS, buffer);
                pdu.type = SNMP_PDU_RESPONSE;
                pdu.error = status;
              }
              else {
                // oid does not exist
                // response packet - object not found
                pdu.type = SNMP_PDU_RESPONSE;
                pdu.error = SNMP_ERR_NO_SUCH_NAME;
              }
              break;
            }

          case vibration_snmp: {

              byte j = 0;
              i++;
              while (oid[j + i + SIZE_ENTERPRISE_NUMBER + 2] != '.' && (j + i + SIZE_ENTERPRISE_NUMBER + 2) < strlen(oid)) {
                aux_sensor[j] = char(oid[j + SIZE_ENTERPRISE_NUMBER + 2 + i]);
                j++;
              }
              sensor_snmp = atoi(aux_sensor);
              //              Serial.println(sensor_snmp);
              if (type_response == just_int) {
                status = pdu.VALUE.encode(SNMP_SYNTAX_INT, int(vibration[sensor_snmp]));
                pdu.type = SNMP_PDU_RESPONSE;
                pdu.error = status;
              }
              else if (type_response == just_string_value) {
                dtostrf(float(vibration[sensor_snmp]), verify_size_number(float(vibration[sensor_snmp])), 2, buffer);
                status = pdu.VALUE.encode(SNMP_SYNTAX_OCTETS, buffer);
                pdu.type = SNMP_PDU_RESPONSE;
                pdu.error = status;
              }
              else {
                // oid does not exist
                // response packet - object not found
                pdu.type = SNMP_PDU_RESPONSE;
                pdu.error = SNMP_ERR_NO_SUCH_NAME;
              }
              break;
            }

          case flame_snmp: {

              byte j = 0;
              i++;
              while (oid[j + i + SIZE_ENTERPRISE_NUMBER + 2] != '.' && (j + i + SIZE_ENTERPRISE_NUMBER + 2) < strlen(oid)) {
                aux_sensor[j] = char(oid[j + SIZE_ENTERPRISE_NUMBER + 2 + i]);
                j++;
              }
              sensor_snmp = atoi(aux_sensor);
              //              Serial.println(sensor_snmp);
              if (type_response == just_int) {
                status = pdu.VALUE.encode(SNMP_SYNTAX_INT, int(percent_flame[sensor_snmp]));
                pdu.type = SNMP_PDU_RESPONSE;
                pdu.error = status;
              }
              else if (type_response == just_string_value) {
                dtostrf(float(percent_flame[sensor_snmp]), verify_size_number(float(percent_flame[sensor_snmp])), 2, buffer);
                status = pdu.VALUE.encode(SNMP_SYNTAX_OCTETS, buffer);
                pdu.type = SNMP_PDU_RESPONSE;
                pdu.error = status;
              }
              else {
                // oid does not exist
                // response packet - object not found
                pdu.type = SNMP_PDU_RESPONSE;
                pdu.error = SNMP_ERR_NO_SUCH_NAME;
              }
              break;
            }

          case door_snmp: {

              byte j = 0;
              i++;
              while (oid[j + i + SIZE_ENTERPRISE_NUMBER + 2] != '.' && (j + i + SIZE_ENTERPRISE_NUMBER + 2) < strlen(oid)) {
                aux_sensor[j] = char(oid[j + SIZE_ENTERPRISE_NUMBER + 2 + i]);
                j++;
              }
              sensor_snmp = atoi(aux_sensor);
              //              Serial.println(sensor_snmp);
              if (type_response == just_int) {
                status = pdu.VALUE.encode(SNMP_SYNTAX_INT, int(door[sensor_snmp]));
                pdu.type = SNMP_PDU_RESPONSE;
                pdu.error = status;
              }
              else if (type_response == just_string_value) {
                if (door[sensor_snmp] == DOOR_OPEN) {
                  strcpy("Porta ABERTA", buffer);
                }
                else if (door[sensor_snmp] == DOOR_CLOSED) {
                  strcpy("Porta Fechada", buffer);
                }
                status = pdu.VALUE.encode(SNMP_SYNTAX_OCTETS, buffer);
                pdu.type = SNMP_PDU_RESPONSE;
                pdu.error = status;
              }
              else {
                // oid does not exist
                // response packet - object not found
                pdu.type = SNMP_PDU_RESPONSE;
                pdu.error = SNMP_ERR_NO_SUCH_NAME;
              }
              break;
            }

          case light: {

              byte j = 0;
              i++;
              while (oid[j + i + SIZE_ENTERPRISE_NUMBER + 2] != '.' && (j + i + SIZE_ENTERPRISE_NUMBER + 2) < strlen(oid)) {
                aux_sensor[j] = char(oid[j + SIZE_ENTERPRISE_NUMBER + 2 + i]);
                j++;
              }
              sensor_snmp = atoi(aux_sensor);
              //              Serial.println(sensor_snmp);
              if (type_response == just_int) {
                status = pdu.VALUE.encode(SNMP_SYNTAX_INT, int(percent_ldr[sensor_snmp]));
                pdu.type = SNMP_PDU_RESPONSE;
                pdu.error = status;
              }
              else if (type_response == just_string_value) {
                dtostrf(float(percent_ldr[sensor_snmp]), verify_size_number(float(percent_ldr[sensor_snmp])), 2, buffer);
                status = pdu.VALUE.encode(SNMP_SYNTAX_OCTETS, buffer);
                pdu.type = SNMP_PDU_RESPONSE;
                pdu.error = status;
              }
              else {
                // oid does not exist
                // response packet - object not found
                pdu.type = SNMP_PDU_RESPONSE;
                pdu.error = SNMP_ERR_NO_SUCH_NAME;
              }
              break;
            }

          case lux_value: {


              //              Serial.println(sensor_snmp);
              if (type_response == just_int) {
                status = pdu.VALUE.encode(SNMP_SYNTAX_INT, int(lux));
                pdu.type = SNMP_PDU_RESPONSE;
                pdu.error = status;
              }
              else if (type_response == just_string_value) {
                dtostrf(float(lux), verify_size_number(float(lux)), 2, buffer);
                status = pdu.VALUE.encode(SNMP_SYNTAX_OCTETS, buffer);
                pdu.type = SNMP_PDU_RESPONSE;
                pdu.error = status;
              }
              else {
                // oid does not exist
                // response packet - object not found
                pdu.type = SNMP_PDU_RESPONSE;
                pdu.error = SNMP_ERR_NO_SUCH_NAME;
              }
              break;
            }
          case smoke_value: {
              if (type_response == just_int) {
                status = pdu.VALUE.encode(SNMP_SYNTAX_INT, int(air_quality));
                pdu.type = SNMP_PDU_RESPONSE;
                pdu.error = status;
              }
              else if (type_response == just_string_value) {
                dtostrf(float(air_quality), verify_size_number(float(air_quality)), 2, buffer);
                status = pdu.VALUE.encode(SNMP_SYNTAX_OCTETS, buffer);
                pdu.type = SNMP_PDU_RESPONSE;
                pdu.error = status;
              }
              else {
                // oid does not exist
                // response packet - object not found
                pdu.type = SNMP_PDU_RESPONSE;
                pdu.error = SNMP_ERR_NO_SUCH_NAME;
              }
              break;
            }

          case pressao_snmp: {
              if (type_response == just_int) {
                status = pdu.VALUE.encode(SNMP_SYNTAX_INT, int(pressure));
                pdu.type = SNMP_PDU_RESPONSE;
                pdu.error = status;
              }
              else if (type_response == just_string_value) {
                Serial.println(pressure);
                Serial.println(float(pressure));
                dtostrf(float(pressure), verify_size_number(float(pressure)), 2, buffer);
                status = pdu.VALUE.encode(SNMP_SYNTAX_OCTETS, buffer);
                pdu.type = SNMP_PDU_RESPONSE;
                pdu.error = status;
              }
              else {
                // oid does not exist
                // response packet - object not found
                pdu.type = SNMP_PDU_RESPONSE;
                pdu.error = SNMP_ERR_NO_SUCH_NAME;
              }
              break;
            }

          case altitude_snmp: {
              if (type_response == just_int) {
                status = pdu.VALUE.encode(SNMP_SYNTAX_INT, int(altitude));
                pdu.type = SNMP_PDU_RESPONSE;
                pdu.error = status;
              }
              else if (type_response == just_string_value) {
                dtostrf(float(altitude), verify_size_number(float(altitude)), 2, buffer);
                status = pdu.VALUE.encode(SNMP_SYNTAX_OCTETS, buffer);
                pdu.type = SNMP_PDU_RESPONSE;
                pdu.error = status;
              }
              else {
                // oid does not exist
                // response packet - object not found
                pdu.type = SNMP_PDU_RESPONSE;
                pdu.error = SNMP_ERR_NO_SUCH_NAME;
              }
              break;
            }

          case temp_interna_snmp: {
              if (type_response == just_int) {
                status = pdu.VALUE.encode(SNMP_SYNTAX_INT, int(temp_interna));
                pdu.type = SNMP_PDU_RESPONSE;
                pdu.error = status;
              }
              else if (type_response == just_string_value) {
                dtostrf(float(temp_interna), verify_size_number(float(temp_interna)), 2, buffer);
                status = pdu.VALUE.encode(SNMP_SYNTAX_OCTETS, buffer);
                pdu.type = SNMP_PDU_RESPONSE;
                pdu.error = status;
              }
              else {
                // oid does not exist
                // response packet - object not found
                pdu.type = SNMP_PDU_RESPONSE;
                pdu.error = SNMP_ERR_NO_SUCH_NAME;
              }
              break;
            }


          case current_snmp: {
              Serial.println("\nFOI!!!!!!!!!!!");
              byte j = 0;
              i++;
              while (oid[j + i + SIZE_ENTERPRISE_NUMBER + 2] != '.' && (j + i + SIZE_ENTERPRISE_NUMBER + 2) < strlen(oid)) {
                aux_sensor[j] = char(oid[j + SIZE_ENTERPRISE_NUMBER + 2 + i]);
                j++;
              }
              sensor_snmp = atoi(aux_sensor);
              //              Serial.println(sensor_snmp);
              if (type_response == just_int) {
                status = pdu.VALUE.encode(SNMP_SYNTAX_INT, int(Irms[sensor_snmp]));
                pdu.type = SNMP_PDU_RESPONSE;
                pdu.error = status;
              }
              else if (type_response == just_string_value) {
                dtostrf(float(Irms[sensor_snmp]), verify_size_number(float(Irms[sensor_snmp])), 2, buffer);
                status = pdu.VALUE.encode(SNMP_SYNTAX_OCTETS, buffer);
                pdu.type = SNMP_PDU_RESPONSE;
                pdu.error = status;
              }
              else {
                // oid does not exist
                // response packet - object not found
                pdu.type = SNMP_PDU_RESPONSE;
                pdu.error = SNMP_ERR_NO_SUCH_NAME;
              }
              break;
            }

          case power_ap_snmp: {

              byte j = 0;
              i++;
              while (oid[j + i + SIZE_ENTERPRISE_NUMBER + 2] != '.' && (j + i + SIZE_ENTERPRISE_NUMBER + 2) < strlen(oid)) {
                aux_sensor[j] = char(oid[j + SIZE_ENTERPRISE_NUMBER + 2 + i]);
                j++;
              }
              sensor_snmp = atoi(aux_sensor);
              //              Serial.println(sensor_snmp);
              if (type_response == just_int) {
                status = pdu.VALUE.encode(SNMP_SYNTAX_INT, int(potencia_aparente[sensor_snmp]));
                pdu.type = SNMP_PDU_RESPONSE;
                pdu.error = status;
              }
              else if (type_response == just_string_value) {
                dtostrf(float(potencia_aparente[sensor_snmp]), verify_size_number(float(potencia_aparente[sensor_snmp])), 2, buffer);
                status = pdu.VALUE.encode(SNMP_SYNTAX_OCTETS, buffer);
                pdu.type = SNMP_PDU_RESPONSE;
                pdu.error = status;
              }
              else {
                // oid does not exist
                // response packet - object not found
                pdu.type = SNMP_PDU_RESPONSE;
                pdu.error = SNMP_ERR_NO_SUCH_NAME;
              }
              break;
            }

          case power_re_snmp: {

              byte j = 0;
              i++;
              while (oid[j + i + SIZE_ENTERPRISE_NUMBER + 2] != '.' && (j + i + SIZE_ENTERPRISE_NUMBER + 2) < strlen(oid)) {
                aux_sensor[j] = char(oid[j + SIZE_ENTERPRISE_NUMBER + 2 + i]);
                j++;
              }
              sensor_snmp = atoi(aux_sensor);
              //              Serial.println(sensor_snmp);
              if (tem_power_factor[sensor_snmp]) {
                if (type_response == just_int) {
                  status = pdu.VALUE.encode(SNMP_SYNTAX_INT, int(potencia_ativa[sensor_snmp]));
                  pdu.type = SNMP_PDU_RESPONSE;
                  pdu.error = status;
                }
                else if (type_response == just_string_value) {
                  dtostrf(float(potencia_ativa[sensor_snmp]), verify_size_number(float(potencia_ativa[sensor_snmp])), 2, buffer);
                  status = pdu.VALUE.encode(SNMP_SYNTAX_OCTETS, buffer);
                  pdu.type = SNMP_PDU_RESPONSE;
                  pdu.error = status;
                }
                else {
                  // oid does not exist
                  // response packet - object not found
                  pdu.type = SNMP_PDU_RESPONSE;
                  pdu.error = SNMP_ERR_NO_SUCH_NAME;
                }
              }
              else {
                // oid does not exist
                // response packet - object not found
                pdu.type = SNMP_PDU_RESPONSE;
                pdu.error = SNMP_ERR_NO_SUCH_NAME;
              }
              break;
            }

          case power_im_snmp: {

              byte j = 0;
              i++;
              while (oid[j + i + SIZE_ENTERPRISE_NUMBER + 2] != '.' && (j + i + SIZE_ENTERPRISE_NUMBER + 2) < strlen(oid)) {
                aux_sensor[j] = char(oid[j + SIZE_ENTERPRISE_NUMBER + 2 + i]);
                j++;
              }
              sensor_snmp = atoi(aux_sensor);
              //              Serial.println(sensor_snmp);
              if (tem_power_factor[sensor_snmp]) {
                if (type_response == just_int) {
                  status = pdu.VALUE.encode(SNMP_SYNTAX_INT, int(potencia_reativa[sensor_snmp]));
                  pdu.type = SNMP_PDU_RESPONSE;
                  pdu.error = status;
                }
                else if (type_response == just_string_value) {
                  dtostrf(float(potencia_reativa[sensor_snmp]), verify_size_number(float(potencia_reativa[sensor_snmp])), 2, buffer);
                  status = pdu.VALUE.encode(SNMP_SYNTAX_OCTETS, buffer);
                  pdu.type = SNMP_PDU_RESPONSE;
                  pdu.error = status;
                }
                else {
                  // oid does not exist
                  // response packet - object not found
                  pdu.type = SNMP_PDU_RESPONSE;
                  pdu.error = SNMP_ERR_NO_SUCH_NAME;
                }
              }
              else {
                // oid does not exist
                // response packet - object not found
                pdu.type = SNMP_PDU_RESPONSE;
                pdu.error = SNMP_ERR_NO_SUCH_NAME;
              }
              break;
            }

          case consumo_ap_snmp: {

              byte j = 0;
              i++;
              while (oid[j + i + SIZE_ENTERPRISE_NUMBER + 2] != '.' && (j + i + SIZE_ENTERPRISE_NUMBER + 2) < strlen(oid)) {
                aux_sensor[j] = char(oid[j + SIZE_ENTERPRISE_NUMBER + 2 + i]);
                j++;
              }
              sensor_snmp = atoi(aux_sensor);
              //              Serial.println(sensor_snmp);
              if (type_response == just_int) {
                status = pdu.VALUE.encode(SNMP_SYNTAX_INT, int(consumo_kvah[sensor_snmp]));
                pdu.type = SNMP_PDU_RESPONSE;
                pdu.error = status;
              }
              else if (type_response == just_string_value) {
                dtostrf(float(consumo_kvah[sensor_snmp]), verify_size_number(float(consumo_kvah[sensor_snmp])), 2, buffer);
                status = pdu.VALUE.encode(SNMP_SYNTAX_OCTETS, buffer);
                pdu.type = SNMP_PDU_RESPONSE;
                pdu.error = status;
              }
              else {
                // oid does not exist
                // response packet - object not found
                pdu.type = SNMP_PDU_RESPONSE;
                pdu.error = SNMP_ERR_NO_SUCH_NAME;
              }
              break;
            }

          case consumo_re_snmp: {

              byte j = 0;
              i++;
              while (oid[j + i + SIZE_ENTERPRISE_NUMBER + 2] != '.' && (j + i + SIZE_ENTERPRISE_NUMBER + 2) < strlen(oid)) {
                aux_sensor[j] = char(oid[j + SIZE_ENTERPRISE_NUMBER + 2 + i]);
                j++;
              }
              sensor_snmp = atoi(aux_sensor);
              //              Serial.println(sensor_snmp);
              if (tem_power_factor[sensor_snmp]) {
                if (type_response == just_int) {
                  status = pdu.VALUE.encode(SNMP_SYNTAX_INT, int(consumo_kwh[sensor_snmp]));
                  pdu.type = SNMP_PDU_RESPONSE;
                  pdu.error = status;
                }
                else if (type_response == just_string_value) {
                  dtostrf(float(consumo_kwh[sensor_snmp]), verify_size_number(float(consumo_kwh[sensor_snmp])), 2, buffer);
                  status = pdu.VALUE.encode(SNMP_SYNTAX_OCTETS, buffer);
                  pdu.type = SNMP_PDU_RESPONSE;
                  pdu.error = status;
                }
                else {
                  // oid does not exist
                  // response packet - object not found
                  pdu.type = SNMP_PDU_RESPONSE;
                  pdu.error = SNMP_ERR_NO_SUCH_NAME;
                }
              }
              else {
                // oid does not exist
                // response packet - object not found
                pdu.type = SNMP_PDU_RESPONSE;
                pdu.error = SNMP_ERR_NO_SUCH_NAME;
              }
              break;
            }

          case consumo_im_snmp: {

              byte j = 0;
              i++;
              while (oid[j + i + SIZE_ENTERPRISE_NUMBER + 2] != '.' && (j + i + SIZE_ENTERPRISE_NUMBER + 2) < strlen(oid)) {
                aux_sensor[j] = char(oid[j + SIZE_ENTERPRISE_NUMBER + 2 + i]);
                j++;
              }
              sensor_snmp = atoi(aux_sensor);
              //              Serial.println(sensor_snmp);
              if (tem_power_factor[sensor_snmp]) {
                if (type_response == just_int) {
                  status = pdu.VALUE.encode(SNMP_SYNTAX_INT, int(consumo_kvarh[sensor_snmp]));
                  pdu.type = SNMP_PDU_RESPONSE;
                  pdu.error = status;
                }
                else if (type_response == just_string_value) {
                  dtostrf(float(consumo_kvarh[sensor_snmp]), verify_size_number(float(consumo_kvarh[sensor_snmp])), 2, buffer);
                  status = pdu.VALUE.encode(SNMP_SYNTAX_OCTETS, buffer);
                  pdu.type = SNMP_PDU_RESPONSE;
                  pdu.error = status;
                }
                else {
                  // oid does not exist
                  // response packet - object not found
                  pdu.type = SNMP_PDU_RESPONSE;
                  pdu.error = SNMP_ERR_NO_SUCH_NAME;
                }
              }
              else {
                // oid does not exist
                // response packet - object not found
                pdu.type = SNMP_PDU_RESPONSE;
                pdu.error = SNMP_ERR_NO_SUCH_NAME;
              }
              break;
            }

          case current_snmp_total: {

              if (type_response == just_int) {
                status = pdu.VALUE.encode(SNMP_SYNTAX_INT, int(Irms_total));
                pdu.type = SNMP_PDU_RESPONSE;
                pdu.error = status;
              }
              else if (type_response == just_string_value) {
                dtostrf(float(Irms_total), verify_size_number(float(Irms_total)), 2, buffer);
                status = pdu.VALUE.encode(SNMP_SYNTAX_OCTETS, buffer);
                pdu.type = SNMP_PDU_RESPONSE;
                pdu.error = status;
              }
              else {
                // oid does not exist
                // response packet - object not found
                pdu.type = SNMP_PDU_RESPONSE;
                pdu.error = SNMP_ERR_NO_SUCH_NAME;
              }
              break;
            }

          case power_ap_snmp_total: {

              if (type_response == just_int) {
                status = pdu.VALUE.encode(SNMP_SYNTAX_INT, int(potencia_aparente_total));
                pdu.type = SNMP_PDU_RESPONSE;
                pdu.error = status;
              }
              else if (type_response == just_string_value) {
                dtostrf(float(potencia_aparente_total), verify_size_number(float(potencia_aparente_total)), 2, buffer);
                status = pdu.VALUE.encode(SNMP_SYNTAX_OCTETS, buffer);
                pdu.type = SNMP_PDU_RESPONSE;
                pdu.error = status;
              }
              else {
                // oid does not exist
                // response packet - object not found
                pdu.type = SNMP_PDU_RESPONSE;
                pdu.error = SNMP_ERR_NO_SUCH_NAME;
              }
              break;
            }

          case consumo_ap_snmp_total: {

              if (type_response == just_int) {
                status = pdu.VALUE.encode(SNMP_SYNTAX_INT, int(consumo_kvah_total));
                pdu.type = SNMP_PDU_RESPONSE;
                pdu.error = status;
              }
              else if (type_response == just_string_value) {
                dtostrf(float(consumo_kvah_total), verify_size_number(float(consumo_kvah_total)), 2, buffer);
                status = pdu.VALUE.encode(SNMP_SYNTAX_OCTETS, buffer);
                pdu.type = SNMP_PDU_RESPONSE;
                pdu.error = status;
              }
              else {
                // oid does not exist
                // response packet - object not found
                pdu.type = SNMP_PDU_RESPONSE;
                pdu.error = SNMP_ERR_NO_SUCH_NAME;
              }
              break;
            }

          case current_snmp_neutro: {

              if (type_response == just_int) {
                status = pdu.VALUE.encode(SNMP_SYNTAX_INT, int(Irms_neutro));
                pdu.type = SNMP_PDU_RESPONSE;
                pdu.error = status;
              }
              else if (type_response == just_string_value) {
                dtostrf(float(Irms_neutro), verify_size_number(float(Irms_neutro)), 2, buffer);
                status = pdu.VALUE.encode(SNMP_SYNTAX_OCTETS, buffer);
                pdu.type = SNMP_PDU_RESPONSE;
                pdu.error = status;
              }
              else {
                // oid does not exist
                // response packet - object not found
                pdu.type = SNMP_PDU_RESPONSE;
                pdu.error = SNMP_ERR_NO_SUCH_NAME;
              }
              break;
            }
            default:{
                  // oid does not exist
                  // response packet - object not found
                  pdu.type = SNMP_PDU_RESPONSE;
                  pdu.error = SNMP_ERR_NO_SUCH_NAME;
              break;
            }
        }
      }
      //
    }
    else {
      // oid does not exist
      // response packet - object not found
      pdu.type = SNMP_PDU_RESPONSE;
      pdu.error = SNMP_ERR_NO_SUCH_NAME;
    }
    //
    Agentuino.responsePdu(&pdu);
  }
  //
  //  Agentuino.freePdu(&pdu);
  //
}

byte verify_size_number(float x) {
  byte i = 3;
  while (int(x / 1) > 0) {
    x = x / 10;
    i++;
  }
  return i;
}

//byte verify_size_number(int x) {
//  byte i = 0;
//  while (int(x / 1) > 0) {
//    x = x / 10;
//    i++;
//  }
//  return i;
//}
//
//byte verify_size_number(double x) {
//  byte i = 3;
//  while (int(x / 1) > 0) {
//    x = x / 10;
//    i++;
//  }
//  return i;
//}

byte verify_oid(char oid[], char* parameter) {
  for (byte i = 0; i < strlen(parameter); i++) {
    //  Serial.print(char(oid[i]));
    //  Serial.print("-");
    //  Serial.println(buffer[i]);
    if ( parameter[i] !=  char(oid[i])  ) {
      return 1;
    }
  }

  return 0;
}

void read_bmp() {
  pressure = bmp180.readPressure();
  altitude = bmp180.readAltitude();
  temp_interna = bmp180.readTemperature();
}

void plot_current_readings(){
        for (byte i = 0; i < NUMBER_CURRENT_SENSORS; i++) {
            Serial.print(Irms[i]);
            if(i==NUMBER_CURRENT_SENSORS-1){
                if(NUMBER_CURRENT_SENSORS>1){
                    float current_total=0;
                    for (byte j = 0; i < NUMBER_CURRENT_SENSORS; i++) {
                      current_total+=Irms[j];
                    }
                    Serial.print(",");
                    Serial.print(current_total);
                }
              Serial.println();
            }
            else{
              Serial.print(",");
            }
      }
}

void read_energy_parameters() {
  //Calcula a corrente
  if (sensor_to_read_power < NUMBER_CURRENT_SENSORS) {
    aux_Irms[sensor_to_read_power] +=current[sensor_to_read_power].calcIrms(NUMBER_READS_power);
    // Irms[sensor_to_read_power]=double(int(Irms[sensor_to_read_power]*100))/double(100);
    
    if (aux_power_cont >= AUX_POWER_READS) {
      for (byte i = 0; i < NUMBER_CURRENT_SENSORS; i++) {
        aux_Irms[i] = aux_Irms[i] / AUX_POWER_READS;
        if (aux_Irms[i] <= double(0.20)) {
          aux_Irms[i] = double(0.0);
        }
        Irms[i] = aux_Irms[i];
        aux_Irms[i] = 0;
      }
      aux_power_cont = 0;
    }


    potencia_aparente[sensor_to_read_power] = float(float(Irms[sensor_to_read_power]) * TENSAO_REDE); //Unidade é VA
    if (tem_power_factor[sensor_to_read_power]) {
      potencia_ativa[sensor_to_read_power] = potencia_aparente[sensor_to_read_power] * power_factor[sensor_to_read_power]; //Unidade é W
      potencia_reativa[sensor_to_read_power] = sqrt(sq(potencia_aparente[sensor_to_read_power]) - sq(potencia_ativa[sensor_to_read_power])); //Unidade é var
    }
    sensor_to_read_power++;
  }
  else {
    sensor_to_read_power = 0;
    aux_power_cont++;
  }
  return;
}

void analise_time_power() {
  if ((millis() - tempo_power) >= (2 * HOUR_power)) { //previne o momento em que millis() zera
    tempo_power = millis();
  }
  if ((millis() - tempo_power) >= HOUR_power) {
    hours_power++;
    EEPROM.write(18, hours_power);

    for (byte i = 0; i < NUMBER_CURRENT_SENSORS; i++) {
      media_potencia_aparente[i] = media_potencia_aparente[i] / float(contador_power);
      if (tem_power_factor[i]) {
        media_potencia_ativa[i] = media_potencia_ativa[i] / float(contador_power);
        media_potencia_reativa[i] = media_potencia_reativa[i] / float(contador_power);
      }

      consumo_kwh[i] += media_potencia_ativa[i];
      if (tem_power_factor[i]) {
        consumo_kvarh[i] += media_potencia_reativa[i];
        consumo_kvah[i] += media_potencia_aparente[i];
      }

      media_potencia_aparente[i] = 0;
      media_potencia_ativa[i] = 0;
      media_potencia_reativa[i] = 0;
    }

    tempo_power = millis();
    contador_power = 0;
  }
  else {
    contador_power++;
    for (byte i = 0; i < NUMBER_CURRENT_SENSORS; i++) {
      media_potencia_aparente[i] += potencia_aparente[i] / 1000.0;
      if (tem_power_factor[i]) {
        media_potencia_ativa[i] += potencia_ativa[i] / 1000.0;
        media_potencia_reativa[i] += potencia_reativa[i] / 1000.0;
      }
    }

  }

  if (hours_power >= DAY_power) {
    days_power++;
    EEPROM.write(17, days_power);
    hours_power = 0;
    EEPROM.write(18, hours_power);
    //Serial.println(days_power);
    tempo_power = millis();
    contador_power = 0;
    for (byte i = 0; i < NUMBER_CURRENT_SENSORS; i++) {
      media_potencia_aparente[i] = 0;
      media_potencia_ativa[i] = 0;
      media_potencia_reativa[i] = 0;
    }
  }

  if (days_power > MONTH_power[months_power]) {
    //    Serial.print("MES:");
    //    Serial.println(months_power);
    months_power++;
    EEPROM.write(16, months_power);
    days_power = 1;
    EEPROM.write(17, days_power);
    hours_power = 0;
    EEPROM.write(18, hours_power);

    for (byte i = 0; i < NUMBER_CURRENT_SENSORS; i++) {
      //      if (tem_power_factor[i]) {
      //        Serial.print("Consumo real : ");
      //        Serial.println(consumo_kwh[i]);
      //        Serial.print("Consumo reativo : ");
      //        Serial.println(consumo_kvarh[i]);
      //      }
      //      Serial.print("Consumo aparente : ");
      //      Serial.println(consumo_kvah[i]);

      consumo_kwh[i] = 0;
      consumo_kvarh[i] = 0;
      consumo_kvah[i] = 0;

      media_potencia_aparente[i] = 0;
      media_potencia_ativa[i] = 0;
      media_potencia_reativa[i] = 0;
    }

    tempo_power = millis();
    contador_power = 0;
  }

  if (months_power > YEAR_power) {

    for (byte i = 0; i < NUMBER_CURRENT_SENSORS; i++) {
      consumo_kwh[i] = 0;
      consumo_kvarh[i] = 0;
      consumo_kvah[i] = 0;

      media_potencia_aparente[i] = 0;
      media_potencia_ativa[i] = 0;
      media_potencia_reativa[i] = 0;
    }

    months_power = 1;
    EEPROM.write(16, months_power);
    days_power = 1;
    EEPROM.write(17, days_power);
    hours_power = 0;
    EEPROM.write(18, hours_power);

    tempo_power = millis();
    contador_power = 0;
  }


  if (MULTIPHASE) {
    double aux_Irms_total = 0;
    float aux_potencia_aparente_total = 0;
    double aux_consumo_kvah_total = 0; //aparente
    double aux_Irms_neutro = 0;

    for (byte i = 0; i < (NUMBER_CURRENT_SENSORS - 1); i++) { //A última porta será sempre o neutro, por isso o "-1"
      aux_Irms_total += Irms[i];
      aux_potencia_aparente_total += potencia_aparente[i];
      aux_consumo_kvah_total += consumo_kvah[i]; //aparente
    }

//    aux_Irms_total /= (NUMBER_CURRENT_SENSORS - 1);
//    aux_potencia_aparente_total  /= (NUMBER_CURRENT_SENSORS - 1);
//    aux_consumo_kvah_total  /= (NUMBER_CURRENT_SENSORS - 1); //aparente


    Irms_total = aux_Irms_total;
    potencia_aparente_total = aux_potencia_aparente_total;
    consumo_kvah_total = aux_consumo_kvah_total; //aparente

    Irms_neutro = Irms[NUMBER_CURRENT_SENSORS - 1];

  }

  return;
}


/************************************************************************************************************************
*****FIM***************************************************FUNÇÕES*******************************************************
*************************************************************************************************************************/


void loop() {
    delayMicroseconds(300);

  /***************************************************************************Leitura dos sensores a cada aprox. 2 segundos, ou 2000ms***************************************************************************************************************/
  //A cada 2 segundos as leituras são atualizadas
  time_exit = millis() - time_enter;
  if (time_exit >= long(8200)) {
    for (byte i = 0; i < real_num_hum_sensors; i++) {
      getTemperature(i);
      getHumidity(i);
    }
    atualiza_media_umidade();
    atualiza_media_temp_dht();


    for (byte i = 0; i < real_num_onewire_ports; i++) {
      nodos_sensores_temperatura[i]->requestTemperatures();
      aux = (byte) nodos_sensores_temperatura[i]->getDeviceCount();  //descobrindo quantos sensores existem num dado corredor
      for (byte k = 0; k < aux; k++) { //percorrendo todos os sensores do corredor "
        getTemperature(*nodos_sensores_temperatura[i], devices[i][k], i, k);
      }
      atualiza_medias(i, aux);
    }
    atualiza_media_geral();

    //if (presenca_web) {
    //   Serial.println("!!!!!!!!!!!!!!!");
    //}
    //if (presenca) {
    //   Serial.println("----------------");
    //}


    //    /*********************FUMACA*********************************************/
    //    if ((millis() - time_mq2) > TIME_NO_ALARMS  ) { //30 min
    //      time_mq2 = millis();
    //      Serial.println("!!!!!!!!!!!!!!!!!!!!!!!");
    //      if (alarms == false) {
    //        calibrate = true;
    //        Serial.println("Calibration starting...");
    //      }
    //      else {
    //        alarms = false;
    //      }
    //    }
    //
    //
    //    if (calibrate) {
    //      MQCalibration(MQ_PIN);
    //    }
    //    else {
    //      MQRead(MQ_PIN);
    //
    //      //  Serial.print("LPG:");
    //      lpg = (MQGetGasPercentage(rs / Ro, GAS_LPG) ); //Se below for craziness and reading the gas value
    //      //  Serial.print(MQGetGasPercentage(rs / Ro, GAS_LPG) );
    //      //  Serial.print (lpg);
    //      //  Serial.print( "ppm" );
    //      //  Serial.print("    ");
    //      //  Serial.print("CO:");
    //      co = (MQGetGasPercentage(rs / Ro, GAS_CO) );
    //      //  Serial.print(MQGetGasPercentage(rs / Ro, GAS_CO) );
    //      //  Serial.print (co);
    //      //  Serial.print( "ppm" );
    //      //  Serial.print("    ");
    //      //
    //      //  Serial.print("SMOKE:");
    //      smoke = (MQGetGasPercentage(rs / Ro, GAS_SMOKE) );
    //      //  Serial.print(MQGetGasPercentage(rs / Ro, GAS_SMOKE) );
    //      //  Serial.print (smoke);
    //      //  Serial.print( "ppm" );
    //      //  Serial.print("\n");
    //
    //      if ((smoke + co + lpg) != 0) {
    //        alarms = true;
    //      }
    //
    //      air_quality = map((smoke + co + lpg), 0, 600, 0, 100); //considera-se que acima de 200ppm num ambiente como datacenter, já esteja tomado pelo gás. Portanto
    //      //considerado a soma de 3 gases, é um valor de 0 a 600. Esse valor é convertido em porcentagem.
    //
    //    }
    //    //delay(200);
    //    /*********************FUMACA*********************************************/

    time_enter = millis();
  }
  
    atualiza_media_umidade();
    atualiza_media_temp_dht();
  /*********************FUMACA*********************************************/
  if ((millis() - time_mq2) > TIME_NO_ALARMS  ) { //30 min
    time_mq2 = millis();
    //    Serial.println("!!!!!!!!!!!!!!!!!!!!!!!");
    if (alarms == false) {
      calibrate = true;
      //      Serial.println("Calibration starting...");
    }
    else {
      alarms = false;
    }
  }


  if (calibrate) {
    MQCalibration(MQ_PIN);
  }
  else {
    MQRead(MQ_PIN);

    //  Serial.print("LPG:");
    lpg = (MQGetGasPercentage(rs / Ro, GAS_LPG) ); //Se below for craziness and reading the gas value
    //  Serial.print(MQGetGasPercentage(rs / Ro, GAS_LPG) );
    //  Serial.print (lpg);
    //  Serial.print( "ppm" );
    //  Serial.print("    ");
    //  Serial.print("CO:");
    co = (MQGetGasPercentage(rs / Ro, GAS_CO) );
    //  Serial.print(MQGetGasPercentage(rs / Ro, GAS_CO) );
    //  Serial.print (co);
    //  Serial.print( "ppm" );
    //  Serial.print("    ");
    //
    //  Serial.print("SMOKE:");
    smoke = (MQGetGasPercentage(rs / Ro, GAS_SMOKE) );
    //  Serial.print(MQGetGasPercentage(rs / Ro, GAS_SMOKE) );
    //  Serial.print (smoke);
    //  Serial.print( "ppm" );
    //  Serial.print("\n");

    if ((smoke + co + lpg) != 0) {
      alarms = true;
    }

    air_quality = map((smoke + co + lpg), 0, 600, 0, 100); //considera-se que acima de 200ppm num ambiente como datacenter, já esteja tomado pelo gás. Portanto
    //considerado a soma de 3 gases, é um valor de 0 a 600. Esse valor é convertido em porcentagem.
    if (air_quality > 100) {
      air_quality = 100;
    }
    air_quality = 100 - air_quality; //Assim, ele será uma medida que, em 100% é ideal e em 0% é péssimo

  }
  //delay(200);
  //    Serial.println(ip[1]);
  //    Serial.println(smoke);
  /*********************FUMACA*********************************************/
  verifica_presenca();
  read_inundacao();
  read_light();
  read_sound();
  read_vibration();
  read_flame();
  read_door();
  read_bmp();
  read_energy_parameters();
  analise_time_power();
  plot_current_readings();
  /*******************************************************************************************************************************************************************************************************************************/


  /****INICIO**************************INFO NO SERIAL E LEITURAS*************************************************/
  //  Serial.print(F("Media da Sala :"));
  //  Serial.print(media_geral);
  //  Serial.println(F(" Celsius degrees"));
  //  Serial.print(F("Media da umidade na Sala :"));
  //  Serial.print(media_umidade);
  //  Serial.println(F("%"));
  //  Serial.println();
  /*****FIM*************************INFO NO SERIAL****************************************************/

  /*******INÍCIO************************************************WEB SERVER********************************************************************************************************************************************************/
  //escuta a entrada de clientes
  client.flush(); //para liberar o cache do cliente para um possível novo cliente
  client = server.available();

  if (client) {
    //uma solicitação http termina com uma linha em branco
    boolean BlankLine = true;

    currentLine = new String;
    user[0] = new String;
    user[1] = new String;
    parameter_name[0] = new String;
    parameter_name[1] = new String;
    msg_output = new String;

    currentLine->reserve(80);
    *currentLine = "";
    *user[0] = "";
    *user[1] = "";
    *parameter_name[0] = "";
    *parameter_name[1] = "";
    *msg_output = "";

    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        //se a linha estiver em branco e o fim da linha for um caractere newline
        //'\n' = fim da solicitação HTTP
        if (c == '\n' && BlankLine) {


          *currentLine += (char)client.read();
          if (*currentLine != "") {
            while (client.available())
            {
              *currentLine += (char)client.read();
            }
          }
          int p1 = -1, p2 = currentLine->indexOf("="), p3 = -2, count = 0;
          //          if(currentLine.length()>0) msg_output += "<br>Parametros recibidos:<br><ol>";
          if (currentLine->length() > 0 && p2 > -1) {
            aux_user = 0;
            while (p2 > -1) {
              *parameter_name[aux_user] = currentLine->substring(p1 + 1, p2);
              if (p3 == -2) p3 = currentLine->indexOf("&");
              else p3 = currentLine->indexOf("&", p3 + count);
              if (p3 == -1) p3 = currentLine->length();
              *user[aux_user] = currentLine->substring(p2 + 1, p3);
              count++;
              p1 = currentLine->indexOf("&", p1 + count);
              p2 = currentLine->indexOf("=", p2 + count);
              if (aux_user == 1) {
                if (*user[1] == pass) {
                  delete(definite_user);
                  definite_user = new String;
                  *definite_user = *user[0];
                  for (byte i = 0; i < definite_user->length(); i++) {
                    if ((*definite_user)[i] == '+')(*definite_user)[i] = ' ';
                  }
                }
              }
              aux_user++;
            }

            if ((*currentLine)[0] == 'f') {
              *definite_user = EMPTY;
            }
          }
          if (currentLine->length() > 0) *msg_output += "</ol>";









          //Inicia o envio de dados via HTTP (aqui, envia-se código HTML em forma de mensagem, o navegador do usuário se encarregará de interpretar o código)

          client.println(F("HTTP/1.1 200 OK"));  //Resposta HTTP padrão
          client.println(F("Content-Type: text/html"));
          client.println(F("\n"));
          client.println(F("<!DOCTYPE html>"));
          // client.println(F("\n"));
          client.println(F("<html>"));
          client.println(F("<head>"));




          // add page title

          client.println("<meta name=\"description\" content=\"LCA Systems v16.1\"/>");

          //          // add a meta refresh tag, so the browser pulls again every x seconds:
          //          client.print("<meta http-equiv=\"refresh\" content=\"");
          //          client.print(refreshPage);
          //          client.println("; url=/\">");

          // add other browser configuration
          client.println("<meta name=\"apple-mobile-web-app-capable\" content=\"yes\">");
          client.println("<meta name=\"apple-mobile-web-app-status-bar-style\" content=\"default\">");
          client.println("<meta name=\"viewport\" content=\"width=device-width, user-scalable=no\">");

          //inserting the styles data, usually found in CSS files.
          client.println(F("<style type=\"text/css\">"));
          client.println(F(""));

          //This will set how the page will look graphically
          client.println(F("html { height:100%; }"));

          client.println(F("  body {"));
          client.println(F("    height: 100%;"));
          client.println(F("    margin: 0;"));
          client.println(F("    font-family: helvetica, sans-serif;"));
          client.println(F("    -webkit-text-size-adjust: none;"));
          client.println(F("   }"));
          client.println(F(""));
          client.println(F("body {"));
          client.println(F("    -webkit-background-size: 100% 21px;"));
          client.println(F("    background-color: #c5ccd3;"));
          client.println(F("    background-image:"));
          client.println(F("    -webkit-gradient(linear, left top, right top,"));
          client.println(F("    color-stop(.75, transparent),"));
          client.println(F("    color-stop(.75, rgba(255,255,255,.1)) );"));
          client.println(F("    -webkit-background-size: 7px;"));
          client.println(F("   }"));
          client.println(F(""));
          client.println(F(".view {"));
          client.println(F("    min-height: 100%;"));
          client.println(F("    overflow: auto;"));
          client.println(F("   }"));
          client.println(F(""));
          client.println(F(".header-wrapper {"));
          client.println(F("    height: 44px;"));
          client.println(F("    font-weight: bold;"));
          client.println(F("    text-shadow: rgba(0,0,0,0.7) 0 -1px 0;"));
          client.println(F("    border-top: solid 1px rgba(255,255,255,0.6);"));
          client.println(F("    border-bottom: solid 1px rgba(0,0,0,0.6);"));
          client.println(F("    color: #fff;"));
          client.println(F("    background-color: #8195af;"));
          client.println(F("    background-image:"));
          client.println(F("    -webkit-gradient(linear, left top, left bottom,"));
          client.println(F("    from(rgba(255,255,255,.4)),"));
          client.println(F("    to(rgba(255,255,255,.05)) ),"));
          client.println(F("    -webkit-gradient(linear, left top, left bottom,"));
          client.println(F("    from(transparent),"));
          client.println(F("    to(rgba(0,0,64,.1)) );"));
          client.println(F("    background-repeat: no-repeat;"));
          client.println(F("    background-position: top left, bottom left;"));
          client.println(F("    -webkit-background-size: 100% 21px, 100% 22px;"));
          client.println(F("    -webkit-box-sizing: border-box;"));
          client.println(F("   }"));
          client.println(F(""));
          client.println(F(".header-wrapper h1 {"));
          client.println(F("    text-align: center;"));
          client.println(F("    font-size: 20px;"));
          client.println(F("    line-height: 44px;"));
          client.println(F("    margin: 0;"));
          client.println(F("   }"));
          client.println(F(""));
          client.println(F(".group-wrapper {"));
          client.println(F("    margin: 9px;"));
          client.println(F("    }"));
          client.println(F(""));
          client.println(F(".group-wrapper h2 {"));
          client.println(F("    color: #4c566c;"));
          client.println(F("    font-size: 17px;"));
          client.println(F("    line-height: 0.8;"));
          client.println(F("    font-weight: bold;"));
          client.println(F("    text-shadow: #fff 0 1px 0;"));
          client.println(F("    margin: 20px 10px 12px;"));
          client.println(F("   }"));
          client.println(F(""));
          client.println(F(".group-wrapper h3 {"));
          client.println(F("    color: #4c566c;"));
          client.println(F("    font-size: 12px;"));
          client.println(F("    line-height: 1;"));
          client.println(F("    font-weight: bold;"));
          client.println(F("    text-shadow: #fff 0 1px 0;"));
          client.println(F("    margin: 20px 10px 12px;"));
          client.println(F("   }"));
          client.println(F(""));
          client.println(F(".group-wrapper h4 {"));  //Text for description
          client.println(F("    color: #212121;"));
          client.println(F("    font-size: 14px;"));
          client.println(F("    line-height: 1;"));
          client.println(F("    font-weight: bold;"));
          client.println(F("    text-shadow: #aaa 1px 1px 3px;"));
          client.println(F("    margin: 5px 5px 5px;"));
          client.println(F("   }"));
          client.println(F(""));
          client.println(F(".group-wrapper table {"));
          client.println(F("    background-color: #fff;"));
          client.println(F("    -webkit-border-radius: 10px;"));

          client.println(F("    -moz-border-radius: 10px;"));
          client.println(F("    -khtml-border-radius: 10px;"));
          client.println(F("    border-radius: 10px;"));


          client.println(F("    font-size: 17px;"));
          client.println(F("    line-height: 20px;"));
          client.println(F("    margin: 9px 0 20px;"));
          client.println(F("    border: solid 1px #a9abae;"));
          client.println(F("    padding: 11px 3px 12px 3px;"));
          client.println(F("    margin-left:auto;"));
          client.println(F("    margin-right:auto;"));

          client.println(F("    -moz-transform :scale(1);")); //Code for Mozilla Firefox
          client.println(F("    -moz-transform-origin: 0 0;"));



          client.println(F("   }"));
          client.println(F(""));


//          //how the green (ON) LED will look
//          client.println(".green-circle {");
//          client.println("    display: block;");
//          client.println("    height: 23px;");
//          client.println("    width: 23px;");
//          client.println("    background-color: #0f0;");
//          //client.println("    background-color: rgba(60, 132, 198, 0.8);");
//          client.println("    -moz-border-radius: 11px;");
//          client.println("    -webkit-border-radius: 11px;");
//          client.println("    -khtml-border-radius: 11px;");
//          client.println("    border-radius: 11px;");
//          client.println("    margin-left: 1px;");
//
//          client.println("    background-image: -webkit-gradient(linear, 0% 0%, 0% 90%, from(rgba(46, 184, 0, 0.8)), to(rgba(148, 255, 112, .9)));@");
//          client.println("    border: 2px solid #ccc;");
//          client.println("    -webkit-box-shadow: rgba(11, 140, 27, 0.5) 0px 10px 16px;");
//          client.println("    -moz-box-shadow: rgba(11, 140, 27, 0.5) 0px 10px 16px; /* FF 3.5+ */");
//          client.println("    box-shadow: rgba(11, 140, 27, 0.5) 0px 10px 16px; /* FF 3.5+ */");
//
//          client.println("    }");
//          client.println("");

//          //how the red (ON) LED will look
//          client.println(".red-circle {");
//          client.println("    display: block;");
//          client.println("    height: 23px;");
//          client.println("    width: 23px;");
//          client.println("    background-color: #f00;");
//          //client.println("    background-color: rgba(60, 132, 198, 0.8);");
//          client.println("    -moz-border-radius: 11px;");
//          client.println("    -webkit-border-radius: 11px;");
//          client.println("    -khtml-border-radius: 11px;");
//          client.println("    border-radius: 11px;");
//          client.println("    margin-left: 1px;");
//
//          client.println("    background-image: -webkit-gradient(linear, 0% 0%, 0% 90%, from(rgba(250, 5, 5, 0.8)), to(rgba(255, 212, 37, .9)));@");
//          client.println("    border: 2px solid #ccc;");
//          client.println("    -webkit-box-shadow: rgba(11, 140, 27, 0.5) 0px 10px 16px;");
//          client.println("    -moz-box-shadow: rgba(11, 140, 27, 0.5) 0px 10px 16px; /* FF 3.5+ */");
//          client.println("    box-shadow: rgba(11, 140, 27, 0.5) 0px 10px 16px; /* FF 3.5+ */");
//
//          client.println("    }");
//          client.println("");

//          //how the black (off)LED will look
//          client.println(".black-circle {");
//          client.println("    display: block;");
//          client.println("    height: 23px;");
//          client.println("    width: 23px;");
//          client.println("    background-color: #040;");
//          client.println("    -moz-border-radius: 11px;");
//          client.println("    -webkit-border-radius: 11px;");
//          client.println("    -khtml-border-radius: 11px;");
//          client.println("    border-radius: 11px;");
//          client.println("    margin-left: 1px;");
//          client.println("    -webkit-box-shadow: rgba(11, 140, 27, 0.5) 0px 10px 16px;");
//          client.println("    -moz-box-shadow: rgba(11, 140, 27, 0.5) 0px 10px 16px; /* FF 3.5+ */");
//          client.println("    box-shadow: rgba(11, 140, 27, 0.5) 0px 10px 16px; /* FF 3.5+ */");
//          client.println("    }");
//          client.println("");

//          //this will add the glare to both of the LEDs
//          client.println("   .glare {");
//          client.println("      position: relative;");
//          client.println("      top: 1;");
//          client.println("      left: 5px;");
//          client.println("      -webkit-border-radius: 10px;");
//          client.println("      -moz-border-radius: 10px;");
//          client.println("      -khtml-border-radius: 10px;");
//          client.println("      border-radius: 10px;");
//          client.println("      height: 1px;");
//          client.println("      width: 13px;");
//          client.println("      padding: 5px 0;");
//          client.println("      background-color: rgba(200, 200, 200, 0.25);");
//          client.println("      background-image: -webkit-gradient(linear, 0% 0%, 0% 95%, from(rgba(255, 255, 255, 0.7)), to(rgba(255, 255, 255, 0)));");
//          client.println("    }");
//          client.println("");


          //and finally this is the end of the style data and header
          client.println(F("</style>"));
          client.println(F("</head>"));







          client.println(F("</head>"));


          if (LUX_SENSOR) {
            if (lux <= lowThreeshold_lux) {
              client.println(F("<body style=\"background-color:#545454;\">"));
            }
            else {
              client.println(F("<body style=\"background-color:lightgrey;\">"));
            }

          }
          else if (NUMBER_LDRs >= 1) {
            boolean dark = true;
            for (byte i = 0; i < NUMBER_LDRs; i++) {
              if (percent_ldr[i] > lowThreeshold_light) {
                dark = false;
                break;
              }
            }
            if (dark) {
              client.println(F("<body style=\"background-color:#545454;\">"));
            }
            else {
              client.println(F("<body style=\"background-color:lightgrey;\">"));
            }
          }
          else {
            client.println(F("<body style=\"background-color:lightgrey;\">"));
          }


          client.println(F("<title>LCA Web Server</title>"));
          client.println(F("<META HTTP-EQUIV=\"refresh\" CONTENT=\"29\">"));



          client.println(text_form_action);




          client.println(F("<h4>Desenvolvido por Lucas Coelho de Almeida - v18.4</h4>"));
          client.println(F("<h4>LCA Module - Web Server</h4>"));
          client.println(F("<h1 style=\"text-align:center;\">SALA COFRE REDUNDANTE</h1>"));
          //          delay(2);


          client.print(F("<table border=\"1\" align=center>"));
          client.print(F("<tr>"));
          client.print(F("<th bgcolor=\"gray\">"));
          //          client.println(F("<center>"));
          if (definite_user->equals(EMPTY)) {
            client.println(F("Solicitante:&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<input type='text' name='Permitido' size='20' maxlength='30' value=''><br>"));
            client.println(F("Senha&nbsp;de&nbsp;Acesso:<input type='password' name='senha' size='20' maxlength='20'value=''><br>"));
            client.println(F("<input type='submit' name='Permitir acesso' value='Permitir acesso'>"));
            client.println(F("</form>"));
          }
          else {
            *msg_output = String("<h2>") + String("Permitido") + ": " + *definite_user + String("</h2>") + "<br />";
            client.println(*msg_output);
            client.println(F("<input type='submit' name='f' value='Finalizar Acesso'>"));
          }
          //          client.println(F("</center>"));

          client.print(F("</th></tr>"));
          client.println(F("</table>"));






          //aqui, envia-se os dados referentes ao sensor de presença
          client.print(F("<table border=\"1\" align=center>"));

          if (NUMBER_DOOR) {
            if (NUMBER_DOOR == 1) {
              client.print(F("<tr>"));
              client.print(F("<th bgcolor=\"white\"><center><h3>PORTA DE ACESSO "));
              if (door[0] == DOOR_OPEN) {
                client.print(F("<td bgcolor=\"gree\"><center><h2>FECHADA</h2></center></td></tr>"));
              }
              else if (door[0] == DOOR_CLOSED) {
                client.print(F("<td bgcolor=\"red\"><center><h2>ABERTA</h2></td></tr>"));
              }
            }
            else {
              for (byte i = 0; i < NUMBER_DOOR; i++) {
                client.print(F("<tr>"));
                client.print(F("<th bgcolor=\"white\"><center><h3>PORTA "));
                client.print(i);
                client.print(F("</h3></center></th>"));
                if (door[i] == DOOR_OPEN) {
                  client.print(F("<td bgcolor=\"gree\"><center><h2>FECHADA</h2></center></td></tr>"));
                }
                else if (door[i] == DOOR_CLOSED) {
                  client.print(F("<td bgcolor=\"red\"><center><h2>ABERTA</h2></td></tr>"));
                }
              }
            }
          }



          client.print(F("<tr>"));
          client.println(F("<th bgcolor=\"white\"><center><h3> PORTA DE ACESSO </h3></center></th>"));
          if (presenca_web == 0) {
            client.print(F("<td bgcolor=\"gree\"><center><h2>SEM MOVIMENTOS</h2></center></td></tr>"));
          }
          else  {
            client.print(F("<td bgcolor=\"red\"><center><h2>MOVIMENTO DETECTADO</h2></td></tr>"));
          }



          //aqui, envia-se os dados referentes ao sensor de fumaca
          client.print(F("<tr>"));
          client.println(F("<th bgcolor=\"white\"><center><h3> QUALIDADE DO AR </h3></center></th>"));
          if (air_quality >= 70) {
            client.print(F("<td bgcolor=\"gree\"><center><h1> "));
          }
          else if (air_quality > float(35) && air_quality <= float(70)) {
            client.print(F("<td bgcolor=\"#FF9900\"><center><h1> "));
          }
          else if (air_quality < float(35)) {
            client.print(F("<td bgcolor=\"red\"><center><h1> "));
          }
          client.print(air_quality);
          client.print(F("<span>&#37;</span></h1></center></td></tr>"));

          //aqui fecha-se a tabela dos sensores de movimento e fumaça
          client.println(F("</table>"));

          if (NUMBER_CURRENT_SENSORS) {


            //aqui, percorre-se todos os sensores de corrente eletrica, imprimindo os valores de corrente, consumo e potencia
            client.println(F("<h2 style=\"text-align:center;\">----PAR&Acirc;METROS DE ENERGIA----</h2>"));



            if (MULTIPHASE) {
              client.print(F("<table border=\"1\" align=center>"));
              client.print(F("<tr>"));
              client.print(F("<th bgcolor=\"gray\"><center><h3>Corrente Total "));
              client.print(F("</h3></center></th>"));
                            client.print(F("<th bgcolor=\"gray\"><center><h3>Corrente Neutro "));
                            client.print(F("</h3></center></th>"));
              client.print(F("<th bgcolor=\"gray\"><center><h3>Pot&ecirc;ncia Aparente Total "));
              client.print(F("</h3></center></th>"));
              client.print(F("<th bgcolor=\"gray\"><center><h3>Consumo Aparente Total "));
              client.print(F("</h3></center></th>"));
              client.print(F("</tr>"));
              client.print(F("<tr>"));
              client.print(F("<th bgcolor=\"gray\"><center><h3>"));
              client.print(Irms_total);
              client.print(F(" A rms</h3></center></th>"));
                            client.print(F("<th bgcolor=\"gray\"><center><h3>"));
                            client.print(Irms_neutro);
                            client.print(F(" A rms</h3></center></th>"));
              client.print(F("<th bgcolor=\"gray\"><center><h3>"));
              client.print(potencia_aparente_total);
              client.print(F(" VA</h3></center></th>"));
              client.print(F("<th bgcolor=\"gray\"><center><h3>"));
              client.print(consumo_kvah_total);
              client.print(F(" kVAH</h3></center></th>"));
              client.print(F("</tr>"));
              client.print(F("</table>"));
            }
            client.print(F("<table border=\"1\" align=center>"));
            for (byte i = 0; i < NUMBER_CURRENT_SENSORS - 1; i++) {
              client.print(F("<tr>"));
              client.print(F("<th bgcolor=\"gray\"><center><h3>Sensor de corrente "));
              client.print(i);
              client.print(F("</h3></center></th>"));

              //USAR CASO ELE PRECISE DE ALGUM THREESHOLD
              //              if (valor_inundacao[i] <= Threeshold_inundacao && valor_inundacao[i] > NO_INUNDACAO) {
              //                client.print(F("<td bgcolor=\"#FF9900\"><center><h3>"));
              //              }
              //              else if (valor_inundacao[i] > Threeshold_inundacao) {
              //                client.print(F("<td bgcolor=\"red\"><center><h3>"));
              //              }
              //              else if (valor_inundacao[i] == NO_INUNDACAO) {
              //                client.print(F("<td bgcolor=\"gree\"><center><h3>"));
              //              }

              client.print(F("<td bgcolor=\"white\"><center><h3>"));
              client.print(Irms[i]);
              client.print(F(" <span>Arms</span></h3>"));
              client.print(F("</center></td>"));

              client.print(F("<td bgcolor=\"gray\"><center><h3>"));
              client.print(F("Pot&ecirc;ncia Ativa"));
              client.print(F("</h3></center></td>"));
              client.print(F("<td bgcolor=\"white\"><center><h3>"));
              if (tem_power_factor[i]) {
                client.print(potencia_ativa[i]);
                client.print(F(" <span>W</span></h3>"));
              }
              else {
              }
              client.print(F("</center></td>"));

              client.print(F("<td bgcolor=\"gray\"><center><h3>"));
              client.print(F("Consumo Ativo"));
              client.print(F("</h3></center></td>"));
              client.print(F("<td bgcolor=\"white\"><center><h3>"));
              if (tem_power_factor[i]) {
                client.print(consumo_kwh[i]);
                client.print(F(" <span>kWh</span></h3>"));
              }
              else {
              }
              client.print(F("</center></td>"));

              client.print(F("<td bgcolor=\"gray\"><center><h3>"));
              client.print(F("Pot&ecirc;ncia Reativa"));
              client.print(F("</h3></center></td>"));
              client.print(F("<td bgcolor=\"white\"><center><h3>"));
              if (tem_power_factor[i]) {
                client.print(potencia_reativa[i]);
                client.print(F(" <span>var</span></h3>"));
              }
              else {
              }
              client.print(F("</center></td>"));

              client.print(F("<td bgcolor=\"gray\"><center><h3>"));
              client.print(F("Excedente Reativo"));
              client.print(F("</h3></center></td>"));
              client.print(F("<td bgcolor=\"white\"><center><h3>"));
              if (tem_power_factor[i]) {
                client.print(consumo_kvarh[i]);
                client.print(F(" <span>kvarh</span></h3>"));
              }
              else {

              }
              client.print(F("</center></td>"));

              client.print(F("<td bgcolor=\"gray\"><center><h3>"));
              client.print(F("Pot&ecirc;ncia Aparente"));
              client.print(F("</h3></center></td>"));
              client.print(F("<td bgcolor=\"white\"><center><h3>"));
              client.print(potencia_aparente[i]);
              client.print(F(" <span>VA</span></h3>"));
              client.print(F("</center></td>"));

              client.print(F("<td bgcolor=\"gray\"><center><h3>"));
              client.print(F("Consumo Aparente"));
              client.print(F("</h3></center></td>"));
              client.print(F("<td bgcolor=\"white\"><center><h3>"));
              client.print(consumo_kvah[i]);
              client.print(F(" <span>kVAh</span></h3>"));
              client.print(F("</center></td>"));










              client.print(F("</tr>"));
            }

            client.print(F("</table>"));
          }

          if (NUMBER_FLAME) {
            //aqui, percorre-se todos os sensores de chama, imprimindo os valores
            client.println(F("<h2 style=\"text-align:center;\">----CHANCE DE PRESEN&Ccedil;A DE FOGO----</h2>"));
            client.print(F("<table border=\"1\" align=center>"));
            for (byte i = 0; i < NUMBER_FLAME; i++) {
              client.print(F("<tr>"));
              client.print(F("<th bgcolor=\"white\"><center><h3>Sensor de chama "));
              client.print(i);
              client.print(F("</h3></center></th>"));
              if (percent_flame[i] > NO_FLAME && percent_flame[i] <= Threeshold_flame) {
                client.print(F("<td bgcolor=\"#FF9900\"><center><h3>"));
              }
              else if (percent_flame[i] > Threeshold_flame) {
                client.print(F("<td bgcolor=\"red\"><center><h3>"));
              }
              else if (percent_flame[i] == NO_FLAME) {
                client.print(F("<td bgcolor=\"gree\"><center><h3>"));
              }
              client.print(percent_flame[i]);
              client.print(F(" <span>&#37;</span>\n</h3>"));
              client.print(F("</center></td>"));
              client.print(F("</tr>"));
            }

            client.print(F("</table>"));
          }


          if (real_num_onewire_ports) { //se existirem sensores de temperatura, as informações serão enviadas
            //aqui, envia-se os dados referentes aos sensores de temperatura
            client.println(F("<h2 style=\"text-align:center;\">TEMPERATURA</h2>"));
            client.print(F("<table border=\"1\" align=center>"));
            for (byte i = 0; i < real_num_onewire_ports; i++) {
              //            nodos_sensores_temperatura[i]->requestTemperatures();
              aux = (byte) nodos_sensores_temperatura[i]->getDeviceCount();  //descobrindo quantos sensores existem num dado corredor
              client.print(F("<tr>"));
              client.print(F("<th bgcolor=\"white\"><center><h3>Corredor "));
              client.print(i);
              client.println(F(" : </h3></center></th>"));
              for (byte k = 0; k < aux; k++) { //percorrendo todos os sensores do corredor "i"
                //              getTemperature(*nodos_sensores_temperatura[i], devices[i][k], i, k);

                //definindo cor de aviso da temperatura
                if (tempC[i][k] < lowlowThreeshold_temp) {
                  client.print(F("<td bgcolor=\"#FF9900\"><center><h4>Sensor "));
                }
                else if (tempC[i][k] < lowThreeshold_temp && tempC[i][k] >= lowlowThreeshold_temp) {
                  client.print(F("<td bgcolor=\"gree\"><center><h4>Sensor "));
                }
                else if (tempC[i][k] >= lowThreeshold_temp && tempC[i][k] < highThreeshold_temp) {
                  client.print(F("<td bgcolor=\"#FF9900\"><center><h4>Sensor "));
                }
                else if (tempC[i][k] >= highThreeshold_temp) {
                  client.print(F("<td bgcolor=\"red\"><center><h4>Sensor "));
                }
                client.print(k);
                client.print(" :");
                client.print(tempC[i][k]);
                //              Serial.println(nodos_sensores_temperatura[i]->validAddress(devices[i][k])); //testes de validação para saber se há algum problema de conexão com os sensores
                //              Serial.println(nodos_sensores_temperatura[i]->isConnected(devices[i][k]));
                client.println(F(" <span>&#186;</span>C </h4></center></td>"));
              }
              //aqui, imprime-se a média do corredor percorrido
              //            atualiza_medias(i, aux);

              //definindo cor de aviso da temperatura média do corredor
              if (media_leitura[i] < lowlowThreeshold_temp) {
                client.print(F("<td bgcolor=\"#FF9900\"><center><h3>M<span>&#233;</span>dia : "));
              }
              else if (media_leitura[i] < lowThreeshold_temp && media_leitura[i] >= lowlowThreeshold_temp) {
                client.print(F("<td bgcolor=\"gree\"><center><h3>M<span>&#233;</span>dia : "));
              }
              else if (media_leitura[i] >= lowThreeshold_temp && media_leitura[i] < highThreeshold_temp) {
                client.print(F("<td bgcolor=\"#FF9900\"><center><h3>M<span>&#233;</span>dia : "));
              }
              else if (media_leitura[i] >= highThreeshold_temp) {
                client.print(F("<td bgcolor=\"red\"><center><h3>M<span>&#233;</span>dia : "));
              }
              client.print(media_leitura[i]);
              client.print(F(" <span>&#186;</span>C </h3></center></td>"));
              client.println();
            }
            client.println(F("</table>"));
            //aqui, imprime-se a média geral da sala
            //          atualiza_media_geral();
            //definindo cor de aviso da temperatura média geral da sala
            if (media_geral < lowlowThreeshold_temp) {
              client.print(F("<h2 style=\"text-align:center;\"><font size=\"6\" color=\"#FF9900\"><strong>"));
            }
            else if (media_geral < lowThreeshold_temp && media_geral >= lowlowThreeshold_temp) {
              client.print(F("<h2 style=\"text-align:center;\"><font size=\"6\" color=\"green\"><strong>"));
            }
            else if (media_geral >= lowThreeshold_temp && media_geral < highThreeshold_temp) {
              client.print(F("<h2 style=\"text-align:center;\"><font size=\"6\" color=\"#FF9900\"><strong>")); //#ff9900 é a melhor cor para se enxergar um tom amarelo alaranjado
            }
            else if (media_geral >= highThreeshold_temp) {
              client.print(F("<h2 style=\"text-align:center;\"><font size=\"6\" color=\"red\"><strong>"));
            }
            client.println(F("Temperatura M<span>&#233;</span>dia Geral da Sala : "));
            client.print(media_geral);
            client.print(F(" <span>&#186;</span>C\n </strong></font></h2>"));
            //            delay(2);
          }


          if (real_num_hum_sensors) { //se existirem sensores de umidade, as informações serão enviadas
            //aqui, percorre-se todos os sensores de umidade, imprimindo os valores de temperatura e umidade associados
            client.println(F("<h2 style=\"text-align:center;\">UMIDADE</h2>"));
            client.print(F("<table border=\"1\" align=center>"));
            for (byte i = 0; i < real_num_hum_sensors; i++) {
              //            getTemperature(*humidity_sensors[i]);
              //            getHumidity(*humidity_sensors[i]);
              client.print(F("<tr>"));
              client.print(F("<th bgcolor=\"white\"><center><h3>Sensor de umidade "));
              client.print(i);
              client.print(F("</h3></center></th>"));
              //definindo cor de aviso da umidade
              if (umidade[i] <= lowlowThreeshold_humidity) {
                client.print(F("<td bgcolor=\"red\"><center><h3>"));
              }
              else if (umidade[i] <= lowThreeshold_humidity && umidade[i] > lowlowThreeshold_humidity) {
                client.print(F("<td bgcolor=\"#FF9900\"><center><h3>"));
              }
              else if (umidade[i] > lowThreeshold_humidity && umidade[i] <= highThreeshold_humidity) {
                client.print(F("<td bgcolor=\"gree\"><center><h3>"));
              }
              else if (umidade[i] > highThreeshold_humidity && umidade[i] < highhighThreeshold_humidity) {
                client.print(F("<td bgcolor=\"#FF9900\"><center><h3>"));
              }
              else if (umidade[i] >= highhighThreeshold_humidity) {
                client.print(F("<td bgcolor=\"red\"><center><h3>"));
              }
              client.print(umidade[i]);
              client.print(F(" <span>&#37;</span>\n</h3>"));
              client.print(F("</center></td>"));
              //definindo cor de aviso da temperatura no sensor de umidade
              if (temp_sensor_umidade[i] < lowlowThreeshold_temp) {
                client.print(F("<td bgcolor=\"#FF9900\"><center><h3>"));
              }
              else if (temp_sensor_umidade[i] < lowThreeshold_temp && temp_sensor_umidade[i] >= lowlowThreeshold_temp) {
                client.print(F("<td bgcolor=\"gree\"><center><h3>"));
              }
              else if (temp_sensor_umidade[i] >= lowThreeshold_temp && temp_sensor_umidade[i] < highThreeshold_temp) {
                client.print(F("<td bgcolor=\"#FF9900\"><center><h3>"));
              }
              else if (temp_sensor_umidade[i] >= highThreeshold_temp) {
                client.print(F("<td bgcolor=\"red\"><center><h3>"));
              }
              client.print(temp_sensor_umidade[i]);
              client.println(F("<span>&#186;</span>C \n</h3>"));
              client.print(F("</center></td>"));
              client.print(F("</tr>"));
            }
            client.print(F("</table>"));
            //imprimir média geral da umidade
            //          atualiza_media_umidade();
            //definindo cor de aviso da umidade média geral
            if (media_umidade <= lowlowThreeshold_humidity) {
              client.print(F("<h2 style=\"text-align:center;\"><font size=\"6\" color=\"red\"><strong>"));
            }
            else if (media_umidade <= lowThreeshold_humidity && media_umidade > lowlowThreeshold_humidity) {
              client.print(F("<h2 style=\"text-align:center;\"><font size=\"6\" color=\"#FF9900\"><strong>"));
            }
            else if (media_umidade > lowThreeshold_humidity && media_umidade <= highThreeshold_humidity) {
              client.print(F("<h2 style=\"text-align:center;\"><font size=\"6\" color=\"green\"><strong>"));
            }
            else if (media_umidade > highThreeshold_humidity && media_umidade < highhighThreeshold_humidity) {
              client.print(F("<h2 style=\"text-align:center;\"><font size=\"6\" color=\"#FF9900\"><strong>"));
            }
            else if (media_umidade >= highhighThreeshold_humidity) {
              client.print(F("<h2 style=\"text-align:center;\"><font size=\"6\" color=\"red\"><strong>"));
            }
            client.println(F("Umidade Relativa M<span>&#233;</span>dia Geral : "));
            client.print(media_umidade);
            client.print(F(" <span>&#37;</span>\n</strong></font></h2>"));
          }

          if (NUMBER_INUNDACAO) {
            //aqui, percorre-se todos os sensores de umidade, imprimindo os valores de temperatura e umidade associados
            client.println(F("<h2 style=\"text-align:center;\">----INUNDA&Ccedil;&Atilde;O----</h2>"));
            client.print(F("<table border=\"1\" align=center>"));
            for (byte i = 0; i < NUMBER_INUNDACAO; i++) {
              client.print(F("<tr>"));
              client.print(F("<th bgcolor=\"white\"><center><h3>Sensor de inunda&ccedil;&atilde;o "));
              client.print(i);
              client.print(F("</h3></center></th>"));
              if (valor_inundacao[i] <= Threeshold_inundacao && valor_inundacao[i] > NO_INUNDACAO) {
                client.print(F("<td bgcolor=\"#FF9900\"><center><h3>"));
              }
              else if (valor_inundacao[i] > Threeshold_inundacao) {
                client.print(F("<td bgcolor=\"red\"><center><h3>"));
              }
              else if (valor_inundacao[i] == NO_INUNDACAO) {
                client.print(F("<td bgcolor=\"gree\"><center><h3>"));
              }
              client.print(valor_inundacao[i]);
              client.print(F(" <span>&#37;</span>\n</h3>"));
              client.print(F("</center></td>"));
              client.print(F("</tr>"));
            }

            client.print(F("</table>"));
          }

          if (NUMBER_SOUND) {
            //aqui, percorre-se todos os sensores de umidade, imprimindo os valores de temperatura e umidade associados
            client.println(F("<h2 style=\"text-align:center;\">----SOM----</h2>"));
            client.print(F("<table border=\"1\" align=center>"));
            for (byte i = 0; i < NUMBER_SOUND; i++) {
              client.print(F("<tr>"));
              client.print(F("<th bgcolor=\"white\"><center><h3>Sensor de som "));
              client.print(i);
              client.print(F("</h3></center></th>"));
              if (db[i] > lowThreeshold_sound && db[i] <= highThreeshold_sound) {
                client.print(F("<td bgcolor=\"#FF9900\"><center><h3>"));
              }
              else if (db[i] > highThreeshold_sound) {
                client.print(F("<td bgcolor=\"red\"><center><h3>"));
              }
              else if (db[i] <= lowThreeshold_sound) {
                client.print(F("<td bgcolor=\"gree\"><center><h3>"));
              }
              client.print(db[i]);
              client.print(F(" <span>&#37;</span>\n</h3>"));
              client.print(F("</center></td>"));
              client.print(F("</tr>"));
            }

            client.print(F("</table>"));
          }

          if (NUMBER_VIBRATION) {
            //aqui, percorre-se todos os sensores de umidade, imprimindo os valores de temperatura e umidade associados
            client.println(F("<h2 style=\"text-align:center;\">----VIBRA&Ccedil;&Atilde;O----</h2>"));
            client.print(F("<table border=\"1\" align=center>"));
            for (byte i = 0; i < NUMBER_VIBRATION; i++) {
              client.print(F("<tr>"));
              client.print(F("<th bgcolor=\"white\"><center><h3>Sensor de vibra&ccedil;&atilde;o "));
              client.print(i);
              client.print(F("</h3></center></th>"));
              if (vibration[i] > lowThreeshold_vibration && vibration[i] <= highThreeshold_vibration) {
                client.print(F("<td bgcolor=\"#FF9900\"><center><h3>"));
              }
              else if (vibration[i] > highThreeshold_vibration) {
                client.print(F("<td bgcolor=\"red\"><center><h3>"));
              }
              else if (vibration[i] <= lowThreeshold_vibration) {
                client.print(F("<td bgcolor=\"gree\"><center><h3>"));
              }
              client.print(vibration[i]);
              client.print(F(" mm&#47;s<sup>2</sup>\n</h3>"));
              client.print(F("</center></td>"));
              client.print(F("</tr>"));
            }

            client.print(F("</table>"));
          }


          if (NUMBER_LDRs >= 1 && !LUX_SENSOR) { //se existirem sensores de umidade, as informações serão enviadas
            //aqui, percorre-se todos os sensores de umidade, imprimindo os valores de temperatura e umidade associados
            client.println(F("<h2 style=\"text-align:center;\">----ILUMINA&Ccedil;&Atilde;O----</h2>"));
            client.print(F("<table border=\"1\" align=center>"));
            for (byte i = 0; i < NUMBER_LDRs; i++) {
              client.print(F("<tr>"));
              client.print(F("<th bgcolor=\"white\"><center><h3>Sensor de ilumina&ccedil;&atilde;o "));
              client.print(i);
              client.print(F("</h3></center></th>"));
              if (percent_ldr[i] < lowThreeshold_light) {
                client.print(F("<td bgcolor=\"#3333FF\"><center><h3>&nbsp;&nbsp;"));
              }
              else if (percent_ldr[i] >= lowThreeshold_light && percent_ldr[i] < highThreeshold_light) {
                client.print(F("<td bgcolor=\"#FF9900\"><center><h3>&nbsp;&nbsp;"));
              }
              else if (percent_ldr[i] > highThreeshold_light) {
                client.print(F("<td bgcolor=\"gree\"><center><h3>&nbsp;&nbsp;"));
              }
              client.print(percent_ldr[i]);
              client.print(F(" <span>&#37;</span>\n&nbsp;&nbsp;</h3>"));
              client.print(F("</center></td>"));
              client.print(F("</tr>"));
            }
            client.print(F("</table>"));
          }
          else if (NUMBER_LDRs >= 1 && LUX_SENSOR) { //se existirem sensores de umidade, as informações serão enviadas
            //aqui, percorre-se todos os sensores de umidade, imprimindo os valores de temperatura e umidade associados
            client.println(F("<h2 style=\"text-align:center;\">----ILUMINA&Ccedil;&Atilde;O----</h2>"));
            client.print(F("<table border=\"1\" align=center>"));
            for (byte i = 0; i < NUMBER_LDRs; i++) {
              client.print(F("<tr>"));
              client.print(F("<th bgcolor=\"white\"><center><h3>Sensor de ilumina&ccedil;&atilde;o "));
              client.print(i);
              client.print(F("</h3></center></th>"));
              if (percent_ldr[i] < lowThreeshold_light) {
                client.print(F("<td bgcolor=\"#3333FF\"><center><h3>&nbsp;&nbsp;"));
              }
              else if (percent_ldr[i] >= lowThreeshold_light && percent_ldr[i] < highThreeshold_light) {
                client.print(F("<td bgcolor=\"#FF9900\"><center><h3>&nbsp;&nbsp;"));
              }
              else if (percent_ldr[i] > highThreeshold_light) {
                client.print(F("<td bgcolor=\"gree\"><center><h3>&nbsp;&nbsp;"));
              }
              client.print(percent_ldr[i]);
              client.print(F(" <span>&#37;</span>\n&nbsp;&nbsp;</h3>"));
              client.print(F("</center></td>"));


              client.print(F("</tr>"));
            }

            client.print(F("<tr>"));
            client.print(F("<th bgcolor=\"white\"><center><h3>Sensor de ilumina&ccedil;&atilde;o em lux </h3></center></th>"));

            if (lux < lowThreeshold_lux) {
              client.print(F("<td bgcolor=\"#3333FF\"><center><h3>&nbsp;&nbsp;"));
            }
            else if (lux >= lowThreeshold_lux && lux < highThreeshold_lux) {
              client.print(F("<td bgcolor=\"#FF9900\"><center><h3>&nbsp;&nbsp;"));
            }
            else if (lux > highThreeshold_lux) {
              client.print(F("<td bgcolor=\"gree\"><center><h3>&nbsp;&nbsp;"));
            }
            client.print(lux);
            client.print(F(" lx\n&nbsp;&nbsp;</h3>"));
            client.print(F("</center></td>"));

            client.print(F("</tr>"));


            client.print(F("</table>"));
          }
          else if (NUMBER_LDRs == 0 && LUX_SENSOR) {
            client.println(F("<h2 style=\"text-align:center;\">----ILUMINA&Ccedil;&Atilde;O----</h2>"));
            client.print(F("<table border=\"1\" align=center>"));
            client.print(F("<tr>"));
            client.print(F("<th bgcolor=\"white\"><center><h3>Sensor de ilumina&ccedil;&atilde;o em lux </h3></center></th>"));

            if (lux < lowThreeshold_lux) {
              client.print(F("<td bgcolor=\"#3333FF\"><center><h3>&nbsp;&nbsp;"));
            }
            else if (lux >= lowThreeshold_lux && lux < highThreeshold_lux) {
              client.print(F("<td bgcolor=\"#FF9900\"><center><h3>&nbsp;&nbsp;"));
            }
            else if (lux > highThreeshold_lux) {
              client.print(F("<td bgcolor=\"gree\"><center><h3>&nbsp;&nbsp;"));
            }
            client.print(lux);
            client.print(F(" lx\n&nbsp;&nbsp;</h3>"));
            client.print(F("</center></td>"));

            client.print(F("</tr>"));
            client.print(F("</table>"));
          }
          else {}

          if (BMP_SENSOR) {
            client.println(F("<h2 style=\"text-align:center;\">INFORMA&Ccedil;&Otilde;S ADICIONAIS</h2>"));
            client.print(F("<table border=\"1\" align=center>"));


            client.print(F("<tr>"));
            client.print(F("<th bgcolor=\"white\"><center><h3>Press&atilde;o Interna</h3></center></th>"));
            client.print(F("<td bgcolor=\"white\"><center><h3>"));
            client.print(pressure);
            client.print(F(" Pa\n</h3>"));
            client.print(F("</center></td>"));
            client.print(F("</tr>"));


            client.print(F("<tr>"));
            client.print(F("<th bgcolor=\"white\"><center><h3>Altitude</h3></center></th>"));
            client.print(F("<td bgcolor=\"white\"><center><h3>"));
            client.print(altitude);
            client.print(F(" m\n</h3>"));
            client.print(F("</center></td>"));
            client.print(F("</tr>"));

            client.print(F("<tr>"));
            client.print(F("<th bgcolor=\"white\"><center><h3>Temperatura Interna</h3></center></th>"));
            client.print(F("<td bgcolor=\"white\"><center><h3>"));
            client.print(temp_interna);
            client.println(F("<span>&#186;</span>C \n</h3>"));
            client.print(F("</center></td>"));
            client.print(F("</tr>"));

            client.print(F("</table>"));
          }


          //          delay(2);
          client.println(F("</body>"));
          client.println(F("</html>"));

          break;
        }

        if ( c == '\n') {
          // iniciando uma nova linha
          BlankLine = true;
        }
        else if (c != '\r') {
          // a linha atual tem um caractere nela
          BlankLine = false;
        }
      }
    }
    // Socket = client.getSocketNumber();
    client.stop();
    client = 0;

    //    msg_output = "";
    //    currentLine = "";
    //    user[0] = "";
    //    user[1] = "";
    //    parameter_name[0] = "";
    //    parameter_name[1] = "";

    delete(msg_output);
    delete(currentLine);
    delete(user[0]);
    delete(user[1]);
    delete(parameter_name[0]);
    delete(parameter_name[1]);
    //    Serial.print("freeMemory()=");
    //    Serial.println(freeMemory());
  }
  /*******FIM************************************************WEB SERVER*********************************************************************************************************************************************************/



  /*
     USE 30 SECONDS OF DELAY TO telnet'S REQUESTS
     OF VALUES. THIS WILL DECREASE THE CHANCE OF
     HAVING CONNECTIONS PROBLEMS. BESIDES THAT, IT
     IS THE BEST INTERVAL VALUE AMBIENT MONITORING.
  */


  /*******INÍCIO************************************************telnet SERVER*****************************************************************************************************************************************************/
  // wait for a new client:
  client_telnet.flush();
  client_telnet = server_telnet.available();
  if (client_telnet) {
    //    Serial.println("!!!!!!!!!!!!!");
    while (client_telnet.connected()) {

      if (client_telnet.available()) {
        //        time_inicio = millis();
        // lê os bytes recebidos do cliente
        command[param] = client_telnet.read();
        //aqui, testa-se se o comando chegou ao tamanho máximo ou se houve uma quebra de linha (tecla enter apertada, por exemplo), indicando um envio de comando finalizado
        if (param == max_command_size || command[param] == CR || command[param] == LF) {
          generateAction(command, param);

          //        for (byte i = 0; i < param; i++) {
          //          Serial.print(command[i]);
          //        }
          //        Serial.println();
          //          time_fim = millis() - time_inicio;
          //          Serial.print("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!Tempo telnet: ");
          //          Serial.println(time_fim);
          cleanWord(command, param);
          param = 0;
          client_telnet.flush();
          client_telnet = server_telnet.available();
        }
        else {
          param++;  //caso o comando ainda não esteja completo, aumenta-se "param" e continua-se escutando o resto do comando
        }
      }
    }
  }
  /*******FIM************************************************WEB SERVER***********************************************************************************************************************************************************/


  /*******INÍCIO************************************************SNMP*****************************************************************************************************************************************************/
  // listen/handle for incoming SNMP requests
  Agentuino.listen();
  //
  // sysUpTime - The time (in hundredths of a second) since
  // the network management portion of the system was last
  // re-initialized.
  if ( millis() - prevMillis > 1000 ) {
    // increment previous milliseconds
    prevMillis += 1000;
    //
    // increment up-time counter
    locUpTime += 100;
  }
  /*******FIM************************************************SNMP***********************************************************************************************************************************************************/


  //    Serial.print("freeMemory()=");
  //    Serial.println(freeMemory()); //Verificar o uso de memória após cada iteração do Loop
}
