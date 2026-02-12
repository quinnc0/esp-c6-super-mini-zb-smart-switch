// Sensor base class definition
// sensors will inherit from this class
class ZbSensor {
public:
    virtual void setup() = 0;   // Initialize sensor
    virtual void tick() = 0;    // Main sensor logic
    virtual void setManufacturerAndModel(const char* manufacturer, const char* model) = 0;  // optional: set Zigbee device info
    virtual void setupBatteryMonitoring() = 0; // Optional battery monitoring setup
};