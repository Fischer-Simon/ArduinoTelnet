#include "ShellCommands.h"

#include <Arduino.h>
#include <ArduinoTelnet.h>

void ResetCommand::execute(Print& output, const std::string& commandName, std::vector<std::string>& args) {
    output.println("Resetting");
    output.flush();
    delay(100);
    ESP.restart();
}

void ResetCommand::printHelp(Print& output, const std::string& commandName, std::vector<std::string>& args) const {
    ShellCommand::printHelp(output, commandName, args);
    output.println("Reset the ESP");
}

void TestCommand::execute(Print& output, const std::string& commandName, std::vector<std::string>& args) {
    output.print("Test: ");
    output.println(args.size());
}

void TestCommand::printUsage(Print& output) const {
    output.println("<arg1> <arg2> ...");
}

void TestCommand::printHelp(Print& output, const std::string& commandName, std::vector<std::string>& args) const {
    ShellCommand::printHelp(output, commandName, args);
    output.println("Print the number of command arguments.");
}

void HexCommand::execute(Print& output, const std::string& commandName, std::vector<std::string>& args) {
    if (args.size() != 1) {
        ArduinoTelnet::printUsage(output, commandName, *this);
        return;
    }
    for (char c : args[0]) {
        output.print(c, HEX);
    }
    output.println();
}

void HexCommand::printUsage(Print& output) const {
    output.println("<str>");
}

void HexCommand::printHelp(Print& output, const std::string& commandName, std::vector<std::string>& args) const {
    ShellCommand::printHelp(output, commandName, args);
    output.println("Prints the hex representation of str");
}
