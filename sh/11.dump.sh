if [ "$depends_only" -ne "1" ]; then
	echo "Destination Directories:"
	echo "       Binary Directory: " $bindir
	echo "         Data Directory: " $instdir
	echo
else
	echo "Dependency only installer."
fi

echo -n "Install Dependencies? "
if [ "$instdepends" -eq "1" ]; then
	echo -n "yes"
	if [ "$is_root" -eq "0" ]; then
		echo -n " (Will ask for root)"
	fi
	echo
else
	echo "no"
fi
