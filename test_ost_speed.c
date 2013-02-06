#include <stdio.h>
#include "mpi.h"
#include "hdf5.h"
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <sys/time.h>
#include <sys/vfs.h>
#include <sys/ioctl.h>

// LUSTRE Structure
// from /usr/include/lustre/lustre_user.h
#define LUSTRE_SUPER_MAGIC 0x0BD00BD0
#  define LOV_USER_MAGIC 0x0BD10BD0
#  define LL_IOC_LOV_SETSTRIPE  _IOW ('f', 154, long)
#  define LL_IOC_LOV_GETSTRIPE  _IOW ('f', 155, long)
#define O_LOV_DELAY_CREATE 0100000000

struct lov_user_ost_data {           // per-stripe data structure
        uint64_t l_object_id;        // OST object ID
        uint64_t l_object_gr;        // OST object group (creating MDS number)
        uint32_t l_ost_gen;          // generation of this OST index
        uint32_t l_ost_idx;          // OST index in LOV
} __attribute__((packed));
struct lov_user_md {                 // LOV EA user data (host-endian)
        uint32_t lmm_magic;          // magic number = LOV_USER_MAGIC_V1
        uint32_t lmm_pattern;        // LOV_PATTERN_RAID0, LOV_PATTERN_RAID1
        uint64_t lmm_object_id;      // LOV object ID
        uint64_t lmm_object_gr;      // LOV object group
        uint32_t lmm_stripe_size;    // size of stripe in bytes
        uint16_t lmm_stripe_count;   // num stripes in use for this object
        uint16_t lmm_stripe_offset;  // starting stripe offset in lmm_objects
        struct lov_user_ost_data  lmm_objects[0]; // per-stripe data
} __attribute__((packed));

int main (int argc, char ** argv) 
{
    int old_mask;
    int perm, f;
    int i, j, k, rank;

    MPI_Init (&argc, &argv);
    MPI_Comm_rank (MPI_COMM_WORLD, &rank);
    
    old_mask = umask (0);
    umask (old_mask);
    perm = old_mask ^ 0666;

    char temp_string[100], dataset_name[100];

    sprintf (temp_string
            ,"%s%d.h5"
            ,"interf"
            ,rank
            );

    k = 0;


    unlink (temp_string);  // clean up old stuff

    struct timeval t1;
    gettimeofday (&t1, NULL);

{
    f = open (temp_string
                 ,O_RDONLY | O_CREAT | O_LOV_DELAY_CREATE
                 ,perm
                 );

    struct lov_user_md lum;
    lum.lmm_magic = LOV_USER_MAGIC;
    lum.lmm_pattern = 0;
    lum.lmm_stripe_size = 0;
    lum.lmm_stripe_count = 1;
    lum.lmm_stripe_offset = 0;
    ioctl (f, LL_IOC_LOV_SETSTRIPE
              ,(void *) &lum
              );
    close (f);

    hid_t fid = H5Fopen (temp_string, H5F_ACC_RDWR, H5P_DEFAULT);

    if (fid < 0)
    {
        fid = H5Fcreate (temp_string, H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT);
        if (fid < 0)
        {
            fprintf (stderr, "HDF5 ERROR: "
                             "cannot open/create %s\n"
                     ,temp_string);
            return -1;
        }
    }

    hid_t h5_dataset_id;
    hid_t h5_dataspace_id, h5_memspace_id;
    herr_t status;
    int rank = 2;
    hsize_t h5_localdims[rank];
    // 512x512 is 2M
    int NX = 512*4;
    int NY = 512*2;
    h5_localdims[0] = NY;
    h5_localdims[1] = NX;
    double my_data[NY][NX];

    for (i = 0; i < NY; i++)
        for (j =0; j < NX; j++)
            my_data[i][j] = i + j + rank;

    h5_dataspace_id = H5Screate_simple (rank, h5_localdims, NULL);

    sprintf (dataset_name, "var");
//    h5_dataset_id = H5Dopen (fid, dataset_name);

    h5_dataset_id = H5Dcreate (fid
                              ,dataset_name
                              ,H5T_NATIVE_DOUBLE
                              ,h5_dataspace_id
                              ,H5P_DEFAULT
                              );

    status = H5Dwrite (h5_dataset_id
                      ,H5T_NATIVE_DOUBLE
                      ,H5S_ALL
                      ,H5S_ALL
                      ,H5P_DEFAULT
                      ,&my_data[0][0]
                      );

    if (status == 0)
    {
//            printf ("%d th iteration is done.\n", k);
    }
    else
    {
        printf ("%d th iteration has error.\n", k);
    }
    k++;
         
    H5Dclose (h5_dataset_id);
    H5Sclose (h5_dataspace_id);
    H5Fclose (fid);

    struct timeval t2;
    gettimeofday (&t2, NULL);

    printf("time: %.6lf\n", t2.tv_sec + t2.tv_usec/1000000.0 - t1.tv_sec - t1.tv_usec/1000000.0);

}
    MPI_Finalize ();

    return 0;
}
