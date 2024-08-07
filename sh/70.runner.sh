build_runner() {
	echo "#!/usr/bin/env sh" > "$1"
	echo "rlone_dir=$2" >> "$1"
	echo "
cd \$rlone_dir
./rlone
" >> "$1"
	chmod +x "$1"
}
