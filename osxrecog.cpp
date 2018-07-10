//
//  main.cpp
//  VoiceRecognition
//
//  Created by lsyb on 3/16/16.
//  Copyright © 2016 lsyb. All rights reserved.
//

#include <iostream>
#include <AudioToolbox/AudioQueue.h>
#include <fstream>
#include <curl/curl.h>
#include "pocketsphinx/pocketsphinx.h"
#include "./SpeechRecognizer.h"




void recordCallback(void * __nullable               inUserData,
                               AudioQueueRef                   inAQ,
                               AudioQueueBufferRef             inBuffer,
                               const AudioTimeStamp *          inStartTime,
                               UInt32                          inNumberPacketDescriptions,
                               const AudioStreamPacketDescription * __nullable inPacketDescs)
{
    SpeechRecognizer* reg=(SpeechRecognizer*)inUserData;
    AudioQueuePause(inAQ);
    reg->Recognize(inBuffer->mAudioData,inBuffer->mAudioDataByteSize/2);
    AudioQueueStart(inAQ,nullptr);
    AudioQueueEnqueueBuffer(inAQ, inBuffer, 0, nullptr);

}


void playCallback(
    void * __nullable       inUserData,
    AudioQueueRef           inAQ,
    AudioQueueBufferRef     inBuffer)
{
  std::cout<<"from playCallback"<<std::endl;
  inBuffer->mAudioDataByteSize=inBuffer->mAudioDataBytesCapacity;
  AudioQueueEnqueueBuffer(inAQ, inBuffer, 0, nullptr);
}



int main(int argc, char * argv[]) {
  // insert code here...

  SpeechRecognizer reg("./model");
  AudioQueueRef recordeQueue;
  AudioQueueRef playQueue;
  AudioStreamBasicDescription recordASBD,playASBD;
  recordASBD.mSampleRate=16000;
  recordASBD.mBitsPerChannel=16;
  recordASBD.mChannelsPerFrame=1;
  recordASBD.mFormatID=kAudioFormatLinearPCM;
  recordASBD.mFramesPerPacket=1;
  recordASBD.mBytesPerFrame=recordASBD.mBitsPerChannel*recordASBD.mChannelsPerFrame/8;
  recordASBD.mBytesPerPacket=recordASBD.mFramesPerPacket*recordASBD.mBytesPerFrame;
  recordASBD.mFormatFlags=kAudioFormatFlagIsSignedInteger;

  playASBD=recordASBD;

  AudioQueueNewInput(&recordASBD, recordCallback, &reg, nullptr, nullptr, 0, &recordeQueue);
  AudioQueueNewOutput(&playASBD, playCallback, nullptr, nullptr, nullptr, 0, &playQueue);

  AudioQueueBufferRef recordBuff[3],playBuffer[3];
  for (int n=0; n<3; n++) {
    AudioQueueAllocateBuffer(recordeQueue,4096,&recordBuff[n]);
    AudioQueueEnqueueBuffer(recordeQueue, recordBuff[n], 0, nullptr);
    AudioQueueAllocateBuffer(playQueue, 4096, &playBuffer[n]);
    playBuffer[n]->mAudioDataByteSize=playBuffer[n]->mAudioDataBytesCapacity;
    AudioQueueEnqueueBuffer(playQueue, playBuffer[n], 0, nullptr);
  }

  AudioQueueStart(recordeQueue, nullptr);
  //    AudioQueueStart(playQueue, nullptr);
  while (1) {
    sleep(5);
    std::cout<<"from mainthread"<<std::endl;
  }
  return 0;
}
