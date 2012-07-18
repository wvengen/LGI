#
# The LGI resource spec file...
#
%define name LGI_resource
%define version 1.31.2
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
Summary: Leiden Grid Infrastructure resource software
Name: %{name}
Version: %{version}
Release: %{release}
License: GPL v3
Group: Services
Distribution: rhel
Source: %{sources}
URL: %{url}
Vendor: Theoretical Chemistry Group Leiden University
Packager: mark somers <m.somers@chem.leidenuniv.nl>
BuildRoot: %{_buildroot}
Prefix: %{prefix}
Requires: /etc/redhat-release, /bin/sed, /sbin/chkconfig, /sbin/service, /bin/cat, /usr/bin/killall, /bin/touch
BuildRequires: gcc-c++
BuildRequires: curl-devel

%description
This is the resource software for running as part of the Leiden Grid Infrastructure Grid. 

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
mkdir -p $RPM_BUILD_ROOT/%{prefix}/sbin 
mkdir -p $RPM_BUILD_ROOT/%{prefix}/docs/hello_world_scripts
mkdir -p $RPM_BUILD_ROOT/%{prefix}/docs/hello_world_pbs_scripts
mkdir -p $RPM_BUILD_ROOT/%{prefix}/docs/hello_world_ll_scripts
mkdir -p $RPM_BUILD_ROOT/%{prefix}/docs/hello_world_sge_scripts
mkdir -p $RPM_BUILD_ROOT/%{prefix}/docs/hello_world_glite_scripts
mkdir -p $RPM_BUILD_ROOT/%{prefix}/certificates
mkdir -p $RPM_BUILD_ROOT/var/run
mkdir -p $RPM_BUILD_ROOT/var/spool/LGI
mkdir -p $RPM_BUILD_ROOT/etc/init.d
mkdir -p $RPM_BUILD_ROOT/etc/profile.d
mkdir -p $RPM_BUILD_ROOT/etc/logrotate.d
cp $RPM_BUILD_DIR/LGI*/src/LGI_daemon $RPM_BUILD_ROOT/%{prefix}/sbin
cp $RPM_BUILD_DIR/LGI*/src/LGI_qsub $RPM_BUILD_ROOT/%{prefix}/bin
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
cp $RPM_BUILD_DIR/LGI*/daemon/hello_world_scripts/* $RPM_BUILD_ROOT/%{prefix}/docs/hello_world_scripts
cp $RPM_BUILD_DIR/LGI*/daemon/hello_world_pbs_scripts/* $RPM_BUILD_ROOT/%{prefix}/docs/hello_world_pbs_scripts
cp $RPM_BUILD_DIR/LGI*/daemon/hello_world_ll_scripts/* $RPM_BUILD_ROOT/%{prefix}/docs/hello_world_ll_scripts
cp $RPM_BUILD_DIR/LGI*/daemon/hello_world_sge_scripts/* $RPM_BUILD_ROOT/%{prefix}/docs/hello_world_sge_scripts
cp $RPM_BUILD_DIR/LGI*/daemon/hello_world_glite_scripts/* $RPM_BUILD_ROOT/%{prefix}/docs/hello_world_glite_scripts
cat << EOF_PROFILE_SH > $RPM_BUILD_ROOT/etc/profile.d/LGI.sh
export PATH=\${PATH}:PATCHTHISPATH/bin
EOF_PROFILE_SH
cat << EOF_PROFILE_CSH > $RPM_BUILD_ROOT/etc/profile.d/LGI.csh
setenv PATH "\${PATH}:PATCHTHISPATH/bin"
EOF_PROFILE_CSH
cat << EOF_DEF_CFG > $RPM_BUILD_ROOT/etc/LGI.cfg
<LGI>
	<ca_certificate_file> PATCHTHIS/certificates/LGI+CA.crt </ca_certificate_file>
	<resource>
		<resource_certificate_file> PATCHTHIS/certificates/PATCHCERTHERE </resource_certificate_file>
		<resource_key_file> PATCHTHIS/certificates/PATCHKEYHERE </resource_key_file>
                <run_directory> /var/spool/LGI </run_directory>
                <owner_allow> </owner_allow>
		<owner_deny> </owner_deny>
                <job_limit> 1 </job_limit>

		<number_of_projects> 1 </number_of_projects>

		<project number='1'>
			<project_name> LGI </project_name>
			<project_master_server> https://gliteui.wks.gorlaeus.net/LGI </project_master_server>
                
                        <owner_allow> </owner_allow>
			<owner_deny> </owner_deny>
                        <job_limit> 1 </job_limit>

			<number_of_applications> 1 </number_of_applications>

			<application number='1'>
				<application_name> hello_world </application_name>

				<owner_allow> <any> 1 </any> </owner_allow>
				<owner_deny> </owner_deny>
                                <job_limit> 1 </job_limit>
                                <max_output_size> 4096 </max_output_size>

				<check_system_limits_script> PATCHTHIS/docs/hello_world_scripts/check_system_limits_script </check_system_limits_script>
				<job_check_limits_script> PATCHTHIS/docs/hello_world_scripts/job_check_limits_script </job_check_limits_script>
                                <job_check_running_script> PATCHTHIS/docs/hello_world_scripts/job_check_running_script </job_check_running_script>
                                <job_check_finished_script> PATCHTHIS/docs/hello_world_scripts/job_check_finished_script </job_check_finished_script>
				<job_prologue_script> PATCHTHIS/docs/hello_world_scripts/job_prologue_script </job_prologue_script>
				<job_run_script> PATCHTHIS/docs/hello_world_scripts/job_run_script </job_run_script>
				<job_epilogue_script> PATCHTHIS/docs/hello_world_scripts/job_epilogue_script </job_epilogue_script>
				<job_abort_script> PATCHTHIS/docs/hello_world_scripts/job_abort_script </job_abort_script>
			</application>

		</project>

	</resource>
</LGI>
EOF_DEF_CFG
cat << EOF_INITD > $RPM_BUILD_ROOT/etc/init.d/LGI_daemon
#!/bin/sh
#
# LGI_daemon: script to start/stop the LGI daemon
#
# chkconfig: 2345 55 25
# description: LGI resource daemon

# source function library
. /etc/init.d/functions

start() {
    echo -n $"Starting LGI daemon: "
    daemon PATCHTHIS/sbin/LGI_daemon -d -l /var/log/LGI.log /etc/LGI.cfg && touch /var/spool/LGI/LGI_daemon.lock
    echo
}

stop() {
    echo -n $"Stopping LGI daemon: "
    killproc PATCHTHIS/sbin/LGI_daemon && rm -rf /var/spool/LGI/LGI_daemon.lock
    echo
}

case "\$1" in
    start)
	[ ! -f /var/spool/LGI/LGI_daemon.lock ] && start
	;;
    stop)
	[ -f /var/spool/LGI/LGI_daemon.lock ] && stop
	;;
    restart)
	stop
	start
        ;;
    status)
        status PATCHTHIS/sbin/LGI_daemon
        ;;
    *)
	echo $"Usage: \$0 {start|stop|restart|status}"
	;;
esac
EOF_INITD
cat << EOF_LOGROTATE > $RPM_BUILD_ROOT/etc/logrotate.d/LGI
/var/log/LGI.log {
    missingok
    notifempty
    size 64M
    rotate 6
    compress
    postrotate
          /usr/bin/killall -HUP LGI_daemon 2> /dev/null || true
    endscript
}
EOF_LOGROTATE

%files
%defattr(-,root,root)
%dir /var/spool/LGI
%{prefix}
%attr(750,root,root) %dir %{prefix}/sbin
%attr(750,root,root) %dir %{prefix}/certificates
%attr(750,root,root) %{prefix}/sbin/LGI_daemon
%config /etc/LGI.cfg
%attr(755,root,root) /etc/init.d/LGI_daemon
/etc/profile.d/LGI.sh
/etc/profile.d/LGI.csh
/etc/logrotate.d/LGI

%preun
if [ "$1" = "0" ]; then
 service LGI_daemon stop &> /dev/null
 chkconfig LGI_daemon off &> /dev/null
fi

%pre
if [ "$1" = "2" ]; then
 service LGI_daemon stop &> /dev/null
fi

%post
HOSTNAME=`hostname -f`
ESCAPED=`echo -e "$RPM_INSTALL_PREFIX" | sed "s/\//\\\\\\\\\//g"`
sed "s/PATCHTHIS/$ESCAPED/" -i /etc/LGI.cfg
sed "s/PATCHTHIS/$ESCAPED/" -i /etc/init.d/LGI_daemon
sed "s/PATCHKEYHERE/$HOSTNAME.key/" -i /etc/LGI.cfg
sed "s/PATCHCERTHERE/$HOSTNAME.crt/" -i /etc/LGI.cfg
sed "s/PATCHTHISPATH/$ESCAPED/" -i /etc/profile.d/LGI.sh
sed "s/PATCHTHISPATH/$ESCAPED/" -i /etc/profile.d/LGI.csh
if [ "$1" = "1" ]; then
 cat << END_OF_MESSAGE

This machine ($HOSTNAME) has been configured as an LGI resource now.

Please place the certificate in the file $RPM_INSTALL_PREFIX/certificates/$HOSTNAME.crt 
and the private key in the file $RPM_INSTALL_PREFIX/certificates/$HOSTNAME.key before 
starting the daemon with 'service LGI_daemon start'. To survive a reboot, use 'chkconfig LGI_daemon on'. 

Also make sure your LGI project server administrator has inserted the resource certificate into the LGI 
database on the LGI project (master) server.

END_OF_MESSAGE
else
 cat << END_OF_MESSAGE_2
LGI_daemon has been stopped as part of the upgrade!
END_OF_MESSAGE_2
fi
