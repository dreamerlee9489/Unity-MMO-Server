#include "console_cmd_app.h"
#include "message_system_help.h"

void ConsoleCmdApp::RegisterHandler()
{
    OnRegisterHandler("-info", BindFunP1(this, &ConsoleCmdApp::HandleAppInfo));
}

void ConsoleCmdApp::HandleHelp()
{
    std::cout << "\t-info.\t\tshow information of app." << std::endl;
}

void ConsoleCmdApp::HandleAppInfo(std::vector<std::string>& params)
{
    Net::CmdApp cmdProto;
    cmdProto.set_cmd_type(Net::CmdApp_CmdType_Info);
    MessageSystemHelp::DispatchPacket(Net::MsgId::MI_CmdApp, cmdProto, nullptr);
}
