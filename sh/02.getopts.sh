usage() {
	splash
	echo "Installer Options:"
	echo "\t-h/--help       = Show this text."
	echo "\t-n/--no-depends = Do not install dependencies."
	echo "\t-b/--bindir     = Specify target binary directory."
	echo "\t-d/--datadir    = Specify target data directory."
}

argnext=0
for arg in "$@"; do
	if [ "$argnext" -eq "1" ]; then
		case $argname in
			"bin")
				bindir=$arg
				;;
			"dat")
				instdir=$arg
				;;
		esac
		argnext=0
	else
		case $arg in
			"-h"|"--help")
				usage
				exit;;
			"-n"|"--no-depends")
				instdepends=0
				;;
			"-b"|"--bindir")
				argname="bin"
				argnext=1
				;;
			"-d"|"--datadir")
				argname="dat"
				argnext=1
				;;
		esac
	fi
done
