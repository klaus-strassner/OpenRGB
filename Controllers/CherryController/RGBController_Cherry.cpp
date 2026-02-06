#include "RGBController_Cherry.h"

RGBController_Cherry::RGBController_Cherry(CherryController* controller)
: controller{controller}
{
    name = "Cherry Favonius Keyboard";  // change once GetDeviceInfo gives proper name
                                        // name = controller->GetDeviceInfo().name();
    vendor = "Cherry";
    description = "Cherry Keyboard Device";
    type = DEVICE_TYPE_KEYBOARD;
    location = controller->GetLocation();
    serial = controller->GetDeviceInfo().serial_number();
 
    SetupModes();
    SetupZones();
}

RGBController_Cherry::~RGBController_Cherry() {
    delete controller;
}

void RGBController_Cherry::SetupModes() {
    auto effects = controller->GetEffects();
    auto settings = controller->GetSettings();
    auto ledBehavior = controller->GetBehaviorDetail(LED_BEHAVIOR_ID).metadata();
    zmk::led_settings::LedSettingsNodeInfo selector;
    
    uint32_t brightnessMin{0}, brightnessMax{0}, speedMin{0}, speedMax{0}, hueMin{0}, hueMax{0};
    
    // assign default LED Behavior
    for(const auto& parameterSet : ledBehavior) {
        // check each parameter if it matches specified strings
        for(const auto& p1 : parameterSet.param1()) {
            if(p1.name() == BRIGHTNESS_SET_STRING) {
                for(const auto& p2 : parameterSet.param2()) if(p2.has_range()) {
                    brightnessMin = p2.range().min();
                    brightnessMax = p2.range().max();
                    break;
                }
            }
            else if(p1.name() == SPEED_SET_STRING) {
                for(const auto& p2 : parameterSet.param2()) if(p2.has_range()) {
                    speedMin = p2.range().min();
                    speedMax = p2.range().max();
                    break;
                }
            }
        }
    }
    
    // find selector
    for(const auto& setting : settings.data()) {
        // get the first selector that isnt the root
        if((setting.storage_flags() & (1U << 31)) && (setting.studio_id() != 1)) {
            selector = setting;
            break;
        }
    }
    
    // create modes
    for(const auto& setting : settings.data()) {
        if(setting.has_parent_studio_id()) {
            // check if we are setting of the selector
            if(setting.parent_studio_id().value() == selector.studio_id()) {
                // find effect associated with setting
                for(const auto& effect : effects.data()) {
                    if(setting.studio_id() == effect.settings_studio_id()) {
                        mode m;
                        auto values = controller->GetSettingsValues(setting.studio_id());
                        
                        m.name = effect.effect_name();
                        m.value = effect.studio_id(); // todo: reduce to match set-effect index
                        m.flags = MODE_FLAG_HAS_SPEED | MODE_FLAG_HAS_BRIGHTNESS;
                        m.color_mode = MODE_COLORS_MODE_SPECIFIC;
                        
                        m.brightness_min = brightnessMin;
                        m.brightness_max = brightnessMax;
                        m.brightness = values.brightness_1_effective().value();
                        m.speed_min = speedMin;
                        m.speed_max = speedMax;
                        m.speed = values.speed_factor_effective().value();
                        m.colors_min = hueMin;
                        m.colors_max = hueMax;
                        
                        modes.push_back(m);
                        break;
                    }    
                }
            }
        }
    }
    
    // find smallest effect studio id in modes
    if(!(modes.empty())) {
        int minEffectStudioId = modes[0].value;
        for(const auto& m : modes) {
            if(m.value < minEffectStudioId) minEffectStudioId = m.value;
        }
        
        // reduce all values by the smalles value within modes
        // to get the correct index for setting those effects
        for(auto& m : modes) {
            m.value = m.value - minEffectStudioId;
        }
    }
}

void RGBController_Cherry::SetupZones() {

    auto physicalLayout = controller->GetPhysicalLayouts().layouts(0);

    zone z;
    z.name = "Keyboard";
    z.leds_min = physicalLayout.keys_size();
    z.leds_max = physicalLayout.keys_size();
    z.leds_count = physicalLayout.keys_size();

}

void RGBController_Cherry::ResizeZone(int /*zone*/, int /*new_size*/) {

}

void RGBController_Cherry::DeviceUpdateLEDs() {

}

void RGBController_Cherry::UpdateZoneLEDs(int /*zone*/) {

}

void RGBController_Cherry::UpdateSingleLED(int /*led*/) {

}

void RGBController_Cherry::DeviceUpdateMode() {

}
