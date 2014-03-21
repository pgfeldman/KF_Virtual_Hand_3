#pragma once

#include <string.h>

class UI_cmd
{
public:
	UI_cmd(void);
	~UI_cmd(void);

	bool isWaiting(){return _waiting;};
	bool isReady(){return !_waiting;};
	void setName(char *name){_name = name;};
	char* getName(){return _name;};
	void* getData(){return _data;};
	char* getCmdName(){return _cmdName;};
	bool equals(char* test){return (_strcmpi(_cmdName, test) == 0);};

	void set(char *name, void *data){
		_cmdName = name;
		_data = data;
		_waiting = true;
	}

	void clear(){
		_cmdName = NULL;
		_data = NULL;
		_waiting = false;
	}
		

private:
	void* _data;
	char* _cmdName;
	char* _name;
	bool _waiting;
};

