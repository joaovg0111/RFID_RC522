## RFID RC522 API

Esta API foi desenvolvida como trabalho da disciplina de Programação de Sistemas Embarcados da UFMG –Prof. Ricardo de Oliveira Duarte –Departamento de Engenharia Eletrônica.

Autores: João Vitor Gomes Mapa da Silva | Tiago de Faria Oliveira

---

### Requisitos de Hardware e Software

Essa API foi desenvolvida para realizar comunicação entre o módulo RFID-RC522 e o microcontrolador STM32F103RBTx. 

![](https://33333.cdn.cke-cs.com/kSW7V9NHUXugvhoQeFaf/images/dc8704896b54dabcfe743ef8b075b007a68ff89343273aca.jpg)

O módulo RFID-RC522 foi desenvolvido para criar um campo eletromagnético de 13,56 MHz e se comunicar com tags RFID (tags padrão ISO 14443A).

O sensor pode se comunicar com um microcontrolador através do padrão SPI (usado no projeto dessa API) de 4 pinos com uma taxa de dados máxima de 10 Mbps. Ele também suporta comunicação sobre os protocolos I2C e UART.

Mais detalhes sobre o hardware desse módulo podem ser encotrados no [datasheet](https://www.nxp.com/docs/en/data-sheet/MFRC522.pdf). 

As imagens abaixo representam o microcontrolador utilizado para desenvolver a API, e o esquema de ligação entre os componentes.

![](https://33333.cdn.cke-cs.com/kSW7V9NHUXugvhoQeFaf/images/6b3b22aa369430e5315bc0765438ab90d1f91489c7df04bc.webp)

![](https://33333.cdn.cke-cs.com/kSW7V9NHUXugvhoQeFaf/images/211953b3e265e3f621e69e4b348d66ea3be43c5b98137096.jpeg)

Para utilizar essa biblioteca é necessário existir uma variável do tipo SPI\_HandleTypeDef inicializada como um barremento SPI. O barramento desejado pode ser selecionado com

```plaintext
void SET_HSPI(SPI_HandleTypeDef hspi)
```

---

### Funções disponíveis

#### Verifica Cartão

O objetivo dessa função é identificar se há um cartão, com tag RFID, próximo ao sensor. Ela é uma função lógica que retorna _verdadeiro_ caso houver cartão e _falso_, caso não haja contato de alguma tag com o sensor. 

retorna _true_ caso sejam iguais e false caso não.

```c
bool IsCardPresent(void)
```

#### Capturar UID do cartão

Essa função retorna o UID do cartão que estive em contato com o sensor RFID. Essa função é útil para situações em que a tag e sensor RFID são usadas para a autenticação de usuários. As catracas de controle de acesso dos prédios da Escola de Engenharia, são um exemplo de locais onde essa função tem aplicação. 

_uid_ é o endereço do UID a ser comparado.

retorna true caso sejam iguais e false caso não.

```c
bool getCardUID(uint8_t* uid)
```

#### Compara UID do cartão

Essa função recebe um ponteiro com endereço de memória de um UID ou de um array de UID's e verifica se algum deles é igual ao UID que está sendo lido através do sensor RFID.

_uid_ é o endereço do cartao que se deseja comparar.

retorna true caso sejam iguais e false caso não.

```c
bool verifyUID(uint8_t*uid)
```

#### Lê um bloco do cartão

Essa função lê um bloco do cartão. 

_blockValue_ é onde os dados armazenados serão retornados.

_blockAddr_ é o endereço do bloco que se deseja ler.

retorna _true_ para sucesso e false para fracasso.

```c
bool readMIFARE(uint8_t* blockValue,uint8_t blockAddr)
```

#### Escreve informações no cartão

Essa função é usada para escrever informações adicionais na etiqueta do cartão RFID.

data é o endereço onde o valor do bloco será escrito. 

_blockAddr_ é o endereço do bloco que se deseja ler.

não tem  retorno.

```c
void writeMIFARE(uint8_t* data,uint8_t blockAddr)
```

---

#### Referências

* [What is RFID? How It Works?](https://lastminuteengineers.com/how-rfid-works-rc522-arduino-tutorial/)
* [https://www.nxp.com/docs/en/data-sheet/MFRC522.pdf](https://www.nxp.com/docs/en/data-sheet/MFRC522.pdf)
* https://www.nxp.com/docs/en/application-note/AN10927.pdf
* [https://github.com/pkourany/MFRC522_RFID_Library](https://github.com/pkourany/MFRC522_RFID_Library)
* [https://embeddedexpert.io/?p=768](https://embeddedexpert.io/?p=768)
* https://stackoverflow.com/ [🙏]
