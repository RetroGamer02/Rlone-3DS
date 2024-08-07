# Some quick and dirty wrappers around "command -v"

getcmd() {
	command -v "$@"
}

hascmd() {
	cmd=$(getcmd $@)
	if [ -z "$cmd" ]; then
		return 1
	else
		return 0
	fi
}

os=""
