//
// Created by genshen on 2019-03-24.
//

#include <iostream>
#include <cstring>
#include <mpi.h>

#include <eam.h>
#include <parser/setfl_parser.h>
#include <cmath>

#define MASTER_PROCESSOR 0

int main(int argc, char **argv) {
    // Initialize the MPI environment
    MPI_Init(&argc, &argv);
    // Get the number of processes
    // Get the number of processes
    int world_rank;
    MPI_Comm_size(MPI_COMM_WORLD, &world_rank);

    // Get the rank of the process
    int own_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &own_rank);

    eam *_pot;
    const std::string filepath = "./FeCuNi.eam.alloy";

    // 读取势函数文件
    //atom_type::_type_atom_types eles = 0;
    if (own_rank == MASTER_PROCESSOR) {
        char tmp[4096];
        sprintf(tmp, "%s", filepath.c_str());

        FILE *pot_file = fopen(tmp, "r");
        if (pot_file == nullptr) { // todo open too many in md.
            // kiwi::logs::e("pot", "open potential file {} failed.\n", filepath);
            std::cerr << "open potential file" << filepath << "failed" << std::endl;
            MPI_Abort(MPI_COMM_WORLD, 1);
            return 0;
        }
        // new parser
        SetflParser *parser = new SetflParser(filepath); // todo delete (vector)
        parser->parseHeader(); // elements count got. // todo parsing error.
        // eles = parser->getEles(); // elements values on non-root processors are 0.

        std::cout << "parser->geEles():" << parser->getEles() << std::endl;

        _pot = eam::newInstance(EAM_STYLE_ALLOY, parser->getEles(), MASTER_PROCESSOR, own_rank, MPI_COMM_WORLD);
        // MPIDomain::sim_processor.comm);
        // read data
        parser->parseBody(_pot); // todo parsing error.
        parser->done();
    } else {
      _pot = eam::newInstance(EAM_STYLE_ALLOY, 0, MASTER_PROCESSOR, own_rank, MPI_COMM_WORLD);
        // MPIDomain::sim_processor.own_rank,
        // MPIDomain::sim_processor.comm);
    }
    // BCast Potential
    _pot->eamBCast(MASTER_PROCESSOR,
                   own_rank,
                   MPI_COMM_WORLD);
    // MPIDomain::sim_processor.own_rank,
    // MPIDomain::sim_processor.comm);
    _pot->interpolateFile(); // interpolation.

    const double lattice_const = 2.85532;
    const double dist2 = 1.851699 *
                         1.851699;//lattice_const * lattice_const / 16;// pow(me[0] - nei[0], 2) + pow(me[1] - nei[1], 2) + pow(me[2] - nei[2], 2);
    const double df_a = 0.15;
    const double df_b = 0.15;
    std::cout << sqrt(dist2 / lattice_const / lattice_const) << "\t" << _pot->toForce(0, 0, dist2, df_a, df_b)
              << std::endl;
    delete _pot;
    // Finalize the MPI environment.
    MPI_Finalize();
    return 0;
}
