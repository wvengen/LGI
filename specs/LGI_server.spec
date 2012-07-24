#
# The LGI server spec file...
#
%define name LGI_server
%define version 1.31.4
%define release 1.el5
%define sources http://gliteui.wks.gorlaeus.net/LGI/LGI.tar.gz
%define url http://gliteui.wks.gorlaeus.net/LGI
%define prefix /var/www/html/LGI
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
Summary: Leiden Grid Infrastructure server software
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
Requires: /etc/redhat-release, /bin/touch, /bin/sed, /sbin/chkconfig, /sbin/service, /bin/cat, httpd, mod_ssl, mysql, mysql-server, php, php-mysql, perl

%description
This is the server software for running a Leiden Grid Infrastructure Grid yourself. 

See %{url}

%prep
mkdir -p $RPM_BUILD_ROOT
rm -rf $RPM_BUILD_ROOT/*
rm -rf $RPM_BUILD_DIR/LGI*

%clean
rm -rf $RPM_BUILD_ROOT/*

%build
tar -zxf $RPM_SOURCE_DIR/LGI*.tar.gz

%install
mkdir -p $RPM_BUILD_ROOT/%{prefix}/certificates
mkdir -p $RPM_BUILD_ROOT/%{prefix}/privatekeys
mkdir -p $RPM_BUILD_ROOT/%{prefix}/basic_interface/java_scripts
mkdir -p $RPM_BUILD_ROOT/%{prefix}/docs
mkdir -p $RPM_BUILD_ROOT/%{prefix}/inc
mkdir -p $RPM_BUILD_ROOT/%{prefix}/interfaces
mkdir -p $RPM_BUILD_ROOT/%{prefix}/resources
mkdir -p $RPM_BUILD_ROOT/%{prefix}/servers
mkdir -p $RPM_BUILD_ROOT/%{prefix}/repository
mkdir -p $RPM_BUILD_ROOT/%{prefix}/tools
mkdir -p $RPM_BUILD_ROOT/%{prefix}/scheduler
mkdir -p $RPM_BUILD_ROOT/etc/httpd/conf.d
mkdir -p $RPM_BUILD_ROOT/etc/init.d
cp -r $RPM_BUILD_DIR/LGI*/basic_interface $RPM_BUILD_ROOT/%{prefix}
cp -r $RPM_BUILD_DIR/LGI*/docs $RPM_BUILD_ROOT/%{prefix}
cp -r $RPM_BUILD_DIR/LGI*/inc $RPM_BUILD_ROOT/%{prefix}
cp -r $RPM_BUILD_DIR/LGI*/interfaces $RPM_BUILD_ROOT/%{prefix}
cp -r $RPM_BUILD_DIR/LGI*/privatekeys $RPM_BUILD_ROOT/%{prefix}
cp -r $RPM_BUILD_DIR/LGI*/resources $RPM_BUILD_ROOT/%{prefix}
cp -r $RPM_BUILD_DIR/LGI*/servers $RPM_BUILD_ROOT/%{prefix}
cp -r $RPM_BUILD_DIR/LGI*/repository $RPM_BUILD_ROOT/%{prefix}
cp -r $RPM_BUILD_DIR/LGI*/tools $RPM_BUILD_ROOT/%{prefix}
cp -r $RPM_BUILD_DIR/LGI*/scheduler $RPM_BUILD_ROOT/%{prefix}
rm -rf $RPM_BUILD_ROOT/%{prefix}/scheduler/check_running
cp $RPM_SOURCE_DIR/LGI*.tar.gz $RPM_BUILD_ROOT/%{prefix}
cp $RPM_BUILD_DIR/LGI*/ChangeLog.txt $RPM_BUILD_ROOT/%{prefix}
cp $RPM_BUILD_DIR/LGI*/LICENSE.txt $RPM_BUILD_ROOT/%{prefix}
cp $RPM_BUILD_DIR/LGI*/LGI.db $RPM_BUILD_ROOT/%{prefix}
cp $RPM_BUILD_DIR/LGI*/SETUP.txt $RPM_BUILD_ROOT/%{prefix}
cp $RPM_BUILD_DIR/LGI*/index.php $RPM_BUILD_ROOT/%{prefix}
cp $RPM_BUILD_DIR/LGI*/LGI.conf $RPM_BUILD_ROOT/%{prefix}
ln -s %{prefix}/LGI.conf $RPM_BUILD_ROOT/etc/httpd/conf.d/LGI.conf
cp $RPM_BUILD_DIR/LGI*/certificates/LGI+CA.crt $RPM_BUILD_ROOT/%{prefix}/certificates
ln -s %{prefix}/certificates/LGI+CA.crt $RPM_BUILD_ROOT/%{prefix}/LGI+CA.crt
cat > $RPM_BUILD_ROOT/%{prefix}/scheduler/LGI_scheduler << END_OF_SCHED
#!/bin/sh
#
# LGI_scheduler: script to start/stop the LGI project server scheduler
#
# chkconfig: 2345 55 25
# description: LGI scheduler
    
# source function library
. /etc/init.d/functions

start() {
    echo -n $"Starting LGI scheduler: "
    daemon --user LGI "PATCHTHIS/scheduler/scheduler.php &" && touch PATCHTHIS/scheduler/LGI_scheduler.lock
    echo
}   
    
stop() {
    echo -n $"Stopping LGI scheduler: "
    killproc PATCHTHIS/scheduler/scheduler.php && rm -f PATCHTHIS/scheduler/LGI_scheduler.lock
    echo
}

case "\$1" in
    start)
        [ ! -f PATCHTHIS/scheduler/LGI_scheduler.lock ] && start
        ;;
    stop)
        [ -f PATCHTHIS/scheduler/LGI_scheduler.lock ] && stop
        ;;
    restart)
        stop
        start
        ;;
    status)
        status PATCHTHIS/scheduler/scheduler.php
        ;;
    *)
        echo $"Usage: \$0 {start|stop|restart|status}"
        ;;
esac
END_OF_SCHED
ln -s %{prefix}/scheduler/LGI_scheduler $RPM_BUILD_ROOT/etc/init.d/LGI_scheduler
cat > $RPM_BUILD_ROOT/%{prefix}/ssl_LGI.conf << END_OF_SSL_LGI
LoadModule ssl_module modules/mod_ssl.so
Listen 443
AddType application/x-x509-ca-cert .crt
AddType application/x-pkcs7-crl    .crl
SSLPassPhraseDialog  builtin
SSLSessionCache         shmcb:/var/cache/mod_ssl/scache(512000)
SSLSessionCacheTimeout  5
SSLMutex default
SSLRandomSeed startup file:/dev/urandom  256
SSLRandomSeed connect builtin
SSLCryptoDevice builtin
<VirtualHost _default_:443>
 ErrorLog logs/ssl_error_log
 TransferLog logs/ssl_access_log
 LogLevel warn
 SSLEngine on
 SSLCipherSuite ALL:!ADH:!EXPORT56:RC4+RSA:+HIGH:+MEDIUM:+LOW:+SSLv2:+EXP
 SSLCertificateFile PATCHTHIS/certificates/LGI@PATCHHOSTNAME.crt
 SSLCertificateKeyFile PATCHTHIS/privatekeys/LGI@PATCHHOSTNAME.key
 SSLCertificateChainFile PATCHTHIS/certificates/LGI+CA.crt
 SSLCACertificateFile PATCHTHIS/certificates/LGI+CA.crt
 # SSLCARevocationFile PATCHTHIS/certificates/LGI+CA.crl
 SSLVerifyClient optional
 SSLVerifyDepth  10
 SSLOptions +ExportCertData
 <Files ~ "\.(cgi|shtml|phtml|php3?)$">
    SSLOptions +StdEnvVars
 </Files>
 <Directory "/var/www/cgi-bin">
    SSLOptions +StdEnvVars
 </Directory>
 SetEnvIf User-Agent ".*MSIE.*" \\
         nokeepalive ssl-unclean-shutdown \\
         downgrade-1.0 force-response-1.0
 CustomLog logs/ssl_request_log \\
          "%t %h %{SSL_PROTOCOL}x %{SSL_CIPHER}x \\"%r\\" %b"
</VirtualHost>
END_OF_SSL_LGI
ln -s %{prefix}/ssl_LGI.conf $RPM_BUILD_ROOT/etc/httpd/conf.d/ssl_LGI.conf

%files
%defattr(-,root,root)
%attr(755,root,root) %dir /etc
%attr(755,root,root) %dir /etc/httpd
%attr(755,root,root) %dir /etc/httpd/conf.d
%attr(755,root,root) %dir %{prefix}
%attr(755,root,root) %dir %{prefix}/certificates
%attr(750,root,root) %dir %{prefix}/privatekeys
%attr(750,root,root) %dir %{prefix}/basic_interface
%attr(640,root,root) %{prefix}/basic_interface/*
%attr(750,root,root) %dir %{prefix}/basic_interface/java_scripts
%attr(755,root,root) %dir %{prefix}/docs
%attr(750,root,root) %dir %{prefix}/inc
%attr(750,root,root) %dir %{prefix}/interfaces
%attr(750,root,root) %dir %{prefix}/resources
%attr(750,root,root) %dir %{prefix}/servers
%attr(6770,root,root) %dir %{prefix}/repository
%attr(750,root,root) %dir %{prefix}/tools
%attr(750,root,root) %dir %{prefix}/scheduler
%attr(644,root,root) %{prefix}/certificates/LGI+CA.crt
%attr(640,root,root) %{prefix}/basic_interface/java_scripts/*
%attr(644,root,root) %{prefix}/docs/*
%attr(755,root,root) %dir %{prefix}/docs/*
%attr(640,root,root) %{prefix}/inc/*
%config(noreplace) %attr(640,root,root) %{prefix}/inc/Config.inc
%attr(640,root,root) %{prefix}/privatekeys/*
%attr(640,root,root) %{prefix}/interfaces/*
%attr(640,root,root) %{prefix}/resources/*
%attr(640,root,root) %{prefix}/servers/*
%attr(640,root,root) %{prefix}/scheduler/*
%attr(750,root,root) %{prefix}/scheduler/scheduler.php
%attr(755,root,root) %{prefix}/scheduler/LGI_scheduler
%attr(640,root,root) %{prefix}/repository/*
%attr(750,root,root) %{prefix}/repository/*.cgi
%attr(640,root,root) %{prefix}/tools/*
%attr(750,root,root) %{prefix}/tools/ManageDB
%attr(644,root,root) %{prefix}/ChangeLog.txt
%attr(644,root,root) %{prefix}/LICENSE.txt
%attr(644,root,root) %{prefix}/LGI.db
%attr(644,root,root) %{prefix}/LGI*.tar.gz
%attr(644,root,root) %{prefix}/SETUP.txt
%attr(640,root,root) %{prefix}/index.php
%attr(644,root,root) %{prefix}/LGI.conf
%config(noreplace) %attr(644,root,root) %{prefix}/ssl_LGI.conf
%attr(644,root,root) %{prefix}/LGI+CA.crt
%attr(640,root,root) %{prefix}/*/.htaccess
%attr(640,root,root) %{prefix}/repository/.LGI_resource_list
%attr(644,root,root) /etc/httpd/conf.d/ssl_LGI.conf
%attr(644,root,root) /etc/httpd/conf.d/LGI.conf
%attr(755,root,root) /etc/init.d/LGI_scheduler

%preun
if [ "$1" = "0" ]; then
 apachectl stop &> /dev/null
 if [ -f $RPM_INSTALL_PREFIX/scheduler/LGI_scheduler.lock ]; then
  service LGI_scheduler stop &> /dev/null
 fi
 chkconfig LGI_scheduler off &> /dev/null
 cat << END_OF_MESSAGE
apache has been stopped as part of the erase!
END_OF_MESSAGE
 PASSWD=""
 mysql -u root --password="$PASSWD" -e 'SHOW DATABASES' mysql &> /dev/null
 if [ "$?" -ne "0" ]; then
  echo
  read -s -p "Enter mysql root password to DROP the LGI database: " PASSWD < /dev/tty
  echo
  echo
 else
  echo 
  echo ! ! ! YOU REALY SHOULD SET A mysql ROOT PASSWORD. USE 'mysqladmin -u root password "yourmysqlrootpassword"' ! ! !
  echo
 fi
 mysql -u root --password="$PASSWD" -e 'SHOW DATABASES' mysql > /dev/null && echo $PASSWD > /root/.tmppasswd
fi

%postun
if [ "$1" = "0" ]; then
 PASSWD=`cat /root/.tmppasswd`; rm -rf /root/.tmppasswd
 mysql -u root --password="$PASSWD" -e 'DROP DATABASE LGI' mysql &> /dev/null
 mysql -u root --password="$PASSWD" -e 'DROP USER "LGI"@"localhost"' mysql &> /dev/null
 rm -rf $RPM_INSTALL_PREFIX/repository/???
 userdel LGI &> /dev/null
 groupdel LGI &> /dev/null
 if [ -f /etc/httpd/conf.d/ssl.conf.SAVED_BY_LGI ]; then
  mv /etc/httpd/conf.d/ssl.conf.SAVED_BY_LGI /etc/httpd/conf.d/ssl.conf &> /dev/null
  echo /etc/httpd/conf.d/ssl.conf has been restored from /etc/httpd/conf.d/ssl.conf.SAVED_BY_LGI
 fi
fi

%pre
LGIPASSWD=`dd if=/dev/urandom count=128 2> /dev/null | tr -dc A-Za-z0-9 | head -c 12`
if [ "$1" = "2" ]; then
 LGIPASSWD=`grep MYSQL_PASSWD $RPM_INSTALL_PREFIX/inc/Config.inc | cut -d'=' -f2 | sed "s/;\$//g" | sed "s/\"//g"`
 if [ -f $RPM_INSTALL_PREFIX/scheduler/LGI_scheduler.lock ]; then
  service LGI_scheduler stop &> /dev/null
 fi
 apachectl stop &> /dev/null
 cat << END_OF_MESSAGE_2
LGI_scheduler and apache have been stopped as part of the upgrade!
END_OF_MESSAGE_2
else
 if [ -f /etc/httpd/conf.d/ssl.conf ]; then
  mv /etc/httpd/conf.d/ssl.conf /etc/httpd/conf.d/ssl.conf.SAVED_BY_LGI
  echo /etc/httpd/conf.d/ssl.conf has been saved to /etc/httpd/conf.d/ssl.conf.SAVED_BY_LGI
 fi
fi
echo $LGIPASSWD > /root/.tmplgipasswd
PASSWD=""
mysql -u root --password="$PASSWD" -e 'SHOW DATABASES' mysql &> /dev/null
if [ "$?" -ne "0" ]; then
 echo
 read -s -p "Enter mysql root password to setup LGI database: " PASSWD < /dev/tty
 echo
 echo
else
 echo
 echo ! ! ! YOU REALY SHOULD SET A mysql ROOT PASSWORD. USE 'mysqladmin -u root password "yourmysqlrootpassword"' ! ! !
 echo
fi
mysql -u root --password="$PASSWD" -e 'SHOW DATABASES' mysql > /dev/null && echo $PASSWD > /root/.tmppasswd

%post
PASSWD=`cat /root/.tmppasswd`; rm -rf /root/.tmppasswd
LGIPASSWD=`cat /root/.tmplgipasswd`; rm -rf /root/.tmplgipasswd
if [ "$1" = "1" ]; then
 luseradd -d $RPM_INSTALL_PREFIX -M -s /sbin/nologin -g LGI LGI
 usermod -a -G LGI apache
 mysqladmin -u root --password="$PASSWD" create "LGI"
 echo "GRANT ALL PRIVILEGES ON LGI.* to \"LGI\"@\"localhost\" IDENTIFIED BY \"$LGIPASSWD\"" | mysql -u root --password="$PASSWD" mysql
 mysql -u LGI --password="$LGIPASSWD" LGI < $RPM_INSTALL_PREFIX/LGI.db
fi
FILES1=`find $RPM_INSTALL_PREFIX/inc -type f -name '*.inc'`
FILES2=`find $RPM_INSTALL_PREFIX/basic_interface $RPM_INSTALL_PREFIX/resources $RPM_INSTALL_PREFIX/servers $RPM_INSTALL_PREFIX/interfaces -type f -name '*.php'`
FILES="$FILES1 $FILES2 $RPM_INSTALL_PREFIX/tools/ManageDB $RPM_INSTALL_PREFIX/scheduler/scheduler.php"
ESCAPED=`echo -e "$RPM_INSTALL_PREFIX" | sed "s/\//\\\\\\\\\//g"`
for f in $FILES
do
 sed "s/..\/inc\//$ESCAPED\/inc\//" -i $f
done
chown -R LGI.LGI $RPM_INSTALL_PREFIX
chmod g+w $RPM_INSTALL_PREFIX/repository
chmod 750 $RPM_INSTALL_PREFIX/docs/screen_shots
chmod 750 $RPM_INSTALL_PREFIX/docs/hello_world_pbs_scripts
chmod 750 $RPM_INSTALL_PREFIX/privatekeys
chmod 770 $RPM_INSTALL_PREFIX
rm -rf $RPM_INSTALL_PREFIX/docs/ExampleInterface.pyc $RPM_INSTALL_PREFIX/docs/ExampleInterface.pyo
HOSTNAME=`hostname -f`
ESCAPED=`echo -e "$RPM_INSTALL_PREFIX" | sed "s/\//\\\\\\\\\//g"`
sed "s/LGIpasswd/$LGIPASSWD/g" -i $RPM_INSTALL_PREFIX/inc/Config.inc
sed "s/gliteui.wks.gorlaeus.net/$HOSTNAME/g" -i $RPM_INSTALL_PREFIX/inc/Config.inc
sed "s/gliteui/$HOSTNAME/g" -i $RPM_INSTALL_PREFIX/inc/Config.inc
sed "s/\.\.\/certificates/$ESCAPED\/certificates/g" -i $RPM_INSTALL_PREFIX/inc/Config.inc
sed "s/\.\.\/privatekeys/$ESCAPED\/privatekeys/g" -i $RPM_INSTALL_PREFIX/inc/Config.inc
sed "s/\/var\/www\/html\/LGI\/repository/$ESCAPED\/repository/g" -i $RPM_INSTALL_PREFIX/inc/Config.inc
sed "s/MYSQL_PASSWD=\"LGIpasswd\"/MYSQL_PASSWD=\"$LGIPASSWD\"/g" -i $RPM_INSTALL_PREFIX/tools/ManageDB
sed "s/LGI_ROOT=\${HOME}\/scheduler/LGI_ROOT=\"$ESCAPED\/scheduler\"/g" -i $RPM_INSTALL_PREFIX/tools/ManageDB
sed "s/PATCHTHIS/$ESCAPED/g" -i $RPM_INSTALL_PREFIX/scheduler/LGI_scheduler
rm -rf /etc/init.d/LGI_scheduler; ln -s $RPM_INSTALL_PREFIX/scheduler/LGI_scheduler /etc/init.d/LGI_scheduler
sed "s/PATCHTHIS/$ESCAPED/g" -i $RPM_INSTALL_PREFIX/ssl_LGI.conf
sed "s/PATCHHOSTNAME/$HOSTNAME/g" -i $RPM_INSTALL_PREFIX/ssl_LGI.conf
rm -rf /etc/httpd/conf.d/ssl_LGI.conf; ln -s $RPM_INSTALL_PREFIX/ssl_LGI.conf /etc/httpd/conf.d/ssl_LGI.conf
sed "s/\/var\/www\/html\/LGI/$ESCAPED/g" -i $RPM_INSTALL_PREFIX/LGI.conf
rm -rf /etc/httpd/conf.d/LGI.conf; ln -s $RPM_INSTALL_PREFIX/LGI.conf /etc/httpd/conf.d/LGI.conf
if [ "$1" = "1" ]; then
 cat << END_OF_MESSAGE_3
This machine ($HOSTNAME) has been configured as an LGI project server now.

Please place the certificate in the file $RPM_INSTALL_PREFIX/certificates/LGI@$HOSTNAME.crt
and the private key in the file $RPM_INSTALL_PREFIX/privatekeys/LGI@$HOSTNAME.key.

Make sure you insert the master server certificate into the LGI database by using
'$RPM_INSTALL_PREFIX/tools/ManageDB add resources allowed'. If $HOSTNAME 
is a slave project server, make sure that the $HOSTNAME certificate 
is also inserted into the master server LGI database. Use the ManageDB tool on the
master server for this.

Double check the SSL configuration of apache in /etc/httpd/conf.d/ssl_LGI.conf before starting 
it with 'apachectl configtest'. Start apache with 'apachectl start' if all is fine.

Start the scheduler with 'service LGI_scheduler start'. To survive a reboot, use 'chkconfig LGI_scheduler on'.

Please read $RPM_INSTALL_PREFIX/SETUP.txt to fine-tune your setup for extra performance.

END_OF_MESSAGE_3
fi
