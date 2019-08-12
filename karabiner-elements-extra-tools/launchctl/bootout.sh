#!/bin/sh

#
# observer
#

/bin/launchctl bootout gui/501/ /Library/LaunchAgents/org.pqrs.karabiner.agent.karabiner_observer.plist
/bin/launchctl disable gui/501/org.pqrs.karabiner.agent.karabiner_observer

#
# grabber
#

/bin/launchctl bootout gui/501/ /Library/LaunchAgents/org.pqrs.karabiner.agent.karabiner_grabber.plist
/bin/launchctl disable gui/501/org.pqrs.karabiner.agent.karabiner_grabber

#
# session_monitor
#

/bin/launchctl bootout gui/501/ /Library/LaunchAgents/org.pqrs.karabiner.karabiner_session_monitor.plist
/bin/launchctl disable gui/501/org.pqrs.karabiner.karabiner_session_monitor

#
# console_user_server
#

/bin/launchctl bootout gui/501/ /Library/LaunchAgents/org.pqrs.karabiner.karabiner_console_user_server.plist
/bin/launchctl disable gui/501/org.pqrs.karabiner.karabiner_console_user_server
