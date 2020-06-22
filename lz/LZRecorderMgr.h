#include <string>
#include <list>


class LZRecorder;

class LZRecorderMgr
{
public:
	LZRecorderMgr();
	~LZRecorderMgr();
	void startRecord(const std::string & channel);
    void stopRecord(const std::string & channel);
    void stopRecordAll();
    void stopRecordLast();
	void getAllRecordChannel(std::list<std::string> & retList) const;
protected:
	std::list<LZRecorder*> recorderList_;
};
