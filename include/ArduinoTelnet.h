#pragma once

#include <WiFiServer.h>
#include <Print.h>
#include <vector>
#include <string>
#include <memory>
#include <map>
#include <list>
#include <sstream>

#include "ShellConnection.h"
#include "ShellCommand.h"
#include "ShellCommandGroup.h"

class ArduinoTelnet {
    friend class ShellConnection;

public:
    ArduinoTelnet(int port, const char* firmwareInfo = nullptr);

    void addCommand(const std::string& name, ShellCommand* command);

    void executeCommand(Print& output, std::vector<std::string>& argv);

    void loop();

    static void printCommandNotFound(Print& output, const std::string& commandName);

    static void printUsage(Print& output, const std::string& commandName, const ShellCommand& command);

private:
    const char* m_firmwareInfo = nullptr;
    std::map<std::string, std::unique_ptr<ShellCommand>> m_commands;
    WiFiServer m_server;
    std::list<ShellConnection> m_connections;
};
