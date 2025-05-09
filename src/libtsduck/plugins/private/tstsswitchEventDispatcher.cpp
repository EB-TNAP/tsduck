//----------------------------------------------------------------------------
//
// TSDuck - The MPEG Transport Stream Toolkit
// Copyright (c) 2005-2025, Thierry Lelegard
// BSD-2-Clause license, see LICENSE.txt file or https://tsduck.io/license
//
//----------------------------------------------------------------------------

#include "tstsswitchEventDispatcher.h"
#include "tsTextFormatter.h"
#include "tsForkPipe.h"
#include "tsTime.h"


//----------------------------------------------------------------------------
// Constructor and destructor.
//----------------------------------------------------------------------------

ts::tsswitch::EventDispatcher::EventDispatcher(const InputSwitcherArgs& opt, Report& log) :
    _opt(opt),
    _log(log),
    _sendCommand(!_opt.eventCommand.empty()),
    _sendUDP(_opt.eventUDP.hasAddress() && _opt.eventUDP.hasPort()),
    _userData(_opt.eventUserData),
    _socket()
{
}

//----------------------------------------------------------------------------
// Send command and UDP message.
//----------------------------------------------------------------------------

bool ts::tsswitch::EventDispatcher::sendCommand(const UString& eventName, const UString& otherParameters)
{
    UString command(_opt.eventCommand);
    command.append(u" ");
    command.append(eventName);
    if (!otherParameters.empty()) {
        command.append(u" ");
        command.append(otherParameters);
    }
    if (!_userData.empty()) {
        command.append(u" ");
        command.append(_userData.toQuoted());
    }
    return ForkPipe::Launch(command, _log, ForkPipe::STDERR_ONLY, ForkPipe::STDIN_NONE);
}


//----------------------------------------------------------------------------
// Send command and UDP message.
//----------------------------------------------------------------------------

bool ts::tsswitch::EventDispatcher::sendUDP(const UString& eventName, json::Object& object)
{
    // Open socket the first time.
    if (!_socket.isOpen()) {
        if (!_socket.open(_opt.eventUDP.generation(), _log) ||
            !_socket.setDefaultDestination(_opt.eventUDP, _log) ||
            (_opt.sockBuffer > 0 && !_socket.setSendBufferSize(_opt.sockBuffer, _log)) ||
            (_opt.eventLocalAddress.hasAddress() && !_socket.setOutgoingMulticast(_opt.eventLocalAddress, _log)) ||
            (_opt.eventTTL > 0 && !_socket.setTTL(_opt.eventTTL, _log)))
        {
            _socket.close(_log);
            return false;
        }
    }

    // Initialize a text formatter for one-liner.
    TextFormatter text(_log);
    text.setString();
    text.setEndOfLineMode(TextFormatter::EndOfLineMode::NONE);

    // Add common fields and format the JSON object.
    object.add(u"origin", u"tsduck");
    object.add(u"command", u"tsswitch");
    object.add(u"event", eventName);
    object.add(u"timestamp", Time::CurrentLocalTime().format());
    object.add(u"user-data", _userData);
    object.print(text);
    const std::string line(text.toString().toUTF8());

    // Send the packet.
    return _socket.send(line.data(), line.size(), _log);
}


//----------------------------------------------------------------------------
// Signal a "new input" event.
//----------------------------------------------------------------------------

bool ts::tsswitch::EventDispatcher::signalNewInput(size_t oldPluginIndex, size_t newPluginIndex)
{
    bool success = true;
    if (_sendCommand) {
        success = sendCommand(u"newinput", UString::Format(u"%d %d", oldPluginIndex, newPluginIndex));
    }
    if (_sendUDP) {
        json::Object root;
        root.add(u"previous-input", oldPluginIndex);
        root.add(u"new-input", newPluginIndex);
        success = sendUDP(u"newinput", root) && success;
    }
    return success;
}
