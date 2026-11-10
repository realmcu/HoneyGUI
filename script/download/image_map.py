IMG_MAP = {
    "rtl87x3e": {
        "SCCD" : {
            'description' : 'SOCV Config File',  #socv 
            'macro' : r'^(SOCV_CFG_ADDR)=(\w+)',
            'address': ''
        },
        "OCCD" : {
            'description' : 'System Config File', # CFG *.bin
            'macro' : r'^(OEM_CFG_ADDR)=(\w+)',
            'address': ''
        },
        "FactoryCode" : {
            'description' : 'Boot Patch File', # boot_patch*.bin
            'macro' : r'^(BOOT_PATCH_ADDR)=(\w+)',
            'address': ''
        },
        "OTA" : {
            'description' : 'OTA Header File', # OTAHeader*.bin
            'macro' : r'^({}_OTA_HDR_ADDR)=(\w+)',
            'address': ''
        },
        "SecureBoot" : {
            'description' : 'Secure Boot Loader Image',  # fsbl*.bin
            'macro' : r'^({}_FSBL_ADDR)=(\w+)',
            'address': ''
        },
        "RomPatch" : {
            'description' : 'ROM Patch Image', # patch_MP*.bin
            'macro' : r'^({}_ROM_PATCH_ADDR)=(\w+)',
            'address': ''
        },
        "AppPatch" : {
            'description' : 'APP Image', # audio*.bin
            'macro' : r'^({}_APP_ADDR)=(\w+)',
            'address': ''
        },
        "DspPatch" : {
            'description' : 'DSP System Image', # dsp_sys_image*.bin
            'macro' :r'^({}_DSP_PATCH_ADDR)=(\w+)',
            'address': ''
        },
        "DspApp" : {
            'description' : 'DSP APP Image', # dsp_app_image*.bin
            'macro' : r'^({}_DSP_APP_ADDR)=(\w+)',
            'address': ''
        },
        "AppData" : {
            'description' : 'APP UI Parameter File', # APP_*.bin
            'macro' : r'^({}_APP_DATA_ADDR)=(\w+)',
            'address': ''
        },
        "DspData" : {
            'description' : 'DSP UI Parameter File', # dsp_config*.bin
            'macro' : r'^({}_DSP_DATA_ADDR)=(\w+)',
            'address': ''
        },
        "ExtImage0" : {
            'description' : 'Ext Image 0', # anc*.bin
            'macro' : r'^({}_EXT_IMG0_ADDR)=(\w+)',
            'address': ''
        },
        "ExtImage1" : {
            'description' : 'Ext Image 1',
            'macro' : r'^({}_EXT_IMG1_ADDR)=(\w+)',
            'address': ''
        },
        "ExtImage6" : {
            'description' : 'Ext Image 6', # upper_patch*.bin
            'macro' : r'^({}_UPPERSTACK_ADDR)=(\w+)',
            'address': ''
        },
        "Platform" : {
            'description' : 'Platform file', # sys_patch*.bin
            'macro' : r'^({}_PLATFORM_ADDR)=(\w+)',
            'address': ''
        },
        "StackPatch" : {
            'description' : 'Stack file', # stack_patch*.bin
            'macro' : r'^({}_STACK_ADDR)=(\w+)',
            'address': ''
        },
        "PreSysPatch" : {
            'description' : 'Pre-sys patch', # platform_ext_image*.bin
            'macro' : r'^(PRE_SYS_PATCH_ADDR)=(\w+)',
            'address': ''
        },
        "PreStackPatch" : {
            'description' : 'Pre-stack patch', # lowerstack_ext_image*.bin
            'macro' : r'^(PRE_STACK_PATCH_ADDR)=(\w+)',
            'address': ''
        },
        "PreUpperstack" : {
            'description' : 'Pre-upperstack patch', # upperstack*.bin
            'macro' : r'^(PRE_UPPER_STACK_PATCH_ADDR)=(\w+)',
            'address': ''
        },
        "VPData" : {
            'description' : 'Voice prompt data', # VPData*.bin
            'macro' : r'^(VP_DATA_ADDR)=(\w+)',
            'address': ''
        },
        "USER_DATA1" : {
            'description' : 'user data1', # user_data1*.bin
            'macro' : r'^(USER_DATA1_ADDR)=(\w+)',
            'address': ''
        },
    },
    "rtl87x3d": {
        "SCCD" : {
            'description' : 'SOCV Config File',  #socv 
            'macro' : r'^(SOCV_CFG_ADDR)=(\w+)',
            'address': ''
        },
        "OCCD" : {
            'description' : 'System Config File', # CFG *.bin
            'macro' : r'^(OEM_CFG_ADDR)=(\w+)',
            'address': ''
        },
        "FactoryCode" : {
            'description' : 'Boot Patch File', # bootpatch *.bin
            'macro' : r'^(BOOT_PATCH_ADDR)=(\w+)',
            'address': ''
        },
        "OTA" : {
            'description' : 'OTA Header File', # OTAHeader*.bin
            'macro' : r'^({}_OTA_HDR_ADDR)=(\w+)',
            'address': ''
        },
        "SecureBoot" : {
            'description' : 'Secure Boot Loader Image',  # fsbl*.bin
            'macro' : r'^({}_FSBL_ADDR)=(\w+)',
            'address': ''
        },
        "RomPatch" : {
            'description' : 'ROM Patch Image', # patch_MP*.bin
            'macro' : r'^({}_ROM_PATCH_ADDR)=(\w+)',
            'address': ''
        },
        "AppPatch" : {
            'description' : 'APP Image', # audio*.bin
            'macro' : r'^({}_APP_ADDR)=(\w+)',
            'address': ''
        },
        "DspPatch" : {
            'description' : 'DSP System Image', # dsp_sys_image*.bin
            'macro' :r'^({}_DSP_PATCH_ADDR)=(\w+)',
            'address': ''
        },
        "DspApp" : {
            'description' : 'DSP APP Image', # dsp_app_image*.bin
            'macro' : r'^({}_DSP_APP_ADDR)=(\w+)',
            'address': ''
        },
        "AppData" : {
            'description' : 'APP UI Parameter File', # APP_*.bin
            'macro' : r'^({}_APP_DATA_ADDR)=(\w+)',
            'address': ''
        },
        "DspData" : {
            'description' : 'DSP UI Parameter File', # dsp_config*.bin
            'macro' : r'^({}_DSP_DATA_ADDR)=(\w+)',
            'address': ''
        },
        "ExtImage0" : {
            'description' : 'Ext Image 0',
            'macro' : r'^({}_EXT_IMG0_ADDR)=(\w+)',
            'address': ''
        },
        "ExtImage1" : {
            'description' : 'Ext Image 1', #dsp2 sys
            'macro' : r'^({}_EXT_IMG1_ADDR)=(\w+)',
            'address': ''
        },
        "ExtImage2" : {
            'description' : 'Ext Image 2',
            'macro' : r'^({}_EXT_IMG2_ADDR)=(\w+)',
            'address': ''
        },
        "ExtImage3" : {
            'description' : 'Ext Image 3', #dsp3_app
            'macro' : r'^({}_EXT_IMG3_ADDR)=(\w+)',
            'address': ''
        },
        "ExtImage6" : {
            'description' : 'Ext Image 6', # upper_patch*.bin
            'macro' : r'^({}_UPPERSTACK_ADDR)=(\w+)',
            'address': ''
        },
        "Platform" : {
            'description' : 'Platform file', # sys_patch*.bin
            'macro' : r'^({}_PLATFORM_ADDR)=(\w+)',
            'address': ''
        },
        "StackPatch" : {
            'description' : 'Stack file', # stack_patch*.bin
            'macro' : r'^({}_STACK_ADDR)=(\w+)',
            'address': ''
        },
        "PreSysPatch" : {
            'description' : 'Pre-sys patch', # platform_ext_image*.bin
            'macro' : r'^(PRE_SYS_PATCH_ADDR)=(\w+)',
            'address': ''
        },
        "PreStackPatch" : {
            'description' : 'Pre-stack patch', # lowerstack_ext_image*.bin
            'macro' : r'^(PRE_STACK_PATCH_ADDR)=(\w+)',
            'address': ''
        },
        "PreUpperstack" : {
            'description' : 'Pre-upperstack patch', # upperstack*.bin
            'macro' : r'^(PRE_UPPER_STACK_PATCH_ADDR)=(\w+)',
            'address': ''
        },
        "VPData" : {
            'description' : 'Voice prompt data', # VPData*.bin
            'macro' : r'^(VP_DATA_ADDR)=(\w+)',
            'address': ''
        },
        "USER_DATA1" : {
            'description' : 'user data1', # user_data1*.bin
            'macro' : r'^(USER_DATA1_ADDR)=(\w+)',
            'address': ''
        },
    },
    "rtl87x3ep": {
        "SCCD" : {
            'description' : 'SOCV Config File',  #socv 
            'macro' : r'^(SOCV_CFG_ADDR)=(\w+)',
            'address': ''
        },
        "OCCD" : {
            'description' : 'System Config File', # CFG *.bin
            'macro' : r'^(OEM_CFG_ADDR)=(\w+)',
            'address': ''
        },
        "FactoryCode" : {
            'description' : 'Boot Patch File', # boot_patch*.bin
            'macro' : r'^(BOOT_PATCH{}_ADDR)=(\w+)',
            'address': ''
        },
        "OTA" : {
            'description' : 'OTA Header File', # OTAHeader*.bin
            'macro' : r'^({}_OTA_HDR_ADDR)=(\w+)',
            'address': ''
        },
        "RomPatch" : {
            'description' : 'ROM Patch Image', # patch_MP*.bin
            'macro' : r'^({}_ROM_PATCH_ADDR)=(\w+)',
            'address': ''
        },
        "AppPatch" : {
            'description' : 'APP Image', # audio*.bin
            'macro' : r'^({}_APP_ADDR)=(\w+)',
            'address': ''
        },
        "DspPatch" : {
            'description' : 'DSP System Image', # dsp_sys_image*.bin
            'macro' :r'^({}_DSP_SYS_ADDR)=(\w+)',
            'address': ''
        },
        "DspApp" : {
            'description' : 'DSP APP Image', # dsp_app_image*.bin
            'macro' : r'^({}_DSP_APP_ADDR)=(\w+)',
            'address': ''
        },
        "AppData" : {
            'description' : 'APP UI Parameter File', # APP_*.bin
            'macro' : r'^({}_APP_CFG_ADDR)=(\w+)',
            'address': ''
        },
        "DspData" : {
            'description' : 'DSP UI Parameter File', # dsp_config*.bin
            'macro' : r'^({}_DSP_CFG_ADDR)=(\w+)',
            'address': ''
        },
        "ExtImage0" : {
            'description' : 'Ext Image 0', # anc*.bin
            'macro' : r'^({}_EXT_IMG0_ADDR)=(\w+)',
            'address': ''
        },
        "ExtImage1" : {
            'description' : 'Ext Image 1',
            'macro' : r'^({}_EXT_IMG1_ADDR)=(\w+)',
            'address': ''
        },
        "ExtImage6" : {
            'description' : 'Ext Image 6', # upper_patch*.bin
            'macro' : r'^({}_UPPERSTACK_ADDR)=(\w+)',
            'address': ''
        },
        "Platform" : {
            'description' : 'Platform file', # sys_patch*.bin
            'macro' : r'^({}_SYS_PATCH_ADDR)=(\w+)',
            'address': ''
        },
        "StackPatch" : {
            'description' : 'Stack file', # stack_patch*.bin
            'macro' : r'^({}_STACK_PATCH_ADDR)=(\w+)',
            'address': ''
        },
        "PreSysPatch" : {
            'description' : 'Pre-sys patch', # platform_ext_image*.bin
            'macro' : r'^(PLATFORM_EXT_ADDR)=(\w+)',
            'address': ''
        },
        "PreStackPatch" : {
            'description' : 'Pre-stack patch', # lowerstack_ext_image*.bin
            'macro' : r'^(LOWERSTACK_EXT_ADDR)=(\w+)',
            'address': ''
        },
        "PreUpperstack" : {
            'description' : 'Pre-upperstack patch', # upperstack*.bin
            'macro' : r'^(UPPERSTACK_ADDR)=(\w+)',
            'address': ''
        },
        "VPData" : {
            'description' : 'Voice prompt data', # VPData*.bin
            'macro' : r'^(VP_DATA_ADDR)=(\w+)',
            'address': ''
        },
        "USER_DATA1" : {
            'description' : 'user data1', # user_data1*.bin
            'macro' : r'^(USER_DATA1_ADDR)=(\w+)',
            'address': ''
        },
    },
    "rtl87x3g": {
        "OCCD" : {
            'description' : 'System Config File', # CFG *.bin
            'macro' : r'^(OEM_CFG_ADDR)=(\w+)',
            'address': ''
        },
        "FactoryCode" : {
            'description' : 'Boot Patch File', # boot_patch*.bin
            'macro' : r'^(BOOT_PATCH{}_ADDR)=(\w+)',
            'address': ''
        },
        "OTA" : {
            'description' : 'OTA Header File', # OTAHeader*.bin
            'macro' : r'^({}_OTA_HDR_ADDR)=(\w+)',
            'address': ''
        },
        "AppPatch" : {
            'description' : 'APP Image', # audio*.bin
            'macro' : r'^({}_APP_ADDR)=(\w+)',
            'address': ''
        },
        "DspPatch" : {
            'description' : 'DSP System Image', # dsp_sys_image*.bin
            'macro' :r'^({}_DSP_SYS_ADDR)=(\w+)',
            'address': ''
        },
        "DspApp" : {
            'description' : 'DSP APP Image', # dsp_app_image*.bin
            'macro' : r'^({}_DSP_APP_ADDR)=(\w+)',
            'address': ''
        },
        "AppData" : {
            'description' : 'APP UI Parameter File', # APP_*.bin
            'macro' : r'^({}_APP_CFG_ADDR)=(\w+)',
            'address': ''
        },
        "DspData" : {
            'description' : 'DSP UI Parameter File', # dsp_config*.bin
            'macro' : r'^({}_DSP_CFG_ADDR)=(\w+)',
            'address': ''
        },
        "ExtImage0" : {
            'description' : 'Ext Image 0', # anc*.bin
            'macro' : r'^({}_EXT_IMG0_ADDR)=(\w+)',
            'address': ''
        },
        "ExtImage1" : {
            'description' : 'Ext Image 1',
            'macro' : r'^({}_EXT_IMG1_ADDR)=(\w+)',
            'address': ''
        },
        "Platform" : {
            'description' : 'Platform file', # sys_patch*.bin
            'macro' : r'^({}_SYS_PATCH_ADDR)=(\w+)',
            'address': ''
        },
        "StackPatch" : {
            'description' : 'Stack file', # stack_patch*.bin
            'macro' : r'^({}_STACK_PATCH_ADDR)=(\w+)',
            'address': ''
        },
        "PreUpperstack" : {
            'description' : 'Pre-upperstack patch', # upperstack*.bin
            'macro' : r'^(UPPERSTACK_ADDR)=(\w+)',
            'address': ''
        },
        "VPData" : {
            'description' : 'Voice prompt data', # VPData*.bin
            'macro' : r'^(VP_DATA_ADDR)=(\w+)',
            'address': ''
        },
        "USER_DATA1" : {
            'description' : 'user data1', # user_data1*.bin
            'macro' : r'^(USER_DATA1_ADDR)=(\w+)',
            'address': ''
        },
    }
}

DOWNLOAD_INFO = {
    "rtl87x3e": {
        "Address": "0x02000000",
        "Size": "0x400000",
        "SRCAddress": "0x21F073C" #BANK0_APP_CFG_ADDR(0x21F0000) + image header size(0x400) + Offset address(0x33C)
    },
    "rtl87x3d": {
        "Address": "0x02000000",
        "Size": "0x1000000",
        "SRCAddress": "0x234833C" #BANK0_APP_CFG_ADDR(0x022C7000) + image header size(0x1000) + Offset address(0x33C)
    },
    "rtl87x3ep": {
        "Address": "0x02000000",
        "Size": "0x400000",
        "SRCAddress": "0x21F073C" #BANK0_APP_CFG_ADDR(0x21F0000) + image header size(0x400) + Offset address(0x33C)
    },
    "rtl87x3g": {
        "Address": "0x70000000",
        "Size": "0x400000",
        "SRCAddress": "0x702A373C" #BANK0_APP_CFG_ADDR(0x702A3000) + image header size(0x400) + Offset address(0x33C)
    },
}