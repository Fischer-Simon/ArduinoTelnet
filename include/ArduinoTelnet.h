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
    explicit ArduinoTelnet(int port, std::string firmwareInfo = "");

    void setFirmwareInfo(std::string firmwareInfo);

    /**
     * Add a shell command. The function takes ownership over the given pointer.
     * @param name
     * @param command
     */
    void addCommand(const std::string& name, ShellCommand* command);

    void addCommand(const std::string& name, std::shared_ptr<ShellCommand> command);

    void executeCommand(Stream& io, std::vector<std::string>& argv);

    void addClient(std::shared_ptr<ShellClient>);

    void loop();

    static void printCommandNotFound(Print& output, const std::string& commandName);

    static void printUsage(Print& output, const std::string& commandName, const ShellCommand& command);

private:
    class WiFiShellClient : public ShellClient {
    public:
        explicit WiFiShellClient(const WiFiClient& client) : m_client{client} {}

        size_t write(uint8_t uint8) override;

        int available() override;

        int read() override;

        int peek() override;

        bool active() override;

        void quit() override;

        size_t write(const uint8_t* buffer, size_t size) override;

        size_t readBytes(char* buffer, size_t length) override;

        size_t readBytes(uint8_t* buffer, size_t length) override;

    private:
        WiFiClient m_client;
    };

    std::string m_firmwareInfo;
    std::map<std::string, std::shared_ptr<ShellCommand>> m_commands;
    WiFiServer m_server;
    std::list<ShellConnection> m_connections;
};
