//
// Created by lordrafa on 31/01/26.
//

#include "genjo_soil_7in1.h"

#define GENJO_SOIL_TEMPMOIST_REG 0x06
#define GENJO_SOIL_MOIST_REG 0x07
#define GENJO_SOIL_EC_REG 0x08
#define GENJO_SOIL_PH_REG 0x09
#define GENJO_SOIL_NPK_REG 0x1E
#define GENJO_SOIL_P_REG 0x1F
#define GENJO_SOIL_K_REG 0x20

#define CRNL "\r\n"
#define TEMP_MOIST_HEAD "Temp(°C)-Moist(%RH): "
#define EC_HEAD "EC(us/cm): "
#define PH_HEAD "PH: "
#define NPK_HEAD "NPK(mg/kg): "

using namespace std;

string GenjoSoil7in1::getTempMoist() const {
    return TEMP_MOIST_HEAD + readMultiReg(GENJO_SOIL_TEMPMOIST_REG, 2) + CRNL;
}

string GenjoSoil7in1::getEC() const { return EC_HEAD + readSingleReg(GENJO_SOIL_EC_REG) + CRNL; }
string GenjoSoil7in1::getPH() const { return PH_HEAD + readSingleReg(GENJO_SOIL_PH_REG) + CRNL; }
string GenjoSoil7in1::getNPK() const { return NPK_HEAD + readMultiReg(GENJO_SOIL_NPK_REG, 3) + CRNL; }
// Private Functions

string GenjoSoil7in1::readSingleReg(const uint8_t reg) const {
    return readMultiReg(reg, 1);
}

string GenjoSoil7in1::readMultiReg(const uint8_t reg, const uint8_t n) const {
    if ((n < 1) || (n > 3)) return "ERROR: Invalid Argument.";

    uint8_t regs_value[6];
    const size_t nregs = _modbus_rtu.readHoldingReg(regs_value, sensor_add, reg, n);
    if (nregs == 0) return "ERROR: Communicating with sensor.";

    string ret = parse_modbus(reg, regs_value[0], regs_value[1]);

    for (int i = 2; i < (n * 2); i += 2) {
        ret += ", ";
        ret += parse_modbus(reg, regs_value[i], regs_value[i + 1]);
    }

    return ret;
}

string GenjoSoil7in1::parse_modbus(const uint8_t reg, const uint8_t reg_value_h, const uint8_t reg_value_l) {
    char buf[16];
    const uint16_t reg_value = (reg_value_h << 8) + reg_value_l;

    switch (reg) {
    case 0x06:
    case 0x09:
        snprintf(buf, sizeof(buf), "%d.%02d", reg_value / 100, reg_value % 100);
        return buf;
    case 0x08:
    case 0x1E:
        return to_string(reg_value);
    default:
        snprintf(buf, sizeof(buf), "[x%02X:x%02X]", reg_value_h, reg_value_l);
        return buf;
    }
}
