#include "LZRecorderMgr.h"
#include "LZRecorder.h"
#include "lzconfig.h"


LZRecorderMgr::LZRecorderMgr() {


}


LZRecorderMgr::~LZRecorderMgr(){
	stopRecordAll();
}



void LZRecorderMgr::startRecord(const std::string & channel){
	if (channel.length() > 0) {
		for (auto it = recorderList_.cbegin(); it != recorderList_.cend(); ++ it) {

			if (channel.compare((*it)->getChannel()) == 0) {
				LzLogWarn("warn: start record same channel, %s",channel.c_str());
				return;
			}
		}
		LZRecorder * recorder = new LZRecorder();
		recorder->joinChannel(channel);
		recorderList_.push_back(recorder);
	}
}


void LZRecorderMgr::stopRecord(const std::string & channel){
	if (channel.length() > 0) {
		for (auto it = recorderList_.cbegin(); it != recorderList_.cend(); ++ it) {

			if (channel.compare((*it)->getChannel()) == 0) {
				LZRecorder * tmp = *it;
				tmp->leaveChannel();
				delete tmp;
				recorderList_.erase(it);
				return;
			}
		}
		LzLogWarn("warn: no such channel:%s", channel.c_str());
	}
}
void LZRecorderMgr::stopRecordAll(){
	for (auto it = recorderList_.cbegin(); it != recorderList_.cend(); ++ it) {
		(*it)->leaveChannel();
		delete (*it);
	}
	recorderList_.clear();
}

void LZRecorderMgr::stopRecordLast(){
	if (!recorderList_.empty()) {
		LZRecorder * tmp = recorderList_.back();
		recorderList_.pop_back();
		tmp->leaveChannel();
		delete tmp;
	}
	else {
		printf("no record anymore");
	}
}

void LZRecorderMgr::getAllRecordChannel(std::list<std::string> & retList) const {
	for (auto it = recorderList_.cbegin(); it != recorderList_.cend(); ++ it) {
		retList.push_back(std::string((*it)->getChannel()));
	}
}
