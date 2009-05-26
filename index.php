<?php
require_once( "inc/Config.inc" );

$LGI_URL = $Config[ "SERVER_URL" ];
$LGI_PROJECT = $Config[ "MYSQL_DEFAULT_DATABASE" ];
?>

<html>
<head>
</head>
<body>
<div style="text-align: center;">
<h1><a name="Leiden_Grid_Infrastructure"></a><span
 style="text-decoration: underline;">L</span>eiden <span
 style="text-decoration: underline;">G</span>rid <span
 style="text-decoration: underline;">I</span>nfrastructure</h1>
<h2 style="font-style: italic;">This is project "<?php echo $LGI_PROJECT ?>" on <?php echo $LGI_URL ?>.</h2>
<br>
<a href="#Main_introduction">[main]</a>
<a href="#Screenshots">[screenshots]</a> <a href="#Documentation">[documentation]</a>
<a href="#Frequently_asked_Questions">[faq]</a> <a href="#Dowload">[dowload]</a>
<a href="#Basic_interface">[basic interface]</a>
<a href="#Support">[support]</a><br>
</div>
<br>
<br>
<h3><a name="Main_introduction"></a>Main introduction to LGI<br>
</h3>
The Leiden Grid Infrastucture (LGI) is an easy to use grid middleware
designed specifically for application oriented research groups.<br>
<br>
LGI is
based on a Linux-Apache-MySQL-PHP (LAMP) stack on the so-called
project-server,
together with x509 client- and server-certificates. The project-server
Remote Procedure Calls (RPC) Application Programming Interface (API)
routines have been implemented in PHP.<br>
<br>
Users submit jobs to an LGI project-server only for specifically
installed applications using either the general web-interface or the
command-line-interface. The web-interface is automatically included and
active on
the project-server and the command-line-interface is easily compiled on
any POSIX system using a C++ compiler with the Standard Template
Library (STL) and libcurl. A link to the basic web-interface for this
project-server can be found <a href="#Basic_interface">below</a>.
Other project and application specific interfaces can make use of the
RPC-APIs LGI has to offer (see the documentation <a
 href="#Documentation">below</a>).<br>
<br>
Resources within the LGI poll an LGI project-server and request work
for applications that have been installed on a resource by
configuring and running a resource-daemon. The
resource-daemon runs as a normal user on the resource and can run
behind a firewall and or a Network Adress Translating (NAT) router. The
resource-daemon can handle
any local back-end (like Torque/PBS or LoadLeveler) through local
scripts specified in a resource-daemon xml configuration file (see
documentation <a href="#Documentation">below</a> for examples).<br>
<br>
Both the user &lt;-&gt; project-server and the resource &lt;-&gt;
project-server communication is encrypted and authenticated through the
x509 standard. <br>
<br>
The LGI software has been licenced under the <a href="LICENSE.txt">GNU
General Public License
version 3</a>.<br>
<br>
<a href="#Leiden_Grid_Infrastructure">[top]</a><br>
<br>
<br>
<h3><a name="Screenshots"></a>Screenshots of the interfaces<br>
</h3>
Here are some screenshots of the basic LGI web-interface and the LGI
command-line-interface on Linux:<br>
<br>
<a href="docs/screen_shots/basic%20interface%20job%20list.png"><img
 alt="" title="basic interface job list"
 src="docs/screen_shots/basic%20interface%20job%20list.png"
 style="border: 0px solid ; width: 200px; height: 200px;"></a><span
 style="text-decoration: underline;"></span>&nbsp; <a
 href="docs/screen_shots/basic%20interface%20submit%20job.png"><img
 alt="" title="basic interface submit job"
 src="docs/screen_shots/basic%20interface%20submit%20job.png"
 style="border: 0px solid ; width: 200px; height: 200px;"></a>&nbsp; <a
 href="docs/screen_shots/basic%20interface%20job%20details.png"><img
 alt="" title="basic interface job details"
 src="docs/screen_shots/basic%20interface%20job%20details.png"
 style="border: 0px solid ; width: 200px; height: 200px;"></a>&nbsp; <a
 href="docs/screen_shots/basic%20interface%20resource%20list.png"><img
 alt="" title="resource list"
 src="docs/screen_shots/basic%20interface%20resource%20list.png"
 style="border: 0px solid ; width: 200px; height: 200px;"></a><br>
<br>
<a href="docs/screen_shots/command%20line%20interface%20LGI_qsub.png"><img
 alt="" title="command line interface submit job"
 src="docs/screen_shots/command%20line%20interface%20LGI_qsub.png"
 style="border: 0px solid ; width: 200px; height: 200px;"></a>&nbsp; <a
 href="docs/screen_shots/command%20line%20interface%20LGI_qstat%20job%20details.png"><img
 alt="" title="command line interface job status"
 src="docs/screen_shots/command%20line%20interface%20LGI_qstat%20job%20details.png"
 style="border: 0px solid ; width: 200px; height: 200px;"></a>&nbsp; <a
 href="docs/screen_shots/command%20line%20interface%20LGI_qdel.png"><img
 alt="" title="command line interface delete job"
 src="docs/screen_shots/command%20line%20interface%20LGI_qdel.png"
 style="border: 0px solid ; width: 200px; height: 200px;"></a>&nbsp; <a
 href="docs/screen_shots/command%20line%20interface%20LGI_filetransfer%20list%20and%20download.png"><img
 alt="" title="command line interface file transfer"
 src="docs/screen_shots/command%20line%20interface%20LGI_filetransfer%20list%20and%20download.png"
 style="border: 0px solid ; width: 200px; height: 200px;"></a><br>
<br>
<a href="#Leiden_Grid_Infrastructure">[top]</a><br>
<br>
<br>
<h3><a name="Documentation"></a>Documentation and example configuration
files on LGI<br>
</h3>
The general design document of LGI can be found <a href="docs/LGI.pdf">here</a>
(pdf).<br>
<br>
The documentation on how to setup and maintain an LGI project-server or
an LGI resource can be found <a href="SETUP.txt">here</a>
(ascii/text). <br>
<br>
The MySQL database structure can be found <a
 href="LGI.db">here</a> (ascii/text).<br>
<br>
An example resource-daemon configuration file can be found <a
 href="docs/LGI.cfg">here</a> (ascii/text).<br>
<br>
Example resource-daemon back-end scripts for Torque/PBS can be found <a
 href="docs/hello_world_pbs_scripts/">here</a> (ascci/text).<br>
<br>
The report written for NCF-NWO on the LGI deployment project (see
support <a href="#Support">below</a>) can be found <a
 href="docs/LGI_report.pdf">here</a> (pdf).<br>
<br>
All these documents are also part of the LGI middleware distribution
you can dowload <a href="#Dowload">below</a>.<br>
<br>
<a href="#Leiden_Grid_Infrastructure">[top]</a><br>
<br>
<br>
<h3><a name="Frequently_asked_Questions"></a>Frequently asked Questions
on LGI<br>
</h3>
<span style="font-weight: bold;">Q:</span> Why was LGI developed?<br>
<span style="font-weight: bold;">A:</span> When the <a
 href="http://theorchem.leidenuniv.nl/">Theoretical Chemistry group</a>
of the <a href="http://leidenuniv.nl/">Leiden University</a> was
looking for a way to connect all <a
 href="http://theorchem.leidenuniv.nl/en/facilities">it's clusters</a>
and the <a href="http://www.sara.nl">dutch supercomputers</a> to a
single easy to use interface, it was found that well-known grid
middleware solutions pose several problems (check out <a
 href="http://www.globusconsortium.org/tutorial/">this tutorial</a>);
1) installing grid middleware on computers not administrated by group
members is impossible without root access, 2) firewalls and NAT routers
pose problems, 3) user interfaces dealing with proxy-certificates and
job submission description languages are hard to use, 4) maintenance,
administration and deployment of the middleware is hard for standard
UNIX admins and 5) some of the applications are licensed to specific
users on specific computers and are binary-only distributed. Moreover
looking at the number of programs used by the group and their
respective use, it was found that they only use about five applications
at most and typical calculations take days to weeks instead of minutes.
Also the number of calculations was found to be small compared to the
numbers for which other grid middleware software was designed. It
appeared that a simple to use, easy to deploy and administrate
middleware could be built in-house to circumvent the above issues. In
short; most grid middlewares are designed for <a
 href="http://www.cs.wisc.edu/condor/htc.html">high throughput computing</a>
rather than <a
 href="http://en.wikipedia.org/wiki/High-performance_computing">high
performance computing</a> and ease of use.<br>
<br>
<span style="font-weight: bold;">Q:</span> Why was LGI developed on a
Linux-Apache-MySQL-PHP stack?<br>
<span style="font-weight: bold;">A:</span> The so-called LAMP-stack is
a well known stack for Linux / UNIX administrators. Installing such a
system is well documented on the web, easily done using
package-managers on whatever your favourite distribution is and runs on
any type of hardware.<br>
<br>
<span style="font-weight: bold;">Q:</span> Why were the resource-daemon
and the command-line-interface tools written in C++ using the Standard
Template Library?<br style="font-weight: bold;">
<span style="font-weight: bold;">A:</span> C++ and the STL, together
with libcurl, are well supported on any system. The standard C++ code
is therefore very portable and installation only requires a single
'make' command to be issued.<br>
<span style="font-weight: bold;"></span><br>
<a href="#Leiden_Grid_Infrastructure">[top]</a><br>
<br>
<br>
<h3><a name="Dowload"></a>Dowload of LGI middleware <br>
</h3>
The LGI middleware source code and documentation can be downloaded from
<a href="LGI.tar.gz">here</a>
(.tar.gz).<br>
<br>
<a href="#Leiden_Grid_Infrastructure">[top]</a><br>
<br>
<br>
<h3><a name="Basic_interface"></a>Basic interfaces of project "<?php echo $LGI_PROJECT ?>"<br>
</h3>
The basic web-interface of this LGI project-server can be reached
through <a href="<?php echo "$LGI_URL/basic_interface/index.php" ?>">this</a> link. You can
only use the basic web-interface if you have a valid personal x509
certificate signed by the LGI Certificate Authority of this project
"<?php echo $LGI_PROJECT ?>". <br>
<br>
To use the basic command-line-interface, you need to <a href="#Dowload">download</a>
the LGI software and compile it on your favourite POSIX system. Be sure
you have installed libcurl too (get it from <a
 href="http://curl.haxx.se/libcurl/">http://curl.haxx.se/libcurl/</a>).
Check out the documentation <a href="#Documentation">above</a> on how
to configure the tools and to see some examples on how to use them.<br>
<br>
<a href="#Leiden_Grid_Infrastructure">[top]</a><br>
<br>
<br>
<h3><a name="Support"></a>Support</h3>
The development of LGI has been supported by the <a
 href="http://theorchem.leidenuniv.nl/">Theoretical Chemistry group</a>
of the <a href="http://www.chem.leidenuniv.nl/">Leiden Institute of
Chemistry</a> of the <a href="http://leidenuniv.nl">Leiden University</a>.
The deployment of LGI for the Theoretical Chemistry Group on their <a
 href="http://theorchem.leidenuniv.nl/en/facilities">local clusters</a>
and the <a href="http://www.sara.nl">national (dutch) super computers</a>
has been supported by <a
 href="http://www.nwo.nl/nwohome.nsf/pages/ACPP_4X6R5C_Eng">NWO-NCF</a>
through grant nr. <a
 href="http://www.nwo.nl/projecten.nsf/pages/2300152441">NRG-2008.02</a>
(see documentation <a href="#Documentation">above</a>).<br>
<br>
<a href="#Leiden_Grid_Infrastructure">[top]</a><br>
<br>
<br>
<br>
</body>
</html>

