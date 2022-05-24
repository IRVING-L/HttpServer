set -x
rm -rf `pwd`/.log_file/
rm -rf `pwd`/build/
mkdir `pwd`/build/
cd `pwd`/build &&
	cmake .. &&
	make