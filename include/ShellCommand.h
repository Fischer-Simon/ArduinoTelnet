#pragma once

#include <string>
#include <vector>

class Print;

class ShellCommand {
public:
    virtual ~ShellCommand() = default;

    virtual void execute(Print& output, const std::string& commandName, std::vector<std::string>& args) = 0;

    virtual void printUsage(Print& output) const;

    virtual void printHelp(Print& output, const std::string& commandName, std::vector<std::string>& args) const;
};
