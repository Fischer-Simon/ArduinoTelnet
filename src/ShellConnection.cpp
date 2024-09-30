#include "ShellConnection.h"

#include <WiFi.h>
#include <sstream>
#include <Ansi.h>

#include "ArduinoTelnet.h"
#include "TelnetCommand.h"

ShellConnection::ShellConnection(ArduinoTelnet& shell, std::shared_ptr<ShellClient> client, std::string firmwareInfo) :
        m_shell{shell},
        m_client{std::move(client)},
        m_firmwareInfo{std::move(firmwareInfo)},
        m_lineBuffer{},
        m_lineBufferIterator{m_lineBuffer.end()},
        m_history{},
        m_historyIterator{m_history.end()} {
}

int ShellConnection::processLine() {
    if (m_lineBuffer.empty()) {
        if (m_gotTelnetCommand || m_client->forceEcho()) {
            m_client->println();
        }
        printPrompt();
        return 0;
    }

    if (m_lineBuffer[0] == '#') {
        return 0;
    }

    std::stringstream lineStream{m_lineBuffer};
    std::string arg;
    arg.reserve(m_lineBuffer.size());
    std::vector<std::string> argv;
    int commandSeparatorPosition = 0;
    bool isInQuotation = false;
    char quotationCharacter = 0;
    bool escapeSequence = false;
    int i = 0;
    for (char c : m_lineBuffer) {
        i++;
        if (escapeSequence) {
            switch (c) {
                case 'n':
                    arg.push_back('\n');
                    break;
                case 't':
                    arg.push_back('\t');
                    break;
                default:
                    arg.push_back(c);
                    break;
            }
            escapeSequence = false;
            continue;
        }
        if (c == '\\') {
            escapeSequence = true;
            continue;
        }
        if (isInQuotation && c == quotationCharacter) {
            isInQuotation = false;
            continue;
        }
        if (!isInQuotation && (c == '"' || c == '\'')) {
            isInQuotation = true;
            quotationCharacter = c;
            continue;
        }
        if (isInQuotation) {
            arg.push_back(c);
            continue;
        }

        if (c == ' ') {
            if (arg.empty()) {
                continue;
            }
            argv.emplace_back(arg);
            arg.clear();
            continue;
        }

        if (c == ';') {
            commandSeparatorPosition = i;
            break;
        }

        arg.push_back(c);
    }
    if (!arg.empty()) {
        argv.emplace_back(arg);
    }

    if (!argv.empty()) {
        if (m_gotTelnetCommand || m_client->forceEcho()) {
            m_client->println();
        }
        if (argv[0] == "help") {
            argv.erase(argv.begin());
            printHelp(*m_client, argv);
        } else {
            m_shell.executeCommand(*m_client, argv);
        }
        m_client->onCommandEnd();
    }

    if (commandSeparatorPosition == 0) {
        if (argv.empty() && m_gotTelnetCommand || m_client->forceEcho()) {
            m_client->println();
        }
        printPrompt();
    }
    return commandSeparatorPosition;
}

void ShellConnection::printHelp(Print& output, std::vector<std::string>& argv) {
    if (argv.empty()) {
        output.print("Available commands: help");
        for (auto& command: m_shell.m_commands) {
            output.print(", ");
            output.print(command.first.c_str());
        }
        output.println();
        output.println("Type help <command> for more information.");
        return;
    }

    auto commandName = argv[0];
    argv.erase(argv.begin());
    auto command = m_shell.m_commands.find(commandName);
    if (command != m_shell.m_commands.end()) {
        command->second->printHelp(output, commandName, argv);
    }
}

void ShellConnection::printPrompt() {
    if (!m_gotTelnetCommand && !m_client->forceEcho()) {
        return;
    }
    m_client->print(WiFiClass::getHostname());
    m_client->print("> ");
}

void ShellConnection::printWelcome() {
    m_client->print("Welcome to the ");
    m_client->print(WiFiClass::getHostname());
    m_client->println(" command line interface.");
    if (!m_firmwareInfo.empty()) {
        m_client->println(m_firmwareInfo.c_str());
    }
    m_client->println("You can type your commands, type 'help' for a list of commands");
    printPrompt();
}

bool ShellConnection::loop() {
    if (!m_client->active()) {
        return false;
    }

    uint8_t c;

    while (m_client->available()) {
        if (m_client->readBytes(&c, 1) != 1) {
            return false;
        }

        if (c == TelnetCommand::IAC) {
            handleTelnetCommand();
            continue;
        }

        if (c == '\n' || c == '\r') {
            if (m_gotTelnetCommand) {
                // Read the additional null byte sent by the telnet client.
                m_client->read();
            }

            if (!m_lineBuffer.empty() || m_historyIterator != m_history.end() || c == '\n') {
                if (m_historyIterator != m_history.end()) {
                    m_lineBuffer = *m_historyIterator;
                }
                if (m_history.empty() || m_history.back() != m_lineBuffer) {
                    m_history.push_back(m_lineBuffer);
                }
                m_historyIterator = m_history.end();
                if (m_history.size() > 10) {
                    m_history.erase(m_history.begin());
                    m_historyIterator = m_history.end();
                }

                while (int i = processLine()) {
                    m_lineBuffer = m_lineBuffer.substr(i);
                }
                overwriteLineBuffer("");
            }
        } else if (c == 3) { // CTRL+c
            m_client->write("^C\r\n");
            printPrompt();
            overwriteLineBuffer("");
            m_historyIterator = m_history.end();
        } else if (c == 27) { // ESC
            m_client->readBytes(&c, 1);
            if (c != '[') {
                continue;
            }
            m_client->readBytes(&c, 1);
            switch (c) {
                case 'A': // Up
                    if (m_historyIterator != m_history.begin()) {
                        const std::string& currentLine = m_historyIterator == m_history.end() ? m_lineBuffer : *m_historyIterator;
                        m_historyIterator--;
                        const std::string& newLine = *m_historyIterator;
                        m_client->write(ANSI_CLEAR_LINE"\r");
                        printPrompt();
                        m_client->write(newLine.c_str());
                    }
                    break;
                case 'B': // Down
                    if (m_historyIterator != m_history.end()) {
                        const std::string& currentLine = *m_historyIterator;
                        m_historyIterator++;
                        const std::string& newLine = m_historyIterator == m_history.end() ? m_lineBuffer : *m_historyIterator;
                        m_client->write(ANSI_CLEAR_LINE"\r");
                        printPrompt();
                        m_client->write(newLine.c_str());
                    }
                    break;
                case 'C': // Right
                    if (m_lineBufferIterator != m_lineBuffer.end()) {
                        m_lineBufferIterator++;
                        m_client->print(ANSI_CURSOR_RIGHT(1));
                    }
                    break;
                case 'D': // Left
                    if (m_historyIterator != m_history.end()) {
                        overwriteLineBuffer(*m_historyIterator);
                        m_historyIterator = m_history.end();
                    }
                    if (m_lineBufferIterator != m_lineBuffer.begin()) {
                        m_lineBufferIterator--;
                        m_client->print(ANSI_CURSOR_LEFT(1));
                    }
                    break;
            }
        } else if (c == 4) { // EOF
            m_client->quit();
        } else if (c == 127 || c == 8) { // DEL
            if (m_historyIterator != m_history.end()) {
                overwriteLineBuffer(*m_historyIterator);
                m_historyIterator = m_history.end();
            }
            if (!m_lineBuffer.empty() && m_lineBufferIterator != m_lineBuffer.begin()) {
                m_lineBufferIterator = m_lineBuffer.erase(m_lineBufferIterator - 1);
                int distance = m_lineBufferIterator - m_lineBuffer.begin();
                m_client->printf(ANSI_CURSOR_LEFT(1) ANSI_CURSOR_SAVE "%s " ANSI_CURSOR_RESTORE, m_lineBuffer.c_str() + distance);
            }
        } else if (c >= 32) {
            if (m_historyIterator != m_history.end()) {
                overwriteLineBuffer(*m_historyIterator);
                m_historyIterator = m_history.end();
            }
            int distance = m_lineBufferIterator - m_lineBuffer.begin();
            if (m_lineBuffer.size() < LineSizeLimit) {
                m_lineBufferIterator = m_lineBuffer.insert(m_lineBufferIterator, (char)c) + 1;
            }
            if (m_lineBufferIterator == m_lineBuffer.end()) {
                if (m_gotTelnetCommand || m_client->forceEcho()) {
                    m_client->write(c);
                }
            } else {
                m_client->printf(ANSI_CURSOR_SAVE "%s" ANSI_CURSOR_RESTORE ANSI_CURSOR_RIGHT(1), m_lineBuffer.c_str() + distance);
            }
        }
    }

    return true;
}

void ShellConnection::handleTelnetCommand() {
    union {
        uint8_t c;
        TelnetCommand cmd;
    };
    uint8_t buf[17];
    buf[16] = 0;

    if (!m_gotTelnetCommand) {
        m_gotTelnetCommand = true;
        writeTelnetResponse(TelnetCommand::WILL, TelnetOption::Echo, *m_client);
        writeTelnetResponse(TelnetCommand::DONT, TelnetOption::Echo, *m_client);
        writeTelnetResponse(TelnetCommand::WILL, TelnetOption::SuppressGoAhead, *m_client);
        printWelcome();
    }

    m_client->readBytes(&c, 1);
//    m_client->printf("IAC,%s", telnetCommandToString(cmd));

    TelnetOption opt;
    if (cmd == TelnetCommand::WILL || cmd == TelnetCommand::DO || cmd == TelnetCommand::WONT || cmd == TelnetCommand::DONT || cmd == TelnetCommand::SB) {
        m_client->readBytes((uint8_t*)&opt, 1);
//        m_client->printf(",%s", telnetOptionToString(opt));
    }

    switch (cmd) {
        case TelnetCommand::EOF_:
            m_client->quit();
            return;
        case TelnetCommand::DO:
            switch (opt) {
                case TelnetOption::SuppressGoAhead:
//                    writeTelnetResponse(TelnetCommand::WILL, opt, m_client);
                    break;
                default:
//                    writeTelnetResponse(TelnetCommand::WONT, opt, m_client);
                    break;
            }
            break;
        case TelnetCommand::WILL:
            switch (opt) {
                case TelnetOption::TerminalType:
//                    writeTelnetResponse(TelnetCommand::DO, opt, m_client);
//                    writeTelnetResponse(TelnetCommand::SB, opt, m_client);
//                    m_client->write((char)1);
//                    writeTelnetCommand(TelnetCommand::SE, m_client);
                    break;
                default:
                    break;
            }
            break;
        case TelnetCommand::WONT:
//            writeTelnetCommand(TelnetCommand::DONT, m_client);
//            writeTelnetOption(opt, m_client);
            break;
        case TelnetCommand::DONT:
//            writeTelnetCommand(TelnetCommand::WONT, m_client);
//            writeTelnetOption(opt, m_client);
            break;
        case TelnetCommand::SB:
            m_client->readBytes(&c, 1);
            for (int i = 0; cmd != TelnetCommand::IAC; i++, m_client->readBytes(&c, 1)) {
                if (i >= 16) {
                    continue;
                }
                buf[i] = c;
            }
//            m_client->printf(",%i", buf[0]);
            if (buf[0] == 1) {
                // Value requested
            } else {
                // Value provided
//                m_client->printf(",%s", &buf[1]);
            }
            m_client->readBytes(&c, 1); // Should be SE.
    }
//    m_client->println();
}

void ShellConnection::overwriteLineBuffer(const std::string& line) {
    m_lineBuffer = line;
    m_lineBufferIterator = m_lineBuffer.end();
}
