#sbs-git:slp/unmodified/libxslt libxslt 1.1.16 64ded0203040e8a50821c584ba754b988fc2ad6b
Name:       libxslt
Summary:    Library providing the Gnome XSLT engine
Version: 1.1.16
Release:    1
Group:      System/Libraries
License:    MIT v2 with Ad Clause License
URL:        http://xmlsoft.org/XSLT/
Source0:    %{name}-%{version}.tar.gz
Patch0:     libxslt-build.patch
Patch10:    libxslt-python-site-packages64.patch
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


%package python
Summary: Python bindings for the libxslt library
Group: Development/Libraries
Requires: libxslt = %{version}
Requires: libxml2 >= 2.6.5
Requires: libxml2-python >= 2.6.5
Requires: python

%description python
The libxslt-python package contains a module that permits applications
written in the Python programming language to use the interface
supplied by the libxslt library to apply XSLT transformations.

This library allows to parse sytlesheets, uses the libxml2-python
to load and save XML and HTML files. Direct access to XPath and
the XSLT transformation context are possible to extend the XSLT language
with XPath functions written in Python.


%prep
%setup -q

# libxslt-build.patch
%patch0 -p1
%patch10 -p1

%build

%configure --disable-static --with-python=/usr

make %{?jobs:-j%jobs}

%install
rm -rf %{buildroot}
mkdir -p %{buildroot}/usr/share/license
cp COPYING %{buildroot}/usr/share/license/%{name}

%make_install

%remove_docs

%post -p /sbin/ldconfig

%postun -p /sbin/ldconfig

%files
%defattr(-,root,root,-)
%manifest libxslt.manifest
%{_libdir}/lib*.so.*
%{_bindir}/xsltproc
/usr/share/license/%{name}

%files devel
%defattr(-,root,root,-)
%{_libdir}/lib*.so
%{_libdir}/*.sh
%{_datadir}/aclocal/libxslt.m4
%{_includedir}/*
/usr/bin/xslt-config
%{_libdir}/pkgconfig/libxslt.pc
%{_libdir}/pkgconfig/libexslt.pc

%files python
%defattr(-,root,root,-)
%manifest libxslt.manifest
%{python_sitelib}/libxslt.py
%{python_sitelib}/libxsltmod.so
