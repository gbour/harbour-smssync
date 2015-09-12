# 
# Do NOT Edit the Auto-generated Part!
# Generated by: spectacle version 0.27
# 

Name:       harbour-smssync

# >> macros
# << macros

%{!?qtc_qmake:%define qtc_qmake %qmake}
%{!?qtc_qmake5:%define qtc_qmake5 %qmake5}
%{!?qtc_make:%define qtc_make make}
%{?qtc_builddir:%define _builddir %qtc_builddir}
Summary:    SmsSync for Jolla
Version:    0.2.0
Release:    1
Group:      Applications/Internet
License:    GPLv3
URL:        https://github.com/gbour/harbour-smssync
Source0:    %{name}-%{version}.tar.bz2
Source100:  harbour-smssync.yaml
Requires:   sailfishsilica-qt5 >= 0.10.9
Requires:   systemd-user-session-targets
BuildRequires:  pkgconfig(sailfishapp) >= 1.0.2
BuildRequires:  pkgconfig(Qt5Core)
BuildRequires:  pkgconfig(Qt5Qml)
BuildRequires:  pkgconfig(Qt5Quick)
BuildRequires:  pkgconfig(Qt5DBus)

%description
Synchronization application for your SMS


%prep
%setup -q -n %{name}-%{version}

# >> setup
# << setup

%build
# >> build pre
# << build pre

%qtc_qmake5 

%qtc_make %{?_smp_mflags}

# >> build post
# << build post

%install
rm -rf %{buildroot}
# >> install pre
# << install pre
%qmake5_install

# >> install post
mkdir -p %{buildroot}%{_libdir}/systemd/user/user-session.target.wants
# << install post

%preun
# >> preun
if [ "$1" = "0" ]; then
  systemctl-user stop harbour-smssyncd
  systemctl-user disable harbour-smssyncd
fi
# << preun

%post
# >> post
systemctl-user daemon-reload
systemctl-user restart harbour-smssyncd
systemctl-user enable harbour-smssyncd
# << post

%files
%defattr(-,root,root,-)
%{_bindir}
%{_sysconfdir}/xdg/%{name}/%{name}d.conf
%{_libdir}/systemd/user/%{name}d.service
# >> files
# << files
