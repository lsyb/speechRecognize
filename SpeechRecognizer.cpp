//
//  SpeechRecognizer.cpp
//  HelloWorldDemo
//
//  Created by BudsPoint on 16/3/17.
//
//

#include "SpeechRecognizer.h"
#include <iostream>

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



size_t writeCallBack(const void *ptr, size_t size, size_t nitems, void *stream)
{
//    printf("tranlation result: %.*s\n",size*nitems,ptr);
    memcpy(stream, ptr, size*nitems);
    return size*nitems;
}

char* SpeechRecognizer::getSentenceTranslation(int16* data,int length)
{
    curl_slist* header=NULL;
    header=curl_slist_append(header,"Transfer-Encoding: chunked");
    header=curl_slist_append(header,"Content-Type: audio/x-wav;codec=pcm;bit=16;rate=16000");
    header=curl_slist_append(header,"Accept: text/plain");
    header=curl_slist_append(header,"Accept-Language: en-US");
    curl_easy_setopt(m_curlHandle,CURLOPT_HTTPHEADER,header);
    curl_easy_setopt(m_curlHandle,CURLOPT_URL,"https://dictation.nuancemobility.net/NMDPAsrCmdServlet/dictation?appId=NMDPTRIAL_wimxpliyou_gmail_com20160311004931&appKey=1b11e3f0620ade13e3490575c79b0d5d489c909a6a08366b7b691b685c954577a628670832461c158f218a56bd98b30534449f9e4cd509c73e25309aba751260&id=57349abd2395");
    CURLcode ret=curl_easy_setopt(m_curlHandle,CURLOPT_FOLLOWLOCATION,1);
    ret=curl_easy_setopt(m_curlHandle,CURLOPT_WRITEFUNCTION,writeCallBack);
    ret=curl_easy_setopt(m_curlHandle,CURLOPT_WRITEDATA,m_RecognitionResult);

    ret=curl_easy_setopt(m_curlHandle, CURLOPT_SSL_VERIFYHOST, 0L);
    ret=curl_easy_setopt(m_curlHandle, CURLOPT_SSL_VERIFYPEER, 0L);
    char* buff=new char[length*2+44];
    char waveHeader[44];
    genWaveHeader(waveHeader, 1, 16000, 16, length*2);
    memcpy(buff, waveHeader, 44);
    memcpy(buff+44,data,length*2);
    ret=curl_easy_setopt(m_curlHandle,CURLOPT_POSTFIELDS,buff);
    ret=curl_easy_setopt(m_curlHandle,CURLOPT_POSTFIELDSIZE,length*2+44);
    ret=curl_easy_perform(m_curlHandle);
    delete[] buff;
    std::cout<<"ret: "<<ret<<std::endl;

}

SpeechRecognizer::SpeechRecognizer(std::string modelRoot)
{
    std::cout<<modelRoot<<std::endl;
    m_curlHandle=curl_easy_init();
    arg_t arg_def[]={POCKETSPHINX_OPTIONS,CMDLN_EMPTY_OPTION};
    m_config=cmd_ln_parse_r(NULL,arg_def,0,NULL,FALSE);
    cmd_ln_set_str_r(m_config,"-hmm",(modelRoot+"/en-us/en-us").c_str());
    cmd_ln_set_str_r(m_config,"-dict",(modelRoot+"/en-us/cmudict-en-us").c_str());
    cmd_ln_set_boolean_r(m_config,"-allphone_ci",TRUE);
    cmd_ln_set_float64_r(m_config,"-kws_threshold",1e-20f);
    cmd_ln_set_str_r(m_config,"-logfn","/dev/null");

    m_ps=ps_init(m_config);
    ps_set_rawdata_size(m_ps,1000000);
    ps_set_jsgf_file(m_ps, "keywords", (modelRoot+"/en-us/keywords").c_str());
    ps_set_search(m_ps,"keywords");
    ps_start_utt(m_ps);
}

SpeechRecognizer::~SpeechRecognizer()
{
    curl_easy_cleanup(m_curlHandle);
    ps_free(m_ps);
    cmd_ln_free_r(m_config);
}

void SpeechRecognizer::setMode(bool iskeyword)
{
    m_bKeywordMode=iskeyword;
    if(iskeyword)
    {
        ps_set_search(m_ps, "keywords");
    }
    else
    {
        ps_set_search(m_ps, "default");
    }
}



char* SpeechRecognizer::Recognize(void *speechData, int frameCount)
{
    char* result="";
    ps_process_raw(m_ps, (const int16_t*)speechData, frameCount, FALSE, FALSE);
    int hasSpeech=ps_get_in_speech(m_ps);
    if (!hasSpeech) {
//        std::cout<<"recognizing...\n"<<std::endl;
        ps_end_utt(m_ps);
        int32 score=0;
        int16* data=NULL;
        int length=0;
        const char* word=ps_get_hyp(m_ps,&score);
        if(word)
        {
            std::cout<<"-------------recognition Result:"<< (word?word:"NULL")<<" ,score : "<<score<<std::endl;
            const char* currentSearch=ps_get_search(m_ps);
            std::cout<<"current search is "<<currentSearch<<std::endl;
            ps_get_rawdata(m_ps, &data, &length);
            std::cout<<"length : "<<length<<std::endl;
            getSentenceTranslation(data,length);
            result=m_RecognitionResult;
        }
        ps_start_utt(m_ps);
        
    }
    return result;
}
