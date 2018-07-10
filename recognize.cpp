#include "Device/PCM/CPCMDevice.h"
#include "SpeechRecognizer.h"
#include <curl/curl.h>
#include <iostream>
#include <fstream>


int main(int argc,char** argv)
{
  CURL* curlHandle=curl_easy_init();
  char* keywords[]={"format","hello","close","hearts"};
  CPCMDevice device;
  device.openDevice(SND_PCM_STREAM_CAPTURE);
  snd_pcm_format_t fmtC=SND_PCM_FORMAT_S16;
  snd_pcm_access_t accessC=SND_PCM_ACCESS_RW_INTERLEAVED;
  unsigned int channelCountC=1;
  unsigned int sampleRateC=16000;
  int softSampleC=1;
  unsigned int periodTimeC=100000;
  device.setHWParams(fmtC,accessC,channelCountC,sampleRateC,softSampleC,periodTimeC);
  SpeechRecognizer recognizer("./model");
  uint16 buff[2048];
  while(1)
  {
    device.read(buff,2048);
    recognizer.Recognize(buff,2048);
  }
  return 0;
}
