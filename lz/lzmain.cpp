
#include "LZRecorderMgr.h"
#include "lzconfig.h"
#include "stdio.h"
#include "string.h"

using namespace agora;

const char * room1 = "VOICECHATROOM_80450220";
const char * room2 = "VOICECHATROOM_1505997";

void parseOperation(const char * input, LZRecorderMgr & mgr);

int main() {
	LzLogInfo("hello word");
	LZRecorderMgr manager;
	char input[200];
	while(1) {
		char * s = fgets(input, 199, stdin);
		if (s) {
			int len = strlen(input);
			if (len > 0) {
				if (input[len-1] == '\n' || input[len-1] == ' ') {
					input[len-1] = '\0';
				}
				parseOperation(input, manager);
			}
		}
	}
}

void parseOperation(const char * input, LZRecorderMgr & mgr){
	int len = strlen(input);
	if (strncmp(input, "list", 4) == 0) {
		std::list<std::string> result;
		mgr.getAllRecordChannel(result);
		for (auto it = result.cbegin(); it != result.cend(); ++ it) {
			printf("%s\n", (*it).c_str());
		}
		return;
	}

	if (strncmp(input, "join", 4) == 0) {
		mgr.startRecord(std::string(input+5, len-5));
		return;
	}

	if (strncmp(input, "leave", 5) == 0) {
		if (len == 5) {
			mgr.stopRecordLast();
		}
		else {
			mgr.stopRecord(std::string(input+6, len-6));
		}
		return;
	}
}
