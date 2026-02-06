//
// Created by lordrafa on 30/01/26.
//

#ifndef AT_CONSOLE_H
#define AT_CONSOLE_H

#include <functional>
#include <map>
#include <string>
#include <vector>

#include <channel.h>

// Simple AT command interface over Serial.
// Commands accepts ending with '\n' (or '\r\n') and ';'.
class AT_Console {
    using string = std::string;
    using AT_Cmd_Handler = std::function<string(const string& fullLine, const std::vector<string>& args)>;
    using AT_Cmd_Handler_Simple = std::function<string()>;

    struct Cmd {
        string name_printable;
        AT_Cmd_Handler handler;
        string desc;
        size_t n_args;
    };

    Channel& _channel;
    bool _echo_en;

    string _cmd_buffer;
    std::map<string, Cmd> _cmds;
    std::vector<string> _cmd_keys;

    size_t _max_cmd_name_lenght = 0;

    string handleATHelpCmd() const;

    void atError(const string& msg = "") const;
    void handleATCmd(const string& cmd);

    inline void print(const string& str) const;
    inline void println(const string& str) const;
    static std::string join(const std::vector<std::string>& v, const std::string& sep);
    string getPadding(const string& cmd_name) const;
    static std::vector<string> splitArgs(const string& s);

public:
    explicit AT_Console(Channel& channel, bool echo_en = true);
    void pool();
    void registerCmd(const string& cmd_name, const AT_Cmd_Handler& handler, const string& desc,
                     const std::vector<string>& args_name);
    void registerCmd(const string& cmd_name, const AT_Cmd_Handler_Simple& handler, const string& desc);
};

#endif //AT_CONSOLE_H
