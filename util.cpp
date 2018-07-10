#include <string.h>
#include <fstream>

void genWaveHeader(char* buff,short channelCount,int sampleRate,short bitsPerSample,int dataLength)
{
  memcpy(buff,"RIFF",4);
  buff+=4;
  int ChunkSize=dataLength+36;
  memcpy(buff,&ChunkSize,4);
  buff+=4;
  memcpy(buff,"WAVE",4);
  buff+=4;
  memcpy(buff,"fmt ",4);
  buff+=4;
  int Subchunk1Size=16;
  memcpy(buff,&Subchunk1Size,4);
  buff+=4;
  short AudioFormat=1;
  memcpy(buff,&AudioFormat,2);
  buff+=2;
  memcpy(buff,&channelCount,2);
  buff+=2;
  memcpy(buff,&sampleRate,4);
  buff+=4;
  int ByteRate=sampleRate*channelCount*bitsPerSample/8;
  memcpy(buff,&ByteRate,4);
  buff+=4;
  short BlockAlign=channelCount*bitsPerSample/8;
  memcpy(buff,&BlockAlign,2);
  buff+=2;
  memcpy(buff,&bitsPerSample,2);
  buff+=2;
  memcpy(buff,"data",4);
  buff+=4;
  memcpy(buff,&dataLength,4);
}

int main()
{
  std::ofstream wave("./out.wav");
  std::ifstream data("./data"); 
  data.seekg(0,std::ifstream::end);
  int len=data.tellg();
  char header[44];
  genWaveHeader(header,1,16000,16,len);
  wave.write(header,44);
  wave.close();

  return 0;
}
