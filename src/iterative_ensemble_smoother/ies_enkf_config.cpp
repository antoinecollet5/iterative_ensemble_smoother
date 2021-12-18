/*
   Copyright (C) 2019  Equinor ASA, Norway.

   The file 'ies_enkf_config.cpp' is part of ERT - Ensemble based Reservoir Tool.

   ERT is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   ERT is distributed in the hope that it will be useful, but WITHOUT ANY
   WARRANTY; without even the implied warranty of MERCHANTABILITY or
   FITNESS FOR A PARTICULAR PURPOSE.

   See the GNU General Public License at <http://www.gnu.org/licenses/gpl.html>
   for more details.
*/

#include <algorithm>
#include <cmath>

#include <ert/util/util.hpp>
#include <ert/util/type_macros.hpp>

#include <ert/analysis/std_enkf.hpp>
#include <ert/analysis/analysis_module.hpp>

#include <ert/analysis/ies/ies_enkf_config.hpp>

#define INVALID_TRUNCATION -1
#define INVALID_SUBSPACE_DIMENSION -1
#define DEFAULT_ENKF_TRUNCATION 0.98
#define DEFAULT_ENKF_SUBSPACE_DIMENSION INVALID_SUBSPACE_DIMENSION

#define DEFAULT_IES_MAX_STEPLENGTH 0.60
#define DEFAULT_IES_MIN_STEPLENGTH 0.30
#define DEFAULT_IES_DEC_STEPLENGTH 2.50
#define MIN_IES_DEC_STEPLENGTH 1.1
#define DEFAULT_IES_SUBSPACE false
#define DEFAULT_IES_INVERSION ies::IES_INVERSION_SUBSPACE_EXACT_R
#define DEFAULT_IES_LOGFILE "ies.log"
#define DEFAULT_IES_AAPROJECTION false

#define IES_ENKF_CONFIG_TYPE_ID 196402021

struct ies::enkf_config_struct {
    UTIL_TYPE_ID_DECLARATION;
    double truncation;      // Controlled by config key: ENKF_TRUNCATION_KEY
    int subspace_dimension; // Controlled by config key: ENKF_SUBSPACE_DIMENSION_KEY (-1: use Truncation instead)
    long option_flags;
    double
        ies_max_steplength; // Controlled by config key: DEFAULT_IES_MAX_STEPLENGTH_KEY
    double
        ies_min_steplength; // Controlled by config key: DEFAULT_IES_MIN_STEPLENGTH_KEY
    double
        ies_dec_steplength; // Controlled by config key: DEFAULT_IES_DEC_STEPLENGTH_KEY
    bool ies_subspace;     // Controlled by config key: DEFAULT_IES_SUBSPACE
    int ies_inversion;     // Controlled by config key: DEFAULT_IES_INVERSION
    char *ies_logfile;     // Controlled by config key: DEFAULT_IES_LOGFILE
    bool ies_aaprojection; // Controlled by config key: DEFAULT_IES_AAPROJECTION
};

ies::enkf_config_type *ies::enkf_config_alloc() {
    ies::enkf_config_type *config =
        static_cast<ies::enkf_config_type *>(util_malloc(sizeof *config));
    UTIL_TYPE_ID_INIT(config, IES_ENKF_CONFIG_TYPE_ID);
    config->ies_logfile = NULL;
    ies::enkf_config_set_truncation(config, DEFAULT_ENKF_TRUNCATION);
    ies::enkf_config_set_enkf_subspace_dimension(
        config, DEFAULT_ENKF_SUBSPACE_DIMENSION);
    ies::enkf_config_set_option_flags(
        config, ANALYSIS_NEED_ED + ANALYSIS_UPDATE_A + ANALYSIS_ITERABLE +
                    ANALYSIS_SCALE_DATA);
    ies::enkf_config_set_max_steplength(config, DEFAULT_IES_MAX_STEPLENGTH);
    ies::enkf_config_set_min_steplength(config, DEFAULT_IES_MIN_STEPLENGTH);
    ies::enkf_config_set_dec_steplength(config, DEFAULT_IES_DEC_STEPLENGTH);
    ies::enkf_config_set_subspace(config, DEFAULT_IES_SUBSPACE);
    ies::enkf_config_set_inversion(config, DEFAULT_IES_INVERSION);
    ies::enkf_config_set_logfile(config, DEFAULT_IES_LOGFILE);
    ies::enkf_config_set_aaprojection(config, DEFAULT_IES_AAPROJECTION);

    return config;
}

/*------------------------------------------------------------------------------------------------*/
/* TRUNCATION -> SUBSPACE_DIMENSION */
double ies::enkf_config_get_truncation(const enkf_config_type *config) {
    return config->truncation;
}

void ies::enkf_config_set_truncation(enkf_config_type *config,
                                     double truncation) {
    config->truncation = truncation;
    if (truncation > 0.0)
        config->subspace_dimension = INVALID_SUBSPACE_DIMENSION;
}

/*------------------------------------------------------------------------------------------------*/
/* SUBSPACE_DIMENSION -> TRUNCATION */
int ies::enkf_config_get_enkf_subspace_dimension(
    const ies::enkf_config_type *config) {
    return config->subspace_dimension;
}

void ies::enkf_config_set_enkf_subspace_dimension(enkf_config_type *config,
                                                  int subspace_dimension) {
    config->subspace_dimension = subspace_dimension;
    if (subspace_dimension > 0)
        config->truncation = INVALID_TRUNCATION;
}

/*------------------------------------------------------------------------------------------------*/
/* OPTION_FLAGS */

long ies::enkf_config_get_option_flags(const ies::enkf_config_type *config) {
    return config->option_flags;
}

void ies::enkf_config_set_option_flags(enkf_config_type *config, long flags) {
    config->option_flags = flags;
}

/*------------------------------------------------------------------------------------------------*/
/* IES_MAX_STEPLENGTH */
double
ies::enkf_config_get_max_steplength(const ies::enkf_config_type *config) {
    return config->ies_max_steplength;
}
void ies::enkf_config_set_max_steplength(ies::enkf_config_type *config,
                                         double ies_max_steplength) {
    config->ies_max_steplength = ies_max_steplength;
}
/*------------------------------------------------------------------------------------------------*/
/* IES_MIN_STEPLENGTH */
double
ies::enkf_config_get_min_steplength(const ies::enkf_config_type *config) {
    return config->ies_min_steplength;
}
void ies::enkf_config_set_min_steplength(ies::enkf_config_type *config,
                                         double ies_min_steplength) {
    config->ies_min_steplength = ies_min_steplength;
}

/*------------------------------------------------------------------------------------------------*/
/* IES_DEC_STEPLENGTH */
double
ies::enkf_config_get_dec_steplength(const ies::enkf_config_type *config) {
    return config->ies_dec_steplength;
}
void ies::enkf_config_set_dec_steplength(ies::enkf_config_type *config,
                                         double ies_dec_steplength) {

    // The formula used to calculate step length has a hard assumption that the
    // steplength is reduced for every step - here that is silently enforced
    // with the std::max(1.1, ....).
    config->ies_dec_steplength =
        std::max(ies_dec_steplength, MIN_IES_DEC_STEPLENGTH);
}

/*------------------------------------------------------------------------------------------------*/
/* IES_INVERSION          */
ies::inversion_type
ies::enkf_config_get_inversion(const ies::enkf_config_type *config) {
    return static_cast<ies::inversion_type>(config->ies_inversion);
}
void ies::enkf_config_set_inversion(ies::enkf_config_type *config,
                                    ies::inversion_type ies_inversion) {
    config->ies_inversion = ies_inversion;
}

/*------------------------------------------------------------------------------------------------*/
/* IES_SUBSPACE      */
bool ies::enkf_config_get_subspace(const ies::enkf_config_type *config) {
    return config->ies_subspace;
}
void ies::enkf_config_set_subspace(ies::enkf_config_type *config,
                                   bool ies_subspace) {
    config->ies_subspace = ies_subspace;
}

/*------------------------------------------------------------------------------------------------*/
/* IES_AAPROJECTION         */
bool ies::enkf_config_get_aaprojection(const ies::enkf_config_type *config) {
    return config->ies_aaprojection;
}
void ies::enkf_config_set_aaprojection(enkf_config_type *config,
                                       bool ies_aaprojection) {
    config->ies_aaprojection = ies_aaprojection;
}

/*------------------------------------------------------------------------------------------------*/
/* IES_LOGFILE       */
char *ies::enkf_config_get_logfile(const ies::enkf_config_type *config) {
    return config->ies_logfile;
}
void ies::enkf_config_set_logfile(ies::enkf_config_type *config,
                                  const char *ies_logfile) {
    config->ies_logfile =
        util_realloc_string_copy(config->ies_logfile, ies_logfile);
}

/*------------------------------------------------------------------------------------------------*/
/* FREE_CONFIG */
void ies::enkf_config_free(ies::enkf_config_type *config) { free(config); }

double
ies::enkf_config_calculate_steplength(const ies::enkf_config_type *ies_config,
                                      int iteration_nr) {
    double ies_max_step = ies::enkf_config_get_max_steplength(ies_config);
    double ies_min_step = ies::enkf_config_get_min_steplength(ies_config);
    double ies_decline_step = ies::enkf_config_get_dec_steplength(ies_config);

    /*
      This is an implementation of Eq. (49) from:

      Geir Evensen, Formulating the history matching problem with consistent error statistics,
      Computational Geosciences (2021) 25:945 –970: https://doi.org/10.1007/s10596-021-10032-7
    */

    double ies_steplength =
        ies_min_step + (ies_max_step - ies_min_step) *
                           pow(2, -(iteration_nr - 1) / (ies_decline_step - 1));

    return ies_steplength;
}
