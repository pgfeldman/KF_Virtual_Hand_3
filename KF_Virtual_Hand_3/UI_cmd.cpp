#include "StdAfx.h"
#include "UI_cmd.h"


UI_cmd::UI_cmd(void)
{
	_waiting = false;
	_data = NULL;
	_name = "unset";
	_cmdName = NULL;
}


UI_cmd::~UI_cmd(void)
{
}
