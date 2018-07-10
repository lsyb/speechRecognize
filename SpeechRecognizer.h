//
//  SpeechRecognizer.h
//  HelloWorldDemo
//
//  Created by BudsPoint on 16/3/17.
//
//

#ifndef __HelloWorldDemo__SpeechRecognizer__
#define __HelloWorldDemo__SpeechRecognizer__

#include <stdio.h>
#include <curl/curl.h>
#include <pocketsphinx/pocketsphinx.h>
#include <string>



class SpeechRecognizer
{
public:
    SpeechRecognizer(std::string modelRoot);
    ~SpeechRecognizer();
    
public:
    void setMode(bool isKeyword);
    char* Recognize(void* speechData,int frameCount);
    
private:
    char* getSentenceTranslation(int16* data,int length);
private:
    ps_decoder_t* m_ps;
    CURL* m_curlHandle;
    cmd_ln_t* m_config;
    bool m_bKeywordMode;
    int m_waveDataLength;
    char m_RecognitionResult[1024];
};


#endif /* defined(__HelloWorldDemo__SpeechRecognizer__) */
