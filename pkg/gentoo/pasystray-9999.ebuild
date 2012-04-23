# Copyright 1999-2012 Gentoo Foundation
# Distributed under the terms of the GNU General Public License v2
# $Header: /var/cvsroot/gentoo-x86/media-sound/pasystray/pasystray-0.1.2.ebuild,v 1.2 2012/03/30 15:11:19 ssuominen Exp $

EAPI=4
inherit eutils gnome2-utils git-2

DESCRIPTION="PulseAudio system tray (replacement for padevchooser)"
HOMEPAGE="http://github.com/christophgysin/pasystray"
EGIT_REPO_URI="git://github.com/christophgysin/${PN}"

LICENSE="LGPL-2.1"
SLOT="0"
KEYWORDS="~amd64 ~x86"
IUSE="avahi notify"

RDEPEND=">=media-sound/pulseaudio-1.0[glib,avahi]
    >=net-dns/avahi-0.6
    >=x11-libs/libnotify-0.7
    x11-libs/gtk+:3"
DEPEND="${RDEPEND}
    dev-util/pkgconfig"

DOCS="AUTHORS README TODO"

pkg_preinst() { gnome2_icon_savelist; }
pkg_postinst() { gnome2_icon_cache_update; }
pkg_postrm() { gnome2_icon_cache_update; }
