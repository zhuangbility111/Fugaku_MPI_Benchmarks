#include "topology.h"

#define LDIM 3
#define TDIM 6

void get_topology() {
    int rank, size;

    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    int i, j, x, y, z;
    int mydimension;
    int coords[LDIM], tcoords[TDIM], reltcoords[TDIM];

    FJMPI_Topology_get_dimension(&mydimension);
    FJMPI_Topology_get_shape(&x, &y, &z);

    if (rank == 0) {
		printf("#----------------------------------------------------------------\n");
		printf("# Topology information: \n");
        printf("# My Dimension= %d\n",mydimension);
        printf("# My Shape: X= %d", x);
        if (y != 0) printf(",Y= %d", y);
        if (z != 0) printf(",Z= %d", z);
        printf("\n\n");
        for ( i=0; i < size ; i++){
            FJMPI_Topology_get_coords(MPI_COMM_WORLD, i, FJMPI_LOGICAL, mydimension, coords);
            FJMPI_Topology_get_coords(MPI_COMM_WORLD, i, FJMPI_TOFU_SYS, TDIM, tcoords);
            FJMPI_Topology_get_coords(MPI_COMM_WORLD, i, FJMPI_TOFU_REL, TDIM, reltcoords);
            switch(mydimension) {
                case 1:
                    printf("# rank to x : rank= %d, logical (X)=( %d ) ",i, coords[0]);
                    break;
                case 2:
                    printf("# rank to xy : rank= %d, logical (X,Y)=( %d, %d ) ",i, coords[0], coords[1]);
                    break;
                case 3:
                    printf("# rank to xyz : rank= %d, logical (X,Y,Z)=( %d, %d, %d ) ", i, coords[0], coords[1], coords[2]);
                    break;
                default:
                    break;
            }
            printf("Tofu (x,y,z,a,b,c)=(");
            for ( j=0; j < TDIM-1; j++) {
                printf("%d,", tcoords[j]);
            }
            printf("%d) ",tcoords[TDIM-1]);

            printf("Tofu (relative to rank 0) (x,y,z,a,b,c)=(");
            for ( j=0; j < TDIM-1; j++) {
                printf("%d,", reltcoords[j]);
            }
            printf("%d)\n",reltcoords[TDIM-1]);
        }
		printf("#----------------------------------------------------------------\n");
    }
}
