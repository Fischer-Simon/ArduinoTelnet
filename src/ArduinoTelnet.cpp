#include "ArduinoTelnet.h"
#include "ShellCommands.h"
#include "WiFi.h"
#include <Print.h>

ArduinoTelnet::ArduinoTelnet(int port, const char* firmwareInfo) {
    m_server.begin(port);
    m_server.setNoDelay(true);
    m_firmwareInfo = firmwareInfo;
    addCommand("test", new TestCommand);
    addCommand("reset", new ResetCommand);
    addCommand("hex", new HexCommand);
}

void ArduinoTelnet::addCommand(const std::string& name, ShellCommand* command) {
    m_commands.emplace(name, std::move(std::unique_ptr<ShellCommand>(command)));
}

void ArduinoTelnet::executeCommand(Print& output, std::vector<std::string>& argv) {
    std::string commandName = argv.front();
    auto command = m_commands.find(commandName);
    if (command == m_commands.end()) {
        ArduinoTelnet::printCommandNotFound(output, commandName);
        return;
    }

    argv.erase(argv.begin());
    command->second->execute(output, commandName, argv);
}

void ArduinoTelnet::loop() {
    while (auto client = m_server.available()) {
        m_connections.emplace_back(*this, client, m_firmwareInfo);
    }
    auto i = m_connections.begin();
    while (i != m_connections.end()) {
        auto& connection = *i;
        if (!connection.loop()) {
            i = m_connections.erase(i);
            continue;
        }
        i++;
    }
}

void ArduinoTelnet::printCommandNotFound(Print& output, const std::string& commandName) {
    output.print(WiFiClass::getHostname());
    output.print(": ");
    output.print(commandName.c_str());
    output.println(": command not found");
}

void ArduinoTelnet::printUsage(Print& output, const std::string& commandName, const ShellCommand& command) {
    output.print("Usage: ");
    output.print(commandName.c_str());
    output.print(' ');
    command.printUsage(output);
}

