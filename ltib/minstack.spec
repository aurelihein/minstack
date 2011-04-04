%define pfx /opt/freescale/rootfs/%{_target_cpu}

Summary         : A minimal stack for UDP/TCP transaction
Name            : minstack
Version         : 0.5.2
Release         : 0
License         : GPL
Vendor          : CASTEL
Packager        : Aurelien BOUIN
Group           : Library 
URL             : www.google.fr
Source          : %{name}-%{version}.tar.gz
BuildRoot       : %{_tmppath}/%{name}
Prefix          : %{pfx}

%Description
%{summary}

%Prep
%setup 

%Build
./configure --prefix=%{_prefix} --host=$CFGHOST --enable-shared --disable-static --enable-test

make

%Install
rm -rf $RPM_BUILD_ROOT
make install DESTDIR=$RPM_BUILD_ROOT/%{pfx}
cd $RPM_BUILD_ROOT/%{pfx}
find -name "*.la" |xargs rm -f

%Clean
rm -rf $RPM_BUILD_ROOT

%Files
%defattr(-,root,root)
%{pfx}/*
