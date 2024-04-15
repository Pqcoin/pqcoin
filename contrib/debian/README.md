
Debian
====================
This directory contains files used to package pqcoind/pqcoin-qt
for Debian-based Linux systems. If you compile pqcoind/pqcoin-qt yourself, there are some useful files here.

## pqcoin: URI support ##


pqcoin-qt.desktop  (Gnome / Open Desktop)
To install:

	sudo desktop-file-install pqcoin-qt.desktop
	sudo update-desktop-database

If you build yourself, you will either need to modify the paths in
the .desktop file or copy or symlink your pqcoin-qt binary to `/usr/bin`
and the `../../share/pixmaps/pqcoin128.png` to `/usr/share/pixmaps`

pqcoin-qt.protocol (KDE)

