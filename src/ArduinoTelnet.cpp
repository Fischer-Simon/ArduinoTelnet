#include "ArduinoTelnet.h"
#include "ShellCommands.h"
#include "WiFi.h"
#include <Print.h>

ArduinoTelnet::ArduinoTelnet(int port, std::string firmwareInfo) :
m_firmwareInfo{std::move(firmwareInfo)} {
    m_server.begin(port);
    m_server.setNoDelay(true);
    addCommand("test", new TestCommand);
    addCommand("reset", new ResetCommand);
    addCommand("hex", new HexCommand);
}

void ArduinoTelnet::setFirmwareInfo(std::string firmwareInfo) {
    m_firmwareInfo = std::move(firmwareInfo);
}

void ArduinoTelnet::addCommand(const std::string& name, ShellCommand* command) {
    addCommand(name, std::shared_ptr<ShellCommand>(command));
}

void ArduinoTelnet::addCommand(const std::string& name, std::shared_ptr<ShellCommand> command) {
    m_commands.emplace(name, std::move(command));
}

void ArduinoTelnet::executeCommand(Stream& io, std::vector<std::string>& argv) {
    std::string commandName = argv.front();
    auto command = m_commands.find(commandName);
    if (command == m_commands.end()) {
        ArduinoTelnet::printCommandNotFound(io, commandName);
        return;
    }

    argv.erase(argv.begin());
    command->second->execute(io, commandName, argv);
}

void ArduinoTelnet::addClient(std::shared_ptr<ShellClient> client) {
    m_connections.emplace_back(*this, std::move(client), m_firmwareInfo);
}

void ArduinoTelnet::loop() {
    while (auto client = m_server.available()) {
        std::shared_ptr<ShellClient> shellClient{new WiFiShellClient{client}};
        m_connections.emplace_back(*this, std::move(shellClient), m_firmwareInfo);
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

size_t ArduinoTelnet::WiFiShellClient::write(uint8_t uint8) {
    return m_client.write(uint8);
}

int ArduinoTelnet::WiFiShellClient::available() {
    return m_client.available();
}

int ArduinoTelnet::WiFiShellClient::read() {
    return m_client.read();
}

int ArduinoTelnet::WiFiShellClient::peek() {
    return m_client.peek();
}

bool ArduinoTelnet::WiFiShellClient::active() {
    return m_client.connected();
}

void ArduinoTelnet::WiFiShellClient::quit() {
    m_client.stop();
}

size_t ArduinoTelnet::WiFiShellClient::write(const uint8_t* buffer, size_t size) {
    return m_client.write(buffer, size);
}

size_t ArduinoTelnet::WiFiShellClient::readBytes(char* buffer, size_t length) {
    return m_client.readBytes(buffer, length);
}

size_t ArduinoTelnet::WiFiShellClient::readBytes(uint8_t* buffer, size_t length) {
    return m_client.readBytes(buffer, length);
}
