detect_debub() {
	if hascmd "apt-get"; then
		export PKGCMD="$(getcmd "apt-get") install"
		export SUDOCMD=$(getcmd "sudo")
		export PKGS="libsdl2-2.0-0 libsdl2-image-2.0-0 libsdl2-mixer-2.0-0"
		export OS="Debian/Ubuntu"
	fi
}

detect_debub
