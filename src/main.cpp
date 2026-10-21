#include <Arduino.h>
#include <ArduinoJson.h>

void setup() 
{
  Serial.begin(115200);
  //? raw string literal (string literal "crua") -> R"("texto aqui")";
  const char* resposta = R"(
  {
    "nome" : "Bruno",
    "idade" : 19,
    "naturalidade" : "Sao Bernardo do Campo/SP",
    "isSingle" : true
  }
  )";

  JsonDocument doc;
  DeserializationError erroJson = deserializeJson(doc, resposta);

  if(erroJson)
  {
    Serial.printf("Erro: %s\n\r", erroJson.c_str());
    return;
  }
  const char* nome = doc["nome"];
  int idade = doc["idade"];
  const char* naturalidade = doc["naturalidade"];
  bool estadoCivil = doc["isSingle"];

  Serial.printf("\nMeu nome é: %s, tenho %d anos, nasci em %s e sou %s", 
                nome,
                idade,
                naturalidade,
                estadoCivil ? "solteiro" : "comprometido"
              );
}

void loop() 
{
  
}