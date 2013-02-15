Name:       libxslt
Summary:    Library providing the Gnome XSLT engine
Version: 1.1.16
Release:    1
Group:      System/Libraries
License:    MIT
URL:        http://xmlsoft.org/XSLT/
Source0:    %{name}-%{version}.tar.gz
Patch0:     libxslt-build.patch
Requires(post): /sbin/ldconfig
Requires(postun): /sbin/ldconfig
BuildRequires:  pkgconfig(libxml-2.0) >= 2.6.27


%description
This C library allows to transform XML files into other XML files
(or HTML, text, ...) using the standard XSLT stylesheet transformation
mechanism. To use it you need to have a version of libxml2 >= 2.6.27
installed. The xsltproc command is a command line interface to the XSLT engine



%package devel
Summary:    Libraries, includes, etc. to embed the Gnome XSLT engine
Group:      Development/Libraries
Requires:   %{name} = %{version}-%{release}

%description devel
This C library allows to transform XML files into other XML files
(or HTML, text, ...) using the standard XSLT stylesheet transformation
mechanism. To use it you need to have a version of libxml2 >= 2.6.27
installed.

%prep
%setup -q

# libxslt-build.patch
%patch0 -p1

%build

./configure --disable-static --prefix=/usr

make %{?jobs:-j%jobs}

%install
rm -rf %{buildroot}

%make_install

%remove_docs

%post -p /sbin/ldconfig

%postun -p /sbin/ldconfig

%files
%defattr(-,root,root,-)
%{_libdir}/lib*.so.*
#%{_libdir}/libxslt-plugins
#%{_libdir}/python2.6/site-packages/libxslt.py
#%{_libdir}/python2.6/site-packages/libxslt.pyc
#%{_libdir}/python2.6/site-packages/libxslt.pyo
#%{_libdir}/python2.6/site-packages/libxsltmod.so

%{_libdir}/python2.7/site-packages/libxslt.py
%{_libdir}/python2.7/site-packages/libxslt.pyc
%{_libdir}/python2.7/site-packages/libxslt.pyo
%{_libdir}/python2.7/site-packages/libxsltmod.so

%{_bindir}/xsltproc

%files devel
%defattr(-,root,root,-)
%{_libdir}/lib*.so
%{_libdir}/*.sh
%{_datadir}/aclocal/libxslt.m4
%{_includedir}/*
/usr/bin/xslt-config
%{_libdir}/pkgconfig/libxslt.pc
%{_libdir}/pkgconfig/libexslt.pc

