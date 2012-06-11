#!/bin/sh
cd `dirname $0`
SPECS=`pwd`

# prepare rpmbuild environment
mkdir -p ${SPECS}/RPM/RPMS ${SPECS}/RPM/BUILD ${SPECS}/RPM/SPECS ${SPECS}/RPM/TMP ${SPECS}/RPM/SOURCES ${SPECS}/RPM/SRPMS

if [ -e ${SPECS}/../src/daemon_main.cpp ]; then
	# if we're in-tree, update the source tarball
	tar -c -z -C ${SPECS}/.. -f ${SPECS}/LGI.tar.gz \*
elif [ ! -e ${SPECS}/LGI.tar.gz ]; then
	# if not, we may need to download it
	wget -q http://gliteui.wks.gorlaeus.net/LGI/LGI.tar.gz
fi

# build
rpmbuild --define "_topdir ${SPECS}/RPM" -ba LGI_server.spec LGI_resource.spec LGI_python.spec LGI_cli.spec

# cleanup
cp ${SPECS}/RPM/RPMS/*/LGI_* .
cp ${SPECS}/RPM/SRPMS/LGI_* .
rm -rf ${SPECS}/RPM
