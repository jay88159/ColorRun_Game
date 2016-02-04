#
_XDCBUILDCOUNT = 
ifneq (,$(findstring path,$(_USEXDCENV_)))
override XDCPATH = E:/PRO/codecomposer/tirtos_tivac_2_10_01_38/packages;E:/PRO/codecomposer/tirtos_tivac_2_10_01_38/products/bios_6_41_00_26/packages;E:/PRO/codecomposer/tirtos_tivac_2_10_01_38/products/ndk_2_24_01_18/packages;E:/PRO/codecomposer/tirtos_tivac_2_10_01_38/products/uia_2_00_02_39/packages;E:/PRO/codecomposer/ccsv6/ccs_base
override XDCROOT = C:/ti/xdctools_3_31_00_24_core
override XDCBUILDCFG = ./config.bld
endif
ifneq (,$(findstring args,$(_USEXDCENV_)))
override XDCARGS = 
override XDCTARGETS = 
endif
#
ifeq (0,1)
PKGPATH = E:/PRO/codecomposer/tirtos_tivac_2_10_01_38/packages;E:/PRO/codecomposer/tirtos_tivac_2_10_01_38/products/bios_6_41_00_26/packages;E:/PRO/codecomposer/tirtos_tivac_2_10_01_38/products/ndk_2_24_01_18/packages;E:/PRO/codecomposer/tirtos_tivac_2_10_01_38/products/uia_2_00_02_39/packages;E:/PRO/codecomposer/ccsv6/ccs_base;C:/ti/xdctools_3_31_00_24_core/packages;..
HOSTOS = Windows
endif
