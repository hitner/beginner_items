#include <string>
#include "IAgoraRecordingEngine.h"

using namespace agora::recording;


class LZRecorder : public agora::recording::IRecordingEngineEventHandler
{
public:
	LZRecorder();
	~LZRecorder();

	void joinChannel(const std::string & channel);
    void leaveChannel();
	const std::string & getChannel() const;
protected:
  virtual void onError(int error, agora::recording::STAT_CODE_TYPE stat_code);
  virtual void onWarning(int warn);

  virtual void onJoinChannelSuccess(const char * channelId, uid_t uid);
  virtual void onLeaveChannel(agora::recording::LEAVE_PATH_CODE code);

  virtual void onUserJoined(uid_t uid, agora::recording::UserJoinInfos &infos);
  virtual void onUserOffline(uid_t uid, agora::recording::USER_OFFLINE_REASON_TYPE reason);

  virtual void audioFrameReceived(unsigned int uid, const AudioFrame *frame) const;
  virtual void videoFrameReceived(unsigned int uid, const VideoFrame *frame) const;
protected:
	std::string channel_;
	agora::recording::IRecordingEngine * recordingEngine_;

};
