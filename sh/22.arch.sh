detect_arch() {
	if hascmd "pacman"; then
		export PKGCMD="$(getcmd "pacman") -S"
		export SUDOCMD=$(getcmd "sudo")
		export PKGS="sdl2 sdl2_image sdl2_mixer"
		export OS="Arch Linux"
	fi
}

if [ -z "$OS" ]; then
	detect_arch
fi
