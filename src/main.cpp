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
    "temperatura" : 37,
    "isSingle" : true
    "hobbies" : [
      "Música",
      "Jogos",
      "Animes",
      "Livros"
      ]
  }
)";

  JsonDocument doc;
  DeserializationError erroJson = deserializeJson(doc, resposta);

  if(erroJson)
  {
    Serial.printf("Erro ao deserializar: %s\n\r", erroJson.c_str());
    return;
  }

  //* isNull() não se usa mais, usamos is<TipoDaVariavel>() que além de verificar se a variável existe,
  //* ainda verifica se o valor está no tipo certo
  String nome = "não definido";
  if(doc["nome"].is<const char*>())
    nome = doc["nome"].as<String>();


  //* verificar a existencia da chave, caso não existir passasse um valor negativo
  //* para indicar a inexistencia, pois 0 neste caso é um valor impossivel
  int idade = doc["idade"] | -1;
  //* verificar a existencia da chave e um valor padrão caso não existir
  String naturalidade = doc["naturalidade"] | "não definido";

  //? normalmente uma variavel booleana tem como valor padrão false
  bool estadoCivil = doc["isSingle"] | false;

  int temperatura;
  temperatura = 36; //? exemplo de variavel existente que já continha um valor anterior

  if(doc["temperatura"].is<int>())
    temperatura = doc["temperatura"];

  //todo TRATAMENTO DE VETORES
  const char* hobbies[10];
  size_t quantidade = 0;
  int tamanhoVetor = sizeof(hobbies) / sizeof(hobbies[0]);
  if(doc["hobbies"].is<JsonArray>())
  {
    for(JsonVariantConst v : doc["hobbies"]. as<JsonArrayConst>())
    {
      if (quantidade >= tamanhoVetor) break; //* limitando a quantidade de itens dentro do meu vetor
      hobbies[quantidade] = v | "";
      quantidade ++;
    }
  }

  Serial.printf("\nMeu nome é %s, tenho %s anos, nasci em %s e sou %s", 
    nome.c_str(),
    idade > 0? String(idade).c_str() : "Não definido",
    naturalidade.c_str(),
    estadoCivil ? "solteiro" : "comprometido"
  );
}

void loop() 
{
  
}