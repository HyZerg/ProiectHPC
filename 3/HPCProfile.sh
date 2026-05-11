spack load hpctoolkit
rm -rf hpctoolkit-acoustics-measurements
rm -rf hpctoolkit-acoustics-database

hpcrun -e REALTIME@100 -t ./acoustics Input.txt
hpcstruct hpctoolkit-acoustics-measurements
hpcprof hpctoolkit-acoustics-measurements
hpcviewer hpctoolkit-acoustics-database

