---
tags:
  - Global
  - Class
---
# Global Class "EntityConfig"

???+ info
    These functions can be accessed via the `EntityConfig` global table.

    **Note that to call these functions, you must use a `.` (period) instead of a `:` (colon)!**

    ???+ example "Example Code"
        ```lua
        local gaperConfig = EntityConfig.GetEntity(EntityType.ENTITY_GAPER)
        ```
        
## Functions

### GetBaby () {: aria-label='Functions' }
#### [EntityConfigBaby](EntityConfigBaby.md) GetBaby ( [BabySubType](https://wofsauge.github.io/IsaacDocs/rep/enums/BabySubType.html) Type ) {: .copyable aria-label='Functions' }
Returns nil if there is no co-op baby with the given ID.

___
### GetEntity () {: aria-label='Functions' }
#### [EntityConfigEntity](EntityConfigEntity.md) GetEntity ( [EntityType](https://wofsauge.github.io/IsaacDocs/rep/enums/EntityType.html) Type, int Variant = -1, int SubType = -1 ) {: .copyable aria-label='Functions' }
Returns nil if no entity exists with the specified Type.

Providing Variant and/or SubType is optional. If a non-existent Variant/SubType is requested, returns the base version of that entity.

___
### GetMaxBabyID () {: aria-label='Functions' }
#### int GetMaxBabyID ( ) {: .copyable aria-label='Functions' }
Returns the highest ID (corresponds to SubType) currently assigned to a valid co-op baby.

___
### GetMaxPlayerType () {: aria-label='Functions' }
#### int GetMaxPlayerType ( ) {: .copyable aria-label='Functions' }
Returns the highest PlayerType currently assigned to a valid character.

___
### GetPlayer () {: aria-label='Functions' }
#### [EntityConfigPlayer](EntityConfigPlayer.md) GetPlayer ( [PlayerType](https://wofsauge.github.io/IsaacDocs/rep/enums/PlayerType.html) PlayerType ) {: .copyable aria-label='Functions' }
Returns nil if no character exists with the specified PlayerType.

___

