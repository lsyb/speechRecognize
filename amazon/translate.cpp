#include <curl/curl.h>
#include <stdio.h>
#include <fstream>
size_t writeCallBack(const void *ptr, size_t size, size_t nitems, void *stream)
{
  printf("%.*s\n",size*nitems,ptr);
  return size*nitems;
}
int main()
{
  CURL* curlHandle=curl_easy_init();
  curl_slist* header=NULL;
  header=curl_slist_append(header,"Transfer-Encoding: chunked");
  header=curl_slist_append(header,"Content-Type: audio/x-wav;codec=pcm;bit=16;rate=16000");
  header=curl_slist_append(header,"Accept: text/plain");
  header=curl_slist_append(header,"Accept-Language: en-US");
  curl_easy_setopt(curlHandle,CURLOPT_HTTPHEADER,header);
  curl_easy_setopt(curlHandle,CURLOPT_URL,"https://dictation.nuancemobility.net/NMDPAsrCmdServlet/dictation?appId=NMDPTRIAL_wimxpliyou_gmail_com20160311004931&appKey=1b11e3f0620ade13e3490575c79b0d5d489c909a6a08366b7b691b685c954577a628670832461c158f218a56bd98b30534449f9e4cd509c73e25309aba751260&id=57349abd2396");
  curl_easy_setopt(curlHandle,CURLOPT_FOLLOWLOCATION,1);
  curl_easy_setopt(curlHandle,CURLOPT_WRITEFUNCTION,writeCallBack);
  curl_easy_setopt(curlHandle,CURLOPT_WRITEDATA,NULL);
  std::ifstream fs("postdata",std::ifstream::binary);
  fs.seekg(0,std::ofstream::end);
  int len=fs.tellg();
  fs.seekg(0,std::ofstream::beg);
  printf("len is %d\n",len);
  char* waveBuff=new char[len];
  fs.read(waveBuff,len);
  curl_easy_setopt(curlHandle,CURLOPT_POSTFIELDS,waveBuff);
  curl_easy_setopt(curlHandle,CURLOPT_POSTFIELDSIZE,len);
  curl_easy_perform(curlHandle);
  return 0;
}
