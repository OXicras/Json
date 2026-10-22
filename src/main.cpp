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
      "Livros"],
    "endereco" : {
      "rua" : "Niteroi",
      "numero" : "10",
      "coordenadas" : {
        "lat" : -23.62,
        "lon" : -46.55
      }
    }
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
    temperatura = doc["temperatura"].as<int>();

  //todo TRATAMENTO DE VETORES
  const char* hobbies[10];
  size_t quantidade = 0;
  //* size_t é um tipo inteiro usado parar representar tamanhos e quantidade
  //* de bytes de memória, números de elementos e índices de vetor, é o tipo que
  //* a funcão sizeof() retorna. Não tem sinal (unsigned) e a largura depende
  //* da arquitetura mas tem o tamanho necessário para representar qualquer objeto
  //* na memória da maquina em questão.

  //* sizeof retorna o tamanho do vetor em bytes porém cada posicão pode ter mais que um byte
  //* por isso dividimos o tamanho total pelo tamanho de um item de vetor existente
  //* como sempre existe a posicão [0], usamos ela como padrão.
  //! sizeof() em printf usar %zu
  size_t tamanhoVetor = sizeof(hobbies) / sizeof(hobbies[0]);


  int numeros[] = {10, 20, 30, 40};
  //* for range-based for loop | for baseado em intervalos
  //* informalmente é chamado de foreach (para cada) pq é o nome
  //* do mesmo recurso em outras linguagens.
  for(int n : numeros)
  {
    Serial.println(n);
  }

  //TODO Esse for de cima faz a mesma coisa que o de baixo

  size_t tamanhoVetorExmp = sizeof(numeros) / sizeof(numeros[0]);
  //? Esse cálculo serve para ter a quantidade de itens dentro de um vetor
  //? sizeof(numeros) = 16 bytes / sizeof(numeros[0]) = 4 bytes   =   4 itens no vetor
  //? tamanho do vetor           / tamanho da posicão que sempre existe em um vetor

  for(int i = 0; i < tamanhoVetorExmp; i++)
  {
    int n = numeros[i];
    Serial.println(n);
  }

  
  if(doc["hobbies"].is<JsonArray>())
  {
    for(JsonVariantConst v : doc["hobbies"].as<JsonArrayConst>())
    {
      if (quantidade >= tamanhoVetor) break; //* evita estouro de vetor
      hobbies[quantidade++] = v | "Não definido";
    }
  }

  //* JsonVariantConst representa um valor Json qualquer: texto, número, booleano, null, objeto ou array
  //* O tipo só é conhecido em tempo de execucão, por isso é necessário verificar com is<T>() e converter
  //* com as<T>(). É usado quando ainda não se sabe ou não importa o que está sendo guardado ali.

  //* JsonArrayConst representa um array Json, e oferece operacões de array:
  //* array.size()        retorna a quantidade de itens
  //* array.isNull()      retorna true, senão aponta para um array
  //* array.nesting()     retorna a profundidade de alinhamento dentro do array
  //* array[i] = item na posicão i ex.: array[1].as<const char*>() retornaria um texto
  //* array[i] | default retorna item ou valor padrão se null
  //* array[i].is<T>() retorna true para o teste de tipo T
  //* Acessar um indice fora do limite do array não trava, retorna valor nulo (0, false, nullptr)

  //* Em casos de numeros ou booleanos é possivel copiar um array para outro (não funciona com strings)
  //* int notas[10];
  //* size_t quantidadeItensCopiados = copyArray(doc["numeros"].as<JsonArrayConst>(),notas);
  //* quando o vetor destino enche não trava e devolve quantos itens foram copiados

  Serial.printf("\nMeu nome é %s, tenho %s anos, nasci em %s e sou %s", 
    nome.c_str(),
    idade > -1? String(idade).c_str() : "Não definido",
    naturalidade.c_str(),
    estadoCivil ? "solteiro" : "comprometido"
  );

  //* Aninhamento
  //* Jeito 01
  const char* rua = doc["endereco"]["rua"].as<const char*>();
  float lat = doc["endereco"]["coordenadas"]["lat"] | 0.0f;

  //* Jeito 02
  JsonObjectConst endereco = doc["endereco"];
  Serial.println(endereco["rua"].as<const char*>());
  Serial.println(endereco["coordenadas"]["lat"].as<float>());

  JsonObjectConst coordenadas = doc["endereco"]["coordenadas"];
  Serial.println(coordenadas["lat"].as<float>());
  Serial.println(coordenadas["lon"] | 0.0f);
}

void loop() {}