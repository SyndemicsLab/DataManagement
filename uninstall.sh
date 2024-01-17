#!/usr/bin/bash

remove () {
	cat "build/install_manifest.txt" | xargs sudo rm
	if [[ -d "/usr/local/include/DataManagement" ]]; then
		sudo rm -r "/usr/local/include/DataManagement"
	fi
	if [[ -d "/usr/local/lib/cmake/DataManagement" ]]; then
		sudo rm -r "/usr/local/lib/cmake/DataManagement"
	fi
}

remove
