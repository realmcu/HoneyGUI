/* for Platform Scenario mode setting */
typedef enum
{
    PLATFORM_SCENARIO_OPERATION_MODE,
    PLATFORM_SCENARIO_VAD_MODE,
    PLATFORM_SCENARIO_USB_SUSPEND_MODE,
    PLATFORM_SCENARIO_MAX,
} PlatformScenarioType;

// PLATFORM_SCENARIO_OPERATION_MODE
typedef enum
{
    OPERATION_HP_MODE               = 0,
    OPERATION_SENSOR_MODE           = 1,
} OperationModeType;

// PLATFORM_SCENARIO_VAD_MODE
typedef enum
{
    VAD_DISABLE_MODE                = 0,
    VAD_ENABLE_MODE                 = 1,
} VADModeType;

// PLATFORM_SCENARIO_USB_SUSPEND_MODE
typedef enum
{
    USB_SUSPEND_DISABLE_MODE        = 0,
    USB_SUSPEND_ENABLE_MODE         = 1,
} USBSuspendModeType;

extern OperationModeType(*platform_scenario_get_mode)(PlatformScenarioType);
extern uint32_t (*platform_scenario_set_mode)(PlatformScenarioType, OperationModeType);

/* for Operation mode callback */
/* OPERATION_MODE_SYSTEM_CHECK:
 *     for APP to check operation mode can be changed or not, callback prototype as OperationModeSystemCheckFunc
 *         parameter: operation mode to be changed into
 *         return value: check pass or not
 * OPERATION_MODE_SYSTEM_ENTER:
 *     for APP to do necessary instruction before operation mode changed, callback prototype as OperationModeSystemEnterFunc
 *         parameter: operation mode to be changed into
 */
typedef enum
{
    OPERATION_MODE_SYSTEM_CHECK     = 1,
    OPERATION_MODE_SYSTEM_ENTER     = 2,
} OperationModeStage;

typedef bool (*OperationModeSystemCheckFunc)(OperationModeType);
typedef void (*OperationModeSystemEnterFunc)(OperationModeType);

void operation_mode_register_callback_func(OperationModeStage stage, void *cb_func);


typedef enum
{
    DVFS_NORMAL_VDD                 = 0,
} DVFSVDDType;

typedef enum
{
    DVFS_VDD_0V9                    = 0,
    DVFS_VDD_0V8                    = 1,
} DVFSVDDMode;

typedef enum
{
    DVFS_SUCCESS                    = 0x0,
    DVFS_BUSY                       = 0x1,
    DVFS_VOLTAGE_FAIL               = 0x2,
    DVFS_CONDITION_FAIL             = 0x4,
    DVFS_SRAM_FAIL                  = 0x8,
    DVFS_NOT_SUPPORT                = 0x10,
} DVFSErrorCode;
extern DVFSErrorCode(*dvfs_set_mode)(DVFSVDDType, DVFSVDDMode);

