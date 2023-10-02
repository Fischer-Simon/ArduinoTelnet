#include "TelnetCommand.h"
#include <Stream.h>
#include <cstdio>

void writeTelnetCommand(TelnetCommand cmd, Stream& stream) {
    stream.write((uint8_t)TelnetCommand::IAC);
    stream.write((uint8_t)cmd);
}

void writeTelnetOption(TelnetOption opt, Stream& stream) {
    stream.write((uint8_t)opt);
}

void writeTelnetResponse(TelnetCommand cmd, TelnetOption opt, Stream& stream) {
    writeTelnetCommand(cmd, stream);
    writeTelnetOption(opt, stream);
}

const char* telnetCommandToString(TelnetCommand command) {
    static char dummy[5];
    switch (command) {
        case TelnetCommand::EOF_:
            return "EOF";
        case TelnetCommand::SE:
            return "SE";
        case TelnetCommand::NOP:
            return "NOP";
        case TelnetCommand::DM:
            return "DM";
        case TelnetCommand::BRK:
            return "BRK";
        case TelnetCommand::IP:
            return "IP";
        case TelnetCommand::AO:
            return "AO";
        case TelnetCommand::AYT:
            return "AYT";
        case TelnetCommand::EC:
            return "EC";
        case TelnetCommand::EL:
            return "EL";
        case TelnetCommand::GA:
            return "GA";
        case TelnetCommand::SB:
            return "SB";
        case TelnetCommand::WILL:
            return "WILL";
        case TelnetCommand::WONT:
            return "WONT";
        case TelnetCommand::DO:
            return "DO";
        case TelnetCommand::DONT:
            return "DONT";
        case TelnetCommand::IAC:
            return "IAC";
    }
    snprintf(dummy, 5, "0x%02x", (uint8_t)command);
    return dummy;
}

const char* telnetCommandToString(uint8_t c) {
    return telnetCommandToString((TelnetCommand)c);
}

const char* telnetOptionToString(TelnetOption option) {
    static char dummy[5];
    switch (option) {
        case TelnetOption::Echo:
            return "Echo";
        case TelnetOption::SuppressGoAhead:
            return "SuppressGoAhead";
        case TelnetOption::Status:
            return "Status";
        case TelnetOption::TimingMark:
            return "TimingMark";
        case TelnetOption::TerminalType:
            return "TerminalType";
        case TelnetOption::WindowSize:
            return "WindowSize";
        case TelnetOption::TerminalSpeed:
            return "TerminalSpeed";
        case TelnetOption::RemoteFlowControl:
            return "RemoteFlowControl";
        case TelnetOption::Linemode:
            return "Linemode";
        case TelnetOption::EnvironmentVariables:
            return "EnvironmentVariables";
    }
    snprintf(dummy, 5, "0x%02x", (uint8_t)option);
    return dummy;
}

const char* telnetOptionToString(uint8_t c) {
    return telnetOptionToString((TelnetOption)c);
}

bool operator==(uint8_t v, TelnetCommand c) {
    return v == (uint8_t)c;
}
