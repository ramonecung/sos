/**
 * intPDBOneShotTest.h
 * Programmable Delay Block (PDB) one shot interrupt test program
 *
 * ARM-based K70F120M microcontroller board
 *   for educational purposes only
 * CSCI E-92 Spring 2014, Professor James L. Frankel, Harvard Extension School
 *
 * Written by James L. Frankel (frankel@seas.harvard.edu)
 */

#ifndef _INT_PDB_ONE_SHOT_TEST_H
#define _INT_PDB_ONE_SHOT_TEST_H

#include "switchcmd.h"

void processSwitchCmd(enum switchState cmd);

void intPDBOneShotTestInit(void);

int main(void);

#endif /* ifndef _INT_PDB_ONE_SHOT_TEST_H */
