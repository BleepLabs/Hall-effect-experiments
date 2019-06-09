#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>

// GUItool: begin automatically generated code
AudioSynthWaveform       waveform1;      //xy=124,334
AudioSynthWaveform       waveform2; //xy=165,475
AudioSynthWaveform       waveform3; //xy=218,551
AudioFilterStateVariable filter2; //xy=334,411
AudioFilterStateVariable filter1;        //xy=348,329
AudioMixer4              mixer2; //xy=538,521
AudioMixer4              mixer1;         //xy=549,392
AudioEffectMultiply      multiply1;      //xy=715,395
AudioOutputAnalog        dac1;           //xy=938,394
AudioConnection          patchCord1(waveform1, 0, filter1, 0);
AudioConnection          patchCord2(waveform2, 0, filter2, 0);
AudioConnection          patchCord3(waveform3, 0, mixer2, 0);
AudioConnection          patchCord4(filter2, 0, mixer1, 1);
AudioConnection          patchCord5(filter1, 0, mixer1, 0);
//AudioConnection          patchCord6(mixer2, 0, multiply1, 1);
//AudioConnection          patchCord7(mixer1, 0, multiply1, 0);
AudioConnection          patchCord8(mixer1, dac1);
// GUItool: end automatically generated code


int mag_read;
int readf[4];
int log_bank[4100];
int filter_offset;
float filter_amp;
float freq3;

void setup() {

  AudioNoInterrupts();
  AudioMemory(20);

  int high = 2600;

  for (int i = 0; i < high; ++i)
  {
    uint32_t logt = pow(i, 2) / high;
    log_bank[i] = logt;
  }

  mixer1.gain(0, 1);
  mixer1.gain(1, 1);

  mixer2.gain(0, 1);
  mixer2.gain(1, 1);

  filter1.frequency(8000);
  filter1.resonance(1.7);

  filter2.frequency(8000);
  filter2.resonance(1.7);


  waveform1.begin(.8, 92.49861792, WAVEFORM_SAWTOOTH);
  waveform2.begin(.8, 138.5913338, WAVEFORM_SAWTOOTH);
  waveform3.begin(1, 0, WAVEFORM_SINE);

  analogReadResolution(12);
  analogReadAveraging(64);

  AudioInterrupts();

}

uint32_t ct, pt[8], latch[4];
int raw[2];
int praw[3];
byte trig[5];
int tlvl[5];
byte reread;
void loop() {


  if (ct - pt[1] > 10 || reread == 1) {  ///cheap way of seeing if it went across a threshold 
    pt[1] = ct;
    praw[0] = raw[0];
    praw[1] = raw[1];
  }
  
  reread = 0;
  
  raw[0] = analogRead(A0);  //direct readings from hall sensors DRV5053VAQLPG
  raw[1] = analogRead(A1);

  int cut = 1240;
  tlvl[0] = 0;

  trig[0] = 0;
  trig[1] = 0;

  trig[2] = 0;
  trig[3] = 0;

  if (praw[0] < cut && raw[0] > cut + 10) {
    trig[0] = 1;
    tlvl[0] = raw[0] - cut; //how far did it get? use this as midi velocity data
    reread = 1;
    if (1 == 0) {
      Serial.print("~0~ ");
      Serial.print(trig[0]);
      Serial.print(" ");
      Serial.println(tlvl[0] * 3);
    }
  }

  if (praw[0] > cut && raw[0] < cut - 10) {
    trig[1] = 1;
    tlvl[1] = (raw[0] - cut) * -1;
    reread = 1;
    if (1 == 0) {
      Serial.print("~1~ ");
      Serial.print(trig[1]);
      Serial.print(" ");
      Serial.println(tlvl[1] * 3);
    }
  }

  if (praw[1] < cut && raw[1] > cut + 10) {
    trig[2] = 1;
    tlvl[2] = raw[1] - cut;
    reread = 1;
    if (1 == 0) {
      Serial.print("~2~ ");
      Serial.print(trig[0]);
      Serial.print(" ");
      Serial.println(tlvl[0] * 3);
    }
  }

  if (praw[1] > cut && raw[1] < cut - 10) {
    trig[3] = 1;
    tlvl[3] = (raw[1] - cut) * -1;
    reread = 1;
    if (1 == 0) {
      Serial.print("~3~ ");
      Serial.print(trig[1]);
      Serial.print(" ");
      Serial.println(tlvl[1] * 3);
    }
  }
  byte notes[5] = {50, 55, 62, 67};


  for (byte n = 0; n < 4; n++) {


    if (trig[n] == 1) {
      //    usbMIDI.sendNoteOff(notes[n], 0, 1);
      usbMIDI.sendNoteOn(notes[n], constrain (tlvl[n] * 3, 0, 127), 1);
      latch[n] = millis();

    }

    if (latch[n] > 0 && ct - latch[n] > 100) {
      latch[n] = 0;
      usbMIDI.sendNoteOff(notes[n], 0, 1);
    }

  }
  readf[0] = log_bank[raw[0]] * filter_amp;
  filter1.frequency(readf[0] + filter_offset);

  readf[1] = log_bank[raw[1]] * filter_amp;
  filter2.frequency(readf[1] + filter_offset);


  ct = millis();
  if (ct - pt[0] > 10) {
    pt[0] = ct;
    byte p = 0;

    if (p == 2) {

      Serial.print(tlvl[0]);
      Serial.print(" ");
      Serial.println(tlvl[0]);
    }

    if (p == 0) {

      Serial.print(raw[0]);
      Serial.print(" ");
      Serial.println(raw[1]);
    }
    if (p == 1) {
      Serial.print(filter_offset);
      Serial.print(" ");
      Serial.println(filter_amp);
    }
    
    filter_offset = (analogRead(A2) * 3) - 4000;
    filter_amp = analogRead(A3) / (4095.00 / 3);
    freq3 = analogRead(A4) / 8.00;
    waveform3.frequency(freq3);
  }

}
