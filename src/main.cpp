#include <Arduino.h>
#include <ArduinoJson.h>

void setup() 
{
  Serial.begin(115200);
  //? raw string literal (string literal "crua") -> R"("texto aqui")";
  const char* resposta = R"(
  {
    "nome" : "Bruno",
    "idade" : "19",
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

  //* isNull() não se usa mais, usamos is<TipoDaVariavel>() que além de verificar se a variável existe,
  //* ainda verifica se o valor está no tipo certo
  String nome = "não definido";
  if(doc["nome"].is<const char*>())
    nome = doc["nome"].as<String>();


  //* verificar a existencia da chave e um valor padrão caso não existir
  String idade = doc["idade"] | "não definido";

  String naturalidade = doc["naturalidade"] | "não definido";

  bool estadoCivil = doc["isSingle"];

  Serial.printf("\nMeu nome é %s, tenho %s anos, nasci em %s e sou %s", 
    nome.c_str(),
    idade.c_str(),
    naturalidade.c_str(),
    estadoCivil ? "solteiro" : "comprometido"
  );
}

void loop() 
{
  
}