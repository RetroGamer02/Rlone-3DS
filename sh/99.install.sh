if [ -z "$OS" ]; then
	echo "Unsupported OS. Please follow manual install steps."
	exit
fi

echo
if [ "$instdepends" -eq "1" ]; then
	echo "Dependency Information:"
	echo "           Detected OS: $OS"
	echo "              Packages: $PKGS"
fi

echo
read -p "Continue? (y/N) " uconfirm
uc=$(echo $uconfirm | cut -c1-1)

if [ "$uc" = "y" ] || [ "$uc" = "Y" ]; then
	echo "Beginning installation..."
	echo "Installing dependencies..."
	eval "$SUDOCMD $PKGCMD $PKGS"

	if [ "$?" -ne "0" ]; then
		echo "ERROR: Unable to install dependencies."
		exit
	fi

	if [ "$depends_only" -ne "1" ]; then
		echo "Building run script..."
		build_runner "$bindir/rlone" "$instdir"

		if [ "$?" -ne "0" ]; then
			echo "ERROR: Unable to build run script."
			exit
		fi

		echo "Installing Rlone..."
		install_rlone "$instdir"

		if [ "$?" -ne "0" ]; then
			echo "ERROR: Unable to run rlone."
			exit
		fi
	fi
fi

echo "Have fun!"
