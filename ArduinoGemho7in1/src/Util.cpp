//
// Created by lordrafa on 3/02/26.
//

#include <genjo_soil_7in1.h>

#include "Util.h"

#include <ArduinoRS485Channel.h>

using namespace std;

void Util::registerSensorCmds(AT_Console& console, GenjoSoil7in1& genjo_soil7_in1) {
    console.registerCmd(
        "AT+TEMP+MOIST?",
        [genjo_soil7_in1] { return genjo_soil7_in1.getTempMoist(); },
        "Read Temperature and Moist (°C - %RH)"
    );
    console.registerCmd(
        "AT+EC?",
        [genjo_soil7_in1] { return genjo_soil7_in1.getEC(); },
        "Read Conductivity (us/cm)"
    );
    console.registerCmd(
        "AT+PH?",
        [genjo_soil7_in1] { return genjo_soil7_in1.getPH(); },
        "Read pH"
    );
    console.registerCmd(
        "AT+NPK?",
        [genjo_soil7_in1] { return genjo_soil7_in1.getNPK(); },
        "Read Nitrogen N (mg/kg)"
    );
}

void Util::registerDebugCmd(AT_Console& console, ArduinoRS485Channel& arduinoRS485Channel) {
    vector<string> args_names;
    args_names.emplace_back("X");
    console.registerCmd(
        "AT+RS485DEBUG",
        [&arduinoRS485Channel](const string&, const std::vector<string>& args) {
            if (!validateBinStr(args[0])) return "ERROR: Invalid argument value (use 1 or 0)";
            arduinoRS485Channel.set_debug_en(args[0] == "1");
            return "";
        },
        "Enables RS485 DEBUG traces (0/1).", args_names);
}

bool Util::validateBinStr(const string& binStr) {
    if (binStr == "0") return true;
    if (binStr == "1") return true;
    return false;
}
