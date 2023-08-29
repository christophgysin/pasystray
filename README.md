# PulseAudio System Tray

[![build](https://github.com/stu/pasystray/actions/workflows/build.yml/badge.svg)](https://github.com/stu/pasystray/actions/workflows/build.yml)

A replacement for the deprecated padevchooser.

pasystray allows setting the default PulseAudio source/sink and moving
streams on the fly between sources/sinks without restarting the client
applications.

<a href="https://repology.org/project/pasystray/versions">
  <img align="right" src="https://repology.org/badge/vertical-allrepos/pasystray.svg?exclude_sources=site&exclude_unsupported=1" alt="Packaging status">
</a>

## Usage

```
Help Options:
  -h, --help                Show help options
  --help-all                Show all help options
  --help-gtk                Show GTK+ Options

Application Options:
  -V, --version             Print version and exit
  -d, --debug               Print debugging information
  -m, --volume-max=N        Maximum volume (in percent)
  -i, --volume-inc=N        Volume increment
  -r, --reverse-scroll      Reverse volume scroll direction
  -t, --no-icon-tooltip     Disable the status icon tooltip for the connected state
  -n, --no-notify           Deprecated, use --notify=none instead
  -a, --always-notify       Deprecated, use --notify=all instead
  --include-monitors        Include monitor sources
  -g, --key-grabbing        Grab volume control keys
  -N, --notify=OPTION       Set notification options, use --notify=help for a list of valid options
  --display=DISPLAY         X display to use
```

## Features

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

## Mouse Bindings

over status icon:
* left/right-click: open menu
* ctrl + left-click: start pavucontrol
* middle-click / alt + left-click: toggle mute default sink
* ctrl + middle-click / ctrl + alt + left-click: toggle mute default source
* scroll up/down: adjust the default sink volume
* ctrl + scroll: adjust the default source volume

over source/sink:
* scroll up/down: adjust the volume of respective stream
* left-click: set as default source/sink
* middle-click / alt + left-click: toggle mute
* right-click: open menu to move all outputs/inputs to this source/sink or rename it

over playback/record stream:
* scroll up/down: adjust the volume of respective stream
* middle-click: toggle mute

## Troubleshooting

If the pasystray icon is not displayed in your Unity system tray, you will have
to whitelist it. Read this for instructions:
http://askubuntu.com/questions/30742/how-do-i-access-and-enable-more-icons-to-be-in-the-system-tray

For Ubuntu 13.04 and above, AppIndicator is mandatory. Unfortunately, the
ctrl + click / ctrl + scroll bindings don't work with AppIndicator.
