import re
import time
import serial
from typing import List, Optional, Tuple


PORT = "/dev/ttyACM0"   # change me
BAUD = 115200
TIMEOUT = 2.0


RE_TEMP_MOIST = re.compile(r"Temp\(°C\)-Moist\(%RH\):\s*([-+]?\d+(?:\.\d+)?)\s*,\s*([-+]?\d+(?:\.\d+)?)")
RE_EC         = re.compile(r"EC\(us/cm\):\s*([-+]?\d+)")
RE_PH         = re.compile(r"PH:\s*([-+]?\d+(?:\.\d+)?)")
RE_NPK        = re.compile(r"NPK\(mg/kg\):\s*([-+]?\d+)\s*,\s*([-+]?\d+)\s*,\s*([-+]?\d+)")


class ATSensor:
    def __init__(self, port: str, baud: int = 115200, timeout: float = 2.0):
        self.ser = serial.Serial(port, baudrate=baud, timeout=timeout)
        time.sleep(2)  # Arduino auto-reset

    def close(self):
        self.ser.close()

    def _send_cmd(self, cmd: str, overall_timeout: float = 5.0) -> List[str]:
        """
        Send AT command and read lines until OK (or ERROR / timeout).
        Returns list of lines excluding final OK/ERROR.
        """
        self.ser.reset_input_buffer()
        self.ser.write((cmd + "\r\n").encode("utf-8"))

        lines: List[str] = []
        t0 = time.monotonic()

        while True:
            if time.monotonic() - t0 > overall_timeout:
                raise TimeoutError(f"Timeout waiting for response to: {cmd}")

            raw = self.ser.readline()
            if not raw:
                continue

            line = raw.decode(errors="ignore").strip()
            if not line:
                continue

            # Many firmwares echo the command back; ignore it
            if line == cmd:
                continue

            if line == "OK":
                break
            if line.startswith("ERROR"):
                raise RuntimeError(f"{cmd} -> {line}")

            lines.append(line)

        return lines

    def ping(self) -> List[str]:
        # Some firmwares respond with 'AT' echo then OK; some respond 'OK' only.
        return self._send_cmd("AT")

    def help(self) -> List[str]:
        return self._send_cmd("AT?")

    def enable_rs485_debug(self, enable: bool) -> List[str]:
        return self._send_cmd(f"AT+RS485DEBUG={1 if enable else 0}")

    def read_temp_moist(self) -> Tuple[float, float]:
        lines = self._send_cmd("AT+TEMP+MOIST?")
        for l in lines:
            m = RE_TEMP_MOIST.search(l)
            if m:
                return float(m.group(1)), float(m.group(2))
        raise ValueError(f"Could not parse TEMP+MOIST from lines: {lines}")

    def read_ec(self) -> int:
        lines = self._send_cmd("AT+EC?")
        for l in lines:
            m = RE_EC.search(l)
            if m:
                return int(m.group(1))
        raise ValueError(f"Could not parse EC from lines: {lines}")

    def read_ph(self) -> float:
        lines = self._send_cmd("AT+PH?")
        for l in lines:
            m = RE_PH.search(l)
            if m:
                return float(m.group(1))
        raise ValueError(f"Could not parse PH from lines: {lines}")

    def read_npk(self) -> Tuple[int, int, int]:
        lines = self._send_cmd("AT+NPK?")
        for l in lines:
            m = RE_NPK.search(l)
            if m:
                return int(m.group(1)), int(m.group(2)), int(m.group(3))
        raise ValueError(f"Could not parse NPK from lines: {lines}")


if __name__ == "__main__":
    sensor = ATSensor(PORT, BAUD, TIMEOUT)
    try:
        print("Ping:", sensor.ping())

        # Optional
        sensor.enable_rs485_debug(True)

        temp, moist = sensor.read_temp_moist()
        ec = sensor.read_ec()
        ph = sensor.read_ph()
        n, p, k = sensor.read_npk()

        print("\n--- Sensor readings ---")
        print(f"Temperature: {temp:.2f} °C")
        print(f"Moisture:    {moist:.2f} %")
        print(f"EC:          {ec} us/cm")
        print(f"pH:          {ph:.2f}")
        print(f"NPK:         N={n} P={p} K={k} mg/kg")
    finally:
        sensor.close()

