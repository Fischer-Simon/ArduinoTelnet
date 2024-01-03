#include "ShellCommands.h"

#include <Arduino.h>
#include <ArduinoTelnet.h>

void ResetCommand::execute(Stream& io, const std::string& commandName, std::vector<std::string>& args) {
    io.println("Resetting");
    io.flush();
    delay(100);
    ESP.restart();
}

void ResetCommand::printHelp(Print& output, const std::string& commandName, std::vector<std::string>& args) const {
    ShellCommand::printHelp(output, commandName, args);
    output.println("Reset the ESP");
}

void TestCommand::execute(Stream& io, const std::string& commandName, std::vector<std::string>& args) {
    io.print("Test: ");
    io.println(args.size());
}

void TestCommand::printUsage(Print& output) const {
    output.println("<arg1> <arg2> ...");
}

void TestCommand::printHelp(Print& output, const std::string& commandName, std::vector<std::string>& args) const {
    ShellCommand::printHelp(output, commandName, args);
    output.println("Print the number of command arguments.");
}

void HexCommand::execute(Stream& io, const std::string& commandName, std::vector<std::string>& args) {
    if (args.size() != 1) {
        ArduinoTelnet::printUsage(io, commandName, *this);
        return;
    }
    for (char c : args[0]) {
        io.print(c, HEX);
    }
    io.println();
}

void HexCommand::printUsage(Print& output) const {
    output.println("<str>");
}

void HexCommand::printHelp(Print& output, const std::string& commandName, std::vector<std::string>& args) const {
    ShellCommand::printHelp(output, commandName, args);
    output.println("Prints the hex representation of str");
}
