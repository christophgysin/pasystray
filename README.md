PulseAudio System Tray
======================
[![Build Status](https://travis-ci.org/christophgysin/pasystray.png?branch=master)](https://travis-ci.org/christophgysin/pasystray)

A replacement for the deprecated padevchooser.

pasystray allows setting the default PulseAudio source/sink and moving
streams on the fly between sources/sinks without restarting the client
applications.

Features
--------

* switch default sink/source
* move playback/record stream to a different sink/source on the fly
* detect pulseaudio instances on the network with avahi
* set X property PULSE_SERVER (like padevchooser's "set default server")
* adjust volume/toggle mute of sinks/sources and playback/record streams
* rename devices (pulseaudio must have module-device-manager loaded)

To detect remote sinks/sources you need to enable module-zeroconf-discover on
the local PulseAudio instance and module-zeroconf-publish on the remote side.

To control a different server than the local instance, run pasystray as:
```bash
$ PULSE_SERVER=remote_host pasystray
```

Additionnals functionnalities
-----------------------------

pasystray is able to manage the following list of pulse audio control:
* paman
* pavucontrol
* pavumeter
* paprefs

Mouse Bindings
--------------

over status icon:
* left/right-click: open menu
* ctrl + left-click: start pavucontrol
* middle-click / alt + left-click: toggle mute default sink
* ctrl + middle-click / ctrl + alt + left-click: toggle mute default source
* scroll up/down: adjust the default sink volume
* ctrl + scroll: adjust the default source volume

over source/sink:
* scroll up/down: adjust the volume of respective stream
* left-click: set as default sink/source
* middle-click / alt + left-click: toggle mute
* right-click: open menu to rename source/sink

over playback/record stream:
* scroll up/down: adjust the volume of respective stream
* middle-click: toggle mute

Troubleshooting
---------------

If the pasystray icon is not displayed in your Unity system tray, you will have
to whitelist it. Read this for instructions:
http://askubuntu.com/questions/30742/how-do-i-access-and-enable-more-icons-to-be-in-the-system-tray

For Ubuntu 13.04 and above, AppIndicator is mandatory. Unfortunately, the
ctrl + click / ctrl + scroll bindings don't work with AppIndicator.
