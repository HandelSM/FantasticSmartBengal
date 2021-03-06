#include "player_mini.h"

void playSound10To90(DFPlayerMini player, byte num, int delayTimeMs = 0)
{
  switch (num) {
    case 10:
      player.playFileAndWait(10, 0, 0, DFPLAYER_WAIT_TIMEOUT);
      delay(delayTimeMs);
      break;
    case 20:
      player.playFileAndWait(11, 0, 0, DFPLAYER_WAIT_TIMEOUT);
      delay(delayTimeMs);
      break;
    case 30:
      player.playFileAndWait(12, 0, 0, DFPLAYER_WAIT_TIMEOUT);
      delay(delayTimeMs);
      break;
    case 40:
      player.playFileAndWait(13, 0, 0, DFPLAYER_WAIT_TIMEOUT);
      delay(delayTimeMs);
      break;
    case 50:
      player.playFileAndWait(14, 0, 0, DFPLAYER_WAIT_TIMEOUT);
      delay(delayTimeMs);
      break;
    case 60:
      player.playFileAndWait(15, 0, 0, DFPLAYER_WAIT_TIMEOUT);
      delay(delayTimeMs);
      break;
    case 70:
      player.playFileAndWait(16, 0, 0, DFPLAYER_WAIT_TIMEOUT);
      delay(delayTimeMs);
      break;
    case 80:
      player.playFileAndWait(17, 0, 0, DFPLAYER_WAIT_TIMEOUT);
      delay(delayTimeMs);
      break;
    case 90:
      player.playFileAndWait(18, 0, 0, DFPLAYER_WAIT_TIMEOUT);
      delay(delayTimeMs);
      break;
    default:
      Serial.println("Erro: numero valido nao detectado.");
  }
}

void playSound1To9(DFPlayerMini player, byte num, int delayTimeMs = 0)
{
  if (num >= 0 && num <= 9) 
  {
    player.playFileAndWait(num, 0, 0, DFPLAYER_WAIT_TIMEOUT);
    delay(delayTimeMs);
  }
  else 
  {
    Serial.println("Erro: num[0, 9] nao detectado.");
  }
}

void playMiscSound(DFPlayerMini player, String text, int delayTimeMs = 0)
{
  char key;
  if (text.length() == 1)
  {
    key = text.charAt(0);
    switch (key) {
      case 'C':
        //Modo detector de cor ativado
        player.playFileAndWait(21, 0, 0, DFPLAYER_WAIT_TIMEOUT);
        delay(delayTimeMs);
        break;
      case 'D':
        //Modo detector de distancia ativado
        player.playFileAndWait(22, 0, 0, DFPLAYER_WAIT_TIMEOUT);
        delay(delayTimeMs);
        break;
      case 'S':
        //Modo sonar ativado
        player.playFileAndWait(23, 0, 0, DFPLAYER_WAIT_TIMEOUT);
        delay(delayTimeMs);
        break;
      case 'O':
        //Objeto detectado a
        player.playFileAndWait(24, 0, 0, DFPLAYER_WAIT_TIMEOUT);
        delay(delayTimeMs);
        break;
      case 'P':
        //Ponto
        player.playFileAndWait(25, 0, 0, DFPLAYER_WAIT_TIMEOUT);
        delay(delayTimeMs);
        break;
      case 'R':
        //Vermelho (Red)
        player.playFileAndWait(26, 0, 0, DFPLAYER_WAIT_TIMEOUT);
        delay(delayTimeMs);
        break;
      case 'l':
        //Laranja
        player.playFileAndWait(27, 0, 0, DFPLAYER_WAIT_TIMEOUT);
        delay(delayTimeMs);
        break;
      case 'a':
        //Amarelo
        player.playFileAndWait(28, 0, 0, DFPLAYER_WAIT_TIMEOUT);
        delay(delayTimeMs);
        break;
      case 'G':
        //Verde (Green)
        player.playFileAndWait(29, 0, 0, DFPLAYER_WAIT_TIMEOUT);
        delay(delayTimeMs);
        break;
      case 'B':
        //Azul (Blue)
        player.playFileAndWait(30, 0, 0, DFPLAYER_WAIT_TIMEOUT);
        delay(delayTimeMs);
        break;
      case 'n':
        //Anil (aNil)
        player.playFileAndWait(31, 0, 0, DFPLAYER_WAIT_TIMEOUT);
        delay(delayTimeMs);
        break;
      case 'v':
        //Violeta
        player.playFileAndWait(32, 0, 0, DFPLAYER_WAIT_TIMEOUT);
        delay(delayTimeMs);
        break;
      case 'b':
        //Branco
        player.playFileAndWait(33, 0, 0, DFPLAYER_WAIT_TIMEOUT);
        delay(delayTimeMs);
        break;
      case 'p':
        //Preto
        player.playFileAndWait(34, 0, 0, DFPLAYER_WAIT_TIMEOUT);
        delay(delayTimeMs);
        break;
      case 'c':
        //Cinza
        player.playFileAndWait(35, 0, 0, DFPLAYER_WAIT_TIMEOUT);
        delay(delayTimeMs);
        break;
      default:
        //Erro
        Serial.println("Erro: String invalida para funcao.");
        delay(delayTimeMs);
    }
  }

  else 
  {
    //Centimentros
    if (text.compareTo("cm") == 0) 
    {
      player.playFileAndWait(19, 0, 0, DFPLAYER_WAIT_TIMEOUT);
    }
    //Metros
    else if (text.compareTo("mt") == 0) 
    {
      player.playFileAndWait(20, 0, 0, DFPLAYER_WAIT_TIMEOUT);
    }

    else 
    {
      Serial.println("Erro: String invalida para funcao.");
      delay(delayTimeMs);
    }
  }
}

void playSoundBlackBox(DFPlayerMini player, String text, int delayTimeMs)
{
  byte num;
  char c = text.charAt(0);
  if (text.length() == 1) {
    if (c >= '1' && c <= '9') {
      playSound1To9(player, c - '0');
    }
    else {
      playMiscSound(player, text, delayTimeMs);
    }
  }

  else if (c >= '1' && c <= '9') {
    num = byte(text.substring(0, 2).toInt());
    playSound10To90(player, byte(num / 10) * 10, delayTimeMs);
    if ((num % 10) != 0) {
      playSound1To9(player, num % 10, delayTimeMs);
    }
  }

  else {
    playMiscSound(player, text, delayTimeMs);
  }
}

void playSound(DFPlayerMini player, String text, int delayTimeMs)
{
  char c = text.charAt(0);
  int sizeText = text.length();
  int indicePonto = text.indexOf('.');
  String antesPonto, depoisPonto;

  if ((c >= '1' && c <= '9') && sizeText > 1 && indicePonto != -1)
  {
    antesPonto = text.substring(0, indicePonto);
    depoisPonto = text.substring(indicePonto + 1, sizeText);

    playSoundBlackBox(player, antesPonto, delayTimeMs);
    playSoundBlackBox(player, "P", delayTimeMs);
    playSoundBlackBox(player, depoisPonto, delayTimeMs);
  }
  else
  {
    playSoundBlackBox(player, text, delayTimeMs);
  }
}
