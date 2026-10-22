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
    "isSingle" : true,
    "hobbies" : [
      "Música",
      "Jogos",
      "Animes",
      "Livros"
      ]
  }
)";

  JsonDocument doc;
  //* DeserializationError é um tipo, uma classe, do ArduínoJson que guarda um código
  //* informando que o parse (Segmentacão de chave valor) terminou
  DeserializationError erroJson = deserializeJson(doc, resposta);

  //! Possibilidades do Deserialization Error
  //* 0 ou Ok                      sucesso
  //? 1 ou EmptyInput              entrada vazia ou timeout da Stream
  //? 2 ou IncompleteInput         Json cortado antes de terminar
  //? 3 ou InvalidInput            Sintáxe inválida
  //? 4 ou NoMemory                Memória insuficiente para montar o documento
  //? 5 ou TooDeep                 Alinhamento acima do limite definido pela funcão 
  //?                              NestingLimit, definido como padrão em
  //?                              ARDUINO_DEFAULT_NESTING_LIMIT = 10; Cada {} ou []
  //?                              aberto dentro de outro, soma um nível.
  //todo                             exemplo:
  //*                                   {
  //*                                     "a" : {
  //*                                               "b" : [1,2];
  //*                                           }
  //*                                   } tem 3 níveis

  switch(erroJson.code())
  {
    case DeserializationError::Ok: break;
    case DeserializationError::EmptyInput: 
      Serial.println("JSON vazio ou expirou o tempo"); 
      return;
    case DeserializationError::InvalidInput:
      Serial.println("JSON mal formado");
      return;
    case DeserializationError::IncompleteInput:
      Serial.println("JSON incompleto");
      return;
    case DeserializationError::NoMemory:
      Serial.println("Sem memoria");
      return;
    case DeserializationError::TooDeep:
      Serial.println("Alinhamento acima do limite definido pela funcão");
      return;
    default:
      Serial.println(erroJson.c_str());
      return;
  }

  // if(erroJson)
  // {
  //   Serial.printf("Erro ao deserializar: %s\n\r", erroJson.c_str());
  //   return;
  // }

  //* isNull() não se usa mais, usamos is<TipoDaVariavel>() que além de verificar se a variável existe,
  //* ainda verifica se o valor está no tipo certo
  String nome = "não definido";
  if(doc["nome"].is<const char*>())
    nome = doc["nome"].as<String>();


  //* verificar a existencia da chave, caso não existir, passa-se um valor negativo (operador | )
  //* para indicar a inexistencia, pois 0 neste caso é um valor impossivel.

  //? O simbolo '|' (pipe) aqui não é um "OU bit a bit", a biblioteca redefine esse
  //? operador para significar "Valor padrão", a lógica é: se o valor da esquerda
  //? puder ser convertido para o tipo da direita ele é usado; se não, usa o da direita.
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
    idade > -1? String(idade).c_str() : "Não definido",
    naturalidade.c_str(),
    estadoCivil ? "solteiro" : "comprometido"
  );
}

void loop() 
{
  
}