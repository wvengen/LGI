#!/bin/sh
if [ -f ${HOME}/.rpmmacros ]; then
 mv ${HOME}/.rpmmacros ${HOME}/.rpmmacros_SAVED_FOR_NOW 
fi
PWD=`pwd`
echo "%_topdir $PWD/RPM" > ${HOME}/.rpmmacros
mkdir -p ${PWD}/RPM/RPMS ${PWD}/RPM/BUILD ${PWD}/RPM/SPECS ${PWD}/RPM/TMP ${PWD}/RPM/SOURCES ${PWD}/RPM/SRPMS
rpmbuild -ba LGI_server.spec LGI_resource.spec LGI_python.spec LGI_cli.spec
cp ${PWD}/RPM/RPMS/*/LGI_* .
cp ${PWD}/RPM/SRPMS/LGI_* .
rm -rf ${PWD}/RPM
if [ -f ${HOME}/.rpmmacros_SAVED_FOR_NOW ]; then
 mv ${HOME}/.rpmmacros_SAVED_FOR_NOW ${HOME}/.rpmmacros
fi
