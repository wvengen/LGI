#!/bin/sh
cd `dirname $0`
SPECS=`pwd`

# prepare rpmbuild environment
mkdir -p ${SPECS}/RPM/RPMS ${SPECS}/RPM/BUILD ${SPECS}/RPM/SPECS ${SPECS}/RPM/TMP ${SPECS}/RPM/SOURCES ${SPECS}/RPM/SRPMS

if [ -e ${SPECS}/../src/daemon_main.cpp ]; then
	# if we're in-tree, update the source tarball
	echo "Generating source tarball..."
	mkdir ${SPECS}/LGI
	ln -s ${SPECS}/../* ${SPECS}/LGI
	rm -f ${SPECS}/LGI/specs
	tar --exclude .svn -c -h -z -f ${SPECS}/RPM/SOURCES/LGI.tar.gz -C ${SPECS} LGI
	rm -f ${SPECS}/LGI/* && rmdir ${SPECS}/LGI
elif [ -e ${SPECS}/LGI.tar.gz ]; then
	# if we have it locally with the specs, copy it
	cp ${SPECS}/LGI.tar.gz ${SPECS}/RPM/SOURCES/
elif [ ! -e ${SPECS}/LGI.tar.gz ]; then
	# if not, we may need to download it (and cache it)
	echo "Downloading sources..."
	wget -q -O ${SPECS}/LGI.tar.gz http://gliteui.wks.gorlaeus.net/LGI/LGI.tar.gz
	cp ${SPECS}/LGI.tar.gz ${SPECS}/RPM/SOURCES/
fi

# build
rpmbuild --define "_topdir ${SPECS}/RPM" -ba LGI_server.spec LGI_resource.spec LGI_python.spec LGI_cli.spec

# cleanup
mv ${SPECS}/RPM/RPMS/*/LGI_* .
mv ${SPECS}/RPM/SRPMS/LGI_* .
rm -rf ${SPECS}/RPM
