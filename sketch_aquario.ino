#include <Arduino.h>
#include <WiFi.h>
#include <ArduinoJson.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>
#include <OneWire.h>
#include <DallasTemperature.h>

 
/* Credenciais wi-fi */
#define ssid_wifi "XXXX"
#define password_wifi "XXXX"

#define TEMPO_ENTRE_CHECAGEM_DE_MENSAGENS   50 //ms
 
/* Token de acesso Telegram */
#define token_acesso_telegram "1358418486:AAG3Yu45PiAth4S5nSoufuUdKAmF6op7Xo8"
#define chat_id_telegram "934722418"
 
/* Definições das mensagens possíveis de serem recebidas */
#define CMD_CLIMA               "Temperatura"
#define CMD_LIGA_RELE_1         "L1"
#define CMD_LIGA_RELE_2         "L2"
#define CMD_LIGA_RELE_3         "L3"
#define CMD_LIGA_RELE_4         "L4"
#define CMD_DESLIGA_RELE_1      "D1"
#define CMD_DESLIGA_RELE_2      "D2"
#define CMD_DESLIGA_RELE_3      "D3"
#define CMD_DESLIGA_RELE_4      "D4"
#define CMD_LIGA_TODOS_RELES    "Lt"
#define CMD_DESLIGA_TODOS_RELES "Dt"
 
/* GPIOs usados */
#define GPIO_RELE1              14
#define GPIO_RELE2              27
#define GPIO_RELE3              13
#define GPIO_RELE4              12
#define GPIO_LED_VERMELHO       21
#define GPIO_LED_VERDE          19
#define GPIO_BOIA1              23
#define GPIO_BOIA2              18
#define GPIO_BOIA3              4
#define GPIO_BOIA4              5
#define GPIO_TEMPERATURA        22


/* Variáveis e objetos globais */
WiFiClientSecure client;
UniversalTelegramBot bot(token_acesso_telegram, client);

unsigned long timestamp_checagem_msg_telegram = 0;
int num_mensagens_recebidas_telegram = 0;
String resposta_msg_recebida;
 
/* Prototypes */
void init_wifi(void);
void conecta_wifi(void);
void verifica_conexao_wifi(void);
unsigned long diferenca_tempo(unsigned long timestamp_referencia);
String trata_mensagem_recebida(String msg_recebida);

OneWire oneWire(GPIO_TEMPERATURA);

DallasTemperature sensors(&oneWire);

float Celsius = 0;
float Fahrenheit = 0;
int boia_1 = 0;
int boia_2 = 0;
int boia_3 = 0;
int boia_4 = 0;

void init_wifi(void) 
{
    Serial.println("------WI-FI -----");
    Serial.print("Conectando-se a rede: ");
    Serial.println(ssid_wifi);
    Serial.println("Aguarde...");    
    conecta_wifi();
}
 
void conecta_wifi(void) 
{
    /* Se ja estiver conectado, nada é feito. */
    if (WiFi.status() == WL_CONNECTED)
        return;
 
    /* refaz a conexão */
    WiFi.begin(ssid_wifi, password_wifi);
     
    while (WiFi.status() != WL_CONNECTED) 
    {        
        vTaskDelay( 100 / portTICK_PERIOD_MS );
        Serial.print(".");
    }
   
    Serial.println();
    Serial.print("Conectado com sucesso a rede wi-fi ");
    Serial.println(ssid_wifi);
    Serial.print("IP: ");
    Serial.println(WiFi.localIP());
}
 
void verifica_conexao_wifi(void)
{
    conecta_wifi(); 
}
 
unsigned long diferenca_tempo(unsigned long timestamp_referencia)
{
    return (millis() - timestamp_referencia);
}
 
String trata_mensagem_recebida(String msg_recebida)
{
    String resposta = "";
    bool comando_valido = false;
    float temperatura_lida = 0.0;
    float umidade_lida = 0.0;
 

    if (msg_recebida.equals(CMD_CLIMA))
    {
      sensors.requestTemperatures();
      Celsius = sensors.getTempCByIndex(0);
      Celsius = Celsius + 2;
      Fahrenheit = sensors.toFahrenheit(Celsius);
      resposta = "Informacoes do clima: temperatura = "+
                   String(Celsius)+
                   "C e = "+
                   String(Fahrenheit)+" F %";   
                    
      comando_valido = true;   
    }
 
    if (msg_recebida.equals(CMD_LIGA_RELE_1))
    {
        digitalWrite(GPIO_RELE1, HIGH);        
        resposta = "Liguei a Bomba dos Kinguios";
        comando_valido = true;
    }
 
    if (msg_recebida.equals(CMD_LIGA_RELE_2))
    {
        digitalWrite(GPIO_RELE2, HIGH);
        resposta = "Liguei a Bomba dos Acarás";
        comando_valido = true;
    }
 
    if (msg_recebida.equals(CMD_LIGA_RELE_3))
    {
        digitalWrite(GPIO_RELE3, HIGH);
        resposta = "Liguei a luz";
        comando_valido = true;
    }
 
    if (msg_recebida.equals(CMD_LIGA_RELE_4))
    {
        digitalWrite(GPIO_RELE4, HIGH);
        resposta = "Rele 4 ligado";
        comando_valido = true;
    }
 
    if (msg_recebida.equals(CMD_DESLIGA_RELE_1))
    {
        digitalWrite(GPIO_RELE1, LOW);
        resposta = "Desliguei a Bomba dos Kinguios";
        comando_valido = true;
    }
 
    if (msg_recebida.equals(CMD_DESLIGA_RELE_2))
    {
        digitalWrite(GPIO_RELE2, LOW);
        resposta = "Desliguei a Bomba dos Acarás";
        comando_valido = true;
    }
 
    if (msg_recebida.equals(CMD_DESLIGA_RELE_3))
    {
        digitalWrite(GPIO_RELE3, LOW);
        resposta = "Desliguei a luz";
        comando_valido = true;
    }
 
    if (msg_recebida.equals(CMD_DESLIGA_RELE_4))
    {
        digitalWrite(GPIO_RELE4, LOW);
        resposta = "Rele 4 desligado";
        comando_valido = true;
    }
 
    if (msg_recebida.equals(CMD_LIGA_TODOS_RELES))
    {
        digitalWrite(GPIO_RELE1, HIGH);
        digitalWrite(GPIO_RELE2, HIGH);
        digitalWrite(GPIO_RELE3, HIGH);
        digitalWrite(GPIO_RELE4, HIGH);
         
        resposta = "Todos os reles foram ligados";
        comando_valido = true;
    }
 
    if (msg_recebida.equals(CMD_DESLIGA_TODOS_RELES))
    {
        digitalWrite(GPIO_RELE1, LOW);
        digitalWrite(GPIO_RELE2, LOW);
        digitalWrite(GPIO_RELE3, LOW);
        digitalWrite(GPIO_RELE4, LOW);

        resposta = "Todos os reles foram desligados";
        comando_valido = true;
    }
 
    if (comando_valido == false)
        resposta = "Comando invalido: "+msg_recebida;      
  
    return resposta;
}
 
void setup() 
{
    Serial.begin(115200);
    sensors.begin();
    /* Configura GPIOs de controle dos relés e deixa 
       todos os relés desacionados */
    pinMode(GPIO_RELE1, OUTPUT);
    pinMode(GPIO_RELE2, OUTPUT);
    pinMode(GPIO_RELE3, OUTPUT);
    pinMode(GPIO_RELE4, OUTPUT);
    pinMode(GPIO_LED_VERDE, OUTPUT);
    pinMode(GPIO_LED_VERMELHO, OUTPUT);   


    digitalWrite(GPIO_RELE1, HIGH);
    digitalWrite(GPIO_RELE2, HIGH);
    digitalWrite(GPIO_RELE3, HIGH);
    digitalWrite(GPIO_RELE4, HIGH);
    pinMode(GPIO_BOIA1, INPUT_PULLUP);
    pinMode(GPIO_BOIA2, INPUT_PULLUP);
    pinMode(GPIO_BOIA3, INPUT_PULLUP);
    pinMode(GPIO_BOIA4, INPUT_PULLUP);

 
    /* Inicializa a conexão wi-fi */
    init_wifi();
 
    /* Inicializa timestamp de checagem de mensagens recebidas via Telegram */
    timestamp_checagem_msg_telegram = millis();
}
 
void loop() 
{
    conecta_wifi(); 
    int i;
    read_boia();
    /* Garante que o wi-fi está conectado */
    verifica_conexao_wifi();   
 
    /* Verifica se é hora de checar por mensagens enviadas ao bot Telegram */
    if ( diferenca_tempo(timestamp_checagem_msg_telegram) >= TEMPO_ENTRE_CHECAGEM_DE_MENSAGENS)
    {
        /* Verifica se há mensagens a serem recebidas */
        num_mensagens_recebidas_telegram = bot.getUpdates(bot.last_message_received + 1);
 
        if (num_mensagens_recebidas_telegram > 0)
        {
            Serial.print("[BOT] Mensagens recebidas: ");
            Serial.println(num_mensagens_recebidas_telegram);
        }
         
        /* Recebe mensagem a mensagem, faz tratamento e envia resposta */
        while(num_mensagens_recebidas_telegram) 
        {
            for (i=0; i<num_mensagens_recebidas_telegram; i++) 
            {                
                resposta_msg_recebida = "";
                resposta_msg_recebida = trata_mensagem_recebida(bot.messages[i].text);
                bot.sendMessage(bot.messages[i].chat_id, resposta_msg_recebida, "");
                Serial.println(bot.messages[i].chat_id);

        }
             
            num_mensagens_recebidas_telegram = bot.getUpdates(bot.last_message_received + 1);
        }
         
        /* Reinicializa timestamp de checagem de mensagens recebidas via Telegram */
        timestamp_checagem_msg_telegram = millis();
    }
 }

void read_boia()                     //função para leitura do sensor e acionamento da bomba
{  
   if(!digitalRead(GPIO_BOIA1) && boia_1 == 0)
   {
      digitalWrite(GPIO_RELE1, LOW);
      digitalWrite(GPIO_LED_VERDE, LOW);
      digitalWrite(GPIO_LED_VERMELHO, HIGH);
      
      Serial.println("desliga bomba  ");
      bot.sendMessage(chat_id_telegram, "A Bomba dos kinguios foi desligada", "");
      boia_1 = 1;
   }
   else if(digitalRead(GPIO_BOIA1) && boia_1 == 1) {                              
      digitalWrite(GPIO_RELE1, HIGH);
      digitalWrite(GPIO_LED_VERDE, HIGH);
      digitalWrite(GPIO_LED_VERMELHO, LOW);
      Serial.println("liga bomba  ");
      bot.sendMessage(chat_id_telegram, "A Bomba dos kinguios foi ligada", "");
      boia_1 = 0;
   }

   if(!digitalRead(GPIO_BOIA2) && boia_2 == 0)
   {
      digitalWrite(GPIO_RELE2, LOW);
      digitalWrite(GPIO_LED_VERDE, LOW);
      digitalWrite(GPIO_LED_VERMELHO, HIGH);
      
      Serial.println("desliga bomba  ");
      bot.sendMessage(chat_id_telegram, "A Bomba dos acarás foi desligada", "");
      boia_2 = 1;
    }
    else if(digitalRead(GPIO_BOIA2) && boia_2 == 1) {                                
      digitalWrite(GPIO_RELE2, HIGH);
      digitalWrite(GPIO_LED_VERDE, HIGH);
      digitalWrite(GPIO_LED_VERMELHO, LOW);
      Serial.println("liga bomba  ");
      bot.sendMessage(chat_id_telegram, "A Bomba dos acarás foi ligada", "");
      boia_2 = 0;
   }

   if(digitalRead(GPIO_BOIA3) && boia_3 == 0)
   {
      digitalWrite(GPIO_RELE3, LOW);     
      Serial.println("liga a bomba de reposição");
      bot.sendMessage(chat_id_telegram, "liguei a bomba de reposição dos acarás", "");
      boia_3 = 1;
    }
    else if(!digitalRead(GPIO_BOIA3) && boia_3 == 1) {                                
      digitalWrite(GPIO_RELE3, HIGH);
      boia_3 = 0;
   }

   if(digitalRead(GPIO_BOIA4) && boia_4 == 0)
   {
      digitalWrite(GPIO_RELE4, LOW);    
     Serial.println("liga a bomba de reposição");
      bot.sendMessage(chat_id_telegram, "liguei a bomba de reposição dos kinguios", ""); 
      boia_4 = 1;
    }
    else if(!digitalRead(GPIO_BOIA4) && boia_4 == 1) {                                
      digitalWrite(GPIO_RELE4, HIGH);
      boia_4 = 0;
   }
    
    
}
