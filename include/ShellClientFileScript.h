#pragma once

#ifdef ARDUINO_TELNET_ENABLE_SCRIPT_SHELL_CLIENT

#include "ShellClient.h"

#include <cstdio>

class ShellClientFileScript : public ShellClient {
public:
    explicit ShellClientFileScript(const char* filename) {
        m_file = fopen(filename, "r");
    }

    ~ShellClientFileScript() override {
        if (m_file) {
            fclose(m_file);
        }
        m_file = nullptr;
    }

    size_t write(uint8_t uint8) override {
        return 1;
    }

    int available() override {
        fillBuffer();
        return static_cast<int>(m_bufferFill);
    }

    int read() override {
        int value = -1;
        fillBuffer();
        if (m_bufferFill) {
            value = m_buffer[m_bufferReadPos];
            m_bufferFill--;
            m_bufferReadPos++;
        }
        return value;
    }

    int peek() override {
        int value = -1;
        fillBuffer();
        if (m_bufferFill) {
            value = m_buffer[m_bufferReadPos];
        }
        return value;
    }

    bool active() override {
        return m_file != nullptr;
    }

    void quit() override {
        if (m_file) {
            fclose(m_file);
        }
        m_file = nullptr;
    }

private:
    void fillBuffer() {
        if (m_bufferFill > 0 || !m_file) {
            return;
        }
        m_bufferFill = fread(m_buffer, 1, sizeof(m_buffer), m_file);
        if (m_bufferFill == 0) {
            fclose(m_file);
            m_file = nullptr;
        }
        m_bufferReadPos = 0;
    }

    FILE* m_file{nullptr};
    size_t m_bufferFill{0};
    size_t m_bufferReadPos{0};
    uint8_t m_buffer[256]{0};
};

#endif
