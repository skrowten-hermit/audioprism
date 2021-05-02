/*
 *******************************************************************************
 * 
 * This file is part of AudioPrism Project.
 *
 * AudioPrism is a free software: you can redistribute it and/or modify it under
 * the terms of the GNU Affero General Public License as published by the Free
 * Software Foundation (FSF), either version 3 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the Affero GNU General Public License
 * version 3 along with this program.  If not, see http://www.gnu.org/licenses/
 * 
 *******************************************************************************
 * 
 * File         : source/include/constref.h.
 * Description  : AudioPrism's constants are defined here.
 * Created On   : 15-09-2020.
 * Author       : Sreekanth Sreekumar.
 * Modified On  : 22-03-2021.
 * Modified By  : Sreekanth Sreekumar.
 * Changes Made : More threshold constants added.
 * 
 *******************************************************************************
 * 
 */


#ifndef CONSTREF_H
#define CONSTREF_H


#ifndef AP_VERSION
#define AP_VERSION "v1.0-rc1"
#endif

#ifndef APGA_THRESHOLD
#define APGA_THRESHOLD 2.0
#endif

#ifndef APNA_THRESHOLD
#define APNA_THRESHOLD -30.0
#endif

#ifndef APHG_THRESHOLD
#define APHG_THRESHOLD 8.0
#endif

#ifndef APVH_THRESHOLD
#define APVH_THRESHOLD 17.0
#endif

#ifndef APLG_THRESHOLD
#define APLG_THRESHOLD -8.0
#endif

#ifndef APVL_THRESHOLD
#define APVL_THRESHOLD -17.0
#endif

#ifndef APWF_THRESHOLD
#define APWF_THRESHOLD 0.01
#endif

#ifndef APCDD_THRESHOLD
#define APCDD_THRESHOLD 5.0
#endif

#ifndef APCDP_THRESHOLD
#define APCDP_THRESHOLD 4
#endif

#ifndef APCDS_THRESHOLD
#define APCDS_THRESHOLD -65
#endif

#ifndef APNBD_THRESHOLD
#define APNBD_THRESHOLD 8
#endif

#ifndef APNBS_THRESHOLD
#define APNBS_THRESHOLD -50
#endif

#ifndef APDD_THRESHOLD
#define APDD_THRESHOLD 8.0
#endif

#ifndef APDE_THRESHOLD
#define APDE_THRESHOLD -60.0
#endif

#ifndef APDS_THRESHOLD
#define APDS_THRESHOLD -50
#endif

#ifndef DEFAULT_INPUT
#define DEFAULT_INPUT "../../inputs/references/custom/speech/male_8k.wav"
#endif

#ifndef DEFAULT_OUTPUT
#define DEFAULT_OUTPUT "results.yaml"
#endif

#ifndef DEFAULT_LOUTPUT
#define DEFAULT_LOUTPUT "loader.yaml"
#endif

#ifndef DEFAULT_AOUTPUT
#define DEFAULT_AOUTPUT "attrs.yaml"
#endif

#ifndef DEFAULT_VOUTPUT
#define DEFAULT_VOUTPUT "verify.yaml"
#endif

#ifndef DEFAULT_DOUTPUT
#define DEFAULT_DOUTPUT "defects.yaml"
#endif


#endif
