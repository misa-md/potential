//
// Created by genshen on 2018-12-23.
//

#ifndef POT_PARSER_H
#define POT_PARSER_H

#include <string>
#include "types.h"
#include "eam.h"
#include "atom_type_lists.h"

/**
 * parse eam potential file.
 */
class Parser {
public:
    // all types are saved in this object while parsing.
    AtomPropsList type_lists;

    explicit Parser(const std::string filename);

    /**
     * parsing header of potential file, and get necessary data such as elements count.
     */
    virtual void parseHeader() = 0;

    /**
     * parse body of potential file, and save data in potential file to eam instance
     * @param eam_instance the pointer to eam.
     */
    virtual void parseBody(eam *eam_instance) = 0;

    virtual void done();

    // get the elements count.
    inline atom_type::_type_atom_types getEles() {
        return elements_size;
    }

protected:
    // the size/count of elements in potential file.
    atom_type::_type_atom_types elements_size;
    FILE *pot_file;

    void grab(FILE *fptr, int n, double *list);

private:
    const std::string pot_filename;
};

#endif //POT_PARSER_H
