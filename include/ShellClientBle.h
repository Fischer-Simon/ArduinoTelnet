#pragma once

#ifdef ARDUINO_TELNET_ENABLE_BLE_SHELL_CLIENT

#include "ShellClient.h"

#include <memory>
#include <ArduinoBLE.h>

template <typename T, size_t Size>
class RingBuffer {
public:
    RingBuffer() = default;

    bool push(const T* val, int size) {
        if (m_bufferFill + size >= Size) {
            return false;
        }
        memcpy(m_buffer + m_bufferFill, val, size);
        m_bufferFill += size;
        return true;
    }

    bool pop(int size) {
        if (m_bufferFill < size) {
            return false;
        }
        memmove(m_buffer, m_buffer + size, m_bufferFill - size);
        m_bufferFill -= size;
        return true;
    }

    const T* data() {
        return m_buffer;
    }

    int available() {
        return m_bufferFill;
    }

    void clear() {
        m_bufferFill = 0;
    }

private:
    T m_buffer[Size]{0};
    int m_bufferFill{0};
};

class ShellClientBle : public ShellClient {
public:
    static std::shared_ptr<ShellClientBle> getInstance(BLEService& bleService) {
        if (s_instance) {
            return s_instance;
        }
        s_instance.reset(new ShellClientBle{bleService});
        return s_instance;
    }

    size_t write(uint8_t uint8) override {
        while (!m_txBuffer.push(&uint8, 1)) {
            loop();
            BLE.poll(0);
        }
        return 1;
    }

    int available() override {
        if (!m_rxBuffer.available()) {
            BLE.poll(0);
        }
        return m_rxBuffer.available();
    }

    int read() override {
        int value = -1;
        if (!m_rxBuffer.available()) {
            BLE.poll(0);
        }
        if (m_rxBuffer.available()) {
            value = m_rxBuffer.data()[0];
            m_rxBuffer.pop(1);
        }
        return value;
    }

    int peek() override {
        int value = -1;
        if (!m_rxBuffer.available()) {
            BLE.poll(0);
        }
        if (m_rxBuffer.available()) {
            value = m_rxBuffer.data()[0];
        }
        return value;
    }

    bool active() override {
        return true;
    }

    void quit() override {
    }

    void loop() {
        if (!m_txBuffer.available()) {
            return;
        }
        int writeSize = std::min(m_txCharacteristic.valueSize(), m_txBuffer.available());
        if (m_txCharacteristic.writeValue(m_txBuffer.data(), writeSize)) {
            m_txBuffer.pop(writeSize);
        }
    }

    void onCommandEnd() override {
        while (m_txBuffer.available()) {
            loop();
            BLE.poll(0);
        }
        while (!m_txCharacteristic.writeValue(m_txBuffer.data(), 0)) {
            BLE.poll(1);
        }
    }

private:
    explicit ShellClientBle(BLEService& bleService) :
            m_rxCharacteristic{"f72bac71-f66f-4cce-b83f-a4218f482706", BLEWrite | BLENotify, 128},
            m_txCharacteristic{"f72bac71-f66f-4cce-b83f-a4218f482707", BLEWrite | BLENotify, 128},
            m_writeTimeout{millis() + 1000}{
        bleService.addCharacteristic(m_rxCharacteristic);
        bleService.addCharacteristic(m_txCharacteristic);
        m_rxCharacteristic.setEventHandler(BLEWritten, &onRxCharacteristicWritten);
    }

    void onRxCharacteristicWritten() {
        auto valueLength = m_rxCharacteristic.valueLength();

        auto value = m_rxCharacteristic.value();
        m_rxBuffer.push(value, valueLength);
    }

    static void onRxCharacteristicWritten(BLEDevice, BLECharacteristic) {
        s_instance->onRxCharacteristicWritten();
    }

    RingBuffer<uint8_t, 256> m_rxBuffer;
    RingBuffer<uint8_t, 256> m_txBuffer;
    BLECharacteristic m_rxCharacteristic;
    BLECharacteristic m_txCharacteristic;

    unsigned long m_writeTimeout;

    static std::shared_ptr<ShellClientBle> s_instance;
};

#endif
