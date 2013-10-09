/*

 * (C) Copyright 2013 University of Antwerp (http://www.cosys-lab.be) and others.
 *
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the GNU Lesser General Public License
 * (LGPL) version 2.1 which accompanies this distribution, and is available at
 * http://www.gnu.org/licenses/lgpl-2.1.html
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * Contributors:
 *     maarten.weyn@uantwerpen.be
 *     glenn.ergeerts@uantwerpen.be
 *
 *
 *  The high level API to be used by applications which use the dash7 stack
 */

#ifndef D7STACK_H_
#define D7STACK_H_

#ifdef __cplusplus
extern "C" {
#endif

/*
 *  Configuration section
 *
 *  TODO get from configure step or similar
 */

#define DEBUG

#include "phy/phy.h"
#include "dll/dll.h"
#include "nwl/nwl.h"
#include "trans/trans.h"
#include "pres/pres.h"

void d7aoss_init(const filesystem_address_info *address_info);

#ifdef __cplusplus
extern "C" {
#endif

#endif /* D7STACK_H_ */
