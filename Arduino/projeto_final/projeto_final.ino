#include <avr/eeprom.h>
#include <EEPROM.h>

//ligar transito             iniciaTransito()            #iniTran
//desligar todos:            desligaPinos()              #deslAll
//ligar todos:               ligaTodosOsPinos()          #ligAll
//apenas Verde ligado:       ligaPinoVerde()             #ligVerde
//apenas Amarelo ligado:     ligaPinoAmarelo()           #ligAmarelo
//apenas Vermelho ligado:    ligaPinoVermelho()          #ligVermelho
//definir tempo Verde:       alterarValorTempoVerde()    #tVerdeLigNN     //dois numeros apenas
//definir tempo Amarelo:     alterarValorTempoAmarelo()  #tAmareloLigNN   //dois numeros apenas
//definir tempo Vermelho:    alterarValorTempoVermelho() #tVermelhoLigNN  //dois numeros apenas
//reset:                     reset()                     #reset 
//piscar o Amarelo           piscaAmarelo()              #AmareloPisca
//Reduzir a 1 para manuten:  manutencao()                #manutencao
//apenas VerdePed ligado:    ligaSoPinoVerdePed()        #PligVerde
//apenas VermelhoPed ligado: ligaSoPinoVermelhoPed()     #PligVermelho


int pinoVerde = 8;
int pinoAmarelo = 9;
int pinoVermelho = 10;
int pinoVerde2 = 11;
int pinoAmarelo2 = 12;
int pinoVermelho2 = 13;

int pinoVermelhoPed2 = 7;
int pinoVerdePed2 = 6;

int pinoVermelhoPed = 5;
int pinoVerdePed = 4;

const int endComandoEEPROM = 19;

const int TEMPO_PADRAO_VERDE = 60;
const int TEMPO_PADRAO_AMARELO = 3;
const int TEMPO_PADRAO_VERMELHO = 60;

int tempoVerde;
int tempoAmarelo;
int tempoVermelho;
int tComando;

const int TAMANHO_MAXIMO_CHAR = 14;
char COMANDO_PADRAO[TAMANHO_MAXIMO_CHAR] = "#iniTran";

int auxIniVerde = 0;
int auxFinVerde = auxIniVerde+1;
int auxNullFinVerde = auxFinVerde;

int auxIniAmarelo = auxNullFinVerde + 1;
int auxFinAmarelo = auxIniAmarelo + 1;
int auxNullFinAmarelo = auxFinAmarelo ;


int auxIniVermelho = auxNullFinAmarelo + 1;
int auxFinVermelho = auxIniVermelho + 1;
int auxNullFinVermelho = auxFinVermelho ;


int auxIniComando = auxNullFinVermelho +1;
int auxFinComando = auxIniComando + TAMANHO_MAXIMO_CHAR -1;
int auxNullFinComando = auxFinComando +1;

char comandoLoop[TAMANHO_MAXIMO_CHAR];

int numTarefa = 0;
String s;


void setup() {
  Serial.begin(9600);  
  iniciaOsPinos();
  while(!eeprom_is_ready()){}
  iniciaValoresTempo();
  pegarValorComandoEEPROM();
  numTarefa = retornarNumTarefaValida(comandoLoop);
}

void loop() {
  if(Serial.available()>0){
    delay(500);
    String s = Serial.readString();    
    Serial.println(1);
    if(s[0]=='#' && s[1] == 't'){
      pegarStringTComando(s);
      //Serial.print("Valor de Comando no if : ");
      //Serial.println(comandoLoop); 
    }else{
       pegarStringComando(s);
       //Serial.print("Valor de Comando no else : ");
       //Serial.println(comandoLoop);
       armazenarValorComandoEEPROM(comandoLoop);
     }
  }
  //Serial.print("Valor de Comando : ");
  //Serial.println(comandoLoop);
  //Serial.print("Valor da numTarefa : ");
  numTarefa = retornarNumTarefaValida(comandoLoop);
  //Serial.println(numTarefa);
  fazerTarefa(numTarefa);
}

void clearEEPROM(){
 for(int i=0;i<=endComandoEEPROM;i++){
  EEPROM.write(i,0);
 } 
}

void imprimeEEPROM(){
  char c;
  for(int i=0;i<=endComandoEEPROM;i++){
    if(i<5){
      int j;
      byte low = EEPROM.read(i+1); 
      byte high = EEPROM.read(i);
      j = (high << 8) + low; 
      Serial.print("Posicao da memoria ");
      Serial.print(i);
      Serial.print(" e ");
      Serial.print(i+1);
      Serial.print(" possuem valor : ");
      Serial.println(j);
      i++;
    }else{
        c=EEPROM.read(i);
        Serial.print("Posicao da memoria ");
        Serial.print(i);
        Serial.print(" tem valor : ");
        Serial.println(c);
     }
  } 
}

void escreveValoresPadroesNaEEPROM(){
  armazenarValorTempoVerdeEEPROM(TEMPO_PADRAO_VERDE);
  armazenarValorTempoAmareloEEPROM(TEMPO_PADRAO_AMARELO);
  armazenarValorTempoVermelhoEEPROM(TEMPO_PADRAO_VERMELHO);
  iniciaValorComandoPadrao();
}

void testeValoresDaEEPROM(){
  int tempoVerdeTeste;
  byte low;
  byte high;
    
  high = EEPROM.read(0); //byte mais significado
  low = EEPROM.read(1);  //byte menos significativo
  tempoVerdeTeste = (high << 8) + low;
  Serial.print("Teste valor tempoVerde : ");
  Serial.println(tempoVerdeTeste);

  int tempoAmareloTeste;   
  
  high = EEPROM.read(2); //byte mais significado
  low = EEPROM.read(3);  //byte menos significativo
  tempoAmareloTeste = (high << 8) + low;
  Serial.print("Teste valor tempoAmarelo : ");
  Serial.println(tempoAmareloTeste);
  
  int tempoVermelhoTeste;
    
  high = EEPROM.read(4); //byte mais significado
  low = EEPROM.read(5);  //byte menos significativo
  tempoVermelhoTeste = (high << 8) + low;
  Serial.print("Teste valor tempoVemelho : ");
  Serial.println(tempoVermelhoTeste);

  char comandoTeste[TAMANHO_MAXIMO_CHAR];
  for(int i = 6; i<=endComandoEEPROM;i++){
    int posicao = i-6;
    comandoTeste[posicao] = EEPROM.read(i);
    if(comandoTeste[posicao]=='\0'){
      i=20;
    }
  }
  Serial.print("Teste valor comando : ");
  Serial.println(comandoTeste);
}

void iniciaValoresTempo(){
  int verde = pegarValorTempoVerdeEEPROM(); 
  int amarelo = pegarValorTempoAmareloEEPROM();
  int vermelho = pegarValorTempoVermelhoEEPROM();

  if(verde>0){tempoVerde = verde;} 
    else{armazenarValorTempoVerdeEEPROM(tempoVerde);};
  if(amarelo>0){tempoAmarelo = amarelo;}
    else{armazenarValorTempoAmareloEEPROM(tempoAmarelo);}
  if(vermelho>0){tempoVermelho = vermelho;}
    else{armazenarValorTempoVermelhoEEPROM(tempoVermelho);}
}

void alterarValorTempoVerde(int verde){
    armazenarValorTempoVerdeEEPROM(verde);
    tempoVerde=verde;
}

void alterarValorTempoAmarelo(int amarelo){
    armazenarValorTempoAmareloEEPROM(amarelo);
    tempoAmarelo=amarelo;
}

void alterarValorTempoVermelho(int vermelho){
    armazenarValorTempoVermelhoEEPROM(vermelho);
    tempoVermelho=vermelho;
}

void manutencao(){
  desligaPinos();
  delay(100);
  ligaTodosOsPinos();
}

int pegarValorTempoVerdeEEPROM(){  
  int tempVerde = 0;
  byte low;
  byte high;

  high = EEPROM.read(0);
  low = EEPROM.read(1);

  tempVerde = (high << 8) + low;
   
  return tempVerde; 
}

int pegarValorTempoAmareloEEPROM(){    
  int tempAmarelo = 0;

  byte low;
  byte high;

  high = EEPROM.read(2);
  low = EEPROM.read(3);

  tempAmarelo = (high << 8) + low;
  
  return tempAmarelo; 
}

int pegarValorTempoVermelhoEEPROM(){  
  int tempVermelho = 0;
  byte low;
  byte high;

  high = EEPROM.read(4);
  low = EEPROM.read(5);

  tempVermelho = (high << 8) + low;
  
  return tempVermelho; 
}

void armazenarValorTempoVerdeEEPROM(int verde){
  int x = verde;

  EEPROM.write(0,highByte(x));
  EEPROM.write(1,lowByte(x));
}

void armazenarValorTempoAmareloEEPROM(int amarelo){
  int x = amarelo;

  EEPROM.write(2,highByte(x));
  EEPROM.write(3,lowByte(x));
}

void armazenarValorTempoVermelhoEEPROM(int vermelho){
  int x = vermelho;

  EEPROM.write(4,highByte(x));
  EEPROM.write(5,lowByte(x));
}

void pegarValorComandoEEPROM(){
  for(int i = 6; i<=endComandoEEPROM;i++){
    int posicao = i-6;
    comandoLoop[posicao] = EEPROM.read(i);
    if(comandoLoop[posicao]=='\0'){
      i=20;
    }
  }
}

void iniciaValorComandoPadrao(){
  armazenarValorComandoEEPROM(COMANDO_PADRAO);
  pegarValorComandoEEPROM();
}

void armazenarValorComandoEEPROM(char c[TAMANHO_MAXIMO_CHAR]){
  for(int i = 6; i<=endComandoEEPROM;i++){
    int posicao = i-6;
    EEPROM.write(i,c[posicao]);
    if(c[posicao]=='\0'){
      i=20;
    }
  }
}

void pegarStringComando(String s){
  char c[TAMANHO_MAXIMO_CHAR];
  int posUltChar;
  for(int j=0; j<TAMANHO_MAXIMO_CHAR;j++){
    if(s[j]!='\n'){
      if(!ehNumero(s[j])){
        c[j]=s[j];
        posUltChar = j;
      }else{
          c[j]='\0';
          j=TAMANHO_MAXIMO_CHAR;
       }
    }else{
       c[j]='\0'; 
       j=TAMANHO_MAXIMO_CHAR;
     }
  }
  for(int j=0;j<=posUltChar;j++){
    comandoLoop[j] = c[j];
    if(j==posUltChar){
      comandoLoop[j+1]='\0';
    }        
  }  
}

void pegarStringTComando(String s){
  char c[23];
  int posUltChar;
  for(int j=0; j<=22;j++){
    c[j] = s[j];
    if(ehNumero(s[j])){
      posUltChar = j-1;
      int numFinal;
      int num1 = passarParaInt(s[j]);
      if(ehNumero(s[j+1])){
        int num2 = passarParaInt(s[j+1]);
        numFinal = (num1*10)+num2;
        tComando = numFinal;          
      } else{
          numFinal = num1;
          tComando = numFinal;
        }
      c[j] = '\0';
      j=23; 
    }
    if(j<23){
      if(s[j+1] == '\n'){ // Verificacao caso alguem digite #tcomandozuado
        c[j+1] = '\0';
        posUltChar = j;
        j=23;
      }
    } else{
        c[posUltChar+1]='\0';
      }
  }
  //char c reuniu todos os chars, agora coloca-los no comandoLoop
  for(int j=0;j<=posUltChar;j++){
    comandoLoop[j] = c[j];
    if(j+1==posUltChar){
      comandoLoop[j+1]='\0';
    }        
  }  
}

bool ehNumero(char c){
  if(c=='0' || c=='1' || c=='2' || c=='3' || c=='4' || c=='5' 
     || c=='6' || c=='7' || c=='8' || c=='9'){
    return true;
  } else{
      return false;
    }  
}

int passarParaInt(char c){
  int i = c-'0';
  return i;  
}

void iniciaOsPinos(){
   pinMode(pinoVerde,OUTPUT);
   pinMode(pinoAmarelo,OUTPUT);  
   pinMode(pinoVermelho,OUTPUT);
   pinMode(pinoVerde2,OUTPUT);
   pinMode(pinoAmarelo2,OUTPUT);  
   pinMode(pinoVermelho2,OUTPUT); 
   pinMode(pinoVermelhoPed,OUTPUT);
   pinMode(pinoVermelhoPed2,OUTPUT);
   pinMode(pinoVerdePed,OUTPUT);  
   pinMode(pinoVerdePed2,OUTPUT); 
}

void iniciaTransito(){
   desligaPinos();
   
   // VERDE E VERMELHO2
   ligaPinoVerde();
   ligaPinoVermelho2();
   desligaPinoVerdePed();
   ligaPinoVermelhoPed();
   desligaPinoVermelhoPed2();
   ligaPinoVerdePed2();
   parcelarDelay(tempoVerde);

   // AMARELO E VERMELHO2 
   desligaPinoVerde();
   ligaPinoAmarelo();   
   //Piscar pedestre vermelho no primeiro transito
   piscaVermelhoPed2(tempoAmarelo);

   // VERMELHO E VERDE2
   desligaPinoAmarelo(); 
   ligaPinoVermelho();
   desligaPinoVermelho2();
   ligaPinoVerde2();
   desligaPinoVermelhoPed();
   ligaPinoVerdePed();
   desligaPinoVerdePed2();
   ligaPinoVermelhoPed2();
   parcelarDelay(tempoVermelho);
      
   // VERMELHO E AMARELO2
   desligaPinoVerde2();
   ligaPinoAmarelo2();
   piscaVermelhoPed(tempoAmarelo);
}

void piscaVermelhoPed(int tempoAmarelo){
  for(int i=1; i<=tempoAmarelo; i++){
    if(i%2==0){
      desligaPinoVermelhoPed(); 
    } else {
        ligaPinoVermelhoPed();
      }
    parcelarDelay(1); 
  }  
}

void piscaVermelhoPed2(int tempoAmarelo){  
  for(int i=1; i<=tempoAmarelo; i++){
    if(i%2==0){
      desligaPinoVermelhoPed2(); 
    } else {
        ligaPinoVermelhoPed2();
      }
    parcelarDelay(1); 
  }  
}

void desligaPinos(){
  digitalWrite(pinoVerde,LOW);
  digitalWrite(pinoAmarelo,LOW);
  digitalWrite(pinoVermelho,LOW);
  digitalWrite(pinoVerde2,LOW);
  digitalWrite(pinoAmarelo2,LOW);
  digitalWrite(pinoVermelho2,LOW);
  digitalWrite(pinoVermelhoPed,LOW);
  digitalWrite(pinoVermelhoPed2,LOW);
  digitalWrite(pinoVerdePed,LOW); 
  digitalWrite(pinoVerdePed2,LOW);  
}

void ligaPinoVerde(){
  digitalWrite(pinoVerde,HIGH);
}

void ligaPinoVerde2(){
  digitalWrite(pinoVerde2,HIGH);
}

void ligaPinoAmarelo(){
  digitalWrite(pinoAmarelo,HIGH);
}

void ligaPinoAmarelo2(){
  digitalWrite(pinoAmarelo2,HIGH);
}


void ligaPinoVermelho(){
  digitalWrite(pinoVermelho,HIGH);
}

void ligaPinoVermelhoPed(){
  digitalWrite(pinoVermelhoPed,HIGH);
}

void ligaPinoVermelhoPed2(){
  digitalWrite(pinoVermelhoPed2,HIGH);
}

void ligaPinoVerdePed(){
  digitalWrite(pinoVerdePed,HIGH);
}

void ligaPinoVerdePed2(){
  digitalWrite(pinoVerdePed2,HIGH);
}

void ligaPinoVermelho2(){
  digitalWrite(pinoVermelho2,HIGH);
}

void desligaPinoVerde(){
  digitalWrite(pinoVerde,LOW);
}

void desligaPinoVerde2(){
  digitalWrite(pinoVerde2,LOW);
}

void desligaPinoAmarelo(){
  digitalWrite(pinoAmarelo,LOW);
}

void desligaPinoAmarelo2(){
  digitalWrite(pinoAmarelo2,LOW);
}


void desligaPinoVermelho(){
  digitalWrite(pinoVermelho,LOW);
}

void desligaPinoVermelho2(){
  digitalWrite(pinoVermelho2,LOW);
}

void desligaPinoVermelhoPed(){
  digitalWrite(pinoVermelhoPed,LOW);  
}

void desligaPinoVermelhoPed2(){
  digitalWrite(pinoVermelhoPed2,LOW);  
}

void desligaPinoVerdePed(){
  digitalWrite(pinoVerdePed,LOW);  
}

void desligaPinoVerdePed2(){
  digitalWrite(pinoVerdePed2,LOW);  
}

void ligaTodosOsPinos(){
  digitalWrite(pinoVerde,HIGH);
  digitalWrite(pinoAmarelo,HIGH);
  digitalWrite(pinoVermelho,HIGH);
  digitalWrite(pinoVerde2,HIGH);
  digitalWrite(pinoAmarelo2,HIGH);
  digitalWrite(pinoVermelho2,HIGH);
  digitalWrite(pinoVerdePed,HIGH);
  digitalWrite(pinoVerdePed2,HIGH);
  digitalWrite(pinoVermelhoPed,HIGH);
  digitalWrite(pinoVermelhoPed2,HIGH);
}


void piscaAmarelo(){
  parcelarDelay(1);
  ligaPinoAmarelo();
  ligaPinoAmarelo2();
  parcelarDelay(1);
  desligaPinos();
}

void reset(){
  clearEEPROM();
  escreveValoresPadroesNaEEPROM();
  tempoVerde = pegarValorTempoVerdeEEPROM();
  tempoAmarelo = pegarValorTempoAmareloEEPROM();
  tempoVermelho = pegarValorTempoVermelhoEEPROM();
}

void parcelarDelay(int tempo){
  for(int i = 1; i<=1000; i++){
    delay(tempo);  
  }
}

bool arraysIguais(char c1[TAMANHO_MAXIMO_CHAR],char c2[TAMANHO_MAXIMO_CHAR]){
  for(int i = 0; i<TAMANHO_MAXIMO_CHAR; i++){
    if(c1[i]!=c2[i]){
      return false;
      break;  
    }
    if(c1[i+1]=='\0' || c2[i+1]=='\0'){
      i=TAMANHO_MAXIMO_CHAR+1;  
    };
  };      
  return true;
}

int retornarNumTarefaValida(char c[TAMANHO_MAXIMO_CHAR]){
  char iniTranTemp[TAMANHO_MAXIMO_CHAR] = "#iniTran";
  char deslAllTemp[TAMANHO_MAXIMO_CHAR] = "#deslAll";
  char ligAllTemp[TAMANHO_MAXIMO_CHAR] = "#ligAll";
  char ligVerdeTemp[TAMANHO_MAXIMO_CHAR] = "#ligVerde";
  char ligAmareloTemp[TAMANHO_MAXIMO_CHAR] = "#ligAmarelo";
  char ligVermelhoTemp[TAMANHO_MAXIMO_CHAR] = "#ligVermelho";
  char tVerdeLigNNTemp[TAMANHO_MAXIMO_CHAR] = "#tVerdeLig";
  char tAmareloLigNNTemp[TAMANHO_MAXIMO_CHAR] = "#tAmareloLig";
  char tVermelhoLigNNTemp[TAMANHO_MAXIMO_CHAR] = "#tVermelhoLig";
  char resetTemp[TAMANHO_MAXIMO_CHAR] = "#reset";
  char AmareloPiscaTemp[TAMANHO_MAXIMO_CHAR] = "#AmareloPisc";
  char ManutencaoTemp[TAMANHO_MAXIMO_CHAR] = "#manutencao";
  char PligVerdeTemp[TAMANHO_MAXIMO_CHAR] = "#PligVerde";
  char PligVermelhoTemp[TAMANHO_MAXIMO_CHAR] = "#PligVermelho";
  
  if(arraysIguais(c,iniTranTemp)){return 1;}
  else if(arraysIguais(c,deslAllTemp)){return 2;}
  else if(arraysIguais(c,ligAllTemp)){return 3;}
  else if(arraysIguais(c,ligVerdeTemp)){return 4;}
  else if(arraysIguais(c,ligAmareloTemp)){return 5;}
  else if(arraysIguais(c,ligVermelhoTemp)){return 6;}
  else if(arraysIguais(c,tVerdeLigNNTemp)){return 7;}
  else if(arraysIguais(c,tAmareloLigNNTemp)){return 8;}
  else if(arraysIguais(c,tVermelhoLigNNTemp)){return 9;}
  else if(arraysIguais(c,resetTemp)){return 10;}
  else if(arraysIguais(c,AmareloPiscaTemp)){return 11;}
  else if(arraysIguais(c,ManutencaoTemp)){return 12;}
  else if(arraysIguais(c,PligVerdeTemp)){return 13;}
  else if(arraysIguais(c,PligVermelhoTemp)){return 14;}  
  else return 0;
}

void fazerTarefa(int j){
  char iniTranTemp[TAMANHO_MAXIMO_CHAR] = "#iniTran";
  char deslAllTemp[TAMANHO_MAXIMO_CHAR] = "#deslAll";
  char ligAllTemp[TAMANHO_MAXIMO_CHAR] = "#ligAll";
  char ligVerdeTemp[TAMANHO_MAXIMO_CHAR] = "#ligVerde";
  char ligAmareloTemp[TAMANHO_MAXIMO_CHAR] = "#ligAmarelo";
  char ligVermelhoTemp[TAMANHO_MAXIMO_CHAR] = "#ligVermelho";
  char tVerdeLigNNTemp[TAMANHO_MAXIMO_CHAR] = "#tVerdeLig";
  char tAmareloLigNNTemp[TAMANHO_MAXIMO_CHAR] = "#tAmareloLig";
  char tVermelhoLigNNTemp[TAMANHO_MAXIMO_CHAR] = "#tVermelhoLig";
  char resetTemp[TAMANHO_MAXIMO_CHAR] = "#reset";
  char AmareloPiscaTemp[TAMANHO_MAXIMO_CHAR] = "#AmareloPisc";
  char ManutencaoTemp[TAMANHO_MAXIMO_CHAR] = "#manutencao";
  char PligVerdeTemp[TAMANHO_MAXIMO_CHAR] = "#PligVerde";
  char PligVermelhoTemp[TAMANHO_MAXIMO_CHAR] = "#PligVermelho";

  if(j>=1){
    switch(j){
      case 1:
        //#iniTran
        iniciaTransito();
        armazenarValorComandoEEPROM(iniTranTemp);
        pegarValorComandoEEPROM();
      break;
        
      case 2:
        //#deslAll
        desligaPinos();
        armazenarValorComandoEEPROM(deslAllTemp);
        pegarValorComandoEEPROM();
      break;

      case 3:
        //#ligAll
        ligaTodosOsPinos();
        armazenarValorComandoEEPROM(ligAllTemp);
        pegarValorComandoEEPROM();
      break;

      case 4:
        //#ligVerde
        desligaPinos();
        ligaPinoVerde();
        armazenarValorComandoEEPROM(ligVerdeTemp);
        pegarValorComandoEEPROM();
      break;

      case 5:
        //#ligAmarelo
        desligaPinos();
        ligaPinoAmarelo();
        ligaPinoAmarelo2();
        armazenarValorComandoEEPROM(ligAmareloTemp);
        pegarValorComandoEEPROM();
      break;

      case 6:
        //#ligVermelho
        desligaPinos();
        ligaPinoVermelho();
        ligaPinoVerde2();
        armazenarValorComandoEEPROM(ligVermelhoTemp);
        pegarValorComandoEEPROM();       
      break;

      case 7:
        //#tVerdeLig
        if(tComando>0){
          alterarValorTempoVerde(tComando);
        }
        armazenarValorComandoEEPROM(ManutencaoTemp);
        pegarValorComandoEEPROM();             
      break;

      case 8:
        //#tAmareloLig
        if(tComando>0){
          alterarValorTempoAmarelo(tComando);
        }
        armazenarValorComandoEEPROM(ManutencaoTemp);
        pegarValorComandoEEPROM();        
      break;

      case 9:
        //#tVermelhoLig
        if(tComando>0){
          alterarValorTempoVermelho(tComando);
        }
        armazenarValorComandoEEPROM(ManutencaoTemp);
        pegarValorComandoEEPROM();
        
      break;

      case 10:
        reset();
      break;

      case 11:
        //#AmareloPisc
        piscaAmarelo();
        armazenarValorComandoEEPROM(AmareloPiscaTemp);
        pegarValorComandoEEPROM();
      break;

      case 12:
        //#manutencao
        manutencao();
        armazenarValorComandoEEPROM(ManutencaoTemp);
        pegarValorComandoEEPROM();
      break;

      case 13:
        //#PligVerde
        desligaPinos();
        ligaPinoVerdePed();
        armazenarValorComandoEEPROM(PligVerdeTemp);
        pegarValorComandoEEPROM();
      break;

      case 14:
        //#PligVermelho
        desligaPinos();
        ligaPinoVermelhoPed();
        armazenarValorComandoEEPROM(PligVermelhoTemp);
        pegarValorComandoEEPROM();
      break;
    }  
  }
}
