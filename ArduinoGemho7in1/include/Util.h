//
// Created by lordrafa on 3/02/26.
//

#ifndef UTIL_H
#define UTIL_H

#include <ArduinoRS485Channel.h>
#include <at_console.h>
#include <genjo_soil_7in1.h>

class Util final {
    static bool validateBinStr(const std::string& binStr);

public:
    Util() = delete;
    ~Util() = default;

    static void registerSensorCmds(AT_Console& console, GenjoSoil7in1& genjo_soil7_in1);
    static void registerDebugCmd(AT_Console& console, ArduinoRS485Channel& arduinoRS485Channel);
};

#endif //UTIL_H
