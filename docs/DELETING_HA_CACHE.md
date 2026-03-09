# Deleting HA's cache for this device
HA keeps a record of each device, even when removed, so if the device's nature changes by too much, it can cause some strange errors.

For example, I had earlier used this device to test the usability of TMP36 sensors. When trying to later add a DHT sensor, the tmp sensor entities remained and would not allow the any readings of the DHT sensor. The best way to handle this is to delete the device and entity registries associated with the board for a fresh start.

## Location of relevant files
`config/storage/core.device_registry`
`config/storage/core.entity_registry`

## Deleting entries
Find the problematic entity entries by searching in the entity registry. Note the device id and other identifying parameters. To fully clean the registries, search for entries that match the device id and delete those as well. 

If you delete the last entry in a list, make sure to remove the trailing comma from the new last entry.