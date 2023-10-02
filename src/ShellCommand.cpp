#include "ShellCommand.h"

#include "ArduinoTelnet.h"

void ShellCommand::printUsage(Print& output) const {
    output.println();
}

void ShellCommand::printHelp(Print& output, const std::string& commandName, std::vector<std::string>& args) const {
    ArduinoTelnet::printUsage(output, commandName, *this);
}
