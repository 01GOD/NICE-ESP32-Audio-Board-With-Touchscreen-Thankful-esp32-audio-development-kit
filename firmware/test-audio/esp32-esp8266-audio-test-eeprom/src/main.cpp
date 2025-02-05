#include <Arduino.h>
#include <SD.h>

#include "AudioFileSourcePROGMEM.h"
#include "AudioGeneratorWAV.h"
#include "AudioOutputI2S.h"
#include "AudioOutputMixer.h"

// VIOLA sample taken from https://ccrma.stanford.edu/~jos/pasp/Sound_Examples.html
#include "viola.h"

AudioGeneratorWAV *wav[2];
AudioFileSourcePROGMEM *file[2];
AudioOutputI2S *out;
AudioOutputMixer *mixer;
AudioOutputMixerStub *stub[2];

void setup()
{
  Serial.begin(SERIAL_BAUD);
  delay(1000);
  Serial.printf("WAV start\n");
  
  #ifdef PIN_I2S_EN
  pinMode(PIN_I2S_EN, OUTPUT);
  digitalWrite(PIN_I2S_EN, LOW);
  #endif

  file[0] = new AudioFileSourcePROGMEM(viola, sizeof(viola));
  out = new AudioOutputI2S();
  mixer = new AudioOutputMixer(32, out);
  stub[0] = mixer->NewInput();
  stub[0]->SetGain(0.3);
  wav[0] = new AudioGeneratorWAV();

  #ifdef PIN_I2S_EN
  digitalWrite(PIN_I2S_EN, HIGH);
  #endif
  Serial.printf("starting 1\n");
  wav[0]->begin(file[0], stub[0]);
}

void loop()
{
  static uint32_t start = 0;
  static bool go = false;

  if (!start)
    start = millis();

  if (wav[0]->isRunning())
  {
    if (!wav[0]->loop())
    {
      Serial.printf("stopping 1\n");
      wav[0]->stop();
      stub[0]->stop();
    }
  }

  if (millis() - start > 3000)
  {
    if (!go)
    {
      Serial.printf("starting 2\n");
      stub[1] = mixer->NewInput();
      stub[1]->SetGain(0.4);
      wav[1] = new AudioGeneratorWAV();
      file[1] = new AudioFileSourcePROGMEM(viola, sizeof(viola));
      wav[1]->begin(file[1], stub[1]);
      go = true;
    }

    if (wav[1]->isRunning())
    {
      if (!wav[1]->loop())
      {
        Serial.printf("stopping 2\n");
        wav[1]->stop();
        stub[1]->stop();
        #ifdef PIN_I2S_EN
        digitalWrite(PIN_I2S_EN, LOW);
        #endif
      }
    }
  }
}