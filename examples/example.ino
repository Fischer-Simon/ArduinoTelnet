#include <Arduino.h>
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
