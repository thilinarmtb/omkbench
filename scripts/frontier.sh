#!/bin/bash

: ${PROJ_ID:="CSC262"}
: ${OMK_INSTALL_DIR:=./install}
: ${QUEUE:="batch"}
: ${OCCA_VERBOSE:=1}

### Don't touch anything that follows this line. ###
if [ $# -ne 3 ]; then
  echo "Usage: [PROJ_ID=value] $0 <backend> <# compute nodes> <hh:mm:ss>"
  exit 0
fi

if [ -z "$PROJ_ID" ]; then
  echo "ERROR: PROJ_ID is empty"
  exit 1
fi

if [ -z "$QUEUE" ]; then
  echo "ERROR: QUEUE is empty"
  exit 1
fi

if [ -z "$OMK_INSTALL_DIR" ]; then
  echo "ERROR: OMK_INSTALL_DIR is empty"
  exit 1
fi

bin=${OMK_INSTALL_DIR}/bin/driver
if [ ! -f $bin ]; then
  echo "Cannot find" $bin
  exit 1
fi

backend=$1
nodes=$2
time=$3

gpu_per_node=1
core_per_node=1
let nn=$nodes*$gpu_per_node
let ntasks=nn

time_fmt=`echo $time|tr ":" " "|awk '{print NF}'`
if [ "$time_fmt" -ne "3" ]; then
  echo "Warning: time is not in the format <hh:mm:ss>"
  echo $time
  exit 1
fi

striping_factor=256
striping_unit=67108864
if [ $nodes -lt 1024 ]; then
  striping_factor=128
  striping_unit=16777216
elif [ $nodes -lt 128 ]; then
  striping_factor=64
  striping_unit=16777216
fi
MPICH_MPIIO_HINTS="*:cray_cb_write_lock_mode=2:cray_cb_nodes_multiplier=4:\
striping_unit=${striping_unit}:striping_factor=${striping_factor}:\
romio_cb_write=enable:romio_ds_write=disable:romio_no_indep_rw=true"

# sbatch
SFILE=s.bin
echo "#!/bin/bash" > $SFILE
echo "#SBATCH -A $PROJ_ID" >>$SFILE
echo "#SBATCH -J omk" >>$SFILE
echo "#SBATCH -o %x-%j.out" >>$SFILE
echo "#SBATCH -t $time" >>$SFILE
echo "#SBATCH -N $nodes" >>$SFILE
echo "#SBATCH -p $QUEUE" >>$SFILE
echo "#SBATCH -C nvme" >>$SFILE
echo "#SBATCH --exclusive" >>$SFILE
echo "#SBATCH --ntasks-per-node=$gpu_per_node" >>$SFILE
echo "#SBATCH --gpus-per-task=1" >>$SFILE
echo "#SBATCH --gpu-bind=closest" >>$SFILE
echo "#SBATCH --cpus-per-task=$core_per_node" >>$SFILE
echo "" >>$SFILE

echo "module load PrgEnv-gnu" >> $SFILE
echo "module load craype-accel-amd-gfx90a" >> $SFILE
echo "module load cray-mpich" >> $SFILE
echo "module load rocm" >> $SFILE
echo "module unload cray-libsci" >> $SFILE
echo "module list" >> $SFILE
echo "" >>$SFILE

echo "rocm-smi" >>$SFILE
echo "rocm-smi --showpids" >>$SFILE
echo "" >>$SFILE

echo "# which nodes am I running on?" >>$SFILE
echo "squeue -u \$USER" >>$SFILE
echo "" >>$SFILE

echo "export OMK_INSTALL_DIR=${OMK_INSTALL_DIR}" >>$SFILE
echo "" >>$SFILE

echo "export MPICH_GPU_SUPPORT_ENABLED=1" >>$SFILE
echo "export MPICH_OFI_NIC_POLICY=NUMA" >>$SFILE
echo "" >>$SFILE

## These must be set before compiling so the executable picks up GTL
echo "export PE_MPICH_GTL_DIR_amd_gfx90a=\"-L${CRAY_MPICH_ROOTDIR}/gtl/lib\"" \
  >> $SFILE
echo "export PE_MPICH_GTL_LIBS_amd_gfx90a=\"-lmpi_gtl_hsa\"" >> $SFILE
echo "" >> $SFILE

echo "ulimit -s unlimited " >>$SFILE
echo "" >> $SFILE

echo "date" >>$SFILE
echo "" >> $SFILE

echo "srun -N $nodes -n $ntasks $bin --backend ${backend}" >>$SFILE

sbatch $SFILE

# clean-up
rm $SFILE
