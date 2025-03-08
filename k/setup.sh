#Install K editor

if_install_pkg gcc make

if [ -f $overdir/src/k21/k ]; then
    cd $overdir/src/k21
    make clean
fi

cmcdir $vardir/k
if [ ! -f b.c ]; then
    cp -r $overdir/src/k21/* .
fi
make install
cd $overdir

sh setup.kback

if [ -d /etc/cron.daily ]; then
    cp -v setup.kback	/etc/cron.daily/kback
fi

if [ ! -f /etc/termcap ]; then
    install -m 644 termcap /etc
fi
