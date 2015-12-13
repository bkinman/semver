/* Copyright 2015, Brandon Kinman
 * This file is part of The semver library.
 *
 * semver library is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.

 * semver library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Foobar.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef _semver_h_
#define _semver_h_

#include <stdio.h>

/*!*****************************************************************************
 * @file semver.h
 *
 * @author Brandon Kinman
 *
 * @date 23 Feb 2015
 *
 * @brief This library is a helper library for semantic versioning. It is based
 *        upon version 2.0.0 of the semver specification. Details regarding
 *        semantic versioning, as well as the full specification details may
 *        be found at http://semver.org.
 *
 *        NOTE: Usage examples may be found in the unit tests provided with
 *              this library.
 *
 ******************************************************************************/

/******************************************************************************
 * Globals/Externs
 ******************************************************************************/

/******************************************************************************
 * #defines
 ******************************************************************************/

/******************************************************************************
 * type definitions /enums
 ******************************************************************************/

struct semver_;
typedef struct semver_ semver_t;

/******************************************************************************
 * function prototypes
 ******************************************************************************/

/******************************************************************************
 *  @brief Creates and initializes a semantic version context.
 *
 *   NOTE: Newly created semvers have MAJOR, MINOR, and PATCH versions of 0
 *         and include no pre-release strings or build meta-data.
 *
 *  @param p2o_semver Second pointer to the semver to be create
 *
 *  @return 0 for success, nonzero otherwise
 *****************************************************************************/
int semver_create(semver_t **p2o_semver);

/******************************************************************************
 *  @brief Destroy and deinitialize a sementic version context.
 *
 *  @param po_semver Pointer to the semver to be destroyed and deinitialized.
 *
 *  @return 0 for success, nonzero otherwise
 *****************************************************************************/
int semver_destroy(semver_t *po_semver);

/******************************************************************************
 *  @brief Returns the major version of the semver.
 *
 *  @param p_semver Pointer to the semver.
 *  @return major version upon success, negative otherwise.
 *****************************************************************************/
int semver_get_major(const semver_t *p_semver);

/******************************************************************************
 *  @brief Returns the minor version of the semver.
 *
 *  @param p_semver Pointer to the semver.
 *  @return minor version upon success, negative otherwise.
 *****************************************************************************/
int semver_get_minor(const semver_t *p_semver);

/******************************************************************************
 *  @brief Returns the patch version of the semver.
 *
 *  @param p_semver Pointer to the semver.
 *  @return patch version upon success, negative otherwise.
 *****************************************************************************/
int semver_get_patch(const semver_t *p_semver);


/******************************************************************************
 *  @brief Returns the pre-release string portion of the semver as an
 *         outparam. If the pre release component of the semver does not exist,
 *         then the pr_str outparam shall be set to NULL.
 *
 *         NOTE: It is responsibility of the user to free *p2o_pr_str
 *         NOTE2: str_len does not include the terminating NULL byte.
 *
 *  @param p_semver   Pointer to the semver.
 *  @param p2o_pr_str (OUTPARAM) The pre release string.
 *  @return 0 upon success, negative otherwise.
 *****************************************************************************/
int semver_get_pr_str(const semver_t *p_semver,
                      char** p2o_pr_str,
                      uint16_t *po_str_len);

/******************************************************************************
 *  @brief Returns the build meta-data string portion of the semver as an
 *         outparam. If the build meta-data component of the semver does not
 *         exist, then the bmd_str outparam shall be set to NULL.
 *
 *         NOTE: It is responsibility of the user to free *p2o_bmd_str
 *         NOTE2: str_len does not include the terminating NULL byte.
 *
 *  @param p_semver   Pointer to the semver.
 *  @param p2o_bmd_str (OUTPARAM) The build meta-data string.
 *  @return 0 upon success, negative otherwise.
 *****************************************************************************/
int semver_get_bmd_str(const semver_t *p_semver,
                      char** p2o_bmd_str,
                      uint16_t *po_str_len);

/******************************************************************************
 *  @brief Sets the MAJOR version
 *
 *  @param p_semver Pointer to the semver.
 *  @return 0 upon success, negative otherwise.
 *****************************************************************************/
int semver_set_major(semver_t *p_semver, uint32_t major);

/******************************************************************************
 *  @brief Sets the MINOR version
 *
 *  @param p_semver Pointer to the semver.
 *  @return 0 upon success, negative otherwise.
 *****************************************************************************/
int semver_set_minor(semver_t *p_semver, uint32_t minor);

/******************************************************************************
 *  @brief Sets the PATCH version
 *
 *  @param p_semver Pointer to the semver.
 *  @return 0 upon success, negative otherwise.
 *****************************************************************************/
int semver_set_patch(semver_t *p_semver, uint32_t patch);

/******************************************************************************
 *  @brief Sets the pre-release string portion of the semver.
 *
 *         NOTE: this function will fail is the pre-release string is invalid.
 *
 *  @param p_semver   Pointer to the semver.
 *  @param pr_str     The pre release string.
 *  @param pr_str_len The pre-release string length.
 *  @return 0 upon success, negative otherwise.
 *****************************************************************************/
int semver_set_pr_str(semver_t *p_semver,
                      const char* pr_str,
                      uint16_t pr_str_len);

/******************************************************************************
 *  @brief Sets the build meta-data string portion of the semver.
 *
 *         NOTE: this function will fail is the build-meta data
 *               string is invalid.
 *
 *  @param p_semver   Pointer to the semver.
 *  @param bmd_str    The build meta-data string.
 *  @param pr_str_len The pre-release string length.
 *  @return 0 upon success, negative otherwise.
 *****************************************************************************/
int semver_set_bmd_str(semver_t *p_semver,
                      const char* bmd_str,
                      uint16_t pr_str_len);

/******************************************************************************
 *  @brief Converts a semver_t to a string.
 *
 *  NOTE: The semver_str outparam is dynamically allocated.
 *
 *  @param p2o_semver_str (OUTPARAM) The semver string.
 *  @param po_len         (OUTPARAM) The length of the semver string.
 *
 *  @return 0 if success, positive if invalid, negative if error
 *****************************************************************************/
int semver_to_str(const semver_t *p_semver,
                  char** p2o_semver_str,
                  int* po_len);

/******************************************************************************
 *  @brief Converts a string to a semver_t.
 *
 *  NOTE: The semver_str outparam is dynamically allocated.
 *
 *  @param semver_str     The semver string.
 *  @param semver_str_len The lenght of the semver string.
 *  @param p2o_semver      (OUTPARAM) The resulting semver context.
 *
 *  @return 0 if success, positive if invalid, negative if error
 *****************************************************************************/
int semver_str_to_semver(const char* semver_str,
                         uint16_t semver_str_len,
                         semver_t **p2o_semver);

/******************************************************************************
 *  @brief Compares two semantic versions using the rules of precedence
 *         outlined in semver 2.0.0
 *
 *         If sva precedes svb, -1 is written to the result.
 *         If svb precedes sva, +1 is written to the result.
 *         If sva and svb are equal, 0 is written to the result.
 *
 *  @param p_sva     Pointer to semver a.
 *  @param p_cfg     Pointer to semver b.
 *  @param po_result (OUTPARAM) Pointer to the result.
 *  @return 0 for success, nonzero otherwise
 *****************************************************************************/
int semver_compare(const semver_t *p_sva,
                   const semver_t *p_svb,
                   int *po_result);

/******************************************************************************
 *  @brief Compares two semantic version strings using the rules of precedence
 *         outlined in semver 2.0.0
 *
 *         If stra precedes strb, -1 is written to the result.
 *         If strb precedes stra, +1 is written to the result.
 *         If sva and svb are equal, 0 is written to the result.
 *
 *          NOTE: Pre-release versions will be used to determine precedence.
 *                However, build meta-data will not be used to  determine
 *                precedence.
 *
 *  @param stra The first string to be compared.
 *  @param strb The second string to be compared.
 *  @param len  The number of bytes to be compared.
 *  @param po_result The result of the comparison.
 *
 *  @return 0 for success, nonzero otherwise
 *****************************************************************************/
int semver_str_compare(const char* stra,
                       const char* strb,
                       uint16_t len,
                       int *po_result);

/******************************************************************************
 *  @brief Determines if a semver is valid.
 *
 *  @param p_semver Pointer a semver.
 *
 *  @return 0 if valid, positive if invalid, negative if error
 *****************************************************************************/
int semver_is_valid(const semver_t *p_semver);

/******************************************************************************
 *  @brief Determines if a semver string is valid.
 *
 *  @param p_semver The semver string.
 *  @param len      The length of the semver string.
 *
 *  @return 0 if valid, positive if invalid, negative if error
 *****************************************************************************/
int semver_str_is_valid(const char* semver_str, uint8_t len);

/******************************************************************************
 * "Private" definitions, do me a SOLID and don't poke this stuff directly. :)
 ******************************************************************************/
struct semver_
 {
    uint32_t major;
    uint32_t minor;
    uint32_t patch;

    char** pr_identifiers;
    uint16_t num_pr_identifiers;

    char* bmd_str;
    uint16_t bmd_str_len;
 };

#endif /* _semver_h_ */
