#!/bin/bash

while [ "$1" != "" ]; do
	releaseNum=$1
	echo "Creating tag release/$releaseNum and deleting branch origin/release/$releaseNum"
	git tag -a release/$releaseNum -m "Release $releaseNum" origin/release/$releaseNum && git push origin --delete release/$releaseNum && git push origin --tags
	shift
done
