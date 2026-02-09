#include "RGBController_Cherry.h"

#define NA  0xFFFFFFFF
#define CHERRY_MATRIX_MAP_HEIGHT 5
#define CHERRY_MATRIX_MAP_WIDTH 15

static unsigned int matrixMap[CHERRY_MATRIX_MAP_HEIGHT][CHERRY_MATRIX_MAP_WIDTH] = { 
    { 0,  1,  2,  3,  4,  5,  6,  7,  8,  9,  10, 11, 12, 13, 14 },
    { 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29 },
    { 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, NA, 42, 43 },
    { 44, NA, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, NA },
    { 57, 58, 59, NA, NA, NA, 60, NA, NA, 61, 62, 63, 64, 65, 66 } };


RGBController_Cherry::RGBController_Cherry(CherryController* controller)
: controller{controller}
{
    auto deviceInfo = controller->GetDeviceInfo();

    name = "Cherry Favonius Keyboard";  // once GetDeviceInfo gives proper name
                                        // name = deviceInfo.name();
    vendor = "Cherry";
    description = "Cherry Keyboard Device";
    type = DEVICE_TYPE_KEYBOARD;
    location = controller->GetLocation();
    serial = deviceInfo.serial_number();
 
    SetupModes();
    SetupZones();
}

RGBController_Cherry::~RGBController_Cherry() {
    for(const auto& zone : zones) {
        if(zone.matrix_map != NULL) {
            delete zone.matrix_map;
        }
    }

    delete controller;
}

void RGBController_Cherry::SetupModes() {
    auto effects = controller->GetEffects();
    auto settings = controller->GetSettings();
    auto ledBehavior = controller->GetBehaviorDetail(LED_BEHAVIOR_ID).metadata();
    zmk::led_settings::LedSettingsNodeInfo selector;
    
    uint32_t brightnessMin{0}, brightnessMax{0}, speedMin{0}, speedMax{0};
    
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
                // for(const auto& effect : effects.data()) {
                for(int i = effects.data().size() - 1; i >= 0; i--) {
                    auto effect = effects.data().at(i);
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
                        m.colors.resize(1);
                        m.colors_min = 1;
                        m.colors_max = 1;
                        
                        modes.push_back(m);
                        controller->AddLedEffects(effect);
                        break;
                    }
                }
            }
        }
    }
    
    // find smallest effect studio id in modes
    if(!(modes.empty())) {
        controller->SetEffectStudioIdOffset(modes[0].value);
        for(const auto& m : modes) {
            if(m.value < controller->GetEffectStudioIdOffset()) controller->SetEffectStudioIdOffset(m.value);
        }
        
        // reduce all values by the smalles value within modes
        // to get the correct index for setting those effects
        for(auto& m : modes) {
            m.value = m.value - controller->GetEffectStudioIdOffset();
        }
    }

    // init old values
    modeIdOld = modes[active_mode].value;
    speedOld = modes[active_mode].speed;
    brightnessOld = modes[active_mode].brightness;
}

void RGBController_Cherry::SetupZones() {
    auto physicalLayout = controller->GetPhysicalLayouts().layouts(0);

    zone z;
    z.name = "Keyboard";
    z.type = ZONE_TYPE_MATRIX;
    z.leds_min = physicalLayout.keys_size();
    z.leds_max = physicalLayout.keys_size();
    z.leds_count = physicalLayout.keys_size();

    z.matrix_map = new matrix_map_type;
    z.matrix_map->height = CHERRY_MATRIX_MAP_HEIGHT;
    z.matrix_map->width = CHERRY_MATRIX_MAP_WIDTH;
    z.matrix_map->map = (unsigned int *)&matrixMap;

    zones.push_back(z);


    for(int ledId = 0; ledId < physicalLayout.keys_size(); ledId++) {
        led l;
        l.name = "Keyboard LED ";
        l.name.append(std::to_string(ledId));
        leds.push_back(l);
    }

    SetupColors();
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
    if(modes[active_mode].value != modeIdOld) {
        controller->UpdateMode(modes[active_mode].value);
        speedOld = modes[active_mode].speed;
        brightnessOld = modes[active_mode].brightness;
    }
    else if(modes[active_mode].speed != speedOld) {
        controller->UpdateSpeed(modes[active_mode].value, modes[active_mode].speed);
        speedOld = modes[active_mode].speed;
    }
    else if(modes[active_mode].brightness != brightnessOld) {
        controller->UpdateBrightness(modes[active_mode].value, modes[active_mode].speed);
        brightnessOld = modes[active_mode].brightness;
    }
}
