is_root=0

if [ "$(id -u)" -eq "0" ]; then
	is_root=1
	bindir=/usr/local/bin
	instdir=/usr/local/share/rlone
fi
