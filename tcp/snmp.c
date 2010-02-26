/*
 * snmp.c
 *
 *  Created on: 19.2.2010
 *      Author: Administrator
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>

#include <sys/version.h>
#include <sys/confos.h>
#include <sys/confnet.h>
#include <sys/environ.h>

#include <pro/snmp.h>
#include <pro/snmp_api.h>
#include <pro/snmp_mib.h>
#include "snmp.h"

#if defined(STK501)
#define BOARD_NAME  "STK 501"
#elif defined(INTECH21)
#define BOARD_NAME  "Intech 21"
#elif defined(XNUT_100)
#define BOARD_NAME  "XNUT 100"
#elif defined(XNUT_105)
#define BOARD_NAME  "XNUT 105"
#elif defined(AT91SAM7X_EK)
#define BOARD_NAME  "AT91SAM7X-EK"
#elif defined(AT91SAM9260_EK)
#define BOARD_NAME  "AT91SAM9260-EK"
#elif defined(CHARON2)
#define BOARD_NAME  "Charon 2"
#elif defined(WOLF)
#define BOARD_NAME  "Wolf"
#elif defined(ARTHERNET1)
#define BOARD_NAME  "Arthernet 1"
#elif defined(MMNET02)
#define BOARD_NAME  "MMNET02"
#elif defined(ETHERNUT1)
#define BOARD_NAME  "Ethernut 1"
#elif defined(ETHERNUT2)
#define BOARD_NAME  "Ethernut 2"
#elif defined(ETHERNUT3)
#define BOARD_NAME  "Ethernut 3"
#elif defined(ETHERNUT5)
#define BOARD_NAME  "Ethernut 5"
#else
#define BOARD_NAME  "Custom"
#endif

#define SNMP_OID 		1, 3, 6, 1, 3, 55, 0
#define MAX_STR_LEN		64
#define MAX_FILE_LEN	20

#define MAG_OWN_FILE	0
#define MAG_OWN_FACE	1
#define MAG_OWN_MFACE   2
#define MAG_OWN_WHERE	3
#define MAG_OWN_WHO	    4

static u_char *MibVarsOwnGet(CONST SNMPVAR *, OID *, size_t *, int, size_t *, WMETHOD **);

static OID base_oid[] = { SNMP_OID };
static size_t base_oidlen = sizeof(base_oid) / sizeof(OID);

static SNMPVAR mib_variables[] = {
    {MAG_OWN_FILE, ASN_OCTET_STR, ACL_RWRITE, MibVarsOwnGet, 1, {0}},
    {MAG_OWN_FACE, ASN_INTEGER, ACL_RONLY, MibVarsOwnGet, 1, {1}},
    {MAG_OWN_MFACE, ASN_INTEGER, ACL_RONLY, MibVarsOwnGet, 1, {2}},
    //{MAG_OWN_WHERE, ASN_SEQUENCE, ACL_RONLY, MibVarsSysGet, 1, {3}},
    //{MAG_OWN_WHO, ASN_SEQUENCE ASN_OCTET_STR, ACL_RONLY, MibVarsSysGet, 1, {4}},
};

static u_char *filename;

static char sys_descr[MAX_STR_LEN];
static char *sys_contact;
static char *sys_name;
static char *sys_location;
static time_t sys_starttime;

static long sys_uptime;
static long sys_services = 72;
static OID sys_oid[] = {

	SNMP_OID_ENTERPRISES,
    3444,                       /* egnite */
    3,							/* egnite products=1 local=2 experimental=3 */
    6,                       	/* egnite Nut/OS */
};

static char *UpdateStringEnv(char *name, char *var, char *value)
{
    if (var) {
        free(var);
    }
    if ((var = malloc(strlen(value) + 1)) != NULL) {
        strcpy(var, value);
        if (name) {
            setenv(name, value, 1);
        }
    }
    return var;
}

/*!
 * \brief Register MIB II system variables.
 *
 * \note Preliminary code with hard coded values.
 */
int MibRegisterOwnVars(void)
{
    char *cp;

    strcpy(sys_descr, BOARD_NAME);
    strcat(sys_descr, " Nut/OS ");
    strcat(sys_descr, NutVersionString());

    if ((cp = getenv("SYSCONTACT")) != NULL) {
        sys_contact = UpdateStringEnv(NULL, sys_contact, cp);
    }
    if ((cp = getenv("SYSNAME")) != NULL) {
        sys_name = UpdateStringEnv(NULL, sys_name, cp);
    } else {
        sys_name = UpdateStringEnv(NULL, sys_name, confos.hostname);
    }
    if ((cp = getenv("SYSLOCATION")) != NULL) {
        sys_location = UpdateStringEnv(NULL, sys_location, cp);
    }

    sys_starttime = time(NULL);

    return SnmpMibRegister(base_oid, base_oidlen, mib_variables, sizeof(mib_variables) / sizeof(SNMPVAR));
}

static int MibVarsOwnName(int action, u_char * var_val, u_char var_val_type, size_t var_val_len, OID * name, size_t name_len)
{
	if (var_val_len > MAX_FILE_LEN)
	{
		return SNMP_ERR_WRONGLENGTH;
	}

    if (filename) {
        free(filename);
    }

	if ((filename = malloc((var_val_len + 1)*sizeof(char))) != NULL) {
		strcpy(filename, var_val);
		/*if (name) {
			setenv(name, value, 1);
		}*/

	}

	return 0;

}

static int MibVarsOwnSet(int action, u_char * var_val, u_char var_val_type, size_t var_val_len, OID * name, size_t name_len)
{
    size_t len = SNMP_MAX_LEN;
    u_char *value;
    u_char *cp;
    size_t size;

    if (action != SNMP_ACT_COMMIT) {
        return 0;
    }

    if (var_val_type != ASN_OCTET_STR) {
        return SNMP_ERR_WRONGTYPE;
    }
    if (var_val_len > MAX_STR_LEN) {
        return SNMP_ERR_WRONGLENGTH;
    }
    size = MAX_STR_LEN;
    if ((value = malloc(MAX_STR_LEN) + 1) == NULL) {
        return SNMP_ERR_RESOURCEUNAVAILABLE;
    }
    AsnOctetStringParse(var_val, &len, &var_val_type, value, &size);
    value[size] = 0;
    for (cp = value; *cp; cp++) {
        if (!isprint(*cp)) {
            free(value);
            return SNMP_ERR_WRONGVALUE;
        }
    }
    if (action == SNMP_ACT_COMMIT) {
        switch (name[7]) {
        case 4:
            sys_contact = UpdateStringEnv("SYSCONTACT", sys_contact, (char *) value);
            break;
        case 5:
            sys_name = UpdateStringEnv("SYSNAME", sys_name, (char *) value);
            break;
        case 6:
            sys_location = UpdateStringEnv("SYSLOCATION", sys_location, (char *) value);
            break;
        default:
        	sys_location = UpdateStringEnv("SYSLOCATION", sys_location, (char *) value);
        	printf("Hellurei\n");
        	break;
        	/*filename = UpdateStringEnv("TIEDOSTO", filename, (char *) value);
        	printf("%s\n", filename);
        	break;*/
        }
    }
    free(value);


    return 0;
}

/*!
 * \brief Access the specified MIB variable.
 *
 * \param vp
 * \param name    Contains the name to look for, either exact or one that
 *                is in front. On return the exact name is stored here.
 * \param namelen Number of sub identifiers in the name upon entry. On
 *                return the length of the exact name is stored here.
 * \param exact   If not zero, the name must match exactly. Otherwise we
 *                want the first name that is following the given one.
 * \param varlen  Size of the variable.
 */
static u_char *MibVarsOwnGet(CONST SNMPVAR * vp, OID * name, size_t * namelen, int exact, size_t * varlen, WMETHOD ** wmethod)
{
    static u_char empty[1];
    int rc;
    OID *fullname;
    size_t fullnamelen = base_oidlen + vp->var_namelen + 1;

    fullname = malloc(fullnamelen * sizeof(OID));
    memcpy(fullname, base_oid, base_oidlen * sizeof(OID));
    memcpy(fullname + base_oidlen, vp->var_name, vp->var_namelen * sizeof(OID));
    *(fullname + fullnamelen - 1) = 0;

    rc = SnmpOidCmp(name, *namelen, fullname, fullnamelen);
    if ((exact && rc) || (!exact && rc >= 0)) {
        return NULL;
    }
    memcpy(name, fullname, fullnamelen * sizeof(OID));
    *namelen = fullnamelen;

    *wmethod = NULL;
    *varlen = sizeof(long);
    switch (vp->var_magic) {
    case MAG_OWN_FILE:
    	printf("OID = 0\n");
    /*	*wmethod = MibVarsOwnSet;
    	if (sys_contact) {
    		*varlen = strlen(sys_contact);
    		return (u_char *) sys_contact;
    	}
    	 *varlen = 0;*/
    	 return empty;
    case MAG_OWN_MFACE:
			printf("OID = 1\n");
            return (u_char *) & sys_services;
    case MAG_OWN_FACE:
    	printf("OID = 2\n");
        return (u_char *) & sys_services;
    }
    return NULL;
}

