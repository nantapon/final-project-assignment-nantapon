#!/bin/sh

make -C netsim_app clean
cd netsim_ui
npm run build
cd ..
rm -rf buildroot/output/build/netsim*
make -C buildroot
