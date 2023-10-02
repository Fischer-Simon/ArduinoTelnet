#pragma once

#include "ShellCommand.h"

class ResetCommand : public ShellCommand {
public:
    void execute(Print& output, const std::string& commandName, std::vector<std::string>& args) override;

    void printHelp(Print& output, const std::string& commandName, std::vector<std::string>& args) const override;
};

class TestCommand : public ShellCommand {
public:
    void execute(Print& output, const std::string& commandName, std::vector<std::string>& args) override;

    void printUsage(Print& output) const override;

    void printHelp(Print& output, const std::string& commandName, std::vector<std::string>& args) const override;
};

class HexCommand : public ShellCommand {
public:
    void execute(Print& output, const std::string& commandName, std::vector<std::string>& args) override;

    void printUsage(Print& output) const override;

    void printHelp(Print& output, const std::string& commandName, std::vector<std::string>& args) const override;
};
