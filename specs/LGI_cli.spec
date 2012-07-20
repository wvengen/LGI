#
# The LGI command line interface spec file...
#
%define name LGI_cli
%define version 1.31.3
%define release 1.el5
%define sources http://gliteui.wks.gorlaeus.net/LGI/LGI.tar.gz
%define url http://gliteui.wks.gorlaeus.net/LGI
%define prefix /usr/local/LGI
#
# define RPM build area so that the following vars will be defined:
#
# RPM_OPT_FLAGS RPM_PACKAGE_RELEASE RPM_PACKAGE_NAME RPM_SOURCE_DIR RPM_BUILD_ROOT 
# RPM_PACKAGE_VERSION RPM_OS RPM_DOC_DIR RPM_BUILD_DIR RPM_ARCH
#
%define _builddir %{_topdir}/BUILD/
%define _rpmdir %{_topdir}/RPMS/
%define _sourcedir %{_topdir}/SOURCES/
%define _specdir %{_topdir}/SPEC/
%define _srcrpmdir %{_topdir}/SRPMS/
%define _tmppath %{_topdir}/TMP/
%define _buildroot %{_topdir}/ROOT/%{name}
#
# Create header for rpm...
#
Summary: Leiden Grid Infrastructure command line interface
Name: %{name}
Version: %{version}
Release: %{release}
License: GPL v3
Group: Applications/Interface
Distribution: rhel
Source: %{sources}
URL: %{url}
Vendor: Theoretical Chemistry Group Leiden University
Packager: mark somers <m.somers@chem.leidenuniv.nl>
BuildRoot: %{_buildroot}
Prefix: %{prefix}
Requires: /etc/redhat-release, /bin/sed
BuildRequires: gcc-c++
BuildRequires: curl-devel

%description
This is the command line interface to the Leiden Grid Infrastructure Grid. 

See %{url}

%prep
mkdir -p $RPM_BUILD_ROOT
rm -rf $RPM_BUILD_ROOT/*
rm -rf $RPM_BUILD_DIR/LGI*

%clean
rm -rf $RPM_BUILD_ROOT/*

%build
tar -zxf $RPM_SOURCE_DIR/LGI*.tar.gz
cd $RPM_BUILD_DIR/LGI*/src
make

%install
mkdir -p $RPM_BUILD_ROOT/%{prefix}/bin 
mkdir -p $RPM_BUILD_ROOT/%{prefix}/docs
mkdir -p $RPM_BUILD_ROOT/%{prefix}/certificates
mkdir -p $RPM_BUILD_ROOT/etc/profile.d
cp $RPM_BUILD_DIR/LGI*/src/LGI_qstat $RPM_BUILD_ROOT/%{prefix}/bin
cp $RPM_BUILD_DIR/LGI*/src/LGI_qsub $RPM_BUILD_ROOT/%{prefix}/bin
cp $RPM_BUILD_DIR/LGI*/src/LGI_qdel $RPM_BUILD_ROOT/%{prefix}/bin
cp $RPM_BUILD_DIR/LGI*/src/LGI_filetransfer $RPM_BUILD_ROOT/%{prefix}/bin
cp $RPM_BUILD_DIR/LGI*/src/binhex $RPM_BUILD_ROOT/%{prefix}/bin
cp $RPM_BUILD_DIR/LGI*/src/hexbin $RPM_BUILD_ROOT/%{prefix}/bin
cp $RPM_BUILD_DIR/LGI*/src/hash $RPM_BUILD_ROOT/%{prefix}/bin
cp $RPM_BUILD_DIR/LGI*/src/csv $RPM_BUILD_ROOT/%{prefix}/bin
cp $RPM_BUILD_DIR/LGI*/src/xml $RPM_BUILD_ROOT/%{prefix}/bin
cp $RPM_BUILD_DIR/LGI*/docs/LGI.pdf $RPM_BUILD_ROOT/%{prefix}/docs
cp $RPM_BUILD_DIR/LGI*/ChangeLog.txt $RPM_BUILD_ROOT/%{prefix}/docs
cp $RPM_BUILD_DIR/LGI*/LICENSE.txt $RPM_BUILD_ROOT/%{prefix}/docs
cp $RPM_BUILD_DIR/LGI*/certificates/LGI+CA.crt $RPM_BUILD_ROOT/%{prefix}/certificates
cat << EOF_PROFILE_SH > $RPM_BUILD_ROOT/etc/profile.d/LGI.sh
export PATH=\${PATH}:PATCHTHISPATH/bin
EOF_PROFILE_SH
cat << EOF_PROFILE_CSH > $RPM_BUILD_ROOT/etc/profile.d/LGI.csh
setenv PATH "\${PATH}:PATCHTHISPATH/bin"
EOF_PROFILE_CSH

%files
%defattr(-,root,root)
%attr(755,root,root) %dir %{prefix}
%attr(755,root,root) %dir %{prefix}/docs
%attr(755,root,root) %dir %{prefix}/bin
%attr(755,root,root) %dir %{prefix}/certificates
%attr(755,root,root) %dir /etc
%attr(755,root,root) %dir /etc/profile.d
%attr(755,root,root) %{prefix}/bin/*
%attr(644,root,root) %{prefix}/docs/*
%config(noreplace) %attr(644,root,root) %{prefix}/certificates/LGI+CA.crt
%attr(644,root,root) /etc/profile.d/LGI.sh
%attr(644,root,root) /etc/profile.d/LGI.csh

%post
ESCAPED=`echo -e "$RPM_INSTALL_PREFIX" | sed "s/\//\\\\\\\\\//g"`
sed "s/PATCHTHISPATH/$ESCAPED/g" -i /etc/profile.d/LGI.sh
sed "s/PATCHTHISPATH/$ESCAPED/g" -i /etc/profile.d/LGI.csh
