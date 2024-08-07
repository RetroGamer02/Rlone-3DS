install_rlone() {
	mkdir -p "$1"
	cp SOURCE.LICENSES "$1"
	cp README "$1"
	cp rlone "$1"
	cp -r res "$1"
}
