#include "LZRecorder.h"
#include <assert.h>
#include "lzconfig.h"


const static char * appId = "6405f7ca0f9e4ae4a8dfb8ca41f56a14";

LZRecorder::LZRecorder() {
	
	recordingEngine_ = agora::recording::IRecordingEngine::createAgoraRecordingEngine(appId, this);
	assert(recordingEngine_ != NULL);

}


LZRecorder::~LZRecorder() {
	recordingEngine_->release();
}

void LZRecorder::joinChannel(const std::string & channel) {
	LzLogInfo("join channel:%s", channel.c_str());
	printf("join channel:%s", channel.c_str());
	channel_ = channel;

	RecordingConfig config;
	config.isAudioOnly = true;
	config.appliteDir = "../bin";
	config.isMixingEnabled = true;	
	bool ret = recordingEngine_->joinChannel(NULL, channel.c_str(), 0, config);

	assert(ret);

}

void LZRecorder::leaveChannel(){
	printf("leave channel:%s", channel_.c_str());
	assert(recordingEngine_);
	recordingEngine_->leaveChannel();
}


const std::string & LZRecorder::getChannel() const {
	return channel_;
}
/**
     *  Callback when an error occurred during the runtime of recording engine
     *
     *
     *  @param error        Error code
     *  @param stat_code    state code
     *
     */
 void LZRecorder::onError(int error, STAT_CODE_TYPE stat_code) {
	LzLogError("error:%d, stat_code:%d", error, stat_code);


}

     /**
     *  Callback when an warning occurred during the runtime of recording engine
     *
     *
     *  @param warn         warning code
     *
     */
     void LZRecorder::onWarning(int warn){

	LzLogWarn("warn:%d", warn);

}
   
     /**
     *  Callback when the user hase successfully joined the specified channel
     *
     *
     *  @param channelID    channel ID 
     *  @param uid          User ID
     *
     */
     void LZRecorder::onJoinChannelSuccess(const char * channelId, uid_t uid) {

	LzLogInfo("lzzjoin channel:%s success, uid:%d", channelId, uid);
}
   
    /**
     *  Callback when recording application successfully left the channel
     *
     *
     *  @param code        leave path code
     *
     */
     void LZRecorder::onLeaveChannel(LEAVE_PATH_CODE code){

	LzLogInfo("leave channel code:%d", code);

}

    /**
     *  Callback when another user successfully joined the channel
     *
     *
     *  @param uid          user ID
     *  @param infos        user join information    
     *
     */
     void LZRecorder::onUserJoined(uid_t uid, UserJoinInfos &infos){

}
   
     /**
     *  Callback when a user left the channel or gone offline
     *
     *
     *  @param uid          user ID
     *  @param reason       offline reason    
     *
     */
     void LZRecorder::onUserOffline(uid_t uid, USER_OFFLINE_REASON_TYPE reason){

}

     /**
     *  Callback when received a audio frame
     *
     *
     *  @param uid          user ID
     *  @param frame        pointer to received audio frame    
     *
     */
     void LZRecorder::audioFrameReceived(unsigned int uid, const AudioFrame *frame) const{


}

      /**
     *  Callback when received a video frame
     *
     *
     *  @param uid          user ID
     *  @param frame        pointer to received video frame    
     *
     */
     void LZRecorder::videoFrameReceived(unsigned int uid, const VideoFrame *frame) const {


}
