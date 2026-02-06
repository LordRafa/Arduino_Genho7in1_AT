//
// Created by lordrafa on 30/01/26.
//

#include "at_console.h"

#include <sstream>

#define MIN_PADDING 3

using namespace std;

static const string AT_OK = "OK";

static const string AT_ERR = "ERROR";
static const string AT_ERR_LINE_TOO_LONG = "LINE_TOO_LONG";
static const string AT_ERR_BAD_CMD = "BAD_CMD";
static const string AT_ERR_UNKNOWN_CMD = "UNKNOWN_CMD";

static const string AT_HELP_BASE = "\r\nAT command interface:\r\n";


AT_Console::AT_Console(Channel& channel, const bool echo_en): _channel(channel), _echo_en(echo_en) {
    _cmds.clear();
    registerCmd("AT", [this] { return ""; }, "ping");
    registerCmd("AT?", [this] { return handleATHelpCmd(); }, "show help");
    println("GenjoSoil7in1 AT Console (type AT? for help)");
}

void AT_Console::registerCmd(const string& cmd_name, const AT_Cmd_Handler_Simple& handler, const string& desc) {
    if (cmd_name.length() > _max_cmd_name_lenght) _max_cmd_name_lenght = cmd_name.length();
    _cmd_keys.push_back(cmd_name);
    const AT_Cmd_Handler h = [this, handler](const string&, const std::vector<string>&) { return handler(); };
    const vector<string> args_names;
    _cmds[cmd_name] = {cmd_name, h, desc, 0};
}

void AT_Console::registerCmd(const string& cmd_name, const AT_Cmd_Handler& handler, const string& desc,
                             const vector<string>& args_name) {
    const string name_printable = cmd_name + (args_name.empty() ? "" : "=" + join(args_name, ","));
    if (name_printable.length() > _max_cmd_name_lenght) _max_cmd_name_lenght = name_printable.length();
    _cmd_keys.push_back(cmd_name);
    _cmds[cmd_name] = {name_printable, handler, desc, args_name.size()};
}

void AT_Console::pool() {
    while (_channel.available()) {
        const char ch = static_cast<char>(_channel.read());
        if (_echo_en) _channel.write(ch);
        if (ch == '\r') continue; // ignore CR
        if ((ch == '\n') || (ch == ';')) {
            string cmd = _cmd_buffer;
            _cmd_buffer = ""; // Free the buffer ASAP in case cmd crashes
            handleATCmd(cmd);
        }
        else {
            if (_cmd_buffer.length() < 120) _cmd_buffer += ch;
            else {
                _cmd_buffer = "";
                atError(AT_ERR_LINE_TOO_LONG);
            }
        }
    }
}

// Private Methods

string AT_Console::handleATHelpCmd() const {
    auto help = string(AT_HELP_BASE);
    for (const auto& cmd_key : _cmd_keys) {
        const Cmd cmd_info = _cmds.at(cmd_key);
        help += cmd_info.name_printable;
        help += getPadding(cmd_info.name_printable);
        help += "- ";
        help += cmd_info.desc;
        help += "\r\n";
    }
    return help;
}

string AT_Console::getPadding(const string& cmd_name) const {
    string padding(_max_cmd_name_lenght - cmd_name.length() + MIN_PADDING, ' ');
    return padding;
}

void AT_Console::atError(const string& msg) const {
    string errStr = AT_ERR;
    if (!msg.empty()) {
        errStr += ": " + msg;
    }
    println(errStr);
}

void AT_Console::handleATCmd(const string& cmd) {
    if (cmd.find("AT") != 0) {
        atError(AT_ERR_BAD_CMD);
        return;
    }

    string token = cmd;
    string args;

    const size_t eq = cmd.find('=');
    if (eq != string::npos) {
        token = cmd.substr(0, eq);
        args = cmd.substr(eq + 1);
    }

    const auto it = _cmds.find(token);
    if (it == _cmds.end()) {
        atError(AT_ERR_UNKNOWN_CMD);
        return;
    }

    const vector<string> args_v = splitArgs(args);
    if (args_v.size() != it->second.n_args) {
        atError(AT_ERR_BAD_CMD);
        return;
    }
    const string cmd_output = it->second.handler(cmd, args_v);

    print(cmd_output);
    if (cmd_output.rfind("ERROR", 0) == 0) println("");
    else println(AT_OK);
}

vector<string> AT_Console::splitArgs(const string& s) {
    vector<string> args;

    if (s.empty()) return args;

    stringstream ss(s);
    string token;

    while (getline(ss, token, ',')) {
        args.push_back(token);
    }

    return args;
}

void AT_Console::print(const string& str) const {
    _channel.write(reinterpret_cast<const uint8_t*>(str.data()), str.length());
    _channel.flush();
}

void AT_Console::println(const string& str) const {
    print(str + "\r\n");
}


std::string AT_Console::join(const std::vector<std::string>& v, const std::string& sep) {
    std::string result;
    for (size_t i = 0; i < v.size(); ++i) {
        if (i > 0) result += sep;
        result += v[i];
    }
    return result;
}
