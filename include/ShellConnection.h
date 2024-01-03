#pragma once

#include <Arduino.h>
#include <WiFiClient.h>
#include <vector>
#include "ShellClient.h"

class ArduinoTelnet;

class ShellConnection {
public:
    static const int LineSizeLimit = 1024;

    ShellConnection(ArduinoTelnet& shell, std::shared_ptr<ShellClient> client, std::string firmwareInfo);

    bool loop();

    void handleTelnetCommand();

private:
    /**
     * Process one command.
     * Commands can be separated by ';'
     * @return The character index after the first encountered ';' or 0 if none was encountered
     */
    int processLine();

    void printHelp(Print& output, std::vector<std::string>& argv);

    void printWelcome();

    void printPrompt();

    void overwriteLineBuffer(const std::string&);

    ArduinoTelnet& m_shell;
    std::shared_ptr<ShellClient> m_client;
    std::string m_firmwareInfo;
    bool m_gotTelnetCommand{false};
    std::string m_lineBuffer;
    std::string::iterator m_lineBufferIterator;
    std::vector<std::string> m_history;
    std::vector<std::string>::iterator m_historyIterator;
};
