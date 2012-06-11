#
# The LGI python interface spec file...
#
%define name LGI_python
%define version 1.31
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
Summary: Leiden Grid Infrastructure python interface
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
Requires: /etc/redhat-release, python

%description
This is the python interface to the Leiden Grid Infrastructure Grid. 

See %{url}

%prep
mkdir -p $RPM_BUILD_ROOT
rm -rf $RPM_BUILD_ROOT/*
rm -rf $RPM_BUILD_DIR/LGI*
rm -rf $RPM_SOURCE_DIR/LGI*

%clean
rm -rf $RPM_BUILD_ROOT/*

%build
tar -zxf $RPM_SOURCE_DIR/LGI*.tar.gz

%install
mkdir -p $RPM_BUILD_ROOT/%{prefix}/python
mkdir -p $RPM_BUILD_ROOT/%{prefix}/docs
cp $RPM_BUILD_DIR/LGI*/python/LGI.py $RPM_BUILD_ROOT/%{prefix}/python
cp $RPM_BUILD_DIR/LGI*/docs/LGI.pdf $RPM_BUILD_ROOT/%{prefix}/docs
cp $RPM_BUILD_DIR/LGI*/docs/ExampleInterface.py $RPM_BUILD_ROOT/%{prefix}/docs
cp $RPM_BUILD_DIR/LGI*/ChangeLog.txt $RPM_BUILD_ROOT/%{prefix}/docs
cp $RPM_BUILD_DIR/LGI*/LICENSE.txt $RPM_BUILD_ROOT/%{prefix}/docs

%files
%defattr(-,root,root)
%{prefix}

%postun
if [ "$1" = "0" ]; then
 rm -rf /usr/lib/python*/LGI.* &> /dev/null
 rm -rf /usr/lib64/python*/LGI.* &> /dev/null
fi

%post
rm -rf $RPM_INSTALL_PREFIX/docs/ExampleInterface.pyc
rm -rf $RPM_INSTALL_PREFIX/docs/ExampleInterface.pyo
rm -rf /usr/lib/python*/LGI.*
rm -rf /usr/lib64/python*/LGI.*
for l in `ls -d /usr/lib/* /usr/lib64/* 2> /dev/null | grep '\/python'`
do
 ln -s -T $RPM_INSTALL_PREFIX/python/LGI.py $l/LGI.py
 ln -s -T $RPM_INSTALL_PREFIX/python/LGI.pyc $l/LGI.pyc
 ln -s -T $RPM_INSTALL_PREFIX/python/LGI.pyo $l/LGI.pyo
done
