#include "ShellCommandGroup.h"

#include "ArduinoTelnet.h"

void ShellCommandGroup::printHelp(Print& output, const std::string& commandName, std::vector<std::string>& args) const {
    if (!args.empty()) {
        printSubCommandHelp(output, commandName, args);
        return;
    } else {
        ArduinoTelnet::printUsage(output, commandName, *this);
        for (const auto& subCommand: m_subCommands) {
            ArduinoTelnet::printUsage(output, commandName + " " + subCommand.first, *subCommand.second);
        }
    }
}

void ShellCommandGroup::addSubCommand(const std::string& name, ShellCommand* command) {
    m_subCommands.emplace(name, std::move(std::unique_ptr<ShellCommand>(command)));
}

void ShellCommandGroup::executeSubCommands(Stream& io, const std::string& mainCommandName, std::vector<std::string>& args) {
    if (args.empty()) {
        ArduinoTelnet::printUsage(io, mainCommandName, *this);
        return;
    }

    std::string subCommandName = args.front();
    auto command = m_subCommands.find(subCommandName);
    if (command == m_subCommands.end()) {
        ArduinoTelnet::printCommandNotFound(io, subCommandName);
        return;
    }

    args.erase(args.begin());
    command->second->execute(io, mainCommandName + " " + subCommandName, args);
}

void ShellCommandGroup::printSubCommandUsage(Print& output) const {
    output.print('[');
    bool first = true;
    for (const auto& subCommand : m_subCommands) {
        if (first) {
            first = false;
        } else {
            output.print(',');
        }
        output.print(subCommand.first.c_str());
    }
    output.println("] <...>");
}

void ShellCommandGroup::printSubCommandHelp(Print& output, const std::string& mainCommandName, std::vector<std::string>& args) const {
    if (args.empty()) {
        return;
    }

    std::string subCommandName = args.front();
    auto command = m_subCommands.find(subCommandName);
    if (command == m_subCommands.end()) {
        ArduinoTelnet::printCommandNotFound(output, subCommandName);
    }

    args.erase(args.begin());
    command->second->printHelp(output, mainCommandName + " " + subCommandName, args);
}
