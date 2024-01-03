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

        size_t readBytes(char* buffer, size_t#include <Arduino.h>
#include <ArduinoTelnet.h>
#include <WiFi.h>

//#define WIFI_SSID ""
//#define WIFI_PASSWORD ""
//
#include "secrets.h"

std::unique_ptr<ArduinoTelnet> telnet;

class ExampleCommand : public ShellCommand {
public:
    void execute(Stream& io, const std::string& commandName, std::vector<std::string>& args) override {
        io.println("This is a command example");
    }

    void printHelp(Print& output, const std::string& commandName, std::vector<std::string>& args) const override {
        output.println("Just print an example text");
    }
};

class ExampleArgCommand : public ShellCommand {
public:
    void execute(Stream& io, const std::string& commandName, std::vector<std::string>& args) override {
        if (args.size() != 1) {
            ArduinoTelnet::printUsage(io, commandName, *this);
            return;
        }
        io.print("The first argument is: ");
        io.println(args[0].c_str());
    }

    void printUsage(Print& output) const override {
        output.println("<arg>");
    }

    void printHelp(Print& output, const std::string& commandName, std::vector<std::string>& args) const override {
        output.println("Print the first given argument");
    }
};

class ExampleCommandGroup : public ShellCommandGroup {
public:
    ExampleCommandGroup() {
        addSubCommand("example1", new ExampleCommand);
        addSubCommand("example2", new ExampleArgCommand);
    }
};

void setup() {
    Serial.begin(115200);

    Serial.print("Connecting to " WIFI_SSID);
    WiFiClass::setHostname("telnet-example");
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    while (!WiFi.isConnected()) {
        Serial.print('.');
        delay(200);
    }
    Serial.println();

    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());

    telnet.reset(new ArduinoTelnet{23});
    telnet->addCommand("example", new ExampleCommand);
    telnet->addCommand("example-arg", new ExampleArgCommand);
    telnet->addCommand("example-group", new ExampleCommandGroup);
}

void loop() {
    telnet->loop();
}
 length) override;

        size_t readBytes(uint8_t* buffer, size_t length) override;

    private:
        WiFiClient m_client;
    };

    std::string m_firmwareInfo;
    std::map<std::string, std::shared_ptr<ShellCommand>> m_commands;
    WiFiServer m_server;
    std::list<ShellConnection> m_connections;
};
