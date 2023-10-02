#pragma once

#include <Arduino.h>
#include <WiFiClient.h>
#include <vector>

class ArduinoTelnet;

class ShellConnection {
public:
    static const int LineSizeLimit = 1024;

    ShellConnection(ArduinoTelnet& shell, const WiFiClient& client, const char* firmwareInfo);

    bool loop();

    void handleTelnetCommand();

private:
    void processLine();

    void printHelp(Print& output, std::vector<std::string>& argv);

    void printWelcome();

    void printPrompt();

    void overwriteLineBuffer(const std::string&);

    ArduinoTelnet& m_shell;
    WiFiClient m_client;
    const char* m_firmwareInfo;
    bool m_gotTelnetCommand{false};
    std::string m_lineBuffer;
    std::string::iterator m_lineBufferIterator;
    std::vector<std::string> m_history;
    std::vector<std::string>::iterator m_historyIterator;
};
