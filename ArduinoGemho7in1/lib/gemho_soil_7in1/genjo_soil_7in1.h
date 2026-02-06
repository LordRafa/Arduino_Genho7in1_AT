//
// Created by lordrafa on 30/01/26.
//

#ifndef GENJO_SOIL_7IN1_H
#define GENJO_SOIL_7IN1_H

#include <string>
#include <modbus_rtu.h>

class GenjoSoil7in1 {
    using string = std::string;

    ModbusRTU _modbus_rtu;
    const uint8_t sensor_add = 1;

    string readSingleReg(uint8_t reg) const;
    string readMultiReg(uint8_t reg, uint8_t n) const;
    static string parse_modbus(uint8_t reg, uint8_t reg_value_h, uint8_t reg_value_l);

public:
    explicit GenjoSoil7in1(const ModbusRTU& modbus_rtu): _modbus_rtu(modbus_rtu) {}

    string getTempMoist() const;
    string getEC() const;
    string getPH() const;
    string getNPK() const;
};

#endif //GENJO_SOIL_7IN1_H
