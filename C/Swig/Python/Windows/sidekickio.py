# This file was automatically generated by SWIG (http://www.swig.org).
# Version 4.0.2
#
# Do not make changes to this file unless you know what you are doing--modify
# the SWIG interface file instead.

from sys import version_info as _swig_python_version_info
if _swig_python_version_info < (2, 7, 0):
    raise RuntimeError("Python 2.7 or later required")

# Import the low-level C/C++ module
if __package__ or "." in __name__:
    from . import _sidekickio
else:
    import _sidekickio

try:
    import builtins as __builtin__
except ImportError:
    import __builtin__

def _swig_repr(self):
    try:
        strthis = "proxy of " + self.this.__repr__()
    except __builtin__.Exception:
        strthis = ""
    return "<%s.%s; %s >" % (self.__class__.__module__, self.__class__.__name__, strthis,)


def _swig_setattr_nondynamic_instance_variable(set):
    def set_instance_attr(self, name, value):
        if name == "thisown":
            self.this.own(value)
        elif name == "this":
            set(self, name, value)
        elif hasattr(self, name) and isinstance(getattr(type(self), name), property):
            set(self, name, value)
        else:
            raise AttributeError("You cannot add instance attributes to %s" % self)
    return set_instance_attr


def _swig_setattr_nondynamic_class_variable(set):
    def set_class_attr(cls, name, value):
        if hasattr(cls, name) and not isinstance(getattr(cls, name), property):
            set(cls, name, value)
        else:
            raise AttributeError("You cannot add class attributes to %s" % cls)
    return set_class_attr


def _swig_add_metaclass(metaclass):
    """Class decorator for adding a metaclass to a SWIG wrapped class - a slimmed down version of six.add_metaclass"""
    def wrapper(cls):
        return metaclass(cls.__name__, cls.__bases__, cls.__dict__.copy())
    return wrapper


class _SwigNonDynamicMeta(type):
    """Meta class to enforce nondynamic attributes (no new attributes) for a class"""
    __setattr__ = _swig_setattr_nondynamic_class_variable(type.__setattr__)


VENDOR_EP_SIZE = _sidekickio.VENDOR_EP_SIZE
MAX_PACKET_SIZE = _sidekickio.MAX_PACKET_SIZE
TIMEOUT_MS = _sidekickio.TIMEOUT_MS
NUM_RETRIES = _sidekickio.NUM_RETRIES
SK_VENDOR_ID = _sidekickio.SK_VENDOR_ID
SK_PRODUCT_ID_APP = _sidekickio.SK_PRODUCT_ID_APP
SK_PRODUCT_ID_DFU = _sidekickio.SK_PRODUCT_ID_DFU
SK_NVM_SIZE = _sidekickio.SK_NVM_SIZE
SK_APP_SIZE = _sidekickio.SK_APP_SIZE
class SidekickIO(object):
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc="The membership flag")
    __repr__ = _swig_repr
    CMD_ECHO = _sidekickio.SidekickIO_CMD_ECHO
    CMD_CFG = _sidekickio.SidekickIO_CMD_CFG
    CMD_GPIO_CFG = _sidekickio.SidekickIO_CMD_GPIO_CFG
    CMD_GPIO_PIN_SET = _sidekickio.SidekickIO_CMD_GPIO_PIN_SET
    CMD_GPIO_PIN_READ = _sidekickio.SidekickIO_CMD_GPIO_PIN_READ
    CMD_GPIO_GET_INTRPT_STATUS = _sidekickio.SidekickIO_CMD_GPIO_GET_INTRPT_STATUS
    CMD_GPIO_CLR_INTRPT_STATUS = _sidekickio.SidekickIO_CMD_GPIO_CLR_INTRPT_STATUS
    CMD_GPIO_ENA_PIN_INTRPT = _sidekickio.SidekickIO_CMD_GPIO_ENA_PIN_INTRPT
    CMD_GPIO_WRITE_PARALLEL = _sidekickio.SidekickIO_CMD_GPIO_WRITE_PARALLEL
    CMD_GPIO_DIS_PIN_INTRPT = _sidekickio.SidekickIO_CMD_GPIO_DIS_PIN_INTRPT
    CMD_SPIM_TRANSFER_DATA = _sidekickio.SidekickIO_CMD_SPIM_TRANSFER_DATA
    CMD_DFU_START = _sidekickio.SidekickIO_CMD_DFU_START
    CMD_DFU_WRITE_DATA = _sidekickio.SidekickIO_CMD_DFU_WRITE_DATA
    CMD_DFU_READ_DATA = _sidekickio.SidekickIO_CMD_DFU_READ_DATA
    CMD_DFU_RESET_READ_PTR = _sidekickio.SidekickIO_CMD_DFU_RESET_READ_PTR
    CMD_DFU_DONE_WRITING = _sidekickio.SidekickIO_CMD_DFU_DONE_WRITING
    CMD_DFU_RESET = _sidekickio.SidekickIO_CMD_DFU_RESET
    CMD_I2CM_TRANSACTION = _sidekickio.SidekickIO_CMD_I2CM_TRANSACTION
    SK_ERROR_NONE = _sidekickio.SidekickIO_SK_ERROR_NONE
    SK_ERROR_UNKNOWN_CMD = _sidekickio.SidekickIO_SK_ERROR_UNKNOWN_CMD
    SK_ERROR_NO_MEMORY = _sidekickio.SidekickIO_SK_ERROR_NO_MEMORY
    SK_ERROR_MALFORMED_PACKET = _sidekickio.SidekickIO_SK_ERROR_MALFORMED_PACKET
    SK_ERROR_RESOURCE_BUSY = _sidekickio.SidekickIO_SK_ERROR_RESOURCE_BUSY
    SK_ERROR_PARAMETER = _sidekickio.SidekickIO_SK_ERROR_PARAMETER
    SK_ERROR_STATE = _sidekickio.SidekickIO_SK_ERROR_STATE
    SK_ERROR_OPERATION_FAILED = _sidekickio.SidekickIO_SK_ERROR_OPERATION_FAILED
    SK_ERROR_MAX = _sidekickio.SidekickIO_SK_ERROR_MAX
    LAYOUT_CONFIG_GPIO = _sidekickio.SidekickIO_LAYOUT_CONFIG_GPIO
    LAYOUT_CONFIG_I2C_MASTER = _sidekickio.SidekickIO_LAYOUT_CONFIG_I2C_MASTER
    LAYOUT_CONFIG_SPI_MASTER = _sidekickio.SidekickIO_LAYOUT_CONFIG_SPI_MASTER
    GPIO_CONFIG_DIR_IN = _sidekickio.SidekickIO_GPIO_CONFIG_DIR_IN
    GPIO_CONFIG_DIR_OUT = _sidekickio.SidekickIO_GPIO_CONFIG_DIR_OUT
    GPIO_CONFIG_PULL_NONE = _sidekickio.SidekickIO_GPIO_CONFIG_PULL_NONE
    GPIO_CONFIG_PULL_UP = _sidekickio.SidekickIO_GPIO_CONFIG_PULL_UP
    GPIO_CONFIG_PULL_DOWN = _sidekickio.SidekickIO_GPIO_CONFIG_PULL_DOWN
    INTRPT_STATUS_GPIO_Msk = _sidekickio.SidekickIO_INTRPT_STATUS_GPIO_Msk
    INTRPT_STATUS_SPI_Msk = _sidekickio.SidekickIO_INTRPT_STATUS_SPI_Msk
    FW_MODE_APP = _sidekickio.SidekickIO_FW_MODE_APP
    FW_MODE_DFU = _sidekickio.SidekickIO_FW_MODE_DFU
    FW_MODE_MAX = _sidekickio.SidekickIO_FW_MODE_MAX

    def __init__(self, *args):
        _sidekickio.SidekickIO_swiginit(self, _sidekickio.new_SidekickIO(*args))
    __swig_destroy__ = _sidekickio.delete_SidekickIO

    def transfer_cmd(self, rsp, rsplen, cmd, fmt):
        return _sidekickio.SidekickIO_transfer_cmd(self, rsp, rsplen, cmd, fmt)

    def error2str(self, error_code):
        return _sidekickio.SidekickIO_error2str(self, error_code)

    def config_layout_gpio(self):
        return _sidekickio.SidekickIO_config_layout_gpio(self)
    I2CM_CLK_SEL_100KHZ = _sidekickio.SidekickIO_I2CM_CLK_SEL_100KHZ
    I2CM_CLK_SEL_400KHZ = _sidekickio.SidekickIO_I2CM_CLK_SEL_400KHZ
    I2CM_CMD_WRITE_DATA = _sidekickio.SidekickIO_I2CM_CMD_WRITE_DATA
    I2CM_CMD_WRITE_BYTE = _sidekickio.SidekickIO_I2CM_CMD_WRITE_BYTE
    I2CM_CMD_READ_DATA = _sidekickio.SidekickIO_I2CM_CMD_READ_DATA
    I2CM_CMD_STOP = _sidekickio.SidekickIO_I2CM_CMD_STOP

    def send_config_layout_i2cm(self, *args):
        return _sidekickio.SidekickIO_send_config_layout_i2cm(self, *args)

    def get_fw_error_code(self):
        return _sidekickio.SidekickIO_get_fw_error_code(self)

    def i2cm_write_data(self, slave_addr, data, len):
        return _sidekickio.SidekickIO_i2cm_write_data(self, slave_addr, data, len)

    def i2cm_read_data(self, slave_addr, data, len):
        return _sidekickio.SidekickIO_i2cm_read_data(self, slave_addr, data, len)

    def i2cm_write_register(self, slave_addr, reg, data, len):
        return _sidekickio.SidekickIO_i2cm_write_register(self, slave_addr, reg, data, len)

    def i2cm_write_register_byte(self, slave_addr, reg, val):
        return _sidekickio.SidekickIO_i2cm_write_register_byte(self, slave_addr, reg, val)

    def i2cm_read_register(self, slave_addr, reg, data, len):
        return _sidekickio.SidekickIO_i2cm_read_register(self, slave_addr, reg, data, len)

    def i2cm_read_register_byte(self, slave_addr, reg, val):
        return _sidekickio.SidekickIO_i2cm_read_register_byte(self, slave_addr, reg, val)
    SPI_MODE_0 = _sidekickio.SidekickIO_SPI_MODE_0
    SPI_MODE_1 = _sidekickio.SidekickIO_SPI_MODE_1
    SPI_MODE_2 = _sidekickio.SidekickIO_SPI_MODE_2
    SPI_MODE_3 = _sidekickio.SidekickIO_SPI_MODE_3
    SPI_DATA_ORDER_MSB = _sidekickio.SidekickIO_SPI_DATA_ORDER_MSB
    SPI_DATA_ORDER_LSB = _sidekickio.SidekickIO_SPI_DATA_ORDER_LSB

    def config_layout_spim(self, *args):
        return _sidekickio.SidekickIO_config_layout_spim(self, *args)

    def spim_transfer_packet(self, cs_index, buf, len):
        return _sidekickio.SidekickIO_spim_transfer_packet(self, cs_index, buf, len)

    def spim_transfer(self, cs_index, buf, len):
        return _sidekickio.SidekickIO_spim_transfer(self, cs_index, buf, len)

    def spim_write(self, cs_index, data, len):
        return _sidekickio.SidekickIO_spim_write(self, cs_index, data, len)

    def spim_write_byte(self, cs_index, val):
        return _sidekickio.SidekickIO_spim_write_byte(self, cs_index, val)

    def gpio_config(self, gpio_index, dir, pull):
        return _sidekickio.SidekickIO_gpio_config(self, gpio_index, dir, pull)

    def gpio_pin_set(self, gpio_index, level):
        return _sidekickio.SidekickIO_gpio_pin_set(self, gpio_index, level)

    def gpio_read(self, gpio_index):
        return _sidekickio.SidekickIO_gpio_read(self, gpio_index)

    def gpio_get_intrpt_status(self, gpio_int_mask):
        return _sidekickio.SidekickIO_gpio_get_intrpt_status(self, gpio_int_mask)

    def gpio_clr_intrpt_status(self, gpio_int_mask):
        return _sidekickio.SidekickIO_gpio_clr_intrpt_status(self, gpio_int_mask)

    def gpio_enable_pin_intrpt(self, gpio_index, handler, obj):
        return _sidekickio.SidekickIO_gpio_enable_pin_intrpt(self, gpio_index, handler, obj)

    def gpio_disable_pin_intrpt(self, gpio_index):
        return _sidekickio.SidekickIO_gpio_disable_pin_intrpt(self, gpio_index)

    def send_echo(self, data, len, match):
        return _sidekickio.SidekickIO_send_echo(self, data, len, match)

    def test_echo(self):
        return _sidekickio.SidekickIO_test_echo(self)

    def main_loop_task(self):
        return _sidekickio.SidekickIO_main_loop_task(self)

    def test_dfu(self):
        return _sidekickio.SidekickIO_test_dfu(self)

    def reset_into_mode(self, mode):
        return _sidekickio.SidekickIO_reset_into_mode(self, mode)

    def firmware_update(self, fw_data, fw_data_len, reset_mode):
        return _sidekickio.SidekickIO_firmware_update(self, fw_data, fw_data_len, reset_mode)

    def firmware_update_from_file(self, fw_file, reset_mode):
        return _sidekickio.SidekickIO_firmware_update_from_file(self, fw_file, reset_mode)

# Register SidekickIO in _sidekickio:
_sidekickio.SidekickIO_swigregister(SidekickIO)



