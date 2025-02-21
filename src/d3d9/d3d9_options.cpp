#include "d3d9_options.h"

#include "d3d9_caps.h"

namespace dxvk {

  static int32_t parsePciId(const std::string& str) {
    if (str.size() != 4)
      return -1;
    
    int32_t id = 0;

    for (size_t i = 0; i < str.size(); i++) {
      id *= 16;

      if (str[i] >= '0' && str[i] <= '9')
        id += str[i] - '0';
      else if (str[i] >= 'A' && str[i] <= 'F')
        id += str[i] - 'A' + 10;
      else if (str[i] >= 'a' && str[i] <= 'f')
        id += str[i] - 'a' + 10;
      else
        return -1;
    }

    return id;
  }


  D3D9Options::D3D9Options(const Rc<DxvkDevice>& device, const Config& config) {
    const Rc<DxvkAdapter> adapter = device != nullptr ? device->adapter() : nullptr;

    // Fetch these as a string representing a hexadecimal number and parse it.
    this->customVendorId        = parsePciId(config.getOption<std::string>("d3d9.customVendorId"));
    this->customDeviceId        = parsePciId(config.getOption<std::string>("d3d9.customDeviceId"));
    this->customDeviceDesc      = config.getOption<std::string>("d3d9.customDeviceDesc");

    this->maxFrameLatency       = config.getOption<int32_t> ("d3d9.maxFrameLatency",       0);
    this->presentInterval       = config.getOption<int32_t> ("d3d9.presentInterval",       -1);
    this->shaderModel           = config.getOption<int32_t> ("d3d9.shaderModel",           3);
    this->evictManagedOnUnlock  = config.getOption<bool>    ("d3d9.evictManagedOnUnlock",  false);
    this->dpiAware              = config.getOption<bool>    ("d3d9.dpiAware",              true);
    this->allowLockFlagReadonly = config.getOption<bool>    ("d3d9.allowLockFlagReadonly", true);
    this->strictConstantCopies  = config.getOption<bool>    ("d3d9.strictConstantCopies",  false);
    this->strictPow             = config.getOption<bool>    ("d3d9.strictPow",             true);
    this->lenientClear          = config.getOption<bool>    ("d3d9.lenientClear",          false);
    this->numBackBuffers        = config.getOption<int32_t> ("d3d9.numBackBuffers",        0);
    this->deferSurfaceCreation  = config.getOption<bool>    ("d3d9.deferSurfaceCreation",  false);
    this->hasHazards            = config.getOption<bool>    ("d3d9.hasHazards",            false);
    this->samplerAnisotropy     = config.getOption<int32_t> ("d3d9.samplerAnisotropy",     -1);
    this->maxAvailableMemory    = config.getOption<uint32_t>("d3d9.maxAvailableMemory",    UINT32_MAX);
    this->supportDFFormats      = config.getOption<bool>    ("d3d9.supportDFFormats",      true);
    this->swvpFloatCount        = config.getOption<uint32_t>("d3d9.swvpFloatCount",        caps::MaxFloatConstantsSoftware);
    this->swvpIntCount          = config.getOption<uint32_t>("d3d9.swvpIntCount",          caps::MaxOtherConstantsSoftware);
    this->swvpBoolCount         = config.getOption<uint32_t>("d3d9.swvpBoolCount",         caps::MaxOtherConstantsSoftware);

    this->d3d9FloatEmulation    = true; // <-- Future Extension?

    applyTristate(this->d3d9FloatEmulation, config.getOption<Tristate>("d3d9.floatEmulation", Tristate::Auto));

    // This is not necessary on Nvidia.
    if (adapter != nullptr && adapter->matchesDriver(DxvkGpuVendor::Nvidia, VK_DRIVER_ID_NVIDIA_PROPRIETARY_KHR, 0, 0))
      this->hasHazards          = false;
  }

}