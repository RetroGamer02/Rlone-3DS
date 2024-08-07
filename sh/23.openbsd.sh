detect_openbsd() {
	if hascmd "pkg_add"; then
		export PKGCMD=$(getcmd "pkg_add")
		export SUDOCMD=$(getcmd "doas")
		export PKGS="sdl2 sdl2-image sdl2-mixer"
		export OS="OpenBSD"
	fi
}

if [ -z "$OS" ]; then
	detect_openbsd
fi
