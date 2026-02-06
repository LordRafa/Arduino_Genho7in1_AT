/*
  RP2040 (Raspberry Pi Pico) - Arduino-Pico core
  - USB UART menu on Serial
  - Sensor RS485/TTL UART on UART0 via Serial1 (TX=GP0, RX=GP1 by default)

  Notes:
  - This sketch assumes you already have a proper RS485 transceiver wired.
    If your RS485 module has DE/RE pins, you MUST toggle them for TX/RX.
    See RS485_DE_PIN section below.
  - Sensor protocol per manual: 9600 8N1, CRC16 (poly 0xA001), CRC bytes low then high.
*/

#include <Arduino.h>
#include <at_console.h>
#include <genjo_soil_7in1.h>

#include "Util.h"
#include "ArduinoSerialChannel.h"
#include "ArduinoRS485Channel.h"

using namespace std;

static constexpr uint32_t USB_BAUD = 115200;
static constexpr uint32_t SENSOR_BAUD = 9600;

static constexpr uint32_t DE_PIN = 2;

// ---------- Serial configuration ----------

AT_Console* console;
GenjoSoil7in1* genjo_soil7_in1;

ArduinoSerialChannel* usbSerialChannel;
ArduinoRS485Channel* arduinoRS485Channel;
ModbusRTU* modbus_rtu;

void setup() {
    while (!Serial) delay(10);
    Serial.begin(USB_BAUD);
    usbSerialChannel = new ArduinoSerialChannel(Serial);
    console = new AT_Console(*usbSerialChannel);

    Serial1.begin(SENSOR_BAUD);
    arduinoRS485Channel = new ArduinoRS485Channel(Serial1, Serial, DE_PIN);
    modbus_rtu = new ModbusRTU(*arduinoRS485Channel);
    genjo_soil7_in1 = new GenjoSoil7in1(*modbus_rtu);

    Util::registerSensorCmds(*console, *genjo_soil7_in1);

    Util::registerDebugCmd(*console, *arduinoRS485Channel);
}

void loop() {
    console->pool();
}
