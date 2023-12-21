//
// Created by genshen on 2023/12/21.
//

#include <eam_c_api.h>
#include <mpi.h>
#include <stdio.h>

int main(int argc, char **argv) {
  MPI_Init(&argc, &argv);
  int world_rank;
  int own_rank;
  MPI_Comm_size(MPI_COMM_WORLD, &world_rank);
  MPI_Comm_rank(MPI_COMM_WORLD, &own_rank);

  char *eam_file_path = "./VTiTa.eam.fs";
  int master_rank = 0;
  int status = eam_pot_init(eam_file_path, 1, own_rank, master_rank, MPI_COMM_WORLD);
  if (status != 0) {
    printf("open or parse the potential file error.\n");
  }

  const double dist2 = 1.851699 * 1.851699;
  const double df_a = 0.15;
  const double df_b = 0.15;
  printf("force: %f\n", eam_pot_to_force(1, 1, dist2, df_a, df_b));

  eam_pot_destroy(); // free memory
  return 0;
}
