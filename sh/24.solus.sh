detect_solus() {
	if hascmd "eopkg"; then
		export PKGCMD="$(getcmd "eopkg") install"
		export SUDOCMD=$(getcmd "sudo")
		export PKGS="sdl2 sdl2_image sdl2_mixer"
		export OS="Solus"
	fi
}

if [ -n "$OS" ]; then
	detect_solus
fi
