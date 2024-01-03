#pragma once

#include <map>
#include <memory>

#include "ShellCommand.h"

class ShellCommandGroup : public ShellCommand {
public:
    void execute(Stream& io, const std::string& commandName, std::vector<std::string>& args) override {
        executeSubCommands(io, commandName, args);
    }

    void printUsage(Print& output) const override {
        printSubCommandUsage(output);
    }

    void printHelp(Print& output, const std::string& commandName, std::vector<std::string>& args) const override;

protected:
    void addSubCommand(const std::string& name, ShellCommand* command);

    void executeSubCommands(Stream& io, const std::string& mainCommandName, std::vector<std::string>& args);

    void printSubCommandUsage(Print& output) const;

    void printSubCommandHelp(Print& output, const std::string& commandName, std::vector<std::string>& args) const;

    std::map<std::string, std::unique_ptr<ShellCommand>> m_subCommands;
};
